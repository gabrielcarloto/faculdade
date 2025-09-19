import { logger as pinoLogger } from '../logger.js';
import { SocketManager } from './socket.js';
import { MessageProtocol, type Message } from './protocol.js';
import { SaferUDPConnection, type Remote } from './connection.js';
import { FlowManager } from './flow.js';

const logger = pinoLogger.child({ category: 'SaferUDP' });

export { SaferUDPConnection, type Remote, type Message };

export class SaferUDP {
  private socket: SocketManager;
  private protocol = new MessageProtocol();
  private connections = new Map<string, SaferUDPConnection>();
  private flowManager = new FlowManager();

  private messageCallback:
    | ((ctx: { messages: Message[]; buffer: Buffer; remote: Remote }) => void)
    | undefined;

  constructor(
    messageCallback: typeof this.messageCallback,
    options: { packetLossRate?: number } = {},
  ) {
    this.socket = new SocketManager(options);
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

      const connection = new SaferUDPConnection(
        normalizedRemote,
        (ctx) => {
          this.messageCallback?.({ ...ctx, remote: normalizedRemote });
        },
        this.flowManager,
        this.socket,
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
