#Neste exemplo de Makefile, os arquivos objetos (.o) são gerados separadamente, assim, somente arquivos fonte (.c) alterados são recompilados para gerar um novo .o. Então o novo .o é linkado com os outros .o (dos arquivos não alterados) para gerar o executável. O make sabe o que deve ser recompilado com base nos timestamps dos arquivos fonte e seus respectivos arquivos objeto.


#cria a variável CC para o nome do compilador utilizado (gcc)
CC = g++

#Define o nome para o arquivo executável (saida) gerado após a compilação
EXEC = saida

#Cria variável SRC com todos os arquivos fonte (.c) presentes do diretório atual
SRC = $(wildcard *.cpp)

#Cria variável OBJ com todos os arquivos .o criados a partir de cada arquivo .c
OBJ = $(SRC:.cpp=.o)


######### Sintaxe ###########
#regra: dependências
#<TAB>	comando	
#############################

#================
#A regra "all" é usada para construir o sistema de forma incremental 
all: $(EXEC)
	@echo " "
	@echo "Copilação concluída!!"
	@echo " "

#================
#A regra $(EXEC): tem como dependências os arquivos objeto (.o), ou seja, para criar o executavel saida precisamos dos 4 arquivos .o (progPrincipal.o, mult.c soma.o, par.o)
# $@ expande a regra-alvo; neste caso saida
# $^ expande as dependências da regra; neste exemplo os 4 arquivos .o 
#O comando cria o executável ($@ regra-alvo) a partir dos arquivos .o ($^ dependências). Equivalente à gcc -o saida progPrincipal.o mult.o soma.o par.o
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ 

#================
#A regra %.o: (ou seja, a regra para cada um dos arquivos .o) tem como dependências os arquivos .c (%c)
#O comando gera os arquivos .o ($@ regra-alvo) a partir de seus respectivos arquivos .c ($^ dependências). 
%.o: %.c
	$(CC) -o $@ -c $^ 

#================
#A regra clean: apaga os arquivos .o ao digitarmos make clean na console
clean: 
	rm -f *.o

#================
#A regra execClean: apaga o arquivo executável ao digitarmos make cleanExec na console	
cleanExec:
	rm -f $(EXEC)
