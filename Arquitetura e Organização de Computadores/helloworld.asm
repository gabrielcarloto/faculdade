section .data 
  message db "Olá bundo!!"
  len equ $- message

global _start
section .text 

_start:
  ; printar a mensagem
  mov rax, 1 ; write syscall
  mov rdi, 1 ; stdout
  mov rsi, message 
  mov rdx, len
  syscall

  ; exit
  mov rax, 60 ; exit syscall
  mov rdi, 0 ; codigo de erro
  syscall
