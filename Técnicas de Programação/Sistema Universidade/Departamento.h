#pragma once
#include "Base.h"

class Universidade;
class Disciplina;
class Curso;

class Departamento : public Base {
  Universidade *universidade;
  PointerVector<Curso> cursos;

  friend Curso;

public:
  Departamento(const std::string nome, Universidade *universidade);
  ~Departamento();

  auto getCursos() -> PointerVector<Curso>;
  auto criaCurso(std::string nome) -> std::shared_ptr<Curso>;
};
