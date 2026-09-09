import random
import sys

import pika
import json
import threading

# TODO:
# - [ ] Interface
#   - [ ] Visualizar produtos
#   - [ ] Realizar pedidos
#   - [ ] Consultar pedidos e status
#   - [ ] Excluir pedidos
# - [ ] Publicação
#   - [ ] `pedido.criado` com ID do pedido, produtos, quantidades e demais infos
#   - [ ] `pedido.excluido` com ID
# - [ ] Consumo
#   - [ ] `pagamento.aprovado`
#   - [ ] `pagamento.recusado`
#   - [ ] `pedido.enviado`
#   - [ ] `pedido.estoque_ok`
#   - [ ] `estoque.indisponivel`
# - [ ] Assinaturas
# - [ ] Catálogo (products.json)

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
        match method.routing_key:
            case "pedido.estoque_ok":
                ...
            case "estoque.indisponivel":
                ...
            case "pagamento.aprovado":
                ...
            case "pagamento.recusado":
                ...
            case "pedido.enviado":
                ...


def print_products_list():
    print("=========================")
    for product in products_list:
        print("ID: ", product["id"])
        print("Nome: ", product["nome"])
        print("Categoria: ", product["categoria"])
        print("Preco: ", product["preco"])
        print("=========================")


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
        orders_statuses[order_id] = {"status": "created"}


def print_statuses(): ...


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
