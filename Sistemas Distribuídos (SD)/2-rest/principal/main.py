import random
import uvicorn
import sys
import pika
import json
import threading
import crypto
import messages
from fastapi import FastAPI
from contextlib import asynccontextmanager


MS_NAME = "principal"
EXCHANGE = "eCommerce"
QUEUE = "fila.principal"
BINDINGS = [
    "pagamento.aprovado",
    "pagamento.recusado",
    "pedido.enviado",
    "pedido.estoque_ok",
    "estoque.indisponivel",
]

orders_lock = threading.Lock()
orders_statuses = {}

keyring = crypto.get_keyring(MS_NAME)

with open("catalogo.json", "r", encoding="utf-8") as f:
    products_list = json.load(f)


def consumer_worker():
    consumer_conn = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
    channel = consumer_conn.channel()

    channel.exchange_declare(EXCHANGE, "direct")
    _ = channel.queue_declare(QUEUE)

    for routing_key in BINDINGS:
        _ = channel.queue_bind(QUEUE, EXCHANGE, routing_key)

    def callback(ch, method, properties, body):
        # NOTE: remover comentário depois :)
        # if not crypto.verify_message(keyring, properties, body):
        #     return

        data = json.loads(body)
        order_id = data["id"]

        with orders_lock:
            existing_order = orders_statuses.get(order_id)
            if (
                existing_order is not None
                and "excluido" in existing_order["status"].lower()
            ):
                return

            match method.routing_key:
                case "pedido.estoque_ok":
                    orders_statuses[order_id] = {"status": "Estoque disponível"}
                case "pagamento.aprovado":
                    orders_statuses[order_id] = {"status": "Pagamento aprovado"}
                case "pedido.enviado":
                    orders_statuses[order_id] = {"status": "Enviado"}
                case "estoque.indisponivel" | "pagamento.recusado":
                    orders_statuses[order_id] = {
                        "status": f"Excluido ({method.routing_key})"
                    }

        if method.routing_key in ("estoque.indisponivel", "pagamento.recusado"):
            order = {"id": order_id}
            messages.publish(
                order, channel, MS_NAME, keyring, EXCHANGE, "pedido.excluido"
            )

    channel.basic_consume(
        queue=QUEUE,
        on_message_callback=callback,
        auto_ack=True,
    )

    channel.start_consuming()


def print_products_list():
    print("=" * 40)
    for product in products_list:
        print("ID: ", product["id"])
        print("Nome: ", product["nome"])
        print("Categoria: ", product["categoria"])
        print("Preco: ", product["preco"])
        print("=" * 40)


def place_order(channel):
    orders = []

    while True:
        print("\n(1) Adicionar produto")
        print("(2) Finalizar pedido")
        opt = input("\nDigite sua opção: ")

        if opt == "1":
            prod_id = int(input("Insira o ID do produto: "))
            exists = any(p["id"] == prod_id for p in products_list)

            if not exists:
                print("Produto inexistente :(")
                continue

            quantity = int(input("Quantidade: "))
            orders.append({"id": prod_id, "quantity": quantity})
        if opt == "2":
            break

    order_id = random.randint(1000, 9999)
    order = {"id": order_id, "products": orders}

    messages.publish(order, channel, MS_NAME, keyring, EXCHANGE, "pedido.criado")

    with orders_lock:
        orders_statuses[order_id] = {"status": "Pedido criado"}


def delete_order(channel):
    with orders_lock:
        orders = dict(orders_statuses)

    if not orders:
        print("Nenhum pedido para excluir.")
        return

    print_statuses()

    try:
        order_id = int(input("Insira o ID do pedido a excluir: "))
    except ValueError:
        print("ID inválido :(")
        return

    current = orders.get(order_id)

    if current is None:
        print("Pedido inexistente :(")
        return

    status = current["status"]

    if status == "Enviado" or status.startswith("Excluido"):
        print(f"Pedido {order_id} ({status}) não pode ser excluído.")
        return

    order = {"id": order_id}

    messages.publish(order, channel, MS_NAME, keyring, EXCHANGE, "pedido.excluido")

    with orders_lock:
        orders_statuses[order_id] = {"status": "Excluido (usuario)"}

    print(f"Pedido {order_id} excluído.")


def print_statuses():
    print("=" * 40)
    for order, data in orders_statuses.items():
        print(order, " | ", data["status"])
    print("=" * 40)


@asynccontextmanager
async def lifespan(app: FastAPI):
    thread = threading.Thread(target=consumer_worker, daemon=True)
    thread.start()
    yield


app = FastAPI(lifespan=lifespan)


@app.get("/")
def read_root():
    return {"Hello": "World"}


@app.get("/pedidos")
def get_orders():
    return orders_statuses


if __name__ == "__main__":
    uvicorn.run("principal.main:app", port=8000, reload=True)
