import { SaferUDP } from './safer-udp/index.js';
import { logger as pinoLogger } from './logger.js';

const PORT = 3000;

const logger = pinoLogger.child({ category: 'Server' });

const server = new SaferUDP((message) => {
  logger.info('Recebi a mensagem! Conteúdo: ' + message.buffer.toString());
});

server.listen(PORT);
