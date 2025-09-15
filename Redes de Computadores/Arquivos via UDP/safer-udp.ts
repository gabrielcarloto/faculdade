import dgram from 'node:dgram';
import crypto from 'node:crypto';
import pino from 'pino';
import pretty from 'pino-pretty';

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

const logger = pino({ level: 'debug' }, pretty({ colorize: true }));

const DEFAULT_TIMEOUT_MS = 250;
const MAX_BATCH = 5;

// TODO: lidar com desconexão?

export class SaferUDP {
  private socket = dgram.createSocket('udp4');
  // private connections = new Map<string, any>()

  private remote:
    | dgram.RemoteInfo
    | { address: string | undefined; port: number }
    | null = null;

  private maxBatchMessages = 1;
  private outgoingMessagesQueue = new Map<
    number,
    {
      complete: boolean;
      sent: boolean;
      timedOut: boolean;
      retries: number;
      buf: Buffer;
    }
  >();

  private timeouts: Record<
    'sentMessages' | 'receivedAck',
    null | NodeJS.Timeout
  > = {
    sentMessages: null,
    receivedAck: null,
  };

  private lastSentChunk: number = -1;
  private receivedMessages = new Map<
    number,
    { acked: boolean; message: Message }
  >();

  private messageCallback: ((msg: Message) => void) | undefined;

  constructor(messageCallback: typeof this.messageCallback) {
    this.messageCallback = messageCallback;

    this.socket.on('message', (msg, remoteInfo) => {
      if (
        this.remote &&
        (remoteInfo.address !== this.remote.address ||
          remoteInfo.port !== this.remote.port)
      ) {
        return;
      }

      if (!this.remote) {
        this.remote = remoteInfo;
      }

      this.handleMessage(this.parse(msg));
    });

    this.socket.on('close', () => {
      this.remote = null;
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

  ack(chunk?: number) {
    if (!this.remote) {
      throw new Error('É necessário uma remote para enviar mensagens');
    }

    const ack = chunk ?? this.getNextAck();

    if (typeof ack !== 'number') return;

    const message = this.serialize({ ack });

    this.socket.send(
      message,
      this.remote.port,
      this.remote.address,
      (error) => {
        if (error) {
          return logger.error('Erro ao enviar ACK: ' + error.message);
        }

        logger.debug('ACK enviado: ' + ack);

        Array.from(this.receivedMessages.entries()).forEach(
          ([chunk, context]) => {
            if (chunk <= ack) context.acked = true;
          },
        );
      },
    );
  }

  listen(port: number) {
    this.socket.bind(port, () => logger.info(`Escutando na porta: ${port}`));
  }

  connect(port: number, address = '127.0.0.1') {
    this.remote = {
      address,
      port,
    };

    logger.debug('Conectado a ' + address + ':' + port);
  }

  private getNextAck() {
    const chunksAwaitingAck: Array<number> = [];

    for (const [chunk, context] of this.receivedMessages.entries()) {
      if (!context.acked) chunksAwaitingAck.push(chunk);
    }

    if (!chunksAwaitingAck.length) {
      return;
    }

    const sortedChunks = chunksAwaitingAck.sort((a, b) => (a < b ? -1 : 0));

    let nextAck = sortedChunks[0];
    let i;

    for (i = 0; i < sortedChunks.length - 1; i++) {
      if (sortedChunks[i] === sortedChunks[i + 1]! - 1)
        nextAck = sortedChunks[i + 1];
    }

    return nextAck;
  }

  send(
    data: Buffer,
    headers = {
      chunk: ++this.lastSentChunk,
      sum: this.generateChecksum(data),
      ack: undefined,
      complete: true,
    } satisfies Headers,
  ) {
    if (!this.remote) {
      throw new Error('É necessário uma remote para enviar mensagens');
    }

    const message = this.serialize(headers, data);
    this.outgoingMessagesQueue.set(headers.chunk, {
      sent: false,
      timedOut: false,
      complete: headers.complete,
      retries: 0,
      buf: message,
    });

    this.handleMessageQueue();
  }

  private handleMessageQueue() {
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

    if (messagesToRetry.length) return this.retryMessages(messagesToRetry);

    if (!currentlyAwaitingAck.length && chunks.length)
      return this.sendMessagesBatch(chunks);
  }

  private sendMessagesBatch(chunks: number[]) {
    if (this.timeouts.sentMessages) {
      clearTimeout(this.timeouts.sentMessages);
      this.timeouts.sentMessages = null;
    }

    // TODO: eu deveria enviar mensagens completas uma de cada vez?

    const queue = this.outgoingMessagesQueue;

    const currentBatch = chunks.slice(
      0,
      Math.min(chunks.length, this.maxBatchMessages),
    );

    logger.debug('Enviando batch: ' + currentBatch.join(', '));

    currentBatch.forEach((chunk) => {
      const context = queue.get(chunk)!;
      queue.set(chunk, { ...context, sent: true });

      this.socket.send(
        context.buf,
        this.remote!.port,
        this.remote!.address,
        (error) => {
          if (error) {
            queue.set(chunk, context);
            return logger.error(
              `Erro ao enviar o chunk ${chunk}: ${error.message}`,
            );
          }

          const timeout = DEFAULT_TIMEOUT_MS * 2;

          if (this.timeouts.sentMessages) {
            clearTimeout(this.timeouts.sentMessages);
          }

          this.timeouts.sentMessages = setTimeout(
            () => this.handleSentMessagesTimeout(chunk),
            timeout,
          );
        },
      );
    });
  }

  private handleSentMessagesTimeout(lastChunk: number) {
    this.timeouts.sentMessages = null;
    const queue = this.outgoingMessagesQueue;
    const chunks = Array.from(queue.keys()).sort((a, b) => (a < b ? -1 : 1));

    chunks.forEach((chunk) => {
      const context = queue.get(chunk)!;
      if (chunk <= lastChunk) queue.set(chunk, { ...context, timedOut: true });
    });

    this.handleMessageQueue();
  }

  private retryMessages(chunks: number[]) {
    const queue = this.outgoingMessagesQueue;

    logger.debug('Fazendo uma nova tentativa: ' + chunks.join(', '));

    chunks.forEach((chunk) => {
      const context = queue.get(chunk)!;

      queue.set(chunk, {
        ...context,
        retries: context.retries + 1,
        timedOut: false,
      });

      this.socket.send(
        context.buf,
        this.remote!.port,
        this.remote!.address,
        (error) => {
          if (error) {
            queue.set(chunk, context);

            return logger.error(
              `Erro ao enviar novamente o chunk ${chunk}: ${error.message}`,
            );
          }

          if (this.timeouts.sentMessages) {
            clearTimeout(this.timeouts.sentMessages);
          }

          const timeout =
            DEFAULT_TIMEOUT_MS * 2 * (queue.get(chunk)!.retries + 1);

          this.timeouts.sentMessages = setTimeout(
            () => this.handleSentMessagesTimeout(chunk),
            timeout,
          );
        },
      );
    });
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

      // TODO: ignorar completos
      this.receivedMessages.set(message.headers.chunk!, {
        acked: false,
        message,
      });

      if (this.timeouts.receivedAck) {
        clearTimeout(this.timeouts.receivedAck);
        this.timeouts.receivedAck = null;
      }

      this.timeouts.receivedAck = setTimeout(
        () => this.ack(),
        DEFAULT_TIMEOUT_MS,
      );

      if (message.headers.complete) {
        this.messageCallback?.(message);
      }
    }
  }

  private handleAck(ack: number) {
    logger.info('ACK recebido: ' + ack);

    Array.from(this.outgoingMessagesQueue.keys()).forEach((chunk) => {
      if (chunk <= ack) this.outgoingMessagesQueue.delete(chunk);
    });

    this.maxBatchMessages = Math.min(this.maxBatchMessages + 1, MAX_BATCH);
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

  private getCompleteMessage() {}
}
