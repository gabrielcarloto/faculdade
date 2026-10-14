from fastapi.responses import HTMLResponse
import httpx
import uvicorn
from fastapi import FastAPI, Response
from pydantic import BaseModel

import constants

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
        "link": f"http://localhost:{constants.PORTS['mock_pagamento']}/pay/{payload.order_id}"
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
def process_payment(order_id: int, status: str = "APROVADO"):
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
    uvicorn.run(
        "mock_pagamento.main:app", port=constants.PORTS["mock_pagamento"], reload=True
    )
