import argparse
import raft_service
from node import Node, Peer

BASE_ADDRESS = "127.0.0.1"

node_presets = {
    "A": {
        "port": 8000,
    },
    "B": {
        "port": 8001,
    },
    "C": {
        "port": 8002,
    },
    "D": {
        "port": 8003,
    },
}


def parse_args():
    parser = argparse.ArgumentParser(description="RAFT Node")

    parser.add_argument(
        "--id", required=True, help="Identificador do nó ('A', 'B', 'C' ou 'D')"
    )

    return parser.parse_args()


def create_peers(for_node_id: str):
    presets = dict(node_presets)
    presets.pop(for_node_id)

    peers = {
        node_id: Peer(address=BASE_ADDRESS + f":{info['port']}")
        for node_id, info in presets.items()
    }

    return peers


def main():
    args = parse_args()
    node_preset = node_presets.get(args.id)

    if node_preset is None:
        raise RuntimeError("Identificador incorreto")

    peers = create_peers(args.id)
    node = Node(args.id, peers)

    server = raft_service.start_server(node, BASE_ADDRESS + f":{node_preset['port']}")
    node.start()

    try:
        server.wait_for_termination()
    except KeyboardInterrupt:
        server.stop(grace=1)


if __name__ == "__main__":
    main()
