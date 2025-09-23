import { logger as pinoLogger } from '../logger.js';
import { SocketManager } from './socket.js';
import { MessageProtocol, type Message } from './protocol.js';
import { SaferUDPConnection, type Remote } from './connection.js';
import { FlowManager } from './flow.js';

const logger = pinoLogger.child({ category: 'SaferUDP' });

export { SaferUDPConnection, type Remote, type Message };

export interface SaferUDPOptions {
  packetLossRate?: number;
  timeoutDelay?: number;
  timeoutEventWindow?: number;
  initialFlowCeiling?: number;
}

export class SaferUDP {
  private socket: SocketManager;
  private protocol = new MessageProtocol();
  private connections = new Map<string, SaferUDPConnection>();
  private flowManager: FlowManager;
  private options: SaferUDPOptions;

  private messageCallback:
    | ((ctx: {
        messages: Message[];
        buffer: Buffer;
        remote: Remote;
        connection: SaferUDPConnection;
      }) => void | Promise<void>)
    | undefined;

  constructor(
    messageCallback: typeof this.messageCallback,
    options: SaferUDPOptions = {},
  ) {
    this.options = options;
    this.socket = new SocketManager(
      options.packetLossRate !== undefined 
        ? { packetLossRate: options.packetLossRate }
        : {}
    );
    this.flowManager = new FlowManager(options.initialFlowCeiling);
    this.messageCallback = messageCallback;

    this.socket.internalSocket.on('message', (msg, remoteInfo) => {
      const remote: Remote = {
        address: remoteInfo.address,
        port: remoteInfo.port,
      };

      this.handleIncomingMessage(this.protocol.parse(msg), remote);
    });

    logger.info(
      `Tamanho máximo de um pacote SaferUDP: ${SocketManager.maxPacketSize - this.protocol.overhead} bytes`,
    );
  }

  private normalizeAddress(address: string): string {
    if (address === 'localhost') {
      return '127.0.0.1';
    }

    return address;
  }

  private getConnectionKey(remote: Remote): string {
    return `${this.normalizeAddress(remote.address)}:${remote.port}`;
  }

  listen(port: number) {
    this.socket.bind(port);
  }

  connect(remote: Remote): SaferUDPConnection {
    return this.getOrCreateConnection(remote);
  }

  async send(data: Buffer, remote: Remote) {
    const connection = this.getOrCreateConnection(remote);
    await connection.send(data);
  }

  private getOrCreateConnection(remote: Remote): SaferUDPConnection {
    const connectionKey = this.getConnectionKey(remote);

    if (!this.connections.has(connectionKey)) {
      const normalizedRemote: Remote = {
        address: this.normalizeAddress(remote.address),
        port: remote.port,
      };

      const connectionOptions: { timeoutDelay?: number; timeoutEventWindow?: number } = {};
      if (this.options.timeoutDelay !== undefined) {
        connectionOptions.timeoutDelay = this.options.timeoutDelay;
      }
      if (this.options.timeoutEventWindow !== undefined) {
        connectionOptions.timeoutEventWindow = this.options.timeoutEventWindow;
      }

      const connection = new SaferUDPConnection(
        normalizedRemote,
        async (ctx) => {
          await this.messageCallback?.({ ...ctx, remote: normalizedRemote });
        },
        () => {
          this.connections.delete(connectionKey);
          logger.info(`Conexão com ${connectionKey} fechada`);
        },
        this.flowManager,
        this.socket,
        connectionOptions,
      );

      this.connections.set(connectionKey, connection);
      logger.info(`Nova conexão criada: ${connectionKey}`);
    }

    return this.connections.get(connectionKey)!;
  }

  private async handleIncomingMessage(message: Message, remote: Remote) {
    const connection = this.getOrCreateConnection(remote);
    await connection.handleMessage(message);
  }
}
