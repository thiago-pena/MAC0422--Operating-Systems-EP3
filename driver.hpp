#ifndef _DRIVER_HPP
#define _DRIVER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#include "tools.hpp"

typedef struct diretorio dir;
typedef struct arquivo arq;

extern bool mountedFS;

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
    void accessedAtUpdater(int nFat, bool isFile);
    // Usa timeUpdater para atualizar a data de modificação.
    void updateAtUpdater(int nFat);
    // Atualizador de tempo.
    void timeUpdater(int nFat, int pos);
    // Recebe número de fat e carrega bloco
    string loadBlock(int nFat);
    // Percorre caminho de diretório e devolve número fat do destino
    int absolutePath(string dirPath);
    // Ajuda recursivamente absolutePath
    int cdDir(string bloco, string dirName);
    // Remover um bloco
    void remover(int nFat, bool LowLevelFormat);
    // Remove o metadado de um arquivo ou diretorio dentro do mesmo.
    string metaRemover(string bloco, string name);
    // Lê informações de um bloco para o ls
    void listener(int nFat);
  public:
    Driver();
    ~Driver();
    // Monta um arquivo como estrutura de disco
    void mount(char *nomeArq, bool existe);
    // Desmonta arquivo aberto;
    void umount();
    // procura arquivo por nome ( Busca linear )
    bool SearchFile(string absoluteDirName, bool remove, bool LowLevelFormat);
    // Imprime informação e conteúdo de arquivo
    void ImprimeArquivo(string bloco, bool isInit);
    // Remove pasta usando...
    int rmDir(string absoluteDirName, bool LowLevelFormat);
    // Cria um diretorio no caminho especificado, caso for chamado com flag
    // isFile funciona como touch e cria/atualiza arquivo.
    void mkDirAndTouch(string absoluteDirName, bool isFile);
    void mkDirAndTouch2(string absoluteDirName, bool isFile);
    // cp origem destino: cria dentro do sistema de arquivos simulado uma cópia do arquivo
    // origem que está em algum sistema de arquivos real dentro do seu computador. No sistema de
    // arquivos simulado a cópia de origem será salva em destino. Tanto origem quanto destino
    // devem ser informados com o caminho completo tanto dentro do sistema de arquivos real quanto no
    // simulado 1 . Esse comando será executado apenas para copiar arquivos em texto puro.
    void copy(string origem, string destino); // EM PROCESSO (Pena)
    // Aplica ls em um diretório listando seus diretorio e arquivos abaixo.
    void ListDir(string absoluteDirName);
    // Verefica ser um determinado arquivo está dentro de uma pasta.
    void finder(string absoluteDirName, string file);
    // imprime na tela informações seguindo o enunciado.
    void df();

    string getDiskName();

};

#endif
