#pragma once
#include "Departamento.h"
#include "Pessoa.h"
#include "Universidade.h"
#include <memory>
#include <utility>

class Professor : public Trabalhador<Universidade> {
  std::shared_ptr<Departamento> departamento;

public:
  Professor(DataNascimento &nasc, const std::string &nome,
            std::shared_ptr<Universidade> &univ,
            std::shared_ptr<Departamento> &dpto,
            std::string uuid = utils::get_uuid())
      : Trabalhador<Universidade>(univ, nome, nasc, uuid),
        departamento(std::move(dpto)) {}

  Professor(Pessoa &p, std::shared_ptr<Universidade> &univ,
            std::shared_ptr<Departamento> &dpto)
      : Trabalhador<Universidade>(univ, p), departamento(std::move(dpto)) {}

  auto getDepartamento() -> std::shared_ptr<Departamento>;
  void setDepartamento(std::shared_ptr<Departamento> &);
};
