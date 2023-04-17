#include "Disciplina.h"
#include "Aluno.h"
#include <memory>
#include <vector>

auto Disciplina::getAlunos() -> std::vector<std::shared_ptr<Aluno>> {
  return alunos;
}

auto Disciplina::getDepartamento() -> Departamento * { return departamento; }

void Disciplina::setDepartamento(std::shared_ptr<Departamento> &dpto) {
  departamento = dpto.get();
}

void Disciplina::setDepartamento(Departamento *dpto) { departamento = dpto; }

auto Disciplina::getCurso() -> Curso * { return curso; }

void Disciplina::setCurso(Curso *cur) { curso = cur; }
void Disciplina::setCurso(std::shared_ptr<Curso> &cur) { curso = cur.get(); }
