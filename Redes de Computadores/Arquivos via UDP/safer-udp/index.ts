import { logger as pinoLogger } from '../logger.js';
import { SocketManager } from './socket.js';
import { TimeoutManager, type OptionalTimeout } from './timeout.js';
import { MessageProtocol, type Message } from './protocol.js';
import { ChunkManager, type Chunk } from './chunk.js';
import { FlowManager } from './flow.js';

const logger = pinoLogger.child({ category: 'SaferUDP' });

// TODO: lidar com desconexão?

export class SaferUDP {
  private socket: SocketManager;
  private protocol = new MessageProtocol();
  private chunkManager = new ChunkManager();
  private chunkResponseTimeoutManager = new TimeoutManager();
  private flowManager = new FlowManager();

  private lastSentChunk: Chunk = -1;
  private receivedMessagesTimeout: OptionalTimeout = null;

  private messageCallback:
    | ((ctx: { messages: Message[]; buffer: Buffer }) => void)
    | undefined;

  constructor(
    messageCallback: typeof this.messageCallback,
    options: { packetLossRate?: number } = {},
  ) {
    this.socket = new SocketManager(options);
    this.messageCallback = messageCallback;

    this.socket.internalSocket.on('message', (msg, remoteInfo) => {
      if (
        this.socket.remoteInfo &&
        (remoteInfo.address !== this.socket.remoteInfo.address ||
          remoteInfo.port !== this.socket.remoteInfo.port)
      ) {
        return;
      }

      if (!this.socket.remoteInfo) {
        this.socket.connect(remoteInfo.port, remoteInfo.address);
      }

      this.handleMessage(this.protocol.parse(msg));
    });

    logger.info(
      `Tamanho máximo de um pacote SaferUDP: ${SocketManager.maxPacketSize - this.protocol.overhead} bytes`,
    );
  }

  listen(port: number) {
    this.socket.bind(port);
  }

  connect(port: number, address?: string) {
    this.socket.connect(port, address);
  }

  async send(data: Buffer) {
    const messages: Message[] = this.splitBuffer(data).map(
      (buf, index, chunks) => ({
        headers: {
          chunk: ++this.lastSentChunk,
          sum: this.protocol.generateChecksum(buf),
          complete: index === chunks.length - 1,
          ack: undefined,
        },
        body: buf,
      }),
    );

    messages.forEach((message) => {
      const serialized = this.protocol.serialize(message.headers, message.body);

      this.chunkManager.queueChunk(message.headers.chunk!, {
        complete: message.headers.complete,
        buf: serialized,
      });
    });

    await this.handleMessageQueue();
  }

  private async ack(chunk?: number) {
    const ack = chunk ?? this.chunkManager.calculateNextAckNumber();

    if (typeof ack !== 'number') return;

    const message = this.protocol.serialize({ ack });

    try {
      await this.socket.send(message);

      logger.info('ACK enviado: ' + ack);

      this.chunkManager.markChunksAsAcknowledged(ack);
      this.receivedMessagesTimeout = TimeoutManager.clear(
        this.receivedMessagesTimeout,
      );
    } catch (e) {
      if (e instanceof Error) {
        logger.error('Erro ao enviar ACK: ' + e.message);
        this.flowManager.down();
      }
    }
  }

  private async handleMessage(message: Message) {
    if (message.headers.ack !== undefined) {
      await this.handleAck(message.headers.ack);
    }

    if (
      'chunk' in message.headers &&
      'sum' in message.headers &&
      this.protocol.checkIntegrity(message.body, message.headers.sum!)
    ) {
      logger.debug(
        'Mensagem confiável recebida: ' + this.protocol.prettyPrint(message),
      );

      this.chunkManager.storeIncomingChunk(message.headers.chunk!, message);
      this.scheduleAck();

      const messages = this.chunkManager.extractCompleteMessage();

      if (!messages) return;

      const buffer = this.getCompleteBuffer(messages);
      this.messageCallback?.({ messages, buffer });
    }
  }

  private async handleAck(ack: number) {
    logger.info('ACK recebido: ' + ack);

    this.chunkResponseTimeoutManager.remove(ack);
    this.chunkManager.processAcknowledgment(ack);
    this.flowManager.up();

    await this.handleMessageQueue();
  }

  private async handleChunkTimeout(chunk: number) {
    const context = this.chunkManager.getOutgoingChunk(chunk);
    if (!context) return;

    logger.info(`Chunk ${chunk} deu timeout`);

    this.flowManager.down();
    await this.handleMessageQueue();
  }

  private async handleMessageQueue() {
    const chunks = this.chunkManager.getAllOutgoingChunksSorted();

    const currentlyAwaitingAck = this.chunkManager.getChunksAwaitingAck();

    const messagesToRetry = this.chunkManager
      .getAllOutgoing()
      .filter((chunk) => this.chunkResponseTimeoutManager.hasTimedOut(chunk));

    if (messagesToRetry.length) {
      return await this.retryMessages(
        this.flowManager.getBatch(messagesToRetry),
      );
    }

    if (!currentlyAwaitingAck.length && chunks.length) {
      return await this.sendMessagesBatch(this.flowManager.getBatch(chunks));
    }
  }

  private async sendMessagesBatch(chunks: number[]) {
    logger.debug('Enviando batch: ' + chunks.join(', '));

    for (const chunk of chunks) {
      const context = this.chunkManager.getOutgoingChunk(chunk)!;

      try {
        await this.socket.send(context.buf);
        this.chunkManager.updateChunkStatus(chunk, { sent: true });
      } catch (e) {
        if (e instanceof Error) {
          logger.error(`Erro ao enviar o chunk ${chunk}: ${e.message}`);
          this.flowManager.down();
        }
      } finally {
        this.chunkResponseTimeoutManager.set(
          chunk,
          () => this.handleChunkTimeout(chunk),
          TimeoutManager.DEFAULT_DELAY * 2,
        );
      }
    }
  }

  private async retryMessages(chunks: number[]) {
    logger.debug('Fazendo uma nova tentativa: ' + chunks.join(', '));

    for (const chunk of chunks) {
      const context = this.chunkManager.getOutgoingChunk(chunk);

      try {
        await this.socket.send(context.buf);
      } catch (e) {
        if (e instanceof Error) {
          logger.error(
            `Erro ao enviar novamente o chunk ${chunk}: ${e.message}`,
          );
        }
      } finally {
        this.chunkResponseTimeoutManager.retry(chunk);
      }
    }
  }

  private getCompleteBuffer(messages: Message[]) {
    const buffers = messages.map((msg) => msg.body);
    return Buffer.concat(buffers);
  }

  private splitBuffer(buffer: Buffer) {
    const chunks: Buffer[] = [];
    const maxSize = SocketManager.maxPacketSize - this.protocol.overhead;

    for (let i = 0; i < buffer.length; i += maxSize) {
      chunks.push(buffer.subarray(i, i + maxSize));
    }

    return chunks;
  }

  private scheduleAck() {
    TimeoutManager.clear(this.receivedMessagesTimeout);

    this.receivedMessagesTimeout = setTimeout(() => {
      this.receivedMessagesTimeout = TimeoutManager.clear(
        this.receivedMessagesTimeout,
      );

      this.ack();
    }, TimeoutManager.DEFAULT_DELAY);
  }
}

