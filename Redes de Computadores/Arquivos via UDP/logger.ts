import pino from 'pino';
import pretty from 'pino-pretty';
import { cyan, blue, magenta, yellow, gray } from 'colorette';

const categoryColors: Record<string, (msg: string) => string> = {
  Cliente: cyan,
  Servidor: blue,
  SocketManager: magenta,
  SaferUDP: yellow,
};

const start = Date.now();

export const logger = pino(
  { level: 'debug' },
  pretty({
    colorize: true,
    ignore: 'category,time,pid,hostname',
    messageFormat: (log, messageKey) => {
      const rawCategory = log.category as string | undefined;

      const colorize = rawCategory
        ? (categoryColors[rawCategory] ?? ((s) => s))
        : (s: string) => s;

      const category = rawCategory ? colorize(`[${rawCategory}] `) : '';

      const delta = Date.now() - start;
      const relTime = formatRelativeTime(delta);

      return `\t${gray(`[${relTime}]`)}\t${category}${log[messageKey]}`;
    },
  }),
);

function formatRelativeTime(ms: number) {
  const seconds = Math.floor(ms / 1000);
  const milliseconds = ms % 1000;

  return `${seconds}.${String(milliseconds).padStart(3, '0')}`;
}
