# Desenvolvimento Integrado de Sistemas

Disciplina de construção de software completo, de ponta a ponta. Projeto em Go (módulo `conjugate_gradient`).

Solver HTTP de reconstrução iterativa de imagem por gradiente conjugado (algoritmos `CGNE` e `CGNR`, com `gonum/blas`): recebe o sinal e um modelo de matriz, devolve a imagem. Inclui cache de modelos com reserva/liberação por memória, agendador assíncrono com fila de prioridade, controle de memória (cgroup) e profiling de recursos.