import { logger as pinoLogger } from '../logger.js';
import { SocketManager } from './socket.js';
import { TimeoutManager, type OptionalTimeout } from './timeout.js';
import { MessageProtocol, type Message } from './protocol.js';
import { ChunkManager, type Chunk } from './chunk.js';

const MAX_BATCH = 5;

const logger = pinoLogger.child({ category: 'SaferUDP' });

// TODO: lidar com desconexão?

export class SaferUDP {
  private socket: SocketManager;
  private protocol = new MessageProtocol();
  private chunkManager = new ChunkManager();
  private chunkResponseTimeoutManager = new TimeoutManager();

  private lastSentChunk: Chunk = -1;
  private maxBatchMessages = 1;
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
          ack: undefined,
          complete: index === chunks.length - 1,
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

      logger.debug('ACK enviado: ' + ack);

      this.chunkManager.markChunksAsAcknowledged(ack);
      this.receivedMessagesTimeout = TimeoutManager.clear(
        this.receivedMessagesTimeout,
      );
    } catch (e) {
      if (e instanceof Error) {
        logger.error('Erro ao enviar ACK: ' + e.message);
        this.maxBatchMessages = 1;
      }
    }
  }

  private handleMessage(message: Message) {
    if (message.headers.ack !== undefined) {
      this.handleAck(message.headers.ack);
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

      TimeoutManager.clear(this.receivedMessagesTimeout);

      this.receivedMessagesTimeout = setTimeout(() => {
        this.ack();

        this.receivedMessagesTimeout = TimeoutManager.clear(
          this.receivedMessagesTimeout,
        );
      }, TimeoutManager.DEFAULT_DELAY);

      const messages = this.chunkManager.extractCompleteMessage();

      if (!messages) return;

      const buffer = this.getCompleteBuffer(messages);
      this.messageCallback?.({ messages, buffer });
    }
  }

  private handleAck(ack: number) {
    logger.info('ACK recebido: ' + ack);

    this.chunkResponseTimeoutManager.remove(ack);
    this.chunkManager.processAcknowledgment(ack);
    this.maxBatchMessages = Math.min(this.maxBatchMessages + 1, MAX_BATCH);
    this.handleMessageQueue();
  }

  private handleChunkTimeout(chunk: number) {
    const context = this.chunkManager.getOutgoingChunk(chunk);
    if (!context) return;

    logger.info(`Chunk ${chunk} deu timeout`);

    this.chunkManager.updateChunkStatus(chunk, {
      timedOut: true,
    });

    this.maxBatchMessages = 1;
    this.handleMessageQueue();
  }

  private async handleMessageQueue() {
    const chunks = this.chunkManager.getAllOutgoingChunksSorted();

    const currentlyAwaitingAck = this.chunkManager.getChunksAwaitingAck();
    const messagesToRetry = this.chunkManager.getTimedOutChunks();

    if (messagesToRetry.length)
      return await this.retryMessages(messagesToRetry);

    if (!currentlyAwaitingAck.length && chunks.length)
      return await this.sendMessagesBatch(chunks);
  }

  private async sendMessagesBatch(chunks: number[]) {
    const currentBatch = chunks.slice(
      0,
      Math.min(chunks.length, this.maxBatchMessages),
    );

    logger.debug('Enviando batch: ' + currentBatch.join(', '));

    for (const chunk of currentBatch) {
      const context = this.chunkManager.getOutgoingChunk(chunk)!;

      try {
        await this.socket.send(context.buf);
        this.chunkManager.updateChunkStatus(chunk, { sent: true });
      } catch (e) {
        if (e instanceof Error) {
          logger.error(`Erro ao enviar o chunk ${chunk}: ${e.message}`);
          this.maxBatchMessages = 1;
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
        this.chunkManager.updateChunkStatus(chunk, { timedOut: false });

        this.chunkResponseTimeoutManager.set(
          chunk,
          () => this.handleChunkTimeout(chunk),
          TimeoutManager.DEFAULT_DELAY * 2,
        );
      } catch (e) {
        if (e instanceof Error) {
          logger.error(
            `Erro ao enviar novamente o chunk ${chunk}: ${e.message}`,
          );
        }

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
}
