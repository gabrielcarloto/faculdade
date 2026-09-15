import json
import random
import pika

MS_NAME = "promocoes"
EXCHANGE = "Promocoes"
INTERVALO = 5

with open("catalogo.json", "r", encoding="utf-8") as f:
    produtos = json.load(f)


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


def main():
    conn = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
    channel = conn.channel()

    channel.exchange_declare(EXCHANGE, "topic")

    try:
        while True:
            promocao = gerar_promocao()
            routing_key = f"promocao.categoria.{promocao['categoria']}"

            channel.basic_publish(EXCHANGE, routing_key, json.dumps(promocao))

            print(
                f"[promocoes] {routing_key} | {promocao['produto']} "
                f"-{promocao['desconto']}% -> R$ {promocao['preco_promocional']:.2f}"
            )

            conn.sleep(INTERVALO)
    except KeyboardInterrupt:
        conn.close()


if __name__ == "__main__":
    main()
