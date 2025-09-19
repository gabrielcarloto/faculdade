import { logger as pinoLogger } from '../logger.js';

import crypto from 'node:crypto';

export type Headers = {
  chunk: number | undefined;
  ack: number | undefined;
  sum: string | undefined;
  complete: boolean;
};

export type Message = {
  headers: Headers;
  body: Buffer;
};

const logger = pinoLogger.child({ category: 'Protocol' });

export class MessageProtocol {
  private hashingAlgorithm: string;
  private static readonly separator = '\n\n';
  public readonly overhead: number;

  constructor(hashingAlgo = 'md5') {
    this.hashingAlgorithm = hashingAlgo;
    this.overhead = this.calculateOverhead();
    logger.info(`Overhead do protocolo: ${this.overhead} bytes`);
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
      JSON.stringify(headers) +
        MessageProtocol.separator +
        (body ?? '').toString(),
    );
  }

  checkIntegrity(body: Buffer, checksum: string) {
    const hash = this.generateChecksum(body);
    return hash === checksum;
  }

  generateChecksum(buf: Buffer) {
    return crypto
      .createHash(this.hashingAlgorithm)
      .update(buf)
      .digest('binary');
  }

  prettyPrint(msg: Message) {
    return `[${msg.headers.chunk}] ${msg.body.toString()} (${msg.headers.complete ? 'completa' : 'incompleta'})`;
  }

  private calculateOverhead() {
    const WORST_CASE: Required<Headers> = {
      complete: false,
      ack: Number.MAX_SAFE_INTEGER,
      chunk: Number.MAX_SAFE_INTEGER,
      sum: this.generateChecksum(Buffer.from('some string')),
    };

    return Buffer.from(this.serialize(WORST_CASE)).length;
  }
}
