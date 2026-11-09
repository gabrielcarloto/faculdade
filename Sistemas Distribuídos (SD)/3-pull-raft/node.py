import time
from dataclasses import dataclass
from enum import Enum, auto

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

        self.current_term = 0
        self.voted_for: str | None = None
        self.votes_received: set[str] = set()
        self.leader_id = None
        self.role = Role.FOLLOWER

        self.log: list[LogEntry] = []
        self.commit_index: int = -1
        self.state: int = 0

    def is_leader(self):
        return self.role == Role.LEADER

    def become_leader(self):
        if self.role != Role.CANDIDATE:
            raise RuntimeError("Golpe de estado")

        self.role = Role.LEADER
        self.leader_id = self.node_id
        # TODO: notificar outros nodes e monitorar fetch requests

    def become_follower(self, node: NodeMetadata):
        self.leader_id = node.sender_id
        self.current_term = node.term
        self.voted_for = None
        self.role = Role.FOLLOWER
        self.votes_received.clear()
        # TODO: iniciar timer para fetchrequests

    def start_election(self):
        self.current_term += 1
        self.role = Role.CANDIDATE
        self.voted_for = self.node_id
        self.leader_id = None
        self.votes_received = {self.node_id}
        # TODO: pedir votos, election timeout

    def receive_vote(self, peer_id: str, term: int):
        if self.role != Role.CANDIDATE or term != self.current_term:
            return

        self.votes_received.add(peer_id)

        if len(self.votes_received) >= MAJORITY:
            self.become_leader()

    # TODO
    def vote(self): ...

    def update_term(self, to: int):
        if to <= self.current_term:
            # TODO: retornar termo atual
            return

        self.current_term = to
        self.voted_for = None
        self.role = Role.FOLLOWER
        # NOTE: None ou pega de outra forma?
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