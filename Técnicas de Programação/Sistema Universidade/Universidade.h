#pragma once
#include "Base.h"
#include "Curso.h"
#include "Departamento.h"
#include <iostream>
#include <memory>
#include <vector>

class Universidade : public Base {
  PointerVector<Departamento> departamentos;

public:
  Universidade(const std::string nome = "");
  ~Universidade();

  auto getDepartamentos() -> PointerVector<Departamento> &;
  auto criaDepartamento(std::string nome) -> std::shared_ptr<Departamento>;
  void addDepartamento(std::shared_ptr<Departamento> d);
};
