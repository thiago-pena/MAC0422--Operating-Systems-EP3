#ifndef _DRIVER_HPP
#define _DRIVER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#include "tools.hpp"

extern int fat[NUMBLOCKS];

typedef struct diretorio dir;
typedef struct arquivo arq;

struct arquivo { // Estrutura de arquivo
    string name;
    int localFAT;
    int size;
    unsigned long long createdAt;
    unsigned long long updatedAt;
    unsigned long long accessedAt;
    string insideDirName;
    vector<string> content;
};

struct diretorio { // Estrutura de diretório
    string name;
    int localFAT;
    unsigned long long createdAt;
    unsigned long long updatedAt;
    unsigned long long accessedAt;
    string dotdotBar; // "../" diretorio Pai
    vector<string> dotBar; // "./" diretórios filhos
    vector<string> arqPont; // arquivos filhos
};

class Driver {
  private:
    char *diskName;
    dir *dirStruct(string linha);
    //Dada um nome procura o numero fat relacionado ao nome do diretório
    int nFAT(string name, string linha);//EM PROCESSO
    //Retorna o bloco de número nFat
    string nFAT(int nFat);//EM PROCESSO
    // aponta próx bloco free
    int whereIsFree();
  public:
    Driver();
    ~Driver();
    // Monta um arquivo como estrutura de disco
    void mount(char *nomeArq, bool existe);
    // procura arquivo por nome ( Busca linear )
    arq *SearchFile(string name); // DÚVIDA caminho ou busca linear
    // imprime informação e conteúdo de arquivo
    void ImprimeArquivo(arq *file);

    dir *SearchDir(string name);//EM PROCESSO
    //cria um diretorio no caminho especificado
    void mkDir(string dirName);//EM PROCESSO
    void copy(string origem, string destino); // EM PROCESSO (Pena)

};

#endif
