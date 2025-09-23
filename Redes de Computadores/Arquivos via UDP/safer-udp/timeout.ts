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
      maxRetries: number;
    }
  >();

  static readonly DEFAULT_DELAY = 250;
  private defaultDelay: number;
  private maxRetries: number;
  private onMaxRetriesCallback?: () => void;

  constructor(
    defaultDelay: number = TimeoutManager.DEFAULT_DELAY,
    maxRetries: number = 10,
  ) {
    this.defaultDelay = defaultDelay;
    this.maxRetries = maxRetries;
  }

  get delay() {
    return this.defaultDelay;
  }

  setMaxRetriesCallback(callback: () => void) {
    this.onMaxRetriesCallback = callback;
  }

  static clear(timeout: OptionalTimeout) {
    if (timeout) clearTimeout(timeout);
    return null;
  }

  cleanup() {
    Array.from(this.timeouts.keys()).forEach((id) => this.timeouts.delete(id));
  }

  set(id: TimeoutMapKey, callback: TimeoutCallback, ms = this.defaultDelay) {
    const timeout = this.timeouts.get(id);
    if (timeout?.id) clearTimeout(timeout.id);

    this.timeouts.set(id, {
      retries: 0,
      callback,
      baseDelay: ms,
      timedOut: false,
      maxRetries: this.maxRetries,
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

    if (timeout.retries >= timeout.maxRetries) {
      this.timeouts.delete(id);
      this.onMaxRetriesCallback?.();
      return;
    }

    const delay = this.calculateTimeout(timeout.baseDelay, timeout.retries);

    timeout.id = setTimeout(() => {
      timeout.id = null;
      timeout.timedOut = true;
      timeout.callback();
    }, delay);
  }

  getRetries(id: TimeoutMapKey): number {
    return this.timeouts.get(id)?.retries || 0;
  }

  getMaxRetries(id: TimeoutMapKey): number {
    return this.timeouts.get(id)?.maxRetries || this.maxRetries;
  }

  hasTimedOut(id: TimeoutMapKey) {
    return Boolean(this.timeouts.get(id)?.timedOut);
  }

  private calculateTimeout(baseDelay: number, retries: number) {
    return baseDelay * (retries + 1);
  }
}
