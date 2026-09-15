import json
import random
import time

import pika

import crypto
import messages

MS_NAME = "pagamento"
EXCHANGE = "eCommerce"
QUEUE = "fila.pagamento"

TAXA_APROVACAO = 0.7

keyring = crypto.get_keyring(MS_NAME)


def processar_pagamento(channel, pedido):
    pedido_id = pedido["id"]

    print(f"[pagamento] processando pedido {pedido_id}...")
    time.sleep(1)

    aprovado = random.random() < TAXA_APROVACAO
    payload = {
        "id": pedido_id,
    }

    if aprovado:
        print(f"[pagamento] pedido {pedido_id}: APROVADO")
        routing_key = "pagamento.aprovado"
    else:
        print(f"[pagamento] pedido {pedido_id}: RECUSADO")
        routing_key = "pagamento.recusado"

    messages.publish(payload, channel, MS_NAME, keyring, EXCHANGE, routing_key)


def main():
    conn = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
    channel = conn.channel()

    channel.exchange_declare(EXCHANGE, "direct")
    _ = channel.queue_declare(QUEUE)
    _ = channel.queue_bind(QUEUE, EXCHANGE, "pedido.estoque_ok")

    def callback(ch, method, properties, body):
        if not crypto.verify_message(keyring, properties, body):
            return

        processar_pagamento(ch, json.loads(body))

    channel.basic_consume(QUEUE, callback, auto_ack=True)

    try:
        print("[pagamento] iniciando...")
        channel.start_consuming()
    except KeyboardInterrupt:
        channel.stop_consuming()
        conn.close()


if __name__ == "__main__":
    main()
