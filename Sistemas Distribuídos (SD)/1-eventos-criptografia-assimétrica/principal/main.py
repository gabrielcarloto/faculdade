import random
import sys

import pika
import json
import threading

# TODO:
# - [/] Interface
#   - [x] Visualizar produtos
#   - [x] Realizar pedidos
#   - [x] Consultar pedidos e status
#   - [?] Excluir pedidos
# - [x] Publicação
#   - [x] `pedido.criado` com ID do pedido, produtos, quantidades e demais infos
#   - [x] `pedido.excluido` com ID
# - [x] Consumo
#   - [x] `pagamento.aprovado`
#   - [x] `pagamento.recusado`
#   - [x] `pedido.enviado`
#   - [x] `pedido.estoque_ok`
#   - [x] `estoque.indisponivel`
# - [ ] Assinaturas
# - [x] Catálogo (products.json)

orders_lock = threading.Lock()
orders_statuses = {}

with open("catalogo.json", "r", encoding="utf-8") as f:
    products_list = json.load(f)


def consumer_worker():
    consumer_conn = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
    channel = consumer_conn.channel()

    channel.exchange_declare("eCommerce", "direct")
    channel.queue_declare("fila.principal")

    channel.queue_bind("fila.principal", "eCommerce", "pagamento.aprovado")
    channel.queue_bind("fila.principal", "eCommerce", "pagamento.recusado")
    channel.queue_bind("fila.principal", "eCommerce", "pedido.enviado")
    channel.queue_bind("fila.principal", "eCommerce", "pedido.estoque_ok")
    channel.queue_bind("fila.principal", "eCommerce", "estoque.indisponivel")

    def callback(ch, method, properties, body):
        data = json.loads(body)
        order_id = data["id"]

        match method.routing_key:
            case "pedido.estoque_ok":
                with orders_lock:
                    orders_statuses[order_id] = {"status": "Estoque disponível"}
            case "pagamento.aprovado":
                with orders_lock:
                    orders_statuses[order_id] = {"status": "Pagamento aprovado"}
            case "pedido.enviado":
                with orders_lock:
                    orders_statuses[order_id] = {"status": "Enviado"}
            case "estoque.indisponivel" | "pagamento.recusado":
                order = {
                    "id": order_id,
                }

                channel.basic_publish(
                    "eCommerce",
                    "pedido.excluido",
                    json.dumps(order).encode("utf-8"),
                )

                with orders_lock:
                    orders_statuses[order_id] = {
                        "status": f"Excluido ({method.routing_key})"
                    }

    channel.basic_consume(
        queue="fila.principal",
        on_message_callback=callback,
        auto_ack=True,
    )

    channel.start_consuming()


def print_products_list():
    print("=" * 40)
    for product in products_list:
        print("ID: ", product["id"])
        print("Nome: ", product["nome"])
        print("Categoria: ", product["categoria"])
        print("Preco: ", product["preco"])
        print("=" * 40)


def place_order(channel):
    orders = []

    while True:
        print("\n(1) Adicionar produto")
        print("(2) Finalizar pedido")
        opt = input("\nDigite sua opção: ")

        if opt == "1":
            prod_id = int(input("Insira o ID do produto: "))
            exists = any(p["id"] == prod_id for p in products_list)

            if not exists:
                print("Produto inexistente :(")
                continue

            quantity = int(input("Quantidade: "))
            orders.append({"id": prod_id, "quantity": quantity})
        if opt == "2":
            break

    order_id = random.randint(1000, 9999)
    order = {"id": order_id, "products": orders}

    channel.basic_publish(
        "eCommerce",
        "pedido.criado",
        json.dumps(order).encode("utf-8"),
    )

    with orders_lock:
        orders_statuses[order_id] = {"status": "Pedido criado"}


def print_statuses():
    print("=" * 40)
    for order, data in orders_statuses.items():
        print(order, " | ", data["status"])
    print("=" * 40)


def main():
    consumer_thread = threading.Thread(target=consumer_worker, daemon=True)
    consumer_thread.start()

    pub_conn = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
    pub_channel = pub_conn.channel()
    pub_channel.exchange_declare(exchange="eCommerce", exchange_type="direct")

    print_products_list()
    while True:
        print("\n(1) Criar pedido")
        print("(2) Acompanhar pedidos")
        print("(3) Sair")
        opt = input("\nDigite sua opção: ")

        if opt == "1":
            place_order(pub_channel)
        if opt == "2":
            print_statuses()
        if opt == "3":
            sys.exit(0)


if __name__ == "__main__":
    main()
