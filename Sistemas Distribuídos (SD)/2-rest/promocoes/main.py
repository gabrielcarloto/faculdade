import json
import os
import random
import sys
import threading
import time
import traceback
from dotenv import load_dotenv

import httpx

import constants
import crypto
import infra

load_dotenv()

MS_NAME = "promocoes"
EXCHANGE = constants.EXCHANGE_ECOMMERCE
QUEUE = constants.queue_for(MS_NAME)
BINDINGS = ["interesse.promocao"]

INTERVALO = 60

RESEND_URL = "https://api.resend.com/emails"
RESEND_API_KEY = os.getenv("RESEND_API_KEY")
# sem domínio verificado no Resend, o remetente tem que ser onboarding@resend.dev,
# e ele só entrega no e-mail da própria conta do Resend
RESEND_FROM = os.environ.get(
    "RESEND_FROM", "Lojinha Distribuída <onboarding@resend.dev>"
)


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
            # esperado: { action: "subscribe" | "unsubscribe", email: str, categories: str[] }
            data = json.loads(body)
            email = data["email"]
            categories = data.get("categories") or []

            with subscribers_lock:
                if data.get("action") == "unsubscribe":
                    for category in categories or list(subscribers):
                        subscribers.get(category, set()).discard(email)
                else:
                    for category in categories:
                        if category in subscribers:
                            subscribers[category].add(email)

                inscritos = {c: sorted(emails) for c, emails in subscribers.items()}

            print(f"[promocoes] {data.get('action', 'subscribe')} {email}: {inscritos}")
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


def brl(valor):
    return "R$ " + f"{valor:,.2f}".replace(",", "_").replace(".", ",").replace("_", ".")


def montar_email(promo):
    assunto = f"{promo['desconto']}% de desconto: {promo['produto']}"
    html = (
        f"<h2>{promo['produto']} com {promo['desconto']}% de desconto</h2>"
        f"<p>De <s>{brl(promo['preco_original'])}</s> por "
        f"<strong>{brl(promo['preco_promocional'])}</strong>.</p>"
        f"<p>Você recebeu este e-mail porque se inscreveu nas promoções "
        f"da categoria {promo['categoria']}.</p>"
    )
    return assunto, html


def mensagem_de_erro(resposta):
    try:
        return resposta.json().get("message", resposta.text)
    except ValueError:
        return resposta.text


def enviar_emails(promo, destinatarios):
    if not destinatarios:
        print(f"[promocoes] promoção da categoria {promo['categoria']} sem inscritos")
        return

    assunto, html = montar_email(promo)

    if not RESEND_API_KEY:
        print(
            f"[promocoes] sem RESEND_API_KEY, não enviado para {destinatarios}: {assunto}"
        )
        return

    for email in destinatarios:
        try:
            resposta = httpx.post(
                RESEND_URL,
                headers={"Authorization": f"Bearer {RESEND_API_KEY}"},
                json={
                    "from": RESEND_FROM,
                    "to": [email],
                    "subject": assunto,
                    "html": html,
                },
                timeout=10,
            )

            if resposta.is_success:
                print(
                    f"[promocoes] e-mail enviado para {email} (id {resposta.json()['id']}): {assunto}"
                )
            else:
                print(
                    f"[promocoes] Resend recusou {email}: {resposta.status_code} {mensagem_de_erro(resposta)}"
                )
        except Exception as e:
            print(f"[promocoes] falha ao enviar para {email}: {e!r}")


def main():
    if RESEND_API_KEY:
        print(f"[promocoes] Resend configurado, remetente: {RESEND_FROM}")
    else:
        print(
            "[promocoes] RESEND_API_KEY não definida: as promoções só aparecem no terminal"
        )

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