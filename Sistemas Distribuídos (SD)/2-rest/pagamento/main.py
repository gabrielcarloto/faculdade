import json
import random
import threading
import time
from typing import Literal

from fastapi import FastAPI, Request, Response, Depends
from contextlib import asynccontextmanager
import httpx
import pika
from pydantic import BaseModel
import uvicorn

import constants
import crypto
import messages

MS_NAME = "pagamento"
EXCHANGE = "eCommerce"
QUEUE = "fila.pagamento"

TAXA_APROVACAO = 0.7

keyring = crypto.get_keyring(MS_NAME)

pending = set()
pending_lock = threading.Lock()


def consumer_worker():
    conn = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
    channel = conn.channel()

    channel.exchange_declare(EXCHANGE, "direct")
    _ = channel.queue_declare(QUEUE)
    _ = channel.queue_bind(QUEUE, EXCHANGE, "pedido.estoque_ok")

    def callback(ch, _method, properties, body):
        if not crypto.verify_message(keyring, properties, body):
            return

        data = json.loads(body)
        order_id = data["id"]

        with pending_lock:
            pending.add(order_id)

        res = httpx.post(
            f"http://localhost:{constants.PORTS['mock_pagamento']}/generate_payment_link",
            json={
                "order_id": order_id,
                "callback": f"http://localhost:{constants.PORTS['pagamento']}/pagamento/{order_id}",
            },
            timeout=5.0,
        )

        data = res.json()
        link = data["link"]

        messages.publish(
            {"id": order_id, "link": link},
            channel,
            MS_NAME,
            keyring,
            EXCHANGE,
            "pagamento.pendente",
        )

    channel.basic_consume(QUEUE, callback, auto_ack=True)
    channel.start_consuming()


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


def get_channel(request: Request) -> pika.adapters.BlockingConnection:
    return request.app.state.rabbit_channel


app = FastAPI(lifespan=lifespan)


class WebhookPayload(BaseModel):
    status: str


@app.post("/pagamento/{id}", status_code=200)
def update_payment(
    id: int, payload: WebhookPayload, res: Response, channel=Depends(get_channel)
):
    with pending_lock:
        if id not in pending:
            res.status_code = 404
            return

        pending.remove(id)

    routing_key = (
        "pagamento.aprovado" if payload.status == "APROVADO" else "pagamento.recusado"
    )

    messages.publish({"id": id}, channel, MS_NAME, keyring, EXCHANGE, routing_key)


if __name__ == "__main__":
    uvicorn.run("pagamento.main:app", port=constants.PORTS["pagamento"], reload=True)
