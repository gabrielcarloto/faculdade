# Padrões Web

Quatro páginas estáticas em HTML/CSS, com JS (ou TS) no cliente

## Projetos

| Projeto | Stack | Descrição |
| --- | --- | --- |
| [form-musical](form-musical/) | HTML + CSS + TS/JS | Formulário "incrivelmente musical" com três `fieldset` (Identificação pessoal, Estilos Musicais favoritos, Álbuns favoritos), `legend`, `aria-describedby` e `aria-required`; `script.ts` valida nome, e-mail, telefone e data no `blur` e faz `preventDefault` no envio |
| [primeira página](primeira%20página/) | HTML + CSS | "Situação do curso BSI de Gabriel Carloto": relatório com período, carga horária, CR, disciplinas concluídas e obrigatórias faltantes, com sumário em âncoras |
| [ranking](ranking/) | HTML + CSS | "Rank UX": `<ol>` com 5 críticas à UX da web moderna (User Inyerface, Bad UX Game, How I Experience Web Today, Cookie Consent Speed Run, Motherfucking Website), capturas em `<picture>` e ícones SVG |
| [tabela](tabela/) | HTML + CSS + TS/JS | "Álbuns musicais mais vendidos": tabela de 50 linhas reordenável por coluna pelos botões do `thead` (`aria-sort`, `data-type` numérico ou texto); sem JS, o `<noscript>` avisa que a ordenação fica indisponível |
