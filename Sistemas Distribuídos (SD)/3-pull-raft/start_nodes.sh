#!/usr/bin/sh
set -euo pipefail

PROJECT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
MAIN="$PROJECT_DIR/main.py"

PYTHON_BIN="python3.14"

if ! command -v ghostty >/dev/null 2>&1; then
    echo "Erro: Ghostty não encontrado."
    exit 1
fi

if [[ ! -f "$MAIN" ]]; then
    echo "Erro: main.py não encontrado em $PROJECT_DIR"
    exit 1
fi

for id in A B C D; do
    ghostty \
        --working-directory="$PROJECT_DIR" \
        --title="Raft - Node $id" \
        --wait-after-command=true \
        -e "$PYTHON_BIN" "$MAIN" --id "$id" &

done

echo "Cluster iniciado: A, B, C e D."

