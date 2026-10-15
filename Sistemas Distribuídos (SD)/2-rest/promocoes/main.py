import json
import random
import sys
import threading
import time
import traceback

import constants
import crypto
import infra

MS_NAME = "promocoes"
EXCHANGE = constants.EXCHANGE_ECOMMERCE
QUEUE = constants.queue_for(MS_NAME)
BINDINGS = ["interesse.promocao"]

INTERVALO = 10

keyring = crypto.get_keyring(MS_NAME)

with open("catalogo.json", "r", encoding="utf-8") as f:
    produtos = json.load(f)


# categoria -> emails[]
subscribers = {
    "A": set(),
    "B": set(),
    "C": set(),
}

subscribers_lock = threading.Lock()


def consumer_worker():
    channel = infra.connect_consumer(EXCHANGE, QUEUE, BINDINGS)

    def callback(_ch, method, properties, body):
        if not crypto.verify_message(keyring, properties, body):
            return

        try:
            # esperado: { email: str, categories: str[] }
            data = json.loads(body)

            with subscribers_lock:
                for category in data["categories"]:
                    if category in subscribers:
                        subscribers[category].add(data["email"])
        except Exception:
            print(f"[promocoes] erro ao processar {method.routing_key}")
            traceback.print_exc()

    channel.basic_consume(
        queue=QUEUE,
        on_message_callback=callback,
        auto_ack=True,
    )

    channel.start_consuming()


def gerar_promocao():
    produto = random.choice(produtos)
    desconto = random.choice([5, 10, 15, 20, 25, 30, 40, 50])
    preco_promocional = round(produto["preco"] * (1 - desconto / 100), 2)

    return {
        "produto_id": produto["id"],
        "produto": produto["nome"],
        "categoria": produto["categoria"],
        "preco_original": produto["preco"],
        "preco_promocional": preco_promocional,
        "desconto": desconto,
    }


# TODO: enviar emails via Resend
def enviar_emails(promo, subscribers):
    print(
        "Enviando emails da categoria " + promo["categoria"] + " para subs: ",
        subscribers,
    )


def main():
    thread = threading.Thread(target=consumer_worker, daemon=True)
    thread.start()

    try:
        while True:
            promocao = gerar_promocao()

            with subscribers_lock:
                subscribers_snapshot = list(subscribers[promocao["categoria"]])

            enviar_emails(promocao, subscribers_snapshot)

            time.sleep(INTERVALO)
    except KeyboardInterrupt:
        sys.exit()


if __name__ == "__main__":
    main()
