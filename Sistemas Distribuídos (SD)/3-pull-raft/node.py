import json
import os
from pathlib import Path
import random
import threading
import time
from dataclasses import dataclass
from enum import Enum, auto

import grpc
import raft_pb2
import raft_pb2_grpc

NODES = 4
# TODO: fazer mais verificações de quórum
MAJORITY = 3

MIN_TIMEOUT = 2
MAX_TIMEOUT = 5
HEARTBEAT_TIMEOUT = 3
RPC_TIMEOUT = 0.5
COMMIT_TIMEOUT = 3
FETCH_INTERVAL = 0.1

CHANNEL_OPTIONS = [
    ("grpc.initial_reconnect_backoff_ms", 100),
    ("grpc.max_reconnect_backoff_ms", 500),
]

data_directory = Path("./data")
data_directory.mkdir(parents=True, exist_ok=True)


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
    heartbeat_timeout: float = HEARTBEAT_TIMEOUT
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
        self.file_name = data_directory / f"node_{node_id}.json"
        self.lock = threading.RLock()

        self.current_term = 0
        self.voted_for: str | None = None
        self.votes_received: set[str] = set()
        self.election_timer: threading.Timer | None = None

        self.fetch_interval = FETCH_INTERVAL
        self.fetch_timer: threading.Timer | None = None

        self.leader_id = None
        self.role = Role.FOLLOWER

        self.log: list[LogEntry] = []
        self.commit_index: int = -1
        self.state: int = 0

        self.channels = {}
        self.stubs = {}
        self.init_stubs()

    def start(self):
        self.hablar("iniciando")
        self.load()
        self.reset_election_timer()
        self.reset_fetch_timer()

    def become_leader(self):
        if self.role != Role.CANDIDATE:
            raise RuntimeError("Golpe de estado")

        self.role = Role.LEADER
        self.leader_id = self.node_id
        self.stop_election()
        self.stop_fetching()

        for peer in self.peers.values():
            peer.last_request = None
            peer.last_log_index = -1
            peer.last_log_term = 0

        self.hablar(f"virei líder com {len(self.votes_received)} votos")

    def become_follower(self, node: NodeMetadata):
        if node.term < self.current_term:
            return

        if node.term > self.current_term:
            self.current_term = node.term
            self.voted_for = None
            self.save()

        prev_role = self.role
        prev_leader = self.leader_id

        self.leader_id = node.sender_id
        self.role = Role.FOLLOWER
        self.votes_received.clear()
        self.reset_election_timer()
        self.reset_fetch_timer()

        if prev_role != self.role or prev_leader != self.leader_id:
            self.hablar(f"virei seguidor de {node.sender_id}")

    def start_election(self):
        self.current_term += 1
        self.role = Role.CANDIDATE
        self.voted_for = self.node_id
        self.leader_id = None
        self.votes_received = {self.node_id}

        self.hablar(
            f"sem resposta do líder; iniciando eleição (termo {self.current_term})"
        )

        self.reset_election_timer()

        last_index, last_term = self.get_last_index_term()
        self.request_votes(last_index, last_term)

    def receive_vote(self, peer_id: str, term: int):
        if self.role != Role.CANDIDATE or term != self.current_term:
            return

        self.hablar(f"voto recebido de {peer_id}")

        self.votes_received.add(peer_id)

        if len(self.votes_received) >= MAJORITY:
            self.become_leader()

    def vote(
        self, candidate_id: str, term: int, last_log_index: int, last_log_term: int
    ):
        if term < self.current_term:
            self.hablar(f"neguei o voto ao nó {candidate_id}: termo antigo")
            return False

        if term > self.current_term:
            self.update_term(term)

        last_index, last_term = self.get_last_index_term()

        if (last_log_term, last_log_index) < (last_term, last_index):
            self.hablar(f"neguei o voto ao nó {candidate_id}: log desatualizado")
            return False

        if self.voted_for not in (None, candidate_id):
            self.hablar(
                f"neguei o voto ao nó {candidate_id}: já votei no nó {self.voted_for}"
            )
            return False

        self.voted_for = candidate_id
        self.save()

        self.reset_election_timer()
        self.hablar(f"votei em {candidate_id}")
        return True

    def update_term(self, to: int):
        if to <= self.current_term:
            return

        was_leader = self.is_leader()

        self.hablar(f"atualizando termo de {self.current_term} para {to}")

        self.current_term = to
        self.voted_for = None
        self.role = Role.FOLLOWER
        self.leader_id = None
        self.votes_received.clear()
        self.save()

        if was_leader:
            self.reset_election_timer()

    def fetch_request(self):
        with self.lock:
            if self.is_leader():
                return

            if self.leader_id is not None:
                targets = [self.leader_id]
            else:
                targets = list(self.peers)

            last_index, last_term = self.get_last_index_term()
            request = raft_pb2.FetchRequestRequest(
                metadata=raft_pb2.NodeMetadata(
                    sender_id=self.node_id, term=self.current_term
                ),
                last_log_index=last_index,
                last_log_term=last_term,
            )

        for peer_id in targets:
            try:
                response = self.stubs[peer_id].FetchRequest(
                    request, timeout=RPC_TIMEOUT
                )
            except grpc.RpcError:
                with self.lock:
                    if self.leader_id == peer_id:
                        self.hablar(f"o líder ({peer_id}) não respondeu")
                        self.leader_id = None
                continue

            with self.lock:
                if self.handle_fetch_response(request, response):
                    # líder encontrado
                    return

    def handle_fetch_response(self, request, response):
        if response.metadata.term > self.current_term:
            self.update_term(response.metadata.term)

        if not response.is_leader or response.metadata.term < self.current_term:
            return False

        self.become_follower(response.metadata)

        if self.get_last_index_term() != (
            # NOTE: REQUEST. RE-REsponse..?  NÃO! REQUEST!
            request.last_log_index,
            request.last_log_term,
        ):
            return True

        if not response.log_matches:
            removed = self.log.pop()
            self.save()
            self.hablar(
                f"LOG TRUNCADO: apaguei a entrada {len(self.log)} "
                f"({removed.operation.name} {removed.value}, termo {removed.term}), "
                "diferente do log do líder"
            )
            return True

        for entry in response.entries:
            operation = LogOperation(entry.operation)
            self.log.append(LogEntry(entry.term, operation, entry.value))
            self.hablar(
                f"recebi a entrada {entry.index} ({operation.name} {entry.value}) "
            )

        if response.entries:
            self.save()

        new_commit = min(response.commit_index, len(self.log) - 1)
        if new_commit > self.commit_index:
            self.commit_until(new_commit)

        return True

    def receive_request(
        self, peer_id: str, term: int, last_log_index: int, last_log_term: int
    ):
        peer = self.peers.get(peer_id)

        if peer is None:
            # nunca deve acontecer, é só pro lsp não reclamar
            raise RuntimeError("Request recebida de peer inexistente")

        if term > self.current_term:
            self.update_term(term)

        if not self.is_leader():
            # is_leader, log_matches, entries
            return False, False, []

        was_active = peer.is_active()
        peer.last_request = time.monotonic()

        if not was_active:
            self.hablar(
                f"réplica {peer_id} ativa "
                f"({self.count_active_nodes()} de {NODES} nós ativos)"
            )

        if not self.has_entry(last_log_index, last_log_term):
            return True, False, []

        peer.last_log_index = last_log_index
        peer.last_log_term = last_log_term
        self.update_commit_index()

        return True, True, self.log[last_log_index + 1 :]

    def update_commit_index(self):
        indexes = [len(self.log) - 1]
        indexes += [peer.last_log_index for peer in self.peers.values()]
        indexes.sort(reverse=True)
        new_commit = indexes[MAJORITY - 1]

        if (
            new_commit > self.commit_index
            and self.log[new_commit].term == self.current_term
        ):
            self.commit_until(new_commit)

    # CLIENTE
    def client_read(self):
        self.hablar(f"leitura: valor = {self.state}")
        return self.state

    def client_write(self, operation: LogOperation, value: int, simulate_crash: bool):
        with self.lock:
            if not self.is_leader():
                return False, f"o nó {self.node_id} não é o líder"

            active = self.count_active_nodes()
            if active < MAJORITY:
                message = f"sem quórum ({active} de {NODES} nós ativos)"
                self.hablar(f"escrita recusada: {message}")
                return False, f"escrita recusada: {message}"

            self.log.append(LogEntry(self.current_term, operation, value))
            self.save()

            index = len(self.log) - 1
            term = self.current_term

            self.hablar(
                f"gravou no log a entrada {index} ({operation.name} {value}), "
                "esperando o quórum"
            )

            # é mais fácil simular uma queda assim do que tentar acertar o timing
            # encerrando pelo terminal
            if simulate_crash:
                self.hablar("adiós, baby")
                os._exit(1)

        deadline = time.monotonic() + COMMIT_TIMEOUT
        while time.monotonic() < deadline:
            with self.lock:
                if not self.is_leader() or self.current_term != term:
                    return False, "deixou de ser o líder antes do commit"

                if self.commit_index >= index:
                    return True, f"entrada {index} comitada (termo {term})"

            time.sleep(0.01)

        return False, "não comitada: o quórum não confirmou a replicação a tempo"

    # UTILS
    def count_active_nodes(self):
        return 1 + sum(1 for peer in self.peers.values() if peer.is_active())

    def has_entry(self, index: int, term: int):
        log_empty = -1
        if index == log_empty:
            return True

        if index >= len(self.log):
            return False

        return self.log[index].term == term

    def get_last_index_term(self):
        last_log_index = len(self.log) - 1
        last_log_term = self.log[-1].term if self.log else 0
        return (last_log_index, last_log_term)

    def random_timeout(self):
        return random.uniform(MIN_TIMEOUT, MAX_TIMEOUT)

    def is_leader(self):
        return self.role == Role.LEADER

    def hablar(self, str: str):
        print(f"{self.node_id} ({self.role.value}): {str}")

    # BANCO
    def commit_until(self, index: int):
        while self.commit_index < index:
            self.commit_index += 1
            entry = self.log[self.commit_index]
            self.apply(entry)
            self.hablar(
                f"COMMITANDO entrada {self.commit_index} "
                f"({entry.operation.name} {entry.value}): valor = {self.state}"
            )

        self.save()

    def apply(self, entry: LogEntry):
        if entry.operation == LogOperation.SET:
            self.state = entry.value
        elif entry.operation == LogOperation.ADD:
            self.state += entry.value
        elif entry.operation == LogOperation.SUB:
            self.state -= entry.value

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

    # PERSISTÊNCIA
    def save(self):
        data = {
            "current_term": self.current_term,
            "voted_for": self.voted_for,
            "log": [
                {
                    "term": entry.term,
                    "operation": entry.operation.name,
                    "value": entry.value,
                }
                for entry in self.log
            ],
            "commit_index": self.commit_index,
            "state": self.state,
        }

        with open(self.file_name, "w", encoding="utf-8") as file:
            json.dump(data, file, indent=2)

    def load(self):
        if not os.path.exists(self.file_name):
            return

        with open(self.file_name, encoding="utf-8") as file:
            data = json.load(file)  # noqa: F821

        self.current_term = data["current_term"]
        self.voted_for = data["voted_for"]
        self.log = [
            LogEntry(entry["term"], LogOperation[entry["operation"]], entry["value"])
            for entry in data["log"]
        ]
        self.commit_index = data["commit_index"]
        self.state = data["state"]

    # GRPC
    def init_stubs(self):
        for peer_id, peer in self.peers.items():
            channel = grpc.insecure_channel(peer.address, options=CHANNEL_OPTIONS)
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
