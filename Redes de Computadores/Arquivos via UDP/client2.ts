import fs from 'node:fs/promises';
import { SaferUDP } from './safer-udp/index.js';

import { logger as pinoLogger } from './logger.js';

const logger = pinoLogger.child({ category: 'Cliente' });

const file = 'tsconfig.json';

const client = new SaferUDP(async (message) => {
  const string = message.buffer.toString();

  if (string.startsWith('ERRO')) {
    return logger.fatal('Erro: ' + string);
  }

  logger.info('Arquivo recebido!! ');
  await fs.writeFile('./out/tsconfig.json', message.buffer);
});

const remote = { address: 'localhost', port: 3000 };
const connection = client.connect(remote);

await connection.send(Buffer.from(`GET ${file}`));
