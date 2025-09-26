# Transferência de Arquivos via UDP

Para rodar o projeto, você precisa do node.js e npm. Para ver as opções disponíveis, execute:

```sh
# Instala as dependências
npm install

npx tsx cliente.ts --help
#       ^ ou server.ts
```

Os arquivos que o servidor transmitirá devem ficar em `assets/`. Arquivos resultantes serão adicionados em `out/`.

Execução básica:

```sh
# Servidor na porta 3000
npx tsx server.ts --packet-loss-rate 0

# Cliente conectado ao servidor pede o pao.jng
npx tsx client.ts --port 3000 --file pao.jpg -o cliente-pao.jpg
```

Os dois comandos devem ser executados em terminais diferentes.
