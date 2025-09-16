import { SaferUDP } from './safer-udp.js';

const PORT = 3000;

const server = new SaferUDP((message) => {
  console.log('Recebi a mensagem!');
  console.log(message.buffer.toString());
});

server.listen(PORT);
