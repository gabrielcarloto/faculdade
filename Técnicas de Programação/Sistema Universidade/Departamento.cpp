#include "Departamento.h"
#include "Universidade.h"
#include "utils.h"
#include <memory>

Departamento::Departamento(const std::string nome, Universidade *universidade)
    : Base(nome) {
  this->universidade = universidade;
}

Departamento::~Departamento() { cursos.clear(); }

auto Departamento::getCursos() -> PointerVector<Curso> { return cursos; }

auto Departamento::criaCurso(std::string nome) -> std::shared_ptr<Curso> {
  cursos.push_back(std::make_shared<Curso>(nome, this));
  return cursos.back();
}
