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
    // Atualiza a FAT.
    void saveFat();
    // Atualiza o registro de espaço livre.
    void saveFsm();
    // Usa timeUpdater para atualizar a data de acesso.
    void accessedAtUpdater(int nFat);
    // Usa timeUpdater para atualizar a data de modificação.
    void updateAtUpdater(int nFat);
    // Atualizador de tempo.
    void timeUpdater(int nFat, int pos);

    string loadBlock(int nFat);

    int absolutePath(string dirPath);

    int cdDir(string bloco, string dirName);
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
    // cp origem destino: cria dentro do sistema de arquivos simulado uma cópia do arquivo
    // origem que está em algum sistema de arquivos real dentro do seu computador. No sistema de
    // arquivos simulado a cópia de origem será salva em destino. Tanto origem quanto destino
    // devem ser informados com o caminho completo tanto dentro do sistema de arquivos real quanto no
    // simulado 1 . Esse comando será executado apenas para copiar arquivos em texto puro.
    void copy(string origem, string destino); // EM PROCESSO (Pena)
    string getDiskName();

};

#endif
