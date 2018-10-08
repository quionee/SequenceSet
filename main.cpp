/* 
  Trabalho de Estrutura de Dados
  Recordes Bizarros 
  Copyright 2017 by Lorena Tavares, Rodrigo Herculano, William Coelho 
  objeto.hpp: contem a classe e todos os metodos.
  metodos.cpp: implementacao de todas os metodos.
  main.cpp: implementacao do programa principal.
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "objeto.hpp"

using namespace std;

int main()
{
    cout << "\n\n*** Bem-vindo ao Bizarre Guiness Book, o livro de recordes bizarros!!! ***\n";
    ifstream arquivoLeitura("Recordes_Bizarros.bin");
    CabecalhoArquivo cabecalho;
    if (!arquivoLeitura.good()) {
        ofstream arquivoEscrita("Recordes_Bizarros.bin");
        arquivoEscrita.seekp(0);
        arquivoEscrita.write((char*)&cabecalho, sizeof(CabecalhoArquivo));
        arquivoEscrita.flush();
    }
    
    fstream arquivo("Recordes_Bizarros.bin");
    
    Sequencia sequencias;
    int opcao;
    do {
        arquivo.clear();
        cout << "\n\n--- Suas opcoes sao: --- \n\n"
             << "-> 1 para inserir\n"
             << "-> 2 para remover\n"
             << "-> 3 para listar\n"
             << "-> 4 para listar com informacoes das sequencias\n"
             << "-> 5 para buscar pelo ID\n"
             << "-> 6 para buscar pela sequencia\n"
             << "-> 7 para encerrar\n\n"
             << "Digite o numero de sua escolha: ";
        cin >> opcao;
        switch (opcao) {
            case 1:
                sequencias.insere(&arquivo);
                break;
            case 2:
                sequencias.remove(&arquivo);
                break;
            case 3:
                sequencias.imprime(&arquivo);
                break;
            case 4:
                sequencias.imprimeComInfo(&arquivo);
                break;
            case 5:
                sequencias.buscaPeloID(&arquivo);
                break;
            case 6:
                sequencias.buscaPelaSequencia(&arquivo);
                break;
            case 7:
                cout << "\nPrograma encerrado.\n";
                break;
            default:
                cout << "\nOpcao invalida, digite novamente.\n";
        }
    } while (opcao != 7);
    
    return 0;
}
