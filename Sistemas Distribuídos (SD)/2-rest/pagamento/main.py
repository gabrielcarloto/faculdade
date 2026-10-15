import json
import threading
import traceback

from fastapi import FastAPI, Request, Response, Depends
from contextlib import asynccontextmanager
import httpx
import pika
import pika.adapters.blocking_connection
from pydantic import BaseModel

import constants
import crypto
import infra
import messages

MS_NAME = "pagamento"
EXCHANGE = constants.EXCHANGE_ECOMMERCE
QUEUE = constants.queue_for(MS_NAME)

keyring = crypto.get_keyring(MS_NAME)

pending = set()
pending_lock = threading.Lock()


def consumer_worker():
    channel = infra.connect_consumer(EXCHANGE, QUEUE, ["pedido.estoque_ok"])

    def callback(ch, method, properties, body):
        if not crypto.verify_message(keyring, properties, body):
            return

        try:
            data = json.loads(body)
            order_id = data["id"]

            with pending_lock:
                pending.add(order_id)

            res = httpx.post(
                f"{constants.base_url_for('mock_pagamento')}/generate_payment_link",
                json={
                    "order_id": order_id,
                    "callback": f"{constants.base_url_for('pagamento')}/pagamento/{order_id}",
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
        except Exception:
            print(f"[pagamento] erro ao processar {method.routing_key}")
            traceback.print_exc()

    channel.basic_consume(QUEUE, callback, auto_ack=True)
    channel.start_consuming()


@asynccontextmanager
async def lifespan(app: FastAPI):
    pub_conn = pika.BlockingConnection(
        pika.ConnectionParameters(constants.RABBIT_HOST, heartbeat=600)
    )

    pub_channel = pub_conn.channel()
    pub_channel.exchange_declare(exchange=EXCHANGE, exchange_type="direct")

    app.state.rabbit_channel = pub_channel

    thread = threading.Thread(target=consumer_worker, daemon=True)
    thread.start()

    yield

    pub_channel.close()
    pub_conn.close()


def get_channel(request: Request) -> pika.adapters.blocking_connection.BlockingChannel:
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
    infra.run("pagamento.main:app", MS_NAME)
