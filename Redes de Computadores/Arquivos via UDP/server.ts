import fs from 'node:fs/promises';

import { SaferUDP } from './safer-udp/index.js';
import { logger as pinoLogger } from './logger.js';

const PORT = 3000;

const logger = pinoLogger.child({ category: 'Server' });

const server = new SaferUDP(async (message) => {
  const string = message.buffer.toString();

  const [method, path] = string.split(' ');

  if (method !== 'GET') {
    await message.connection.send(Buffer.from('ERRO!! Método inválido'));
    return await message.connection.close();
  }

  const files = await fs.readdir('./assets/');

  if (!path || !files.includes(path)) {
    await message.connection.send(Buffer.from('ERRO!! Arquivo não existe'));
    return await message.connection.close();
  }

  const file = await fs.readFile('./assets/' + path);

  logger.info(`Enviando arquivo '${path}'...`);

  await message.connection.send(file);
  await message.connection.close();
});

server.listen(PORT);
