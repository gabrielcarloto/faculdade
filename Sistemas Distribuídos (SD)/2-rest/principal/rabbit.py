import json
import traceback

import constants
import crypto
import infra
import messages
from principal import state


MS_NAME = "principal"
EXCHANGE = constants.EXCHANGE_ECOMMERCE
QUEUE = constants.queue_for(MS_NAME)
BINDINGS = [
    "pagamento.pendente",
    "pagamento.aprovado",
    "pagamento.recusado",
    "pedido.enviado",
    "pedido.estoque_ok",
    "estoque.indisponivel",
]

keyring = crypto.get_keyring(MS_NAME)


def consumer_worker():
    channel = infra.connect_consumer(EXCHANGE, QUEUE, BINDINGS)

    def callback(ch, method, properties, body):
        if not crypto.verify_message(keyring, properties, body):
            return

        try:
            data = json.loads(body)
            order_id = data["id"]

            with state.orders_lock:
                existing_order = state.orders_statuses.get(order_id)
                if (
                    existing_order is not None
                    and "excluido" in existing_order["status"].lower()
                ):
                    return

                match method.routing_key:
                    case "pedido.estoque_ok":
                        state.orders_statuses[order_id] = {"status": "Estoque disponível"}
                    case "pagamento.pendente":
                        print("link de pagamento: ", data["link"])
                        state.orders_statuses[order_id] = {
                            "status": "Pagamento pendente",
                            "payment_link": data["link"],
                        }
                    case "pagamento.aprovado":
                        state.orders_statuses[order_id] = {"status": "Pagamento aprovado"}
                    case "pedido.enviado":
                        state.orders_statuses[order_id] = {"status": "Enviado"}
                    case "estoque.indisponivel" | "pagamento.recusado":
                        state.orders_statuses[order_id] = {
                            "status": f"Excluido ({method.routing_key})"
                        }

            if method.routing_key in ("estoque.indisponivel", "pagamento.recusado"):
                order = {"id": order_id}
                messages.publish(
                    order, channel, MS_NAME, keyring, EXCHANGE, "pedido.excluido"
                )
        except Exception:
            print(f"[principal] erro ao processar {method.routing_key}")
            traceback.print_exc()

    channel.basic_consume(
        queue=QUEUE,
        on_message_callback=callback,
        auto_ack=True,
    )

    channel.start_consuming()
