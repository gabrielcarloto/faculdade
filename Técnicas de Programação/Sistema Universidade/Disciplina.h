#pragma once
#include "Aluno.h"
#include "Base.h"
#include "Departamento.h"
#include "utils.h"

class Disciplina : public Base {
  std::vector<std::shared_ptr<Aluno>> alunos;
  Departamento *departamento;
  Curso *curso;

public:
  Disciplina(const std::string &nome, Departamento *dpto, Curso *cur,
             const std::string uuid = utils::get_uuid())
      : Base(nome, uuid), departamento(dpto), curso(cur) {}

  auto getAlunos() -> std::vector<std::shared_ptr<Aluno>>;
  auto getDepartamento() -> Departamento *;
  void setDepartamento(std::shared_ptr<Departamento> &);
  void setDepartamento(Departamento *dpto);
  auto getCurso() -> Curso *;
  void setCurso(Curso *);
  void setCurso(std::shared_ptr<Curso> &cur);
};
