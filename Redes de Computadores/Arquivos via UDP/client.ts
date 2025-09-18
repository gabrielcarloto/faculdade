import { SaferUDP } from './safer-udp/index.js';

import { logger as pinoLogger } from './logger.js';

const logger = pinoLogger.child({ category: 'Cliente' });

const client = new SaferUDP((message) => {
  console.log('Recebi a mensagem!');
  console.log(message.buffer.toString());
});

client.connect(3000);

client.send(Buffer.from('Mensagem interessante e pequena para teste'));

const largeText = `
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. 
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. 
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. 
Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.

Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, 
eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo. 
Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos 
qui ratione voluptatem sequi nesciunt.

At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti 
quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia 
deserunt mollitia animi, id est laborum et dolorum fuga.

Esta é uma mensagem de teste grande o suficiente para ser dividida em múltiplos chunks pelo protocolo SaferUDP.
O novo método send() irá automaticamente dividir este buffer em pedaços menores que cabem no MTU da rede.
Cada chunk será enviado com checksum MD5 para verificação de integridade e numeração sequencial para reordenação.
`.repeat(90);

logger.info(`Enviando buffer de ${Buffer.from(largeText).length} bytes...`);

await client.send(Buffer.from(largeText));

await client.send(
  Buffer.from('Mais uma mensagem interessante e pequena para teste'),
);
