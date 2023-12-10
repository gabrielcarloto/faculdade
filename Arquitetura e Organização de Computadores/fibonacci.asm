; compilador online: https://rextester.com/l/nasm_online_compiler

; constantes
section .data 
  msg_inicial: db "Digite o número: ",0 
  msg_inicial_len: equ $- msg_inicial

  msg_resultado: db "Resultado: "
  msg_resultado_len: equ $- msg_resultado

  input_len: equ 100
  output_len: equ 100

; alocação de memória para uso futuro
section .bss
  input resb 100
  output resb 100
  n resq 1

global _start
section .text 
_start:
  ; printa msg inicial
  mov rsi, msg_inicial
  mov rdx, msg_inicial_len
  call print

  ; pede o número para o usuário e converte para int
  call getN
  call atoi

  fibonacci:
    mov rax, 0 ; a = 0
    mov rbx, 1 ; b = 1
    mov rcx, 2 ; i = 2
    mov rdx, 1 ; result = 1

    loop_fib:
      ; se i > n, sai do loop
      cmp rcx, [n]
      jg end_loop_fib 

      mov rdx, rax ; result = a
      add rdx, rbx ; result += b
      mov rax, rbx ; a = b
      mov rbx, rdx ; b = result

      inc rcx ; i++
      jmp loop_fib ; pula para loop 
      
end_loop_fib:
  ; converte resultado para string
  mov rdi, output
  mov rsi, rdx
  mov rdx, output_len
  call itoa

  ; printa msg resultado
  mov rsi, msg_resultado
  mov rdx, msg_resultado_len
  call print

  ; printa o o n-ésimo número da sequência
  mov rsi, output
  mov rdx, output_len
  call print

exit:
  mov rax, 60 ; syscall exit
  mov rdi, 0 ; código de erro 0
  syscall
  ret

getN:
  mov rax, 0 ; syscall read
  mov rdi, 0 ; stdin
  mov rsi, input ; endereço do buffer
  mov rdx, input_len ; tamanho do buffer
  syscall

  ret

; printa uma mensagem na tela
; rsi -> char *; rdx -> int (tamanho da string) 
print:
  mov rax, 1 ; syscall write
  mov rdi, 1 ; stdout
  syscall
  ret

; converte string para número
; rsi -> char *
atoi:
  xor rax, rax ; result = 0

  atoi_loop: 
    movzx rcx, byte [rsi] ; pega o char atual
    inc rsi ; aponta para o próximo char

    ; char < '0' || > '9' termina a conversão
    cmp rcx, '0' 
    jb end_convert 
    cmp rcx, '9' 
    ja end_convert 

    sub rcx, '0' ; conversão de ASCII para número
    imul rax, 10 ; result *= 10
    add rax, rcx ; result += *str - '0'

    jmp atoi_loop ; continuar o loop

  end_convert: 
    mov [n], rax ; resultado em n
    ret

; converte int para string
; rsi -> int; rdi -> char *; rdx -> int (tamanho da string)
itoa:
  mov rax, rsi ; aux = n
  mov rbx, rdi ; str_aux = str_ptr
  mov rcx, 10 ; base = 10

  add rbx, rdx ; str_aux += size
  mov byte [rbx], 0 ; *str_aux = '\0'

  reverseLoop:
    dec rbx ; (*str_aux)--
    xor rdx, rdx ; resto = 0
    div rcx ; aux /= 10; resto = aux % 10
    add dl, '0' ; resto += '0' 
                ; dl é o subregistrador de 8 bits dentro do rdx
                ; ele é utilizado aqui porque cada caractere é um byte
    mov [rbx], dl ; *str_aux = resto

    ; aux != 0 continua o loop
    test rax, rax
    jnz reverseLoop

  ret