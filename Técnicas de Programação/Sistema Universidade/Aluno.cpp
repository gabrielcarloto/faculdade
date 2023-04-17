#include "Aluno.h"

Aluno::Aluno(const std::string nome, DataNascimento &nascimento,
             std::shared_ptr<Universidade> &universidade,
             std::shared_ptr<Curso> &curso, const int matricula,
             std::string uuid)
    : Pessoa(nome, nascimento, uuid) {
  this->universidade = universidade;
  this->curso = curso;
  this->matricula = matricula;
  this->periodo = 0;
}

Aluno::Aluno(Pessoa &pessoa, std::shared_ptr<Universidade> &universidade,
             std::shared_ptr<Curso> &curso, const int matricula)
    : Pessoa(pessoa) {
  this->universidade = universidade;
  this->curso = curso;
  this->matricula = matricula;
  this->periodo = 0;
}
