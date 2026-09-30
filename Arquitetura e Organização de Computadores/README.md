# Arquitetura e Organização de Computadores

Código em Assembly NASM x86-64.

## Arquivos

- `helloworld.asm` — hello world: imprime uma string via syscall `write` e sai via syscall `exit`
- `fibonacci.asm` — lê um número do stdin, converte com sub-rotina `atoi`, calcula fibonacci e imprime com sub-rotina `print`
- `Makefile` — build do `fibonacci.asm`
