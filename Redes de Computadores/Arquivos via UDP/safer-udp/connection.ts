import { ChunkManager, type Chunk } from './chunk.js';
import { TimeoutManager, type OptionalTimeout } from './timeout.js';
import { MessageProtocol, type Message } from './protocol.js';
import { SocketManager } from './socket.js';
import { FlowManager } from './flow.js';
import { logger as pinoLogger } from '../logger.js';

const logger = pinoLogger.child({ category: 'SaferUDPConnection' });

export type Remote = { address: string; port: number };

export class SaferUDPConnection {
  public readonly remote: Remote;

  private protocol = new MessageProtocol();
  private chunkManager = new ChunkManager();
  private chunkResponseTimeoutManager: TimeoutManager;
  private flowManager: FlowManager;
  private socketManager: SocketManager;

  private lastSentChunk: Chunk = -1;
  private receivedMessagesTimeout: OptionalTimeout = null;

  private lastTimeoutEventTime = 0;
  private timeoutEventWindow: number;

  private isPreparingToClose = false;

  private messageCallback: (ctx: {
    messages: Message[];
    buffer: Buffer;
    connection: SaferUDPConnection;
  }) => void | Promise<void>;

  private onCloseCallback: () => void;

  constructor(
    remote: Remote,
    messageCallback: typeof this.messageCallback,
    onClose: typeof this.onCloseCallback,
    flowManager: FlowManager,
    socketManager: SocketManager,
    options: {
      timeoutDelay?: number;
      timeoutEventWindow?: number;
      maxRetries?: number;
    } = {},
  ) {
    this.remote = remote;
    this.messageCallback = messageCallback;
    this.onCloseCallback = onClose;
    this.flowManager = flowManager;
    this.socketManager = socketManager;
    this.chunkResponseTimeoutManager = new TimeoutManager(
      options.timeoutDelay,
      options.maxRetries ?? 10,
    );

    this.chunkResponseTimeoutManager.setMaxRetriesCallback(() => {
      logger.error(`Conexão excedeu o limite de retries. Fechando conexão.`);
      this.closeConnection();
    });

    this.timeoutEventWindow = options.timeoutEventWindow ?? 10;
  }

  close() {
    this.isPreparingToClose = true;

    logger.debug('Enfileirando chunk de fechamento: ' + this.closeChunk);
    const message = this.protocol.serialize({ chunk: this.closeChunk });

    this.chunkManager.queueChunk(this.closeChunk, {
      complete: true,
      buf: message,
    });
  }

  async send(data: Buffer) {
    if (this.isPreparingToClose) {
      throw new Error(
        'Tentativa de enviar uma mensagem para uma conexão fechando',
      );
    }

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

  async handleMessage(message: Message) {
    logger.debug('Mensagem recebida: ' + this.protocol.prettyPrint(message));

    if (message.headers.ack !== undefined) {
      await this.handleAck(message.headers.ack);
    }

    if (
      'chunk' in message.headers &&
      'sum' in message.headers &&
      this.protocol.checkIntegrity(message.body, message.headers.sum!) &&
      !this.isPreparingToClose
    ) {
      logger.debug(
        'Mensagem confiável recebida: ' + this.protocol.prettyPrint(message),
      );

      this.chunkManager.storeIncomingChunk(message.headers.chunk!, message);
      this.scheduleAck();

      const messages = this.chunkManager.extractCompleteMessage();

      if (!messages) return;

      const buffer = this.getCompleteBuffer(messages);
      this.messageCallback?.({ messages, buffer, connection: this });
    }

    if (
      'chunk' in message.headers &&
      'sum' in message.headers &&
      !this.protocol.checkIntegrity(message.body, message.headers.sum!)
    ) {
      logger.debug(
        'Integridade da mensagem não pode ser garantida, descartando...',
      );
    }

    if (
      'chunk' in message.headers &&
      this.isCloseChunk(message.headers.chunk!)
    ) {
      this.handleCloseMessage();
      this.chunkManager.storeIncomingChunk(message.headers.chunk!, message);
      this.scheduleAck(message.headers.chunk!);
    }
  }

  private async ack(chunk?: number) {
    const ack = chunk ?? this.chunkManager.calculateNextAckNumber();

    if (typeof ack !== 'number') return;

    const message = this.protocol.serialize({ ack });

    try {
      await this.socketManager.send(message, this.remote);

      logger.info('ACK enviado: ' + ack);

      this.chunkManager.markChunksAsAcknowledged(ack);
      this.receivedMessagesTimeout = TimeoutManager.clear(
        this.receivedMessagesTimeout,
      );

      if (this.isPreparingToClose && this.isCloseChunk(ack)) {
        logger.debug('Close ACK enviado: ' + ack);
        this.closeConnection();
      }
    } catch (e) {
      if (e instanceof Error) {
        logger.error('Erro ao enviar ACK: ' + e.message);
        this.flowManager.down();
      }
    }
  }

  private async handleAck(ack: number) {
    logger.info('ACK recebido: ' + ack);

    if (this.isCloseChunk(ack)) {
      logger.debug('Close ACK recebido: ' + ack);
      return this.closeConnection();
    }

    this.chunkResponseTimeoutManager.remove(ack);
    this.chunkManager.processAcknowledgment(ack);
    this.flowManager.up();

    await this.handleMessageQueue();
  }

  private async handleChunkTimeout(chunk: number) {
    const context = this.chunkManager.getOutgoingChunk(chunk);
    if (!context) return;

    const currentRetries = this.chunkResponseTimeoutManager.getRetries(chunk);
    const maxRetries = this.chunkResponseTimeoutManager.getMaxRetries(chunk);

    logger.info(
      `Chunk ${chunk} deu timeout (tentativa ${currentRetries + 1}/${maxRetries})`,
    );

    const currentTime = Date.now();
    const timeSinceLastTimeout = currentTime - this.lastTimeoutEventTime;

    if (timeSinceLastTimeout > this.timeoutEventWindow) {
      this.flowManager.down();
    }

    this.lastTimeoutEventTime = currentTime;
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
        await this.socketManager.send(context.buf, this.remote);
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
          this.chunkResponseTimeoutManager.delay * 2,
        );
      }
    }
  }

  private async retryMessages(chunks: number[]) {
    logger.debug('Fazendo uma nova tentativa: ' + chunks.join(', '));

    for (const chunk of chunks) {
      const context = this.chunkManager.getOutgoingChunk(chunk);

      try {
        await this.socketManager.send(context.buf, this.remote);
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

  private handleCloseMessage() {
    this.isPreparingToClose = true;
    logger.info('Fechando conexão...');
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

  private scheduleAck(ack?: number) {
    TimeoutManager.clear(this.receivedMessagesTimeout);

    this.receivedMessagesTimeout = setTimeout(() => {
      this.receivedMessagesTimeout = TimeoutManager.clear(
        this.receivedMessagesTimeout,
      );

      this.ack(ack);
    }, this.chunkResponseTimeoutManager.delay);
  }

  private get closeChunk() {
    return this.lastSentChunk + 0.5;
  }

  private closeConnection() {
    this.chunkManager.cleanup();
    this.chunkResponseTimeoutManager.cleanup();

    if (!this.socketManager.listening) {
      this.socketManager.internalSocket.close();
    }

    this.onCloseCallback();
  }

  private isCloseChunk(chunk: number) {
    return chunk > 0 && !Number.isInteger(chunk);
  }
}
