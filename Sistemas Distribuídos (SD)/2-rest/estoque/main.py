import json
import os
import threading
import traceback
from fastapi import FastAPI
from contextlib import asynccontextmanager

import constants
import crypto
import infra
import messages

MS_NAME = "estoque"
EXCHANGE = constants.EXCHANGE_ECOMMERCE
QUEUE = constants.queue_for(MS_NAME)

keyring = crypto.get_keyring(MS_NAME)

ARQUIVO_ESTADO = "estoque/estoque.json"

with open("catalogo.json", "r", encoding="utf-8") as f:
    catalogo = {p["id"]: p for p in json.load(f)}


def carregar_estado():
    try:
        with open(ARQUIVO_ESTADO, "r", encoding="utf-8") as f:
            estado = json.load(f)
    except FileNotFoundError:
        return {pid: p["estoque"] for pid, p in catalogo.items()}, {}

    disponivel = {int(pid): qtd for pid, qtd in estado["disponivel"].items()}
    reservas = {
        int(pedido_id): {int(pid): qtd for pid, qtd in itens.items()}
        for pedido_id, itens in estado["reservas"].items()
    }

    for pid, p in catalogo.items():
        disponivel.setdefault(pid, p["estoque"])

    return disponivel, reservas


def salvar_estado():
    temporario = ARQUIVO_ESTADO + ".tmp"

    with open(temporario, "w", encoding="utf-8") as f:
        json.dump({"disponivel": disponivel, "reservas": reservas}, f, indent=2)
        f.flush()
        os.fsync(f.fileno())

    os.replace(temporario, ARQUIVO_ESTADO)


disponivel, reservas = carregar_estado()
salvar_estado()


def imprimir_estoque():
    print("-" * 46)
    for pid, p in catalogo.items():
        print(f" {pid} | {p['nome'][:30]:<30} | {disponivel[pid]:>3} un.")
    print("-" * 46)


def agrupar_itens(itens):
    agrupado: dict[int, int] = {}
    for item in itens:
        pid = item["id"]
        agrupado[pid] = agrupado.get(pid, 0) + item["quantity"]
    return agrupado


def tratar_pedido_criado(channel, pedido):
    pedido_id = pedido["id"]
    solicitado = agrupar_itens(pedido.get("products", []))

    indisponiveis = [
        catalogo[pid]["nome"] if pid in catalogo else f"produto {pid}"
        for pid, qtd in solicitado.items()
        if qtd <= 0 or disponivel.get(pid, 0) < qtd
    ]

    if not solicitado or indisponiveis:
        print(f"[estoque] pedido {pedido_id}: INDISPONÍVEL")

        return messages.publish(
            {"id": pedido_id, "produtos_indisponiveis": indisponiveis},
            channel,
            MS_NAME,
            keyring,
            EXCHANGE,
            "estoque.indisponivel",
        )

    for pid, qtd in solicitado.items():
        disponivel[pid] -= qtd

    reservas[pedido_id] = solicitado

    salvar_estado()

    # total = sum(catalogo[pid]["preco"] * qtd for pid, qtd in solicitado.items())
    # produtos = [{"id": pid, "quantity": qtd} for pid, qtd in solicitado.items()]

    print(f"[estoque] pedido {pedido_id}: reservado")
    imprimir_estoque()

    messages.publish(
        {
            "id": pedido_id,
        },
        channel,
        MS_NAME,
        keyring,
        EXCHANGE,
        "pedido.estoque_ok",
    )


def tratar_pedido_excluido(pedido):
    pedido_id = pedido["id"]
    reserva = reservas.pop(pedido_id, None)

    if reserva is None:
        return

    for pid, qtd in reserva.items():
        disponivel[pid] += qtd

    salvar_estado()

    print(f"[estoque] pedido {pedido_id}: itens devolvidos")
    imprimir_estoque()


def consumer_worker():
    channel = infra.connect_consumer(
        EXCHANGE, QUEUE, ["pedido.criado", "pedido.excluido"]
    )

    def callback(ch, method, properties, body):
        if not crypto.verify_message(keyring, properties, body):
            return

        try:
            pedido = json.loads(body)

            if method.routing_key == "pedido.criado":
                tratar_pedido_criado(ch, pedido)
            else:
                tratar_pedido_excluido(pedido)
        except Exception:
            print(f"[estoque] erro ao processar {method.routing_key}")
            traceback.print_exc()

    channel.basic_consume(QUEUE, callback, auto_ack=True)

    imprimir_estoque()

    channel.start_consuming()


@asynccontextmanager
async def lifespan(app: FastAPI):
    thread = threading.Thread(target=consumer_worker, daemon=True)
    thread.start()
    yield


app = FastAPI(lifespan=lifespan)


@app.get("/produtos")
def get_products():
    produtos = [
        p | {"estoque": disponivel[pid]}
        for pid, p in catalogo.items()
        if pid in disponivel
    ]

    return produtos


if __name__ == "__main__":
    infra.run("estoque.main:app", MS_NAME)
