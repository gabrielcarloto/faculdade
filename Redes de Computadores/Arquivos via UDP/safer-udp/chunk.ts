import { logger as pinoLogger } from '../logger.js';
import type { Message } from './protocol.js';

export type Chunk = number;

type OutgoingMeta = {
  complete: boolean;
  sent: boolean;
  buf: Buffer;
};

type ReceivedMeta = {
  acked: boolean;
  joined: boolean;
  message: Message;
};

const logger = pinoLogger.child({ category: 'ChunkManager' });

export class ChunkManager {
  private outgoingChunks = new Map<Chunk, OutgoingMeta>();
  private incomingChunks = new Map<Chunk, ReceivedMeta>();
  private lastAcked = -1;

  constructor() {}

  cleanup() {
    Array.from(this.outgoingChunks.keys()).forEach((chunk) =>
      this.outgoingChunks.delete(chunk),
    );

    Array.from(this.incomingChunks.keys()).forEach((chunk) =>
      this.incomingChunks.delete(chunk),
    );
  }

  queueChunk(chunk: number, meta: Pick<OutgoingMeta, 'complete' | 'buf'>) {
    this.outgoingChunks.set(chunk, {
      sent: false,
      ...meta,
    });

    logger.debug(`Chunk ${chunk} enfileirado para envio`);
  }

  updateChunkStatus(chunk: number, meta: Partial<OutgoingMeta>) {
    const currentMeta = this.getOutgoingChunk(chunk);
    this.outgoingChunks.set(chunk, Object.assign(currentMeta, meta));
  }

  getOutgoingChunk(chunk: number) {
    return this.outgoingChunks.get(chunk)!;
  }

  removeOutgoingChunk(chunk: number) {
    this.outgoingChunks.delete(chunk);
  }

  getAllOutgoing() {
    return Array.from(this.outgoingChunks.keys());
  }

  getAllOutgoingChunksSorted() {
    return Array.from(this.outgoingChunks.keys()).sort((a, b) =>
      a < b ? -1 : 1,
    );
  }

  getChunksAwaitingAck(): Chunk[] {
    return this.getAllOutgoingChunksSorted().filter((chunk) => {
      const context = this.outgoingChunks.get(chunk)!;
      return context.sent;
    });
  }

  getPendingChunks(): Chunk[] {
    return this.getAllOutgoingChunksSorted().filter((chunk) => {
      const context = this.outgoingChunks.get(chunk)!;
      return !context.sent;
    });
  }

  processAcknowledgment(ack: number) {
    const chunksToRemove = Array.from(this.outgoingChunks.keys()).filter(
      (chunk) => chunk <= ack,
    );

    chunksToRemove.forEach((chunk) => {
      this.outgoingChunks.delete(chunk);
    });

    if (chunksToRemove.length > 0) {
      logger.debug(`ACK ${ack}: removidos chunks ${chunksToRemove.join(', ')}`);
    }
  }

  storeIncomingChunk(chunk: number, message: Message) {
    const exists = this.incomingChunks.has(chunk);

    if (exists) {
      return logger.info('Ignorando chunk duplicado: ' + chunk);
    }

    this.incomingChunks.set(chunk, {
      acked: false,
      joined: false,
      message,
    });

    logger.debug(`Chunk ${chunk} recebido e armazenado`);
  }

  markChunksAsAcknowledged(ack: number) {
    const acknowledgedChunks: number[] = [];

    Array.from(this.incomingChunks.entries()).forEach(([chunk, context]) => {
      if (chunk <= ack && !context.acked) {
        context.acked = true;
        acknowledgedChunks.push(chunk);
      }
    });

    if (acknowledgedChunks.length > 0) {
      logger.debug(
        `Chunks marcados como ACKed: ${acknowledgedChunks.join(', ')}`,
      );

      this.lastAcked = acknowledgedChunks.sort((a, b) => a - b).at(-1)!;
    }
  }

  calculateNextAckNumber(): number | undefined {
    const receivedChunks = Array.from(this.incomingChunks.keys())
      .filter((chunk) => !this.incomingChunks.get(chunk)!.acked)
      .sort((a, b) => a - b);

    if (receivedChunks.length === 0) return;

    const lastAcked = Array.from(this.incomingChunks.keys())
      .filter((chunk) => this.incomingChunks.get(chunk)!.acked)
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

  extractCompleteMessage(): Message[] | null {
    const pendingMessages = Array.from(this.incomingChunks.keys())
      .filter((chunk) => !this.incomingChunks.get(chunk)!.joined)
      .sort((a, b) => (a < b ? -1 : 1));

    const canComplete = pendingMessages.find(
      (chunk) => this.incomingChunks.get(chunk)!.message.headers.complete,
    );

    if (typeof canComplete === 'undefined') return null;

    const completeMessage: Message[] = [];
    const processedChunks: number[] = [];

    for (const chunk of pendingMessages) {
      const message = this.incomingChunks.get(chunk)!;

      completeMessage.push(message.message);
      message.joined = true;
      processedChunks.push(chunk);

      if (message.message.headers.complete) {
        break;
      }
    }

    logger.debug(
      `Mensagem completa extraída dos chunks: ${processedChunks.join(', ')}`,
    );

    return completeMessage;
  }

  getLastAcked() {
    return this.lastAcked;
  }
}
