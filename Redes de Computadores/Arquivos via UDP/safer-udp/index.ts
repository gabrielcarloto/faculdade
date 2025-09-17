import crypto from 'node:crypto';
import { logger as pinoLogger } from '../logger.js';
import { SocketManager } from './socket.js';

type Headers = {
  chunk: number | undefined;
  ack: number | undefined;
  sum: string | undefined;
  complete: boolean;
};

type Message = {
  headers: Headers;
  body: Buffer;
};

const DEFAULT_TIMEOUT_MS = 250;
const MAX_BATCH = 5;

const logger = pinoLogger.child({ category: 'SaferUDP' });

// TODO: lidar com desconexão?

export class SaferUDP {
  private socket: SocketManager;
  // private connections = new Map<string, any>()

  private maxBatchMessages = 1;
  private outgoingMessagesQueue = new Map<
    number,
    {
      complete: boolean;
      sent: boolean;
      timedOut: boolean;
      retries: number;
      buf: Buffer;
      timeout: NodeJS.Timeout | null;
    }
  >();

  private timeouts: Record<'receivedAck', null | NodeJS.Timeout> = {
    receivedAck: null,
  };

  private updateTimeout(
    timeoutKey: keyof typeof this.timeouts,
    callback: () => void,
    timeMs: number,
  ) {
    if (this.timeouts[timeoutKey]) {
      clearTimeout(this.timeouts[timeoutKey]!);
    }
    this.timeouts[timeoutKey] = setTimeout(callback, timeMs);
  }

  private setChunkTimeout(chunk: number, timeMs: number) {
    const context = this.outgoingMessagesQueue.get(chunk);
    if (!context) return;

    if (context.timeout) {
      clearTimeout(context.timeout);
    }

    const timeout = setTimeout(() => {
      this.handleChunkTimeout(chunk);
    }, timeMs);

    this.outgoingMessagesQueue.set(chunk, {
      ...context,
      timeout,
    });
  }

  private clearChunkTimeout(chunk: number) {
    const context = this.outgoingMessagesQueue.get(chunk);
    if (context?.timeout) {
      clearTimeout(context.timeout);
      this.outgoingMessagesQueue.set(chunk, {
        ...context,
        timeout: null,
      });
    }
  }

  private handleChunkTimeout(chunk: number) {
    const context = this.outgoingMessagesQueue.get(chunk);
    if (!context) return;

    logger.info(`Chunk ${chunk} deu timeout`);

    this.outgoingMessagesQueue.set(chunk, {
      ...context,
      timedOut: true,
      timeout: null,
    });

    this.handleMessageQueue();
  }

  private lastSentChunk: number = -1;
  private receivedMessages = new Map<
    number,
    { acked: boolean; joined: boolean; message: Message }
  >();

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

      this.handleMessage(this.parse(msg));
    });
  }

  parse(buf: Buffer): Message {
    const message = buf.toString();
    const lines = message.split('\n');

    const dataSeparatorIndex = lines.findIndex((line) => line === '');

    return {
      headers: JSON.parse(lines[0]!) as Headers,
      body: Buffer.from(lines.slice(dataSeparatorIndex + 1).join('\n')),
    };
  }

  serialize(headers: object, body?: Buffer) {
    return Buffer.from(
      JSON.stringify(headers) + '\n\n' + (body ?? '').toString(),
    );
  }

  async ack(chunk?: number) {
    const ack = chunk ?? this.getNextAck();

    if (typeof ack !== 'number') return;

    const message = this.serialize({ ack });

    try {
      await this.socket.send(message);

      logger.debug('ACK enviado: ' + ack);

      Array.from(this.receivedMessages.entries()).forEach(
        ([chunk, context]) => {
          if (chunk <= ack) {
            context.acked = true;
            this.clearChunkTimeout(chunk);
          }
        },
      );
    } catch (e) {
      if (e instanceof Error) {
        logger.error('Erro ao enviar ACK: ' + e.message);
      }
    }
  }

  listen(port: number) {
    this.socket.internalSocket.bind(port, () =>
      logger.info(`Escutando na porta: ${port}`),
    );
  }

  private getNextAck() {
    const receivedChunks = Array.from(this.receivedMessages.keys())
      .filter((chunk) => !this.receivedMessages.get(chunk)!.acked)
      .sort((a, b) => a - b);

    if (receivedChunks.length === 0) {
      return;
    }

    const lastAcked = Array.from(this.receivedMessages.keys())
      .filter((chunk) => this.receivedMessages.get(chunk)!.acked)
      .sort((a, b) => a - b)
      .at(-1);

    let lastSequentialChunk = lastAcked ?? receivedChunks[0]!;

    for (const chunk of receivedChunks) {
      if (chunk === lastSequentialChunk + 1) {
        lastSequentialChunk = chunk;
      } else {
        break;
      }
    }

    if (lastSequentialChunk >= 0) {
      return lastSequentialChunk;
    }

    return;
  }

  async send(data: Buffer, _headers: Partial<Headers> = {}) {
    const headers = Object.assign(
      {
        chunk: ++this.lastSentChunk,
        sum: this.generateChecksum(data),
        ack: undefined,
        complete: true,
      } satisfies Headers,
      _headers,
    );

    const message = this.serialize(headers, data);
    this.outgoingMessagesQueue.set(headers.chunk, {
      sent: false,
      timedOut: false,
      complete: headers.complete,
      retries: 0,
      buf: message,
      timeout: null,
    });

    await this.handleMessageQueue();
  }

  connect(port: number, address?: string) {
    this.socket.connect(port, address);
  }

  private async handleMessageQueue() {
    const queue = this.outgoingMessagesQueue;
    const chunks = Array.from(queue.keys()).sort((a, b) => (a < b ? -1 : 1));

    const currentlyAwaitingAck = chunks.filter((chunk) => {
      const context = queue.get(chunk)!;
      return context.sent;
    });

    const messagesToRetry = currentlyAwaitingAck.filter((chunk) => {
      const context = queue.get(chunk)!;
      return context.timedOut;
    });

    if (messagesToRetry.length)
      return await this.retryMessages(messagesToRetry);

    if (!currentlyAwaitingAck.length && chunks.length)
      return await this.sendMessagesBatch(chunks);
  }

  private async sendMessagesBatch(chunks: number[]) {
    const queue = this.outgoingMessagesQueue;

    const currentBatch = chunks.slice(
      0,
      Math.min(chunks.length, this.maxBatchMessages),
    );

    logger.debug('Enviando batch: ' + currentBatch.join(', '));

    for (const chunk of currentBatch) {
      const context = queue.get(chunk)!;

      try {
        await this.socket.send(context.buf);
        queue.set(chunk, { ...context, sent: true });
      } catch (e) {
        if (e instanceof Error) {
          return logger.error(`Erro ao enviar o chunk ${chunk}: ${e.message}`);
        }
      } finally {
        const timeout = DEFAULT_TIMEOUT_MS * 2;
        this.setChunkTimeout(chunk, timeout);
      }
    }
  }

  private async retryMessages(chunks: number[]) {
    const queue = this.outgoingMessagesQueue;

    logger.debug('Fazendo uma nova tentativa: ' + chunks.join(', '));

    for (const chunk of chunks) {
      const context = queue.get(chunk)!;

      try {
        await this.socket.send(context.buf);

        queue.set(chunk, {
          ...context,
          retries: context.retries + 1,
          timedOut: false,
        });
      } catch (e) {
        if (e instanceof Error) {
          logger.error(
            `Erro ao enviar novamente o chunk ${chunk}: ${e.message}`,
          );
        }

        const timeout =
          DEFAULT_TIMEOUT_MS * 2 * (queue.get(chunk)!.retries + 1);

        this.setChunkTimeout(chunk, timeout);
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
      this.checkIntegrity(message.body, message.headers.sum!)
    ) {
      logger.debug('Mensagem confiável recebida: ' + this.prettyPrint(message));

      this.receivedMessages.set(message.headers.chunk!, {
        acked: false,
        joined: false,
        message,
      });

      this.updateTimeout('receivedAck', () => this.ack(), DEFAULT_TIMEOUT_MS);
      const messages = this.getNextCompleteMessage();

      if (!messages) return;

      const buffer = this.getCompleteBuffer(messages);
      this.messageCallback?.({ messages, buffer });
    }
  }

  private handleAck(ack: number) {
    logger.info('ACK recebido: ' + ack);

    Array.from(this.outgoingMessagesQueue.keys()).forEach((chunk) => {
      if (chunk <= ack) {
        this.clearChunkTimeout(chunk);
        this.outgoingMessagesQueue.delete(chunk);
      }
    });

    this.maxBatchMessages = Math.min(this.maxBatchMessages + 1, MAX_BATCH);
    this.handleMessageQueue();
  }

  private getNextCompleteMessage() {
    const queue = this.receivedMessages;

    const pendingMessages = Array.from(queue.keys()).filter((chunk) => {
      return !queue.get(chunk)!.joined;
    });

    const canComplete = pendingMessages.findLast(
      (chunk) => queue.get(chunk)!.message.headers.complete,
    );

    if (!canComplete) return null;

    const completeMessage: Message[] = [];

    for (const chunk of pendingMessages) {
      const message = queue.get(chunk)!;

      completeMessage.push(message.message);
      message.joined = true;

      if (message.message.headers.complete) {
        break;
      }
    }

    return completeMessage;
  }

  private getCompleteBuffer(messages: Message[]) {
    const buffers = messages.map((msg) => msg.body);
    return Buffer.concat(buffers);
  }

  private checkIntegrity(body: Buffer, checksum: string) {
    const hash = this.generateChecksum(body);
    return hash === checksum;
  }

  private generateChecksum(buf: Buffer) {
    return crypto.createHash('md5').update(buf).digest('binary');
  }

  private prettyPrint(msg: Message) {
    return `[${msg.headers.chunk}] ${msg.body.toString()}`;
  }
}
