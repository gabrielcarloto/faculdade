import raft_pb2_grpc
from node import Node


class RaftService(raft_pb2_grpc.RaftServicer):
    def __init__(self, node: Node):
        self.node = node

    def RequestVote(self, request, context):
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