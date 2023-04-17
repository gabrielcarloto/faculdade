#include "Aluno.h"
#include "Pessoa.h"
#include "Universidade.h"
#include <cstdlib>
#include <memory>

int main() {
  DataNascimento nascimentoTeste = {11, 04, 2000};
  Pessoa Teste("Gabriel", nascimentoTeste);
  std::shared_ptr<Universidade> UTFPR = std::make_shared<Universidade>("UTFPR");

  auto DAINF = UTFPR->criaDepartamento("DAINF");
  auto BSI = DAINF->criaCurso("BSI");
  BSI->adicionaAluno(std::make_shared<Aluno>(Teste, UTFPR, BSI, 15654));

  auto &alunosDeBSI = BSI->getAlunos();

  for (auto &alunoDeBSI : alunosDeBSI) {
    alunoDeBSI->imprimeDados();
  }

  return 0;
}
