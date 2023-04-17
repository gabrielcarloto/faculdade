#include "Universidade.h"
#include "Base.h"
#include "utils.h"
#include <iostream>
#include <memory>
#include <utility>

Universidade::Universidade(const std::string nome) : Base(nome) {}

Universidade::~Universidade() { departamentos.clear(); }

auto Universidade::criaDepartamento(std::string nome)
    -> std::shared_ptr<Departamento> {
  departamentos.push_back(std::make_shared<Departamento>(nome, this));
  return departamentos.back();
}

auto Universidade::getDepartamentos() -> PointerVector<Departamento> & {
  return departamentos;
}

void Universidade::addDepartamento(std::shared_ptr<Departamento> d) {
  departamentos.push_back(d);
}
