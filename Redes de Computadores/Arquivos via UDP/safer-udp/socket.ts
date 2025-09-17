import dgram from 'node:dgram';
import { logger as pinoLogger } from '../logger.js';

const logger = pinoLogger.child({ category: 'SocketManager' });

const DEFAULT_PACKET_LOSS_RATE = 0.05;

export class SocketManager {
  private socket = dgram.createSocket('udp4');
  private packetLossRate: number = DEFAULT_PACKET_LOSS_RATE;

  private remote:
    | dgram.RemoteInfo
    | { address: string | undefined; port: number }
    | null = null;

  constructor(options: { packetLossRate?: number } = {}) {
    this.packetLossRate = options.packetLossRate ?? DEFAULT_PACKET_LOSS_RATE;

    this.socket.on('close', () => {
      this.remote = null;
    });
  }

  get internalSocket() {
    return this.socket;
  }

  get remoteInfo() {
    return this.remote;
  }

  connect(port: number, address = '127.0.0.1') {
    this.remote = {
      address,
      port,
    };

    logger.info('Conectado a ' + address + ':' + port);
  }

  send(msg: string | NodeJS.ArrayBufferView | readonly any[]) {
    return new Promise<undefined>((resolve, reject) => {
      if (!this.remote || !this.remote.port) {
        return reject('Missing remote');
      }

      if (this.simulatePacketLoss()) {
        reject(new Error('Pacote perdido (simulação)'));
      }

      this.socket.send(
        msg,
        this.remote!.port,
        this.remote!.address,
        (error) => {
          if (error) {
            return reject(error);
          }

          resolve(undefined);
        },
      );
    });
  }

  private simulatePacketLoss(): boolean {
    return Math.random() < this.packetLossRate;
  }
}
