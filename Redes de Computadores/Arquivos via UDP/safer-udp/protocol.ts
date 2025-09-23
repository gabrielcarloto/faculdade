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
    const separatorBuffer = Buffer.from(MessageProtocol.separator);
    const separatorIndex = buf.indexOf(separatorBuffer);

    if (separatorIndex === -1) {
      throw new Error('Formato de mensagem inválido: separador não encontrado');
    }

    const headerBuffer = buf.subarray(0, separatorIndex);
    const bodyBuffer = buf.subarray(separatorIndex + separatorBuffer.length);

    return {
      headers: JSON.parse(headerBuffer.toString()) as Headers,
      body: bodyBuffer,
    };
  }

  serialize(headers: object, body?: Buffer) {
    const headerString = JSON.stringify(headers);
    const headerBuffer = Buffer.from(headerString + MessageProtocol.separator);

    if (body) {
      return Buffer.concat([headerBuffer, body]);
    }

    return headerBuffer;
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
    if (msg.headers.chunk) {
      const maxLength = 50;
      const bodyString = msg.body.toString();
      const truncated = bodyString.length > maxLength;

      const displayText = truncated
        ? bodyString.substring(0, maxLength) + `... (${bodyString.length}b)`
        : bodyString;

      const completeDisplayText = msg.headers.complete
        ? 'completa'
        : 'incompleta';

      const closeConnectionDisplayText = !Number.isInteger(msg.headers.chunk)
        ? 'fechar conexão'
        : completeDisplayText;

      return `[${msg.headers.chunk}] ${displayText} (${closeConnectionDisplayText})`;
    }

    if (msg.headers.ack === undefined) return;

    return `[${msg.headers.ack}] (ack)`;
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
