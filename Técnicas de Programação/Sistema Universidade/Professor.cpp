#include "Professor.h"

auto Professor::getDepartamento() -> std::shared_ptr<Departamento> {
  return departamento;
}

void Professor::setDepartamento(std::shared_ptr<Departamento> &dpto) {
  departamento = dpto;
}
