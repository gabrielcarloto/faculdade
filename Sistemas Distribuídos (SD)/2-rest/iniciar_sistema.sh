#!/bin/bash

# Caminhos base do projeto e do ambiente virtual
PROJ_DIR="$HOME/Projetos/2-rest"
VENV_PATH="$HOME/Projetos/venv/bin/activate"

# 1. Ativar o venv e gerar as chaves na pasta raiz
source "$VENV_PATH"
cd "$PROJ_DIR"
python3 -m gerar_chaves

# 2. Comando base executado em cada guia
CMD_BASE="source '$VENV_PATH' && cd '$PROJ_DIR'"

# Função para abrir cada serviço em uma aba separada
abrir_aba() {
  local titulo="$1"
  local comando="$2"
  gnome-terminal --tab --title="$titulo" -- bash -c "$CMD_BASE && $comando; exec bash"
}

# 3. Abrir cada microsserviço e o frontend
abrir_aba "Principal"      "python3 -m principal.main"
abrir_aba "Estoque"        "python3 -m estoque.main"
abrir_aba "Mock Pagamento" "python3 -m mock_pagamento.main"
abrir_aba "Pagamento"      "python3 -m pagamento.main"
abrir_aba "Entrega"        "python3 -m entrega.main"
abrir_aba "Promoções"      "python3 -m promocoes.main"
abrir_aba "Frontend HTTP"  "python3 -m http.server 8089"
