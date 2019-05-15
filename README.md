O procedimento para compilar este compilador para arquitetura x86 através da MAKEFILE presente na pasta raiz é:

make clean - para remover todos os arquivos da compilação anterior, inclusive códigos em C do flex e do yacc

make bison - para gerar os códigos em C do bison que são requisitos para o do flex funcionar

make - irá gerar o código em C do flex e compilar todos os demais


OBS: O MAKEFILE foi codificado para compilar utilizando MacOS 10.13.6, a biblioteca requisitada para o flex no MacOS é linkada através de -ll
diferente do Linux que é -lfl, talvez seja necessário realizar a alteração no arquivo MAKEFILE.
