#ifndef _DIRETORIO_CPP
#define _DIRETORIO_CPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#include "diretorio.hpp"
#include "tools.hpp"

DirTree::DirTree() { //inicializa árvore com o root/
    root = new dir;
    root->localFAT = 0;
    root->dotdotBar = nullptr;

}

DirTree::~DirTree() { // Vare a árvore estilo e-q-d(s)
    delete root;
}

void DirTree::dirRootUpdate(string name, string lFat, string cAt, string updatedAt, string accessedAt)
{
    root->name = name;
    root->localFAT = atol(lFat.c_str());
    root->createdAt = atol(cAt.c_str());
    root->updatedAt = atol(updatedAt.c_str());
    root->accessedAt = atol(accessedAt.c_str());
    if (DEBUG) {
      cout << "Root/>nome: " << root->name << '\n';
      cout << "Root/>localFAT: " << root->localFAT << '\n';
      cout << "Root/>createdAt: " << root->createdAt << '\n';
      cout << "Root/>updatedAt: " << root->updatedAt << '\n';
      cout << "Root/>accessedAt: " << root->accessedAt << '\n';
    }
}

dir* DirTree::searchDir(string name)
{
  if (root->name == name) {
      return root;
  } else {
      return searchDirR(name, root->dotBar[0]);
  }
}

dir* DirTree::searchDirR(string name, dir *a)
{
    if(!a) return nullptr;
}


#endif
