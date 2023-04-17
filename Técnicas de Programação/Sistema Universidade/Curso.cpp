#include "Curso.h"
#include "utils.h"
#include <memory>

Curso::Curso(const std::string &nome, Departamento *dpto, std::string uuid)
    : Base(nome, uuid) {
  departamento = dpto;
}

Curso::~Curso() { alunos.clear(); }

void Curso::adicionaAluno(std::shared_ptr<Aluno> aluno) {
  alunos.push_back(aluno);
}

void Curso::adicionaDisciplina(std::shared_ptr<Disciplina> disc) {
  disciplinas.push_back(disc);
}

auto Curso::criaDisciplina(const std::string &nome)
    -> std::shared_ptr<Disciplina> {
  disciplinas.push_back(std::make_shared<Disciplina>(nome, departamento, this));
  return disciplinas.back();
}

auto Curso::getAlunos() -> PointerVector<Aluno> & { return this->alunos; }
