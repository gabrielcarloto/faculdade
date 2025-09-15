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
const DEFAULT_PACKET_LOSS_RATE = 0.25;

// TODO: lidar com desconexão?

export class SaferUDP {
  private socket = dgram.createSocket('udp4');
  // private connections = new Map<string, any>()

  private remote:
    | dgram.RemoteInfo
    | { address: string | undefined; port: number }
    | null = null;

  private packetLossRate: number = DEFAULT_PACKET_LOSS_RATE;
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
    { acked: boolean; message: Message }
  >();

  private messageCallback: ((msg: Message) => void) | undefined;

  constructor(
    messageCallback: typeof this.messageCallback,
    options: { packetLossRate?: number } = {},
  ) {
    this.messageCallback = messageCallback;
    this.packetLossRate = options.packetLossRate ?? DEFAULT_PACKET_LOSS_RATE;

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

  private simulatePacketLoss(): boolean {
    return Math.random() < this.packetLossRate;
  }

  ack(chunk?: number) {
    if (!this.remote) {
      throw new Error('É necessário uma remote para enviar mensagens');
    }

    const ack = chunk ?? this.getNextAck();

    if (typeof ack !== 'number') return;

    const message = this.serialize({ ack });

    if (this.simulatePacketLoss()) {
      logger.debug(`ACK ${ack} perdido (simulação)`);
      return;
    }

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
            if (chunk <= ack) {
              context.acked = true;
              this.clearChunkTimeout(chunk);
            }
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
      timeout: null,
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

      if (this.simulatePacketLoss()) {
        logger.info(`Chunk ${chunk} perdido`);
        const timeout = DEFAULT_TIMEOUT_MS * 2;
        this.setChunkTimeout(chunk, timeout);
        return;
      }

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
          this.setChunkTimeout(chunk, timeout);
        },
      );
    });
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

      if (this.simulatePacketLoss()) {
        logger.debug(`Retry do chunk ${chunk} perdido`);

        const timeout =
          DEFAULT_TIMEOUT_MS * 2 * (queue.get(chunk)!.retries + 1);

        this.setChunkTimeout(chunk, timeout);
        return;
      }

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

          const timeout =
            DEFAULT_TIMEOUT_MS * 2 * (queue.get(chunk)!.retries + 1);

          this.setChunkTimeout(chunk, timeout);
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

      this.updateTimeout('receivedAck', () => this.ack(), DEFAULT_TIMEOUT_MS);

      if (message.headers.complete) {
        this.messageCallback?.(message);
      }
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
