import { SaferUDP } from './safer-udp.js';

const client = new SaferUDP((message) => {
  console.log('Recebi a mensagem!');
  console.log(message);
});

client.connect(3000);
client.send(Buffer.from('Oi serverrrr'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
client.send(Buffer.from('Oi dnvv'));
