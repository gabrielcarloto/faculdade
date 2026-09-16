import crypto

services = ["principal", "estoque", "pagamento", "entrega", "promocoes"]


def main():
    for name in services:
        crypto.generate_keys(name)


if __name__ == "__main__":
    main()
