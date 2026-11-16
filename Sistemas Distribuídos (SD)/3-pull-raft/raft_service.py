from concurrent import futures

import raft_pb2
import raft_pb2_grpc
import grpc

from node import Node, LogOperation, Peer


class RaftService(raft_pb2_grpc.RaftServicer):
    def __init__(self, node: Node):
        self.node = node

    def RequestVote(self, request, context):
        # lock garante mesmo termo até a resposta
        with self.node.lock:
            voted = self.node.vote(
                candidate_id=request.metadata.sender_id,
                term=request.metadata.term,
                last_log_index=request.last_log_index,
                last_log_term=request.last_log_term,
            )

            return raft_pb2.RequestVoteResponse(
                metadata=raft_pb2.NodeMetadata(
                    sender_id=self.node.node_id, term=self.node.current_term
                ),
                voted=voted,
            )

    def FetchRequest(self, request, context):
        with self.node.lock:
            is_leader, log_matches, entries = self.node.receive_request(
                peer_id=request.metadata.sender_id,
                term=request.metadata.term,
                last_log_index=request.last_log_index,
                last_log_term=request.last_log_term,
            )

            first_index = request.last_log_index + 1

            return raft_pb2.FetchRequestResponse(
                metadata=raft_pb2.NodeMetadata(
                    sender_id=self.node.node_id, term=self.node.current_term
                ),
                is_leader=is_leader,
                log_matches=log_matches,
                entries=[
                    raft_pb2.LogEntry(
                        index=first_index + i,
                        term=entry.term,
                        operation=entry.operation.value,
                        value=entry.value,
                    )
                    for i, entry in enumerate(entries)
                ],
                commit_index=self.node.commit_index,
            )

    def Write(self, request, context):
        success, message = self.node.client_write(
            operation=LogOperation(request.operation),
            value=request.value,
            simulate_crash=request.simulate_crash,
        )

        return raft_pb2.WriteResponse(
            success=success,
            leader_id=self.node.leader_id or "",
            message=message,
        )

    def Read(self, request, context):
        with self.node.lock:
            return raft_pb2.ReadResponse(value=self.node.client_read())


def start_server(node: Node, address: str):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))

    raft_pb2_grpc.add_RaftServicer_to_server(
        RaftService(node),
        server,
    )

    server.add_insecure_port(address)
    server.start()

    print(f"Node {node.node_id} listening on {address}")

    return server
