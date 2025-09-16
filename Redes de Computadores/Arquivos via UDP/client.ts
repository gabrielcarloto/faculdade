import { SaferUDP } from './safer-udp.js';

const client = new SaferUDP((message) => {
  console.log('Recebi a mensagem!');
  console.log(message.buffer.toString());
});

client.connect(3000);

const stringToSend =
  'Oi eu sou uma string bem grande e espero que possa ser enviada com segurança para o servidor :)';

const chunks = stringToSend.split(' ');

chunks.forEach((chunk, index) => {
  client.send(Buffer.from(chunk + ' '), {
    complete: index === chunks.length - 1,
  });
});
