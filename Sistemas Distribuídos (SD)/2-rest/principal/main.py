import random
import uvicorn
import pika
import httpx
import json
import threading
import crypto
import messages
from fastapi import FastAPI, Request, Response, Depends
from contextlib import asynccontextmanager
from pydantic import BaseModel


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


def place_order(orders, channel):
    order_id = random.randint(1000, 9999)
    order = {"id": order_id, "products": orders}

    messages.publish(order, channel, MS_NAME, keyring, EXCHANGE, "pedido.criado")

    with orders_lock:
        orders_statuses[order_id] = {"status": "Pedido criado"}

    return order


def remove_order(order_id, channel):
    with orders_lock:
        orders = dict(orders_statuses)

    if not orders:
        return 404

    current = orders.get(order_id)

    if current is None:
        return 404

    status = current["status"]

    if status.startswith("Excluido"):
        return 410

    if status == "Enviado":
        return 409

    order = {"id": order_id}

    messages.publish(order, channel, MS_NAME, keyring, EXCHANGE, "pedido.excluido")

    with orders_lock:
        orders_statuses[order_id] = {"status": "Excluido (usuario)"}

    return 204


@asynccontextmanager
async def lifespan(app: FastAPI):
    pub_conn = pika.BlockingConnection(
        pika.ConnectionParameters("localhost", heartbeat=600)
    )

    pub_channel = pub_conn.channel()
    pub_channel.exchange_declare(exchange=EXCHANGE, exchange_type="direct")

    app.state.rabbit_channel = pub_channel

    thread = threading.Thread(target=consumer_worker, daemon=True)
    thread.start()

    yield

    pub_channel.close()
    pub_conn.close()


app = FastAPI(lifespan=lifespan)


class ProductOrder(BaseModel):
    id: int
    quantity: int


class Order(BaseModel):
    id: int


def get_channel(request: Request) -> pika.adapters.BlockingConnection:
    return request.app.state.rabbit_channel


@app.get("/produtos")
async def get_products():
    async with httpx.AsyncClient(timeout=5.0) as client:
        response = await client.get(
            "http://localhost:8001/produtos",
        )

        response.raise_for_status()

        return response.json()


@app.get("/pedidos")
def get_orders():
    # TODO: notificações SSE
    return orders_statuses


@app.post("/pedidos", status_code=201)
def create_order(order_list: list[ProductOrder], channel=Depends(get_channel)):
    orders = [order.model_dump() for order in order_list]
    created_order = place_order(orders, channel)
    return created_order


@app.delete("/pedidos")
def delete_order(order: Order, res: Response, channel=Depends(get_channel)):
    status = remove_order(order.id, channel)
    res.status_code = status
    return


@app.post("/promo")
def subscribePromotions(req):
    # TODO: pegar dados do body
    res = ...
    return res


@app.delete("/promo")
def unsubscribePromotions(req):
    # TODO: pegar dados do body
    res = ...
    return res


if __name__ == "__main__":
    uvicorn.run("principal.main:app", port=8000, reload=True)
