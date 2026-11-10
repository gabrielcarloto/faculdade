import random
import threading
import time
from dataclasses import dataclass
from enum import Enum, auto

import grpc
import raft_pb2
import raft_pb2_grpc

NODES = 4
MAJORITY = 3


class Role(Enum):
    FOLLOWER = "follower"
    CANDIDATE = "candidate"
    LEADER = "leader"


class LogOperation(Enum):
    SET = auto()
    ADD = auto()
    SUB = auto()


@dataclass
class Peer:
    address: str
    last_request: float | None = None
    heartbeat_timeout: float = 0.5
    last_log_index: int = -1
    last_log_term: int = 0

    def is_active(
        self,
    ):
        if self.last_request is None:
            return False

        elapsed = time.monotonic() - self.last_request
        return elapsed < self.heartbeat_timeout


@dataclass
class LogEntry:
    term: int
    operation: LogOperation
    value: int


@dataclass
class NodeMetadata:
    sender_id: str
    term: int


class Node:
    def __init__(self, node_id: str, peers: dict[str, Peer]):
        self.node_id = node_id
        self.peers = peers

        # TODO: proteger mudanças com lock?
        self.current_term = 0
        self.voted_for: str | None = None
        self.votes_received: set[str] = set()
        self.election_timer: threading.Timer | None = None

        self.fetch_interval = 0.05
        self.fetch_timer: threading.Timer | None = None

        self.leader_id = None
        self.role = Role.FOLLOWER

        self.log: list[LogEntry] = []
        self.commit_index: int = -1
        self.state: int = 0

        self.channels = {}
        self.stubs = {}
        self.init_stubs()

        self.reset_election_timer()
        self.reset_fetch_timer()

    def become_leader(self):
        if self.role != Role.CANDIDATE:
            raise RuntimeError("Golpe de estado")

        self.role = Role.LEADER
        self.leader_id = self.node_id
        self.stop_election()
        self.stop_fetching()

        # TODO: notificar outros nodes e monitorar fetch requests

    def become_follower(self, node: NodeMetadata):
        if node.term < self.current_term:
            return

        if node.term > self.current_term:
            self.voted_for = None

        self.leader_id = node.sender_id
        self.current_term = node.term
        self.role = Role.FOLLOWER
        self.votes_received.clear()
        self.reset_election_timer()
        self.reset_fetch_timer()

    def start_election(self):
        self.current_term += 1
        self.role = Role.CANDIDATE
        self.voted_for = self.node_id
        self.leader_id = None
        self.votes_received = {self.node_id}
        self.stop_fetching()
        self.reset_election_timer()

        last_index, last_term = self.get_last_index_term()
        self.request_votes(last_index, last_term)

    def receive_vote(self, peer_id: str, term: int):
        if self.role != Role.CANDIDATE or term != self.current_term:
            return

        self.votes_received.add(peer_id)

        if len(self.votes_received) >= MAJORITY:
            self.become_leader()

    def vote(
        self, candidate_id: str, term: int, last_log_index: int, last_log_term: int
    ):
        if term < self.current_term:
            return False

        if term > self.current_term:
            self.update_term(term)

        last_index, last_term = self.get_last_index_term()

        if (last_log_term, last_log_index) < (last_term, last_index):
            return False

        if self.voted_for not in (None, candidate_id):
            return False

        self.voted_for = candidate_id
        self.reset_election_timer()
        return True

    def update_term(self, to: int):
        if to <= self.current_term:
            return

        self.current_term = to
        self.voted_for = None
        self.role = Role.FOLLOWER
        self.leader_id = None
        self.votes_received.clear()

    # TODO
    def fetch_request(self): ...

    def receive_request(self, peer_id: str):
        peer = self.peers.get(peer_id)

        if peer is None:
            raise RuntimeError("Request recebida de peer inexistente")

        if not self.is_leader():
            return ...

        # TODO: verificar termo
        peer.last_request = time.monotonic()
        # TODO: atualizar dados peer e comparar logs

    # UTILS
    def get_last_index_term(self):
        last_log_index = len(self.log) - 1
        last_log_term = self.log[-1].term if self.log else 0
        return (last_log_index, last_log_term)

    def random_timeout(self):
        return random.uniform(0.150, 0.300)

    def is_leader(self):
        return self.role == Role.LEADER

    # TIMERS
    def on_election_timeout(self):
        if self.role in (Role.FOLLOWER, Role.CANDIDATE):
            self.start_election()

    def reset_election_timer(self):
        self.stop_election()

        self.election_timer = threading.Timer(
            self.random_timeout(), self.on_election_timeout
        )

        self.election_timer.start()

    def stop_election(self):
        if self.election_timer is not None:
            self.election_timer.cancel()
            self.election_timer = None

    def on_fetch_timeout(self):
        if self.role == Role.FOLLOWER:
            self.fetch_request()
            self.reset_fetch_timer()

    def reset_fetch_timer(self):
        self.stop_fetching()
        self.fetch_timer = threading.Timer(self.fetch_interval, self.on_fetch_timeout)
        self.fetch_timer.start()

    def stop_fetching(self):
        if self.fetch_timer is not None:
            self.fetch_timer.cancel()
            self.fetch_timer = None

    # GRPC
    def init_stubs(self):
        for peer_id, peer in self.peers.items():
            channel = grpc.insecure_channel(peer.address)
            self.channels[peer_id] = channel
            self.stubs[peer_id] = raft_pb2_grpc.RaftStub(channel)

    def request_votes(self, last_log_index: int, last_log_term: int):
        request = raft_pb2.RequestVoteRequest(
            metadata=raft_pb2.NodeMetadata(
                sender_id=self.node_id, term=self.current_term
            ),
            last_log_index=last_log_index,
            last_log_term=last_log_term,
        )

        for peer_id, stub in self.stubs.items():
            future = stub.RequestVote.future(request, timeout=0.1)

            future.add_done_callback(
                lambda f, pid=peer_id: self.handle_vote_response(pid, f)
            )

    def handle_vote_response(self, peer_id, future):
        try:
            response = future.result()
        except grpc.RpcError:
            return

        if response.metadata.term > self.current_term:
            self.update_term(response.metadata.term)
            return

        if response.voted:
            self.receive_vote(peer_id, response.metadata.term)
