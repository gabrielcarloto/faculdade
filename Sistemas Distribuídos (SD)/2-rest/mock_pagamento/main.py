from fastapi.responses import HTMLResponse
import httpx
from fastapi import FastAPI, Request, Response
from pydantic import BaseModel
from urllib.parse import parse_qs

import constants
import infra

payments = {}

app = FastAPI()


class GeneratePaymentPayload(BaseModel):
    order_id: int
    callback: str


@app.post("/generate_payment_link", status_code=201)
def generate_payment_link(payload: GeneratePaymentPayload, res: Response):
    if (
        payload.order_id in payments
        and payments[payload.order_id]["status"] != "PENDENTE"
    ):
        res.status_code = 409
        return

    payments[payload.order_id] = {}
    payments[payload.order_id]["status"] = "PENDENTE"
    payments[payload.order_id]["callback"] = payload.callback

    return {
        "link": f"{constants.base_url_for('mock_pagamento')}/pay/{payload.order_id}"
    }


@app.get("/pay/{order_id}", response_class=HTMLResponse)
def payment_page(order_id: int):
    html_content = f"""
    <html>
        <head><title>Mock Pagamento - Pedido {order_id}</title></head>
        <body>
            <h1>Pagamento do Pedido #{order_id}</h1>
            <p>Escolha o status do pagamento:</p>
            <form action="/pay/{order_id}/processar" method="POST">
                <button type="submit" name="status" value="APROVADO">Aprovar</button>
                <button type="submit" name="status" value="RECUSADO">Recusar</button>
            </form>
        </body>
    </html>
    """

    return HTMLResponse(content=html_content, status_code=200)


@app.post("/pay/{order_id}/processar", response_class=HTMLResponse)
async def process_payment(order_id: int, request: Request):
    form = parse_qs((await request.body()).decode())
    status = form.get("status", ["APROVADO"])[0]
    status = "APROVADO" if status == "APROVADO" else "RECUSADO"
    callback_url = payments[order_id]["callback"]
    payments[order_id]["status"] = status

    try:
        response = httpx.post(callback_url, json={"status": status}, timeout=5.0)
        response.raise_for_status()
    except Exception as e:
        print(f"Erro ao enviar webhook: {e}")

    html_content = f"""
    <html>
        <head><title>Mock Pagamento - Concluído</title></head>
        <body>
            <h1>Pagamento processado com sucesso!</h1>
            <p>Status enviado: <strong>{status}</strong></p>
            <p>Pode fechar esta aba.</p>
        </body>
    </html>
    """

    return HTMLResponse(content=html_content, status_code=200)


if __name__ == "__main__":
    infra.run("mock_pagamento.main:app", "mock_pagamento")
