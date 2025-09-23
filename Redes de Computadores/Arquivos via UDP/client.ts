import fs from 'node:fs/promises';
import { SaferUDP } from './safer-udp/index.js';
import { parseClientConfig } from './config.js';

import { logger as pinoLogger } from './logger.js';

const config = await parseClientConfig();

if (config.debug) {
  pinoLogger.level = 'debug';
}

const logger = pinoLogger.child({ category: 'Cliente' });

const client = new SaferUDP(
  async (message) => {
    const string = message.buffer.toString();

    if (string.startsWith('ERRO')) {
      return logger.fatal('Erro: ' + string);
    }

    logger.info('Arquivo recebido!! ');

    try {
      await fs.mkdir('./out');
    } catch (_) {}

    const outName = config.outFile ?? config.file;
    await fs.writeFile(`./out/${outName}`, message.buffer);
  },
  {
    packetLossRate: config.packetLossRate,
    timeoutDelay: config.timeoutDelay,
    timeoutEventWindow: config.timeoutEventWindow,
    initialFlowCeiling: config.initialFlowCeiling,
    maxRetries: config.maxRetries,
  },
);

const remote = { address: config.address, port: config.port };
const connection = client.connect(remote);

await connection.send(Buffer.from(`GET ${config.file}`));
