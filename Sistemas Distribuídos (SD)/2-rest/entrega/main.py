import json
import time
import traceback

import constants
import crypto
import infra
import messages

MS_NAME = "entrega"
EXCHANGE = constants.EXCHANGE_ECOMMERCE
QUEUE = constants.queue_for(MS_NAME)

keyring = crypto.get_keyring(MS_NAME)


def tratar_pagamento_aprovado(channel, pedido):
    pedido_id = pedido["id"]

    print(f"[entrega] pedido {pedido_id}: PROCESSANDO")
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
    channel = infra.connect_consumer(EXCHANGE, QUEUE, ["pagamento.aprovado"])
    conn = channel.connection  # mantida para o close() no KeyboardInterrupt

    def callback(ch, method, properties, body):
        if not crypto.verify_message(keyring, properties, body):
            return

        try:
            tratar_pagamento_aprovado(ch, json.loads(body))
        except Exception:
            print(f"[entrega] erro ao processar {method.routing_key}")
            traceback.print_exc()

    channel.basic_consume(QUEUE, callback, auto_ack=True)

    try:
        print("[entrega] iniciando...")
        channel.start_consuming()
    except KeyboardInterrupt:
        channel.stop_consuming()
        conn.close()


if __name__ == "__main__":
    main()
