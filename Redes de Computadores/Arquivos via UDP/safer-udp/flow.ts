import { logger as pinoLogger } from '../logger.js';
import asciichart from 'asciichart';

const logger = pinoLogger.child({ category: 'FlowManager' });

interface FlowHistoryEntry {
  timestamp: number;
  flow: number;
}

export type { FlowHistoryEntry };

export class FlowManager {
  private ceiling = Infinity;
  private currentFlow = 1;

  private startTime = Date.now();
  private history: FlowHistoryEntry[] = [
    { flow: 1, timestamp: this.startTime },
  ];

  constructor() {
    logger.info('Configurado para SlowStart');

    process.on('SIGINT', () => {
      this.printFlowChart();
      process.exit();
    });
  }

  up() {
    if (this.currentFlow < this.ceiling) {
      this.currentFlow *= 2;
      logger.info(`Crescendo exponencialmente. Novo: ${this.currentFlow}`);
    } else {
      this.currentFlow += 1;
      logger.debug(`Crescendo linearmente. Novo: ${this.currentFlow}`);
    }
  }

  down() {
    this.ceiling = Math.max(Math.floor(this.currentFlow / 2), 1);
    this.currentFlow = this.ceiling;

    logger.info(`Cortando fluxo atual pela metade. Novo: ${this.currentFlow}`);
  }

  getBatch(chunks: number[]) {
    this.history.push({
      timestamp: Date.now() - this.startTime,
      flow: this.currentFlow,
    });

    return chunks.slice(0, Math.min(chunks.length, this.currentFlow));
  }

  getFlowHistory(): FlowHistoryEntry[] {
    return [...this.history];
  }

  printFlowChart(): void {
    if (this.history.length < 2) {
      console.log(
        '\n📊 Gráfico de Fluxo: Dados insuficientes para gerar gráfico\n',
      );
      return;
    }

    const flows = this.history.map((entry) => entry.flow);
    const timestamps = this.history.map((entry) => entry.timestamp);

    const minTime = Math.min(...timestamps);
    const maxTime = Math.max(...timestamps);
    const duration = maxTime - minTime;

    console.log('\n📊 Gráfico de Controle de Fluxo vs Tempo');
    console.log(`Duração: ${duration}ms | Entradas: ${this.history.length}`);
    console.log(
      `Fluxo: Min=${Math.min(...flows)} | Max=${Math.max(...flows)} | Atual=${this.currentFlow}`,
    );

    const chart = asciichart.plot(flows, {
      height: Math.min(40, Math.max(...flows)),
      colors: [asciichart.blue],
      format: (x: number) => x.toFixed(0).padStart(3),
    });

    console.log(chart);
    console.log('Eixo X: Sequência de eventos | Eixo Y: Tamanho do fluxo');
    console.log('');
  }
}
