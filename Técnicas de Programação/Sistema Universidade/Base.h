#pragma once
#include "utils.h"
#include <iostream>
#include <memory>
#include <vector>

template <class C> using PointerVector = std::vector<std::shared_ptr<C>>;

class Base {
protected:
  std::string nome;
  const std::string id;

public:
  Base(const std::string nome, std::string uuid = utils::get_uuid());

  void setNome(const std::string nome);
  auto getNome() -> const std::string;
};
