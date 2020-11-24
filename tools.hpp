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


#define DEBUG 1
#define MAXLEN 255
#define FATSIZE 16

#define NUMBLOCKS 20
#define BITMAP 0
#define FATPOS 21
#define ROOT 141
#define BLOCKSIZE 100
#define METADIR 7
#define METAFILE 8

using namespace std;

extern int fat[NUMBLOCKS];
extern int fsm[NUMBLOCKS];

// Coleta informações de data e transforma em um unsigned long int
unsigned long long datainfo();

// Coleta informações de data e transforma em uma string concatenada
string datainfoString();

// Recebe um inteiro e retorna uma string representando o inteiro com 5 dígitos.
string intToString(int i);



#endif
