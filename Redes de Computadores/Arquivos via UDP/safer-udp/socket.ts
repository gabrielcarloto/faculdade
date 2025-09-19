import dgram from 'node:dgram';
import { logger as pinoLogger } from '../logger.js';

const logger = pinoLogger.child({ category: 'SocketManager' });

const DEFAULT_PACKET_LOSS_RATE = 0.025;

export class SocketManager {
  private socket = dgram.createSocket('udp4');
  private packetLossRate: number = DEFAULT_PACKET_LOSS_RATE;

  private remote:
    | dgram.RemoteInfo
    | { address: string | undefined; port: number }
    | null = null;

  constructor(options: { packetLossRate?: number } = {}) {
    this.packetLossRate = options.packetLossRate ?? DEFAULT_PACKET_LOSS_RATE;
    logger.info(
      'Simulação de perda de pacotes configurada com chance de ' +
        this.packetLossRate * 100 +
        '%',
    );

    this.socket.on('close', () => {
      this.remote = null;
    });

    logger.info(
      `Tamanho máximo de um pacote UDP: ${SocketManager.maxPacketSize} bytes`,
    );
  }

  static get maxPacketSize() {
    const ethernetMTU = 1500;
    const ipHeaderLength = 20;
    const udpHeaderLength = 8;
    const justToBeSafe = 500;

    return ethernetMTU - (ipHeaderLength + udpHeaderLength + justToBeSafe);
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
  }

  send(msg: string | NodeJS.ArrayBufferView | readonly any[]) {
    return new Promise<undefined>((resolve, reject) => {
      if (!this.remote || !this.remote.port) {
        return reject(new Error('Missing remote'));
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

  bind(port: number) {
    this.socket.bind(port, () => {
      logger.info(`Escutando na porta: ${port}`);
    });
  }

  private simulatePacketLoss(): boolean {
    return Math.random() < this.packetLossRate;
  }
}
