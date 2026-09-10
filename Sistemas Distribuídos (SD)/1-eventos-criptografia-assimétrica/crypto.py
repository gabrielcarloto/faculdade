from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.primitives import hashes, serialization
from pathlib import Path
import base64
import pika


def generate_keys(ms_name):
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

    with open(ms_name + "/private_key.pem", "wb") as f:
        f.write(private_pem)

    with open("./keys/" + ms_name + "_public.pem", "wb") as f:
        f.write(public_pem)


def get_keyring(ms_name):
    Path("keys").mkdir(parents=True, exist_ok=True)
    Path(ms_name).mkdir(parents=True, exist_ok=True)
    keys_dir = Path("keys")

    public = {}

    for key_path in keys_dir.glob("*_public.pem"):
        sender_id = key_path.stem.replace("_public", "")
        public[sender_id] = serialization.load_pem_public_key(key_path.read_bytes())

    private_path = Path(f"{ms_name}/private_key.pem")
    private = serialization.load_pem_private_key(
        private_path.read_bytes(),
        password=None,
    )

    return {"public": public, "private": private}


def get_signed_props(sender, private_key, body):
    signature = private_key.sign(
        body,
        padding.PSS(
            mgf=padding.MGF1(hashes.SHA256()), salt_length=padding.PSS.MAX_LENGTH
        ),
        hashes.SHA256(),
    )

    signature_b64 = base64.b64encode(signature).decode("utf-8")

    return pika.BasicProperties(
        headers={"X-Signature": signature_b64, "X-Sender": sender}, delivery_mode=2
    )


def verify_message(public_key, properties, body):
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
