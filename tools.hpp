#ifndef _TOOLS_HPP
#define _TOOLS_HPP

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <math.h>
#include <time.h>

#define DEBUG 0
#define MAXLENPROMPT 255
#define NUMBLOCKS 24955 // 99.999.999 bytes
// #define NUMBLOCKS 5000
#define BLOCKSIZE 4000
// #define BLOCKSIZE 100
#define BITMAP 0
#define FATPOS (1 + NUMBLOCKS/BLOCKSIZE)*BLOCKSIZE
#define ROOT (2 + NUMBLOCKS/BLOCKSIZE + (6*NUMBLOCKS - 1)/BLOCKSIZE)*BLOCKSIZE
#define METADIR 7
#define METAFILE 8
#define LOWLEVELFORMAT 0
#define PRINT_RUNTIME 0

using namespace std;


extern int fat[NUMBLOCKS];
extern int fsm[NUMBLOCKS];

// Coleta informações de data e transforma em um unsigned long int
unsigned long long datainfo();

// Coleta informações de data e transforma em uma string concatenada
string datainfoString();

// Recebe uma string de uma data e retorna uma string com formato
// dd/mm/aaaa hh:mm:ss
string dateToStr(string date);

// Recebe um inteiro e retorna uma string representando o inteiro com 5 dígitos.
string intToString(int i);

// Retorna o primeiro bloco livre do gerenciamento de espaço livre
int firstFit();

// Retorna o primeiro bloco livre do gerenciamento de espaço livre, mas partindo
// de um bloco inicial b >= 0
int nextFit(int b);

// Recebe um inteiro k, indicando o bloco de início de um arquivo no disco e
// marca como livres no vetor fsm[] as posições do arquivo dadas pelo vetor
// fat[].
void cleanFsmFile(int k);

#endif
