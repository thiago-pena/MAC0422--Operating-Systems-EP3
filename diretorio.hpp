#ifndef _DIRETORIO_HPP
#define _DIRETORIO_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#include "tools.hpp"

extern int fat[NUMBLOCKS];

typedef struct arquivo arq;
struct arquivo { // Estrutura de arquivo
    string name;
    int localFAT;
    int size;
    unsigned long long createdAt;
    unsigned long long updatedAt;
    unsigned long long accessedAt;
};

typedef struct diretorio dir;
struct diretorio { // Estrutura de diretório
    string name;
    int localFAT;
    unsigned long long createdAt;
    unsigned long long updatedAt;
    unsigned long long accessedAt;
    dir *dotdotBar; // "../" diretorio Pai
    vector<dir*> dotBar; // "./" diretórios filhos
    vector<arq*> arqPont; // arquivos filhos
};

class DirTree {
  private:
    dir *root;
  public:
    DirTree();
    ~DirTree();
    void dirRootUpdate(string name,
                       string lFat,
                       string cAt,
                       string updatedAt,
                       string accessedAt);
    dir* searchDir(string name);
    dir* searchDirR(string name, dir *a);

};

#endif
