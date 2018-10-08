/* 
  Trabalho de Estrutura de Dados
  Recordes Bizarros 
  Copyright 2017 by Lorena Tavares, Rodrigo Herculano, William Coelho 
  objeto.hpp: contem as classe e todos os metodos.
  metodos.cpp: implementacao de todas os metodos.
  main.cpp: implementacao do programa principal.
*/

#ifndef OBJETO_HPP
#define OBJETO_HPP

#include <fstream>

using namespace std;

class Objeto { // classe contendo as informacoes de determinado recorde
    friend class Sequencia;
    int id; 
    char nome[50]; 
    char descricao[200]; 
    char responsavel[100];
    int anoDoRecorde;
    char local[30]; 
};

class CabecalhoArquivo { // classe contendo o cabecalho do arquivo
    friend class Sequencia;
    private:
        int qtdPedacos; // quantidade de pedacos utilizados dentro do arquivo
        int priSequencia; // indice da primeira sequencia no arquivo
        int proxPedacoASerUtilizado; // proximo pedaco a ser utilizado dentro do arquivo
        int proxPedacoASerReutilizado; // pcaso uma sequencia seja inteiramente removida, ela é marcada para reutilizacao e guardada aqui
    public:
        CabecalhoArquivo();
};

class Sequencia { // classe da sequencia duplamente encadeada
    private:
        int atual; // indice da sequencia atual
        int anterior; // variavel que guarda qual a sequencia anterior a sequencia atual
        int proximo; // variavel que guarda qual a sequencia posterior a sequencia atual
        int qtdUtilizada; // quantidade utilizado em ""registros[]" da sequencia atual
        Objeto registros[5]; // um vetor Objeto com 5 posicoes para uso
    public:
        Sequencia();
        void insere(fstream* arquivo);
        void remove(fstream* arquivo);
        void imprime(fstream* arquivo);
        void imprimeComInfo(fstream* arquivo);
        void buscaPeloID(fstream* arquivo);
        void buscaPelaSequencia(fstream* arquivo);
        bool verificaSeJaExisteID(fstream* arquivo, int id);
};

#endif
