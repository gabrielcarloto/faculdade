import json
import random
import time

import pika

import crypto
import messages

MS_NAME = "entrega"
EXCHANGE = "eCommerce"
QUEUE = "fila.entrega"

keyring = crypto.get_keyring(MS_NAME)


def tratar_pagamento_aprovado(channel, pedido):
    pedido_id = pedido["id"]

    time.sleep(1)

    messages.publish(
        {"id": pedido_id},
        channel,
        MS_NAME,
        keyring,
        EXCHANGE,
        "pedido.enviado",
    )

    print(f"[entrega] pedido {pedido_id}: ENVIADO")


def main():
    conn = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
    channel = conn.channel()

    channel.exchange_declare(EXCHANGE, "direct")
    _ = channel.queue_declare(QUEUE)
    _ = channel.queue_bind(QUEUE, EXCHANGE, "pagamento.aprovado")

    def callback(ch, method, properties, body):
        if not crypto.verify_message(keyring, properties, body):
            return

        tratar_pagamento_aprovado(ch, json.loads(body))

    channel.basic_consume(QUEUE, callback, auto_ack=True)

    try:
        print("[entrega] iniciando...")
        channel.start_consuming()
    except KeyboardInterrupt:
        channel.stop_consuming()
        conn.close()


if __name__ == "__main__":
    main()
