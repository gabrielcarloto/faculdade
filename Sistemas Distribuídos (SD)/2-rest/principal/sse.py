import asyncio
import json
import threading

PING_INTERVAL = 15

# conexões abertas (uma por aba)
connections: list[tuple[asyncio.AbstractEventLoop, asyncio.Queue]] = []
connections_lock = threading.Lock()


def notify(event: str, data: dict) -> None:
    with connections_lock:
        targets = list(connections)

    for loop, queue in targets:
        try:
            loop.call_soon_threadsafe(queue.put_nowait, (event, data))
        except RuntimeError:
            pass


async def stream():
    conn = (asyncio.get_running_loop(), asyncio.Queue())

    with connections_lock:
        connections.append(conn)

    try:
        yield "retry: 3000\n\n"

        while True:
            try:
                event, data = await asyncio.wait_for(conn[1].get(), PING_INTERVAL)
            except asyncio.TimeoutError:
                yield ": ping\n\n"
                continue

            yield f"event: {event}\ndata: {json.dumps(data, ensure_ascii=False)}\n\n"
    finally:
        with connections_lock:
            connections.remove(conn)
