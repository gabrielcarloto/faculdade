import { SaferUDP } from './safer-udp/index.js';
import { logger as pinoLogger } from './logger.js';

const PORT = 3000;

const logger = pinoLogger.child({ category: 'Server' });

const server = new SaferUDP((message) => {
  const string = message.buffer.toString();
  if (string.length < 100) logger.info(string);
  else logger.info('Mensagem grande recebida');
});

server.listen(PORT);
