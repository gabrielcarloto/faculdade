PORTS = {"principal": 8000, "estoque": 8001, "pagamento": 8002, "mock_pagamento": 8003}

EXCHANGE_ECOMMERCE = "eCommerce"
RABBIT_HOST = "localhost"


def base_url_for(ms_name):
    return f"http://localhost:{PORTS.get(ms_name)}"


def queue_for(ms_name: str) -> str:
    return f"fila.{ms_name}"
