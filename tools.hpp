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

using namespace std;

// Coleta informações de data e transforma em um int
int datainfo();
int clockinfo();
string charToString(char *a);

#endif
