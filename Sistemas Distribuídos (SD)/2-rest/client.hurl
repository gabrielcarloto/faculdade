# Visualizar produtos
GET http://localhost:8000/produtos
HTTP/1.1 200

# Criar pedido
POST http://localhost:8000/pedidos
[{ "id": 1, "quantity": 1 }, { "id": 2, "quantity": 1 }]

HTTP 201
[Captures]
order_id: jsonpath "$.id"

# Remover pedido
DELETE http://localhost:8000/pedidos
{ "id": {{order_id}} }