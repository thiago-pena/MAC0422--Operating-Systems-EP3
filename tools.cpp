#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <sstream>
    // formatar int com zeros à esquerda
#include <iomanip>
    // formatar int com zeros à esquerda

#include "tools.hpp"

// Coleta informações de data e transforma em um int
unsigned long long datainfo()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return ((tm.tm_year + 1900)*pow(10,10) +
            (tm.tm_mon + 1)*pow(10,8) +
            (tm.tm_mday)*pow(10,6) +
            (tm.tm_hour)*pow(10,4) +
            (tm.tm_min)*pow(10,2) +
            (tm.tm_sec));
}

// Coleta informações de data e transforma em uma string concatenada
string datainfoString()
{
    unsigned long long data = datainfo();
    string dataS = to_string(data);
    return dataS;
}

// Recebe um inteiro e retorna uma string representando o inteiro com 5 dígitos.
string intToString(int i)
{
    if (i < 0) return "-0001";
    int j = i, numDigitos = 0;
    while (j > 0) {
        j /= 10;
        numDigitos++;
    }
    ostringstream s;
    s << setw(5) << setfill('0') << i;
    return s.str();
}
