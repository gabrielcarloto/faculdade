import yargs from 'yargs';
import { hideBin } from 'yargs/helpers';

export interface SaferUDPConfig {
  packetLossRate: number;
  timeoutDelay: number;
  timeoutEventWindow: number;
  initialFlowCeiling: number;
  maxRetries: number;
  debug: boolean;
}

export interface ClientConfig extends SaferUDPConfig {
  port: number;
  address: string;
  file: string;
  outFile: string | undefined;
}

export interface ServerConfig extends SaferUDPConfig {
  port: number;
}

const saferUDPOptions = {
  'packet-loss-rate': {
    type: 'number' as const,
    describe: 'Taxa de simulação de perda de pacotes (0.0 a 1.0)',
    default: 0.0025,
  },
  'timeout-delay': {
    type: 'number' as const,
    describe: 'Delay base para timeouts em ms',
    default: 250,
  },
  'timeout-event-window': {
    type: 'number' as const,
    describe: 'Janela de eventos de timeout em ms',
    default: 10,
  },
  'initial-flow-ceiling': {
    type: 'number' as const,
    describe: 'Teto inicial do controle de fluxo',
    default: Infinity,
  },
  'max-retries': {
    type: 'number' as const,
    describe: 'Número máximo de tentativas antes de fechar a conexão',
    default: 10,
  },
  debug: {
    alias: 'd',
    type: 'boolean' as const,
    describe: 'Habilitar logs de debug',
    default: false,
  },
};

export async function parseClientConfig(): Promise<ClientConfig> {
  const argv = await yargs(hideBin(process.argv))
    .option('port', {
      alias: 'p',
      type: 'number',
      describe: 'Porta do servidor',
      demandOption: true,
    })
    .option('address', {
      alias: 'a',
      type: 'string',
      describe: 'Endereço do servidor',
      default: 'localhost',
    })
    .option('file', {
      alias: 'f',
      type: 'string',
      describe: 'Nome do arquivo a ser requisitado',
      demandOption: true,
    })
    .option('outFile', {
      alias: 'o',
      type: 'string',
      describe: 'Saída do arquivo requisitado (padrão: mesmo nome do arquivo)',
    })
    .options(saferUDPOptions)
    .help()
    .alias('help', 'h').argv;

  return {
    port: argv.port,
    address: argv.address,
    file: argv.file,
    outFile: argv.outFile,
    packetLossRate: argv['packet-loss-rate'],
    timeoutDelay: argv['timeout-delay'],
    timeoutEventWindow: argv['timeout-event-window'],
    initialFlowCeiling: argv['initial-flow-ceiling'],
    maxRetries: argv['max-retries'],
    debug: argv.debug,
  };
}

export async function parseServerConfig(): Promise<ServerConfig> {
  const argv = await yargs(hideBin(process.argv))
    .option('port', {
      alias: 'p',
      type: 'number',
      describe: 'Porta para escutar',
      default: 3000,
    })
    .options(saferUDPOptions)
    .help()
    .alias('help', 'h').argv;

  return {
    port: argv.port,
    packetLossRate: argv['packet-loss-rate'],
    timeoutDelay: argv['timeout-delay'],
    timeoutEventWindow: argv['timeout-event-window'],
    initialFlowCeiling: argv['initial-flow-ceiling'],
    maxRetries: argv['max-retries'],
    debug: argv.debug,
  };
}
