import fs from 'node:fs';

import { SaferUDP } from './safer-udp/index.js';
import { logger as pinoLogger } from './logger.js';

const PORT = 3000;

const logger = pinoLogger.child({ category: 'Server' });

const server = new SaferUDP((message) => {
  const string = message.buffer.toString();
  if (string.length < 100) logger.info(string);
  else {
    logger.info(`Mensagem grande recebida (${message.buffer.length} bytes)`);

    fs.writeFileSync('./out/pao.jpg', message.buffer);
    logger.info('Arquivo escrito!!!');

    return;
  }

  logger.info(
    `Mensagem recebida de ${message.remote.address}:${message.remote.port}`,
  );
});

server.listen(PORT);
