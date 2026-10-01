import json
import crypto


def publish(payload, channel, ms_name, keyring, exchange, routing_key):
    body = json.dumps(payload).encode("utf-8")
    props = crypto.get_signed_props(ms_name, keyring["private"], body)
    channel.basic_publish(exchange, routing_key, body, properties=props)
