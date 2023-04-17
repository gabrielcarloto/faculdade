#pragma once
#include "Aluno.h"
#include "Base.h"
#include "Disciplina.h"
#include <memory>

class Universidade;
class Aluno;
class Departamento;

class Curso : public Base {
  Departamento *departamento;
  PointerVector<Disciplina> disciplinas;
  PointerVector<Aluno> alunos;

public:
  Curso(const std::string &, Departamento *, std::string = utils::get_uuid());
  ~Curso();

  auto criaDisciplina(const std::string &nome) -> std::shared_ptr<Disciplina>;
  auto getDisciplinas() -> PointerVector<Disciplina>;
  auto getAlunos() -> PointerVector<Aluno> &;
  void adicionaAluno(std::shared_ptr<Aluno>);
  void adicionaDisciplina(std::shared_ptr<Disciplina>);
};
