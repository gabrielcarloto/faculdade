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

export class MessageProtocol {
  private hashingAlgorithm: string;
  private static readonly separator = '\n\n';
  public readonly overhead: number;

  constructor(hashingAlgo = 'md5') {
    this.hashingAlgorithm = hashingAlgo;
    this.overhead = this.calculateOverhead();
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
      ack: Number.MAX_SAFE_INTEGER,
      chunk: Number.MAX_SAFE_INTEGER,
      complete: false,
      sum: this.generateChecksum(Buffer.from('some string')),
    };

    return Buffer.from(JSON.stringify(WORST_CASE) + MessageProtocol.separator)
      .length;
  }
}
