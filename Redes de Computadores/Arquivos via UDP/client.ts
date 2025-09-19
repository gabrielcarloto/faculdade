import fs from 'node:fs/promises';
import { SaferUDP } from './safer-udp/index.js';

import { logger as pinoLogger } from './logger.js';

const logger = pinoLogger.child({ category: 'Cliente' });

const client = new SaferUDP((message) => {
  console.log('Recebi a mensagem!');
  console.log(message.buffer.toString());
});

const remote = { address: 'localhost', port: 3000 };
const connection = client.connect(remote);

await connection.send(
  Buffer.from('Mensagem interessante e pequena para teste'),
);

const bread = await fs.readFile('./assets/pao.jpg');

logger.info(`Enviando buffer de ${bread.length} bytes...`);

await connection.send(bread);

await connection.send(
  Buffer.from('Mais uma mensagem interessante e pequena para teste'),
);
