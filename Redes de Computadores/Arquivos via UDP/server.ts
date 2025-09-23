import fs from 'node:fs/promises';

import { SaferUDP } from './safer-udp/index.js';
import { parseServerConfig } from './config.js';
import { logger as pinoLogger } from './logger.js';

const config = await parseServerConfig();

if (config.debug) {
  pinoLogger.level = 'debug';
}

const logger = pinoLogger.child({ category: 'Server' });

const server = new SaferUDP(
  async (message) => {
    const string = message.buffer.toString();

    const [method, path] = string.split(' ');

    if (method !== 'GET') {
      await message.connection.send(Buffer.from('ERRO!! Método inválido'));
      return message.connection.close();
    }

    const files = await fs.readdir('./assets/');

    if (!path || !files.includes(path)) {
      await message.connection.send(Buffer.from('ERRO!! Arquivo não existe'));
      return message.connection.close();
    }

    const file = await fs.readFile('./assets/' + path);

    logger.info(`Enviando arquivo '${path}'...`);

    await message.connection.send(file);
    message.connection.close();
  },
  {
    packetLossRate: config.packetLossRate,
    timeoutDelay: config.timeoutDelay,
    timeoutEventWindow: config.timeoutEventWindow,
    initialFlowCeiling: config.initialFlowCeiling,
    maxRetries: config.maxRetries,
  },
);

server.listen(config.port);
