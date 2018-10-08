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
#include "objeto.hpp"

using namespace std;

CabecalhoArquivo::CabecalhoArquivo() { // construtor do cabecalho do arquivo
    qtdPedacos = 0;
    priSequencia = -1;
    proxPedacoASerUtilizado = 0;
    proxPedacoASerReutilizado = -1;
}

Sequencia::Sequencia() { // construtor da sequencia
    atual = 0;
    anterior = -1;
    proximo = -1;
    qtdUtilizada = 0;
}

void Sequencia::insere(fstream* arquivo) { // realiza a insercao de um novo recorde
    Objeto novoRecorde;
    cout << "\nInsira o ID: ";
    cin >> novoRecorde.id;
    
    if (verificaSeJaExisteID(arquivo, novoRecorde.id) == false) { // caso o ID para insercao ainda nao exista no arquivo, a insercao ocorre normalmente
        cin.ignore();
        cout << "Insira o nome do recorde: ";
        cin.getline(novoRecorde.nome, 50);
        cout << "Insira a descricao do recorde: ";
        cin.getline(novoRecorde.descricao, 200);
        cout << "Insira o responsavel pelo recorde: ";
        cin.getline(novoRecorde.responsavel, 100);
        cout << "Insira o ano do recorde: ";
        cin >> novoRecorde.anoDoRecorde;
        cin.ignore();
        cout << "Insira o local do recorde: ";
        cin.getline(novoRecorde.local, 30);
        
        CabecalhoArquivo cabecalho;
        arquivo->clear();
        arquivo->seekg(0);
        arquivo->read((char*)&cabecalho, sizeof(CabecalhoArquivo));
        
        Sequencia sequencia;
        
        if (cabecalho.qtdPedacos == 0) { // cria a primeira sequencia, inserindo o recorde nela, quando o arquivo esta vazio
            sequencia.registros[0] = novoRecorde;
            ++sequencia.qtdUtilizada;
            ++cabecalho.qtdPedacos;
            ++cabecalho.priSequencia;
            ++cabecalho.proxPedacoASerUtilizado;
            
            arquivo->seekp(0);
            arquivo->write((char*)&cabecalho, sizeof(CabecalhoArquivo));
            arquivo->seekp(sizeof(CabecalhoArquivo));
            arquivo->write((char*)&sequencia, sizeof(Sequencia));
        }
        else { // insere o recorde quando ja existe alguma sequencia
            arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * cabecalho.priSequencia));
            arquivo->read((char*)&sequencia, sizeof(Sequencia));
            
            int contPedacos = 1;
            bool achouSequencia = false;
            while ((contPedacos < cabecalho.qtdPedacos) and (!achouSequencia)) { // acha a sequencia onde o recorde deve ser inserido
                if ((novoRecorde.id < sequencia.registros[sequencia.qtdUtilizada - 1].id) or (cabecalho.qtdPedacos == contPedacos)) {
                    achouSequencia = true;
                }
                else {
                    arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.proximo));
                    arquivo->read((char*)&sequencia, sizeof(Sequencia));
                }
                ++contPedacos;
            }
            
            bool inseriuNoMesmo = false;
            bool inseriuNoAnterior = false;
            bool inseriuNoPosterior = false;
            int indice = 0;
            while ((novoRecorde.id > sequencia.registros[indice].id) and (indice < sequencia.qtdUtilizada)) { // encontra a posicao para insercao dentro da sequencia onde o recorde deve ser inserido
                ++indice;
            }
            
            if (sequencia.qtdUtilizada < 5) { // se a sequencia para insercao nao estiver cheia, o recorde e inserido nela
                //~ int indice = 0;
                for (int i = sequencia.qtdUtilizada; i > indice; --i) {
                    sequencia.registros[i] = sequencia.registros[i - 1];
                }
                sequencia.registros[indice] = novoRecorde;
                ++sequencia.qtdUtilizada;
                
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.atual));
                arquivo->write((char*)&sequencia, sizeof(Sequencia));
                inseriuNoMesmo = true;
            }
            
            if ((sequencia.anterior != -1) and (!inseriuNoMesmo)) { // tenta inserir na sequencia anterior, caso ela exista e a insercao ainda nao tenha ocorrido
                Sequencia sequenciaAnterior;
                arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.anterior));
                arquivo->read((char*)&sequenciaAnterior, sizeof(Sequencia));
                
                if (sequenciaAnterior.qtdUtilizada < 5) { // faz a insercao na sequencia anterior
                    if (indice == 0) {
                        sequenciaAnterior.registros[sequenciaAnterior.qtdUtilizada] = novoRecorde;
                        ++sequenciaAnterior.qtdUtilizada;
                    }
                    else {
                        sequenciaAnterior.registros[sequenciaAnterior.qtdUtilizada] = sequencia.registros[0];
                        cout << sequencia.registros[0].id << endl;
                        for (int i = 0; i < indice; ++i) {
                            sequencia.registros[i] = sequencia.registros[i + 1];
                        }
                        sequencia.registros[indice - 1] = novoRecorde;
                        ++sequenciaAnterior.qtdUtilizada;
                        
                        arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.atual));
                        arquivo->write((char*)&sequencia, sizeof(Sequencia));
                    }
                    arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequenciaAnterior.atual));
                    arquivo->write((char*)&sequenciaAnterior, sizeof(Sequencia));
                    inseriuNoAnterior = true;
                }
            }
            
            if ((!inseriuNoMesmo) and (!inseriuNoAnterior) and (sequencia.proximo != -1)) { // tenta inserir na sequencia posterior, caso ela exista e a insercao ainda nao tenha ocorrido
                Sequencia sequenciaPosterior;
                arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.proximo));
                arquivo->read((char*)&sequenciaPosterior, sizeof(Sequencia));
                
                if (sequenciaPosterior.qtdUtilizada < 5) { // faz a insercao na sequencia posterior
                    for (int i = sequenciaPosterior.qtdUtilizada; i > 0; --i) {
                        sequenciaPosterior.registros[i] = sequenciaPosterior.registros[i - 1];
                    }
                    sequenciaPosterior.registros[0] = sequencia.registros[4];
                    ++sequenciaPosterior.qtdUtilizada;
                    for (int i = 4; i > indice; --i) {
                        sequencia.registros[i] = sequencia.registros[i - 1];
                    }
                    sequencia.registros[indice] = novoRecorde;
                    inseriuNoPosterior = true;
                    
                    arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.atual));
                    arquivo->write((char*)&sequencia, sizeof(Sequencia));
                    arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequenciaPosterior.atual));
                    arquivo->write((char*)&sequenciaPosterior, sizeof(Sequencia));
                }
            }
            
            if ((!inseriuNoMesmo) and (!inseriuNoAnterior) and (!inseriuNoPosterior)) { // aplica a divisao da sequencia, quando a insercao foi impossivel nos casos anteriores
                arquivo->seekg(0);
                arquivo->read((char*)&cabecalho, sizeof(CabecalhoArquivo));
                
                Sequencia novaSequencia; // divisao da sequencia, a primeira parte fica com 3 registros e a segunda com 2
                novaSequencia.registros[0] = sequencia.registros[3];
                novaSequencia.registros[1] = sequencia.registros[4];
                sequencia.qtdUtilizada -= 2;
                novaSequencia.qtdUtilizada = 2;
                
                if (sequencia.proximo == -1) { // se a sequencia que deve ser dividida e a ultima sequencia valida do arquivo
                    novaSequencia.atual = cabecalho.proxPedacoASerUtilizado;
                    novaSequencia.anterior = sequencia.atual;
                    sequencia.proximo = novaSequencia.atual;
                }
                else { // se a sequencia que deve ser dividida nao e a ultima sequencia valida do arquivo
                    Sequencia sequenciaPosterior;
                    arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.proximo));
                    arquivo->read((char*)&sequenciaPosterior, sizeof(Sequencia));
                    
                    novaSequencia.atual = cabecalho.proxPedacoASerUtilizado;
                    novaSequencia.anterior = sequencia.atual;
                    novaSequencia.proximo = sequencia.proximo;
                    sequenciaPosterior.anterior = novaSequencia.atual;
                    sequencia.proximo = novaSequencia.atual;
                    
                    arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequenciaPosterior.atual));
                    arquivo->write((char*)&sequenciaPosterior, sizeof(Sequencia));
                }
                
                if (indice < 3) { // quando o novo recorde deve ser inserido na primeira parte da sequencia apos a divisao da mesma
                    for (int i = sequencia.qtdUtilizada; i > indice; --i) {
                        sequencia.registros[i] = sequencia.registros[i - 1];
                    }
                    sequencia.registros[indice] = novoRecorde;
                    ++sequencia.qtdUtilizada;
                }
                else { // quando o novo recorde deve ser inserido na segunda parte da sequencia apos a divisao da mesma
                    indice = indice - 3;
                    for (int i = novaSequencia.qtdUtilizada; i > indice; --i) {
                        novaSequencia.registros[i] = novaSequencia.registros[i - 1];
                    }
                    novaSequencia.registros[indice] = novoRecorde;
                    ++novaSequencia.qtdUtilizada;
                }
                
                bool existeSequenciaRemovida = false;
                if (cabecalho.proxPedacoASerReutilizado != -1) { // existe uma sequencia para reutilizacao
                    Sequencia sequenciaAuxiliar;
                    arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * cabecalho.proxPedacoASerReutilizado));
                    arquivo->read((char*)&sequenciaAuxiliar, sizeof(Sequencia));
                    
                    novaSequencia.atual = cabecalho.proxPedacoASerReutilizado;
                    sequencia.proximo = novaSequencia.atual;
                    
                    if (sequenciaAuxiliar.proximo != -1) { // existe mais de uma sequencia para reutilizacao
                        cabecalho.proxPedacoASerReutilizado = sequenciaAuxiliar.proximo;
                    }
                    else { // existe apenas uma sequencia para reutilizacao
                        cabecalho.proxPedacoASerReutilizado = -1;
                    }
                    existeSequenciaRemovida = true;
                }
                ++cabecalho.qtdPedacos;
                
                if (!existeSequenciaRemovida) { // caso nao exista sequencia para reutilizacao, cria-se uma nova
                    ++cabecalho.proxPedacoASerUtilizado;
                }
                
                arquivo->seekp(0);
                arquivo->write((char*)&cabecalho, sizeof(CabecalhoArquivo));
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.atual));
                arquivo->write((char*)&sequencia, sizeof(Sequencia));
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * novaSequencia.atual));
                arquivo->write((char*)&novaSequencia, sizeof(Sequencia));
            }
        }
    }
    else { // caso o ID para insercao ja exista no arquivo, o programa nao faz a insercao
        cerr << "\n\nO ID " << novoRecorde.id << " ja existe.\n";
    }
}

void Sequencia::remove(fstream* arquivo) { // realiza a remocao de um recorde
    CabecalhoArquivo cabecalho;
    arquivo->clear();
    arquivo->seekg(0);
    arquivo->read((char*)&cabecalho, sizeof(CabecalhoArquivo));
    
    if (cabecalho.priSequencia == -1) { // arquivo vazio
        cerr << "\n\nArquivo vazio, impossivel realizar remocoes.\n";
    }
    else { // arquivo nao vazio
        cout << "\nDigite o ID a ser removido: ";
        int idRemover;
        cin >> idRemover;
        
        Sequencia sequencia;
        arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * cabecalho.priSequencia));
        arquivo->read((char*)&sequencia, sizeof(Sequencia));
        
        int posicao = cabecalho.priSequencia;
        bool encontrou = false;
        int posRemover;
        while ((posicao != -1) and (!encontrou) and (sequencia.qtdUtilizada > 0)) { // encontra a posicao a remover dentro de determinada sequencia, se o id existir
            if (idRemover <= sequencia.registros[sequencia.qtdUtilizada - 1].id) {
                for (int i = 0; (i < sequencia.qtdUtilizada) and (!encontrou); ++i) {
                    if (idRemover == sequencia.registros[i].id) {
                        posRemover = i;
                        encontrou = true;
                    }
                }
            }
            if (!encontrou) {
                posicao = sequencia.proximo;
                
                arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * posicao));
                arquivo->read((char*)&sequencia, sizeof(Sequencia));
            }
        }
        
        if (encontrou) { // escreve no arquivo a nova sequenica apos "remocao"
            cout << "\n\nID removido.\n";   
            for (int i = posRemover; i < (sequencia.qtdUtilizada - 1); ++i) {
                sequencia.registros[i] = sequencia.registros[i + 1];
            }
            --sequencia.qtdUtilizada;
            
            arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.atual));
            arquivo->write((char*)&sequencia, sizeof(Sequencia));
        }
        else { // a remocao nao foi possivel, pois o ID buscado nao foi encontrado
            cerr << "\nO ID pedido para remocao nao foi encontrado.\n";;
        }
        
        if (sequencia.qtdUtilizada == 0) { // realiza a remocao e altera o necessario, caso a remocao seja do unico elemento restante na sequencia
            if ((sequencia.anterior == -1) and (sequencia.proximo == -1)) { // realiza a remocao se a sequencia for a unica do arquivo
                cabecalho.priSequencia = -1;
            }
            else if (sequencia.anterior == -1) { // realiza a remocao, quando a sequencia vazia e a primeira valida do arquivo
                Sequencia sequenciaPosterior;
                arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.proximo));
                arquivo->read((char*)&sequenciaPosterior, sizeof(Sequencia));
                
                sequenciaPosterior.anterior = -1;
                cabecalho.priSequencia = sequenciaPosterior.atual;
                sequencia.proximo = -1;
                
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.atual));
                arquivo->write((char*)&sequencia, sizeof(Sequencia));
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequenciaPosterior.atual));
                arquivo->write((char*)&sequenciaPosterior, sizeof(Sequencia));
            }
            else if (sequencia.proximo == -1) { // realiza a remocao, quando a sequencia vazia e a ultima valida do arquivo
                Sequencia sequenciaAnterior;
                arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.anterior));
                arquivo->read((char*)&sequenciaAnterior, sizeof(Sequencia));
                
                sequenciaAnterior.proximo = -1;
                sequencia.anterior = -1;
                    
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.atual));
                arquivo->write((char*)&sequencia, sizeof(Sequencia));
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequenciaAnterior.atual));
                arquivo->write((char*)&sequenciaAnterior, sizeof(Sequencia));
            }
            else { // realiza a remocao, quando a sequencia se encontra no meio do arquivo, ou seja, possui sequencia anterior e sequencia posterior
                Sequencia sequenciaAnterior;
                arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.anterior));
                arquivo->read((char*)&sequenciaAnterior, sizeof(Sequencia));
                Sequencia sequenciaPosterior;
                arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.proximo));
                arquivo->read((char*)&sequenciaPosterior, sizeof(Sequencia));
                
                sequenciaAnterior.proximo = sequenciaPosterior.atual;
                sequenciaPosterior.anterior = sequenciaAnterior.atual;
                sequencia.anterior = -1;
                sequencia.proximo = -1;
                
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequencia.atual));
                arquivo->write((char*)&sequencia, sizeof(Sequencia));
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequenciaAnterior.atual));
                arquivo->write((char*)&sequenciaAnterior, sizeof(Sequencia));
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * sequenciaPosterior.atual));
                arquivo->write((char*)&sequenciaPosterior, sizeof(Sequencia));
            }
            
            if (cabecalho.proxPedacoASerReutilizado == -1) { // atualiza a variavel do cabecalho que guarda a sequencia apta para reutilizacao
                cabecalho.proxPedacoASerReutilizado = sequencia.atual;
            }
            else { // atualiza a variavel do cabecalho que guarda a sequencia apta pra reutilizao quando ja existe outra sequencia apta para reutilizacao
                Sequencia sequenciaAuxiliar;
                arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * cabecalho.proxPedacoASerReutilizado));
                arquivo->read((char*)&sequenciaAuxiliar, sizeof(Sequencia));
                sequenciaAuxiliar.proximo = sequencia.atual;
                arquivo->seekp(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * cabecalho.proxPedacoASerReutilizado));
                arquivo->write((char*)&sequenciaAuxiliar, sizeof(Sequencia));
            }
            
            --cabecalho.qtdPedacos;
            arquivo->clear();
            arquivo->seekp(0);
            arquivo->write((char*)&cabecalho, sizeof(CabecalhoArquivo));
        }
    }
}

void Sequencia::imprime(fstream* arquivo) { // imprime os recordes sem separa-los por sequencias
    CabecalhoArquivo cabecalho;
    arquivo->clear();
    arquivo->seekg(0);
    arquivo->read((char*)&cabecalho, sizeof(CabecalhoArquivo));
    Sequencia sequencia;
    arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * cabecalho.priSequencia));
    arquivo->read((char*)&sequencia, sizeof(Sequencia));
    
    int posicao = cabecalho.priSequencia;
    while (posicao != -1) {
        for (int i = 0; i < sequencia.qtdUtilizada; ++i) {
            cout << "\nID: " << sequencia.registros[i].id
                 << "\nNome: " << sequencia.registros[i].nome
                 << "\nDescricao: " << sequencia.registros[i].descricao
                 << "\nResponsavel: " << sequencia.registros[i].responsavel
                 << "\nAno: " << sequencia.registros[i].anoDoRecorde
                 << "\nLocal: " << sequencia.registros[i].local << endl;
        }
        posicao = sequencia.proximo;
        
        arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * posicao));
        arquivo->read((char*)&sequencia, sizeof(Sequencia));
    }
    if (cabecalho.priSequencia == -1) {
        cerr << "\n\nArquivo vazio, nada a listar.\n";
    }
}

void Sequencia::imprimeComInfo(fstream* arquivo) { // imprime os recordes separando-os pelas sequencias (com o cabecalho de cada uma)
    CabecalhoArquivo cabecalho;
    arquivo->clear();
    arquivo->seekg(0);
    arquivo->read((char*)&cabecalho, sizeof(CabecalhoArquivo));
    Sequencia sequencia;
    arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * cabecalho.priSequencia));
    arquivo->read((char*)&sequencia, sizeof(Sequencia));
    
    int posicao = cabecalho.priSequencia;
    while (posicao != -1) {
        cout << "\nSequencia atual: " << sequencia.atual
             << "\nAnterior: "<< sequencia.anterior
             << "\nProximo: " << sequencia.proximo
             << "\nQuantidade utilizada na sequencia: " << sequencia.qtdUtilizada << endl;
        for (int i = 0; i < sequencia.qtdUtilizada; ++i) {
            cout << "\nID: " << sequencia.registros[i].id
                 << "\nNome: " << sequencia.registros[i].nome
                 << "\nDescricao: " << sequencia.registros[i].descricao
                 << "\nResponsavel: " << sequencia.registros[i].responsavel
                 << "\nAno: " << sequencia.registros[i].anoDoRecorde
                 << "\nLocal: " << sequencia.registros[i].local << endl;
        }
        cout << endl;
        posicao = sequencia.proximo;
        
        arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * posicao));
        arquivo->read((char*)&sequencia, sizeof(Sequencia));
    }
    if (cabecalho.priSequencia == -1) {
        cerr << "\n\nArquivo vazio, nada a listar.\n";
    }
}

void Sequencia::buscaPeloID(fstream* arquivo) { // realiza uma busca por ID e imprime (quando encontrado o ID) as informacoes contidas nele
    CabecalhoArquivo cabecalho;
    arquivo->seekg(0);
    arquivo->read((char*)&cabecalho, sizeof(CabecalhoArquivo));
    
    if (cabecalho.priSequencia == -1) {
        cerr << "\n\nArquivo vazio, impossivel realizar buscas.\n";
    }
    else {
        cout << "\nDigite o ID para busca: ";
        int idBuscar;
        cin >> idBuscar;
        
        Sequencia sequencia;
        arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * cabecalho.priSequencia));
        arquivo->read((char*)&sequencia, sizeof(Sequencia));
        
        int posicao = cabecalho.priSequencia;
        bool encontrou = false;
        while (posicao != -1) {
            for (int i = 0; (i < sequencia.qtdUtilizada) and (!encontrou); ++i) {
                if (idBuscar == sequencia.registros[i].id) {
                    cout << "\n\nO ID " << idBuscar << " contem as seguintes informacoes:\n"
                         << "\nNome: " << sequencia.registros[i].nome
                         << "\nDescricao: " << sequencia.registros[i].descricao
                         << "\nResponsavel: " << sequencia.registros[i].responsavel
                         << "\nAno: " << sequencia.registros[i].anoDoRecorde
                         << "\nLocal: " << sequencia.registros[i].local << endl;
                    encontrou = true;
                }
            }
            posicao = sequencia.proximo;
            
            arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * posicao));
            arquivo->read((char*)&sequencia, sizeof(Sequencia));
        }
        if (!encontrou) {
            cerr << "\n\nO ID pedido nao existe.\n";
        }
    }
}

void Sequencia::buscaPelaSequencia(fstream* arquivo) { // realiza uma busca utilizando o indice de uma sequencia, imprime (quando encontrado) todos os recordes contido nessa sequencia
    CabecalhoArquivo cabecalho;
    arquivo->seekg(0);
    arquivo->read((char*)&cabecalho, sizeof(CabecalhoArquivo));
    
    if (cabecalho.priSequencia == -1) {
        cerr << "\n\nArquivo vazio, impossivel realizar buscas.\n";
    }
    else {
        cout << "\nDigite o indice da sequencia para busca: ";
        int seq;
        cin >> seq;
        
        Sequencia sequencia;
        arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * cabecalho.priSequencia));
        arquivo->read((char*)&sequencia, sizeof(Sequencia));
        
        int posicao = cabecalho.priSequencia;
        bool encontrou = false;
        while ((posicao != -1) and (!encontrou)) {
            if (seq == sequencia.atual) {
                cout << "\n\nA sequencia " << seq << " contem os seguintes registros:\n";
                for (int i = 0; i < sequencia.qtdUtilizada; ++i) {
                    cout << "\nID: " << sequencia.registros[i].id
                         << "\nNome: " << sequencia.registros[i].nome
                         << "\nDescricao: " << sequencia.registros[i].descricao
                         << "\nResponsavel: " << sequencia.registros[i].responsavel
                         << "\nAno: " << sequencia.registros[i].anoDoRecorde
                         << "\nLocal: " << sequencia.registros[i].local << endl;
                }
                encontrou = true;
            }
            posicao = sequencia.proximo;
            
            arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * posicao));
            arquivo->read((char*)&sequencia, sizeof(Sequencia));
        }
        if (!encontrou) {
            cerr << "\n\nA sequencia pedida nao existe.\n";
        }
    }
}

bool Sequencia::verificaSeJaExisteID(fstream* arquivo, int id) { // verifica se determinado ID ja existe no arquivo
    CabecalhoArquivo cabecalho;
    arquivo->clear();
    arquivo->seekg(0);
    arquivo->read((char*)&cabecalho, sizeof(CabecalhoArquivo));
    Sequencia sequencia;
    arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * cabecalho.priSequencia));
    arquivo->read((char*)&sequencia, sizeof(Sequencia));
    
    bool jaExiste = false;
    int posicao = cabecalho.priSequencia;
    while (posicao != -1) {
        for (int i = 0; i < sequencia.qtdUtilizada; ++i) {
            if (id == sequencia.registros[i].id) {
                jaExiste = true;
            }
        }
        posicao = sequencia.proximo;
        
        arquivo->seekg(sizeof(CabecalhoArquivo) + (sizeof(Sequencia) * posicao));
        arquivo->read((char*)&sequencia, sizeof(Sequencia));
    }
    return jaExiste;
}
