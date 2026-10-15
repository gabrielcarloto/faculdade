import pika
from pika.adapters.blocking_connection import BlockingChannel
import uvicorn

import constants


def connect_consumer(
    exchange: str,
    queue: str,
    bindings: list[str],
    host: str = constants.RABBIT_HOST,
    **params,
) -> BlockingChannel:
    conn = pika.BlockingConnection(pika.ConnectionParameters(host, **params))
    channel = conn.channel()

    channel.exchange_declare(exchange, "direct")
    _ = channel.queue_declare(queue)

    for routing_key in bindings:
        _ = channel.queue_bind(queue, exchange, routing_key)

    return channel


def run(module_path: str, ms_name: str) -> None:
    uvicorn.run(module_path, port=constants.PORTS[ms_name], reload=True)
