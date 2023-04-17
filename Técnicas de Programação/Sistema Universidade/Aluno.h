#pragma once
#include "Pessoa.h"
#include <memory>

class Universidade;
class Curso;

class Aluno : public Pessoa {
  std::shared_ptr<Universidade> universidade;
  std::shared_ptr<Curso> curso;
  int periodo, matricula;

public:
  Aluno(const std::string, DataNascimento &, std::shared_ptr<Universidade> &,
        std::shared_ptr<Curso> &, const int, std::string = utils::get_uuid());

  Aluno(Pessoa &, std::shared_ptr<Universidade> &, std::shared_ptr<Curso> &,
        const int);
};

template <class C> class Estagiario : public Aluno {

public:
  Estagiario(const std::string nome, DataNascimento &nascimento,
             std::shared_ptr<Universidade> &universidade,
             std::shared_ptr<Curso> &curso, const int matricula,
             std::string uuid = utils::get_uuid())
      : Aluno(nome, nascimento, universidade, curso, matricula, uuid) {}

  Estagiario(Pessoa &pessoa, std::shared_ptr<Universidade> &universidade,
             std::shared_ptr<Curso> &curso, const int matricula)
      : Aluno(pessoa, universidade, curso, matricula){};

  Estagiario(Aluno &aluno) : Aluno(aluno){};
};
