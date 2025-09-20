import dgram from 'node:dgram';
import { logger as pinoLogger } from '../logger.js';
import type { Remote } from './connection.js';

const logger = pinoLogger.child({ category: 'SocketManager' });

const DEFAULT_PACKET_LOSS_RATE = 0.0025;

export class SocketManager {
  private socket = dgram.createSocket('udp4');
  private packetLossRate: number = DEFAULT_PACKET_LOSS_RATE;
  private isListening = false;

  constructor(options: { packetLossRate?: number } = {}) {
    this.packetLossRate = options.packetLossRate ?? DEFAULT_PACKET_LOSS_RATE;
    logger.info(
      'Simulação de perda de pacotes configurada com chance de ' +
        this.packetLossRate * 100 +
        '%',
    );

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

  send(msg: string | NodeJS.ArrayBufferView | readonly any[], to: Remote) {
    return new Promise<undefined>((resolve, reject) => {
      if (this.simulatePacketLoss()) {
        reject(new Error('Pacote perdido (simulação)'));
      }

      this.socket.send(msg, to.port, to.address, (error) => {
        if (error) {
          return reject(error);
        }

        resolve(undefined);
      });
    });
  }

  bind(port: number) {
    this.socket.bind(port, () => {
      this.isListening = true;
      logger.info(`Escutando na porta: ${port}`);
    });
  }

  get listening() {
    return this.isListening;
  }

  private simulatePacketLoss(): boolean {
    return Math.random() < this.packetLossRate;
  }
}
