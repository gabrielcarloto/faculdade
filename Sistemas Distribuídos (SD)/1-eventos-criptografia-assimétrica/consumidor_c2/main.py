import json
import pika

MS_NAME = "consumidor_c2"
EXCHANGE = "Promocoes"
QUEUE = "fila.C2"

BINDINGS = ["promocao.categoria.*"]


def main():
    conn = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
    channel = conn.channel()

    channel.exchange_declare(EXCHANGE, "topic")
    _ = channel.queue_declare(QUEUE)

    for routing_key in BINDINGS:
        _ = channel.queue_bind(QUEUE, EXCHANGE, routing_key)

    def callback(ch, method, properties, body):
        p = json.loads(body)
        print(
            f"[C2] {method.routing_key} | {p['produto']} "
            f"(cat. {p['categoria']}): R$ {p['preco_original']:.2f} -> "
            f"R$ {p['preco_promocional']:.2f} (-{p['desconto']}%)"
        )

    channel.basic_consume(QUEUE, callback, auto_ack=True)

    try:
        print("[C2] iniciando...")
        channel.start_consuming()
    except KeyboardInterrupt:
        channel.stop_consuming()
        conn.close()


if __name__ == "__main__":
    main()
