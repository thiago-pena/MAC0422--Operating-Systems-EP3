#ifndef _DIRETORIO_CPP
#define _DIRETORIO_CPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#include "diretorio.hpp"
#include "tools.hpp"
#include "parser.cpp"


class DirTree {

  private:
    dir *root;

  public:
    DirTree() { //inicializa árvore com o root/
        root = new dir;
        root->name = "root/";
        root->localFAT = 0;

        // int data = datainfo(); //Coleta data do localtime
        // root->createdAt = data;
        // root->updatedAt = data;
        // root->accessedAt = data;
    }

    ~DirTree() { // Vare a árvore estilo e-q-d(s)

    }

    int insereArquivo(string palavra, dir atual)
    {

    }
};


#endif
