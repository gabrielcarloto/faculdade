#include "Base.h"
#include <utility>

Base::Base(const std::string nome, std::string uuid) : id(std::move(uuid)) {
  this->nome = nome;
}

void Base::setNome(const std::string nome) { this->nome = nome; }

auto Base::getNome() -> const std::string { return nome; }
