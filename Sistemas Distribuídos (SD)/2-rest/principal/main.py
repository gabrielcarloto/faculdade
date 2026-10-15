import random
import pika
import pika.adapters.blocking_connection
import httpx
import threading
import constants
import infra
import messages
from principal import rabbit, state
from fastapi import FastAPI, Request, Response, Depends
from contextlib import asynccontextmanager
from pydantic import BaseModel


def place_order(orders, channel):
    order_id = random.randint(1000, 9999)
    order = {"id": order_id, "products": orders}

    messages.publish(
        order,
        channel,
        rabbit.MS_NAME,
        rabbit.keyring,
        rabbit.EXCHANGE,
        "pedido.criado",
    )

    with state.orders_lock:
        state.orders_statuses[order_id] = {"status": "Pedido criado"}

    return order


def remove_order(order_id, channel):
    with state.orders_lock:
        orders = dict(state.orders_statuses)

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

    messages.publish(
        order,
        channel,
        rabbit.MS_NAME,
        rabbit.keyring,
        rabbit.EXCHANGE,
        "pedido.excluido",
    )

    with state.orders_lock:
        state.orders_statuses[order_id] = {"status": "Excluido (usuario)"}

    return 204


@asynccontextmanager
async def lifespan(app: FastAPI):
    pub_conn = pika.BlockingConnection(
        pika.ConnectionParameters(constants.RABBIT_HOST, heartbeat=600)
    )

    pub_channel = pub_conn.channel()
    pub_channel.exchange_declare(exchange=rabbit.EXCHANGE, exchange_type="direct")

    app.state.rabbit_channel = pub_channel

    thread = threading.Thread(target=rabbit.consumer_worker, daemon=True)
    thread.start()

    yield

    pub_channel.close()
    pub_conn.close()


app = FastAPI(lifespan=lifespan)


class ProductOrder(BaseModel):
    id: int
    quantity: int


class PromotionSubscription(BaseModel):
    email: str
    categories: list[str]


def get_channel(request: Request) -> pika.adapters.blocking_connection.BlockingChannel:
    return request.app.state.rabbit_channel


@app.get("/produtos")
def get_products():
    response = httpx.get(
        f"{constants.base_url_for('estoque')}/produtos",
    )

    response.raise_for_status()

    return response.json()


@app.get("/pedidos")
def get_orders():
    # TODO: notificações SSE
    return state.orders_statuses


@app.post("/pedidos", status_code=201)
def create_order(order_list: list[ProductOrder], channel=Depends(get_channel)):
    orders = [order.model_dump() for order in order_list]
    created_order = place_order(orders, channel)
    return created_order


@app.delete("/pedidos/{id}")
def delete_order(id: int, res: Response, channel=Depends(get_channel)):
    status = remove_order(id, channel)
    res.status_code = status
    return


@app.post("/promo")
def subscribePromotions(
    promo: PromotionSubscription, res: Response, channel=Depends(get_channel)
):
    messages.publish(
        promo.model_dump(),
        channel,
        rabbit.MS_NAME,
        rabbit.keyring,
        rabbit.EXCHANGE,
        "interesse.promocao",
    )

    res.status_code = 201
    return


# @app.delete("/promo")
# def unsubscribePromotions(req):
#     # TODO: pegar dados do body e implementar ms promo
#     res = ...
#     return res


if __name__ == "__main__":
    infra.run("principal.main:app", rabbit.MS_NAME)
