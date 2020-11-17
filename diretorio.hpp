#ifndef _DIRETORIO_HPP
#define _DIRETORIO_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#include "tools.hpp"
#include "parser.cpp"

extern int FAT[FATSIZE];

typedef struct arquivo arq;
struct arquivo { // Estrutura de arquivo
    string name;
    int localFAT;
    int size;
    int createdAt;
    int updatedAt;
    int accessedAt;
};

typedef struct diretorio dir;
struct diretorio { // Estrutura de diretório
    string name;
    int localFAT;
    int createdAt;
    int updatedAt;
    int accessedAt;
    dir *dotdotBar; // "../" diretorio Pai
    vector<dir*> dotBar; // "./" diretórios filhos
    vector<arq*> arqPont; // arquivos filhos
};

#endif
