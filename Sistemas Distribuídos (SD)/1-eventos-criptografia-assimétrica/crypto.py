from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.primitives import hashes, serialization
from pathlib import Path
from typing import TypedDict, cast
import base64
import pika


class Keyring(TypedDict):
    public: dict[str, rsa.RSAPublicKey]
    private: rsa.RSAPrivateKey


def generate_keys(ms_name: str):
    private_path = ms_name + "/private_key.pem"
    public_path = "./keys/" + ms_name + "_public.pem"

    if Path(private_path).exists() and Path(public_path).exists():
        return

    Path("keys").mkdir(parents=True, exist_ok=True)
    Path(ms_name).mkdir(parents=True, exist_ok=True)

    private_key = rsa.generate_private_key(
        public_exponent=65537,
        key_size=2048,
    )

    private_pem = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.PKCS8,
        encryption_algorithm=serialization.NoEncryption(),
    )

    public_pem = private_key.public_key().public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo,
    )

    with open(private_path, "wb") as f:
        _ = f.write(private_pem)

    with open(public_path, "wb") as f:
        _ = f.write(public_pem)


def get_keyring(ms_name: str) -> Keyring:
    Path("keys").mkdir(parents=True, exist_ok=True)
    Path(ms_name).mkdir(parents=True, exist_ok=True)
    keys_dir = Path("keys")

    public: dict[str, rsa.RSAPublicKey] = {}

    for key_path in keys_dir.glob("*_public.pem"):
        sender_id = key_path.stem.replace("_public", "")
        public[sender_id] = cast(
            rsa.RSAPublicKey,
            serialization.load_pem_public_key(key_path.read_bytes()),
        )

    private_path = Path(f"{ms_name}/private_key.pem")
    private = cast(
        rsa.RSAPrivateKey,
        serialization.load_pem_private_key(
            private_path.read_bytes(),
            password=None,
        ),
    )

    return {"public": public, "private": private}


def get_signed_props(sender: str, private_key: rsa.RSAPrivateKey, body: bytes):
    signature = private_key.sign(
        body,
        padding.PSS(
            mgf=padding.MGF1(hashes.SHA256()), salt_length=padding.PSS.MAX_LENGTH
        ),
        hashes.SHA256(),
    )

    signature_b64 = base64.b64encode(signature).decode("utf-8")

    return pika.BasicProperties(
        headers={"X-Signature": signature_b64, "X-Sender": sender},
    )


def verify_message(keyring: Keyring, properties: pika.BasicProperties, body: bytes):
    headers = getattr(properties, "headers", None) or {}
    sender = headers.get("X-Sender")

    if not sender:
        return False

    public_key = keyring["public"][sender]

    if public_key is None:
        return False

    signature_b64 = (
        properties.headers.get("X-Signature") if properties.headers else None
    )

    if not signature_b64:
        return False

    try:
        signature = base64.b64decode(signature_b64)

        public_key.verify(
            signature,
            body,
            padding.PSS(
                mgf=padding.MGF1(hashes.SHA256()),
                salt_length=padding.PSS.MAX_LENGTH,
            ),
            hashes.SHA256(),
        )

        return True
    except Exception:
        return False
