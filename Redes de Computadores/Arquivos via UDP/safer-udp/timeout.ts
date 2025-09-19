type TimeoutMapKey = number | string;
type TimeoutCallback = () => void;
export type OptionalTimeout = NodeJS.Timeout | null;

export class TimeoutManager {
  private timeouts = new Map<
    TimeoutMapKey,
    {
      id: OptionalTimeout;
      callback: TimeoutCallback;
      baseDelay: number;
      retries: number;
      timedOut: boolean;
    }
  >();

  constructor() {}

  static readonly DEFAULT_DELAY = 250;

  static clear(timeout: OptionalTimeout) {
    if (timeout) clearTimeout(timeout);
    return null;
  }

  set(
    id: TimeoutMapKey,
    callback: TimeoutCallback,
    ms = TimeoutManager.DEFAULT_DELAY,
  ) {
    const timeout = this.timeouts.get(id);
    if (timeout?.id) clearTimeout(timeout.id);

    this.timeouts.set(id, {
      retries: 0,
      callback,
      baseDelay: ms,
      timedOut: false,
      id: setTimeout(() => {
        const current = this.timeouts.get(id);

        if (current) {
          this.timeouts.set(
            id,
            Object.assign(current, { id: null, timedOut: true }),
          );
        }

        callback();
      }, ms),
    });
  }

  remove(id: TimeoutMapKey) {
    const timeout = this.timeouts.get(id);

    this.timeouts.delete(id);

    if (timeout?.id) {
      clearTimeout(timeout?.id);
    }
  }

  retry(id: TimeoutMapKey) {
    const timeout = this.timeouts.get(id);
    if (!timeout) return;

    timeout.retries++;
    timeout.timedOut = false;

    const delay = this.calculateTimeout(timeout.baseDelay, timeout.retries);

    timeout.id = setTimeout(() => {
      timeout.id = null;
      timeout.timedOut = true;
      timeout.callback();
    }, delay);
  }

  hasTimedOut(id: TimeoutMapKey) {
    return Boolean(this.timeouts.get(id)?.timedOut);
  }

  private calculateTimeout(baseDelay: number, retries: number) {
    return baseDelay * (retries + 1);
  }
}
