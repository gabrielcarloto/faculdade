import time

import grpc
import raft_pb2
import raft_pb2_grpc

OPERATIONS = {
    "set": raft_pb2.DATA_OPERATION_SET,
    "add": raft_pb2.DATA_OPERATION_ADD,
    "sub": raft_pb2.DATA_OPERATION_SUB,
}

WRITE_TIMEOUT = 10
READ_TIMEOUT = 2
MAX_LEADER_SEARCH_ATTEMPTS = 8

ADDRESSES = {
    "A": "127.0.0.1:8000",
    "B": "127.0.0.1:8001",
    "C": "127.0.0.1:8002",
    "D": "127.0.0.1:8003",
}


def next_node(node_id: str):
    ids = list(ADDRESSES)
    return ids[(ids.index(node_id) + 1) % len(ids)]


def write(node_id: str, operation: str, value: int, simulate_crash=False):
    request = raft_pb2.WriteRequest(
        operation=OPERATIONS[operation], value=value, simulate_crash=simulate_crash
    )

    for _ in range(MAX_LEADER_SEARCH_ATTEMPTS):
        print(f"enviando a escrita para o nó {node_id}...")

        try:
            with grpc.insecure_channel(ADDRESSES[node_id]) as channel:
                stub = raft_pb2_grpc.RaftStub(channel)
                response = stub.Write(
                    request, timeout=WRITE_TIMEOUT, wait_for_ready=True
                )
        except grpc.RpcError:
            print(f"  o nó {node_id} não respondeu")
            if simulate_crash:
                return

            node_id = next_node(node_id)
            continue

        if response.success:
            print(f"  SUCESSO: {response.message}")
            return

        if response.leader_id == node_id:
            print(f"  FALHOU: {response.message}")
            return

        if response.leader_id:
            print(
                f"  {response.message}: redirecionando para o nó {response.leader_id}"
            )
            node_id = response.leader_id
        else:
            print(f"  {response.message} e não sabe quem é (eleição em andamento?)")
            node_id = next_node(node_id)
            time.sleep(0.5)

    print("  não foi possível encontrar o líder")


def read(node_id: str):
    try:
        with grpc.insecure_channel(ADDRESSES[node_id]) as channel:
            stub = raft_pb2_grpc.RaftStub(channel)
            response = stub.Read(raft_pb2.ReadRequest(), timeout=READ_TIMEOUT)

    except grpc.RpcError:
        print(f"  nó {node_id}: não respondeu")
        return

    print(f"  nó {node_id}: valor = {response.value}")


def ask(question: str, options: list[str]):
    while True:
        answer = input(question).strip()
        if answer in options:
            return answer

        print("opção inválida")


def ask_number(question: str):
    while True:
        answer = input(question).strip()
        try:
            return int(answer)
        except ValueError:
            print("digite um número inteiro")


def main():
    nodes = list(ADDRESSES)

    while True:
        print()
        print("========== CLIENTE ==========")
        print("1 - escrever")
        print("2 - ler")
        print("3 - escrever e derrubar o líder antes de replicar (cenário 4B)")
        print("0 - sair")
        option = ask("opção: ", ["0", "1", "2", "3"])

        if option in ("1", "3"):
            operation = ask("operação (set, add, sub): ", list(OPERATIONS))
            value = ask_number("valor: ")
            node_id = ask("enviar para qual nó (A a D)? ", nodes)
            write(node_id, operation, value, simulate_crash=(option == "3"))

        elif option == "2":
            node_id = ask("ler de qual nó (A a D, ou 0 para todos)? ", ["0"] + nodes)
            if node_id == "0":
                for each_id in nodes:
                    read(each_id)
            else:
                read(node_id)

        else:
            break


if __name__ == "__main__":
    main()