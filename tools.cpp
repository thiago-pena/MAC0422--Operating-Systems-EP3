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
    if (i == -1) return "-0001";
    if (i == -2) return "-0002";
    int j = i, numDigitos = 0;
    while (j > 0) {
        j /= 10;
        numDigitos++;
    }
    ostringstream s;
    s << setw(5) << setfill('0') << i;
    return s.str();
}

// Retorna o primeiro bloco livre do gerenciamento de espaço livre
int firstFit()
{
    return nextFit(0);
}

// Retorna o primeiro bloco livre do gerenciamento de espaço livre, mas partindo
// de um bloco inicial b >= 0. Se não houver bloco livre após b, procura um
// bloco livre partindo do início.
int nextFit(int b)
{
    int k = b;
    while (k < NUMBLOCKS && fsm[k] != 0)
        k++;
    if (k >= NUMBLOCKS) { // chegou ao fim, procurar algum anterior a b
        k = b;
        while (k >= 0 && fsm[k] != 0)
            k--;
    }
    if (k <= 0) {
        cout << "[ERRO] Não há espaço livre no disco. (k = " << k << ")" << endl;
        exit(1);
    }
    return k;
}

// Recebe um inteiro k, indicando o bloco de início de um arquivo no disco e
// marca como livres no vetor fsm[] as posições do arquivo dadas pelo vetor
// fat[].
void cleanFsmFile(int k) {
    for (int i = k; i >= 0 && fsm[i] == 1; i = fat[i]) {
        fsm[i] = 0;
        cout << "\t\t\t[P] cleanFsmFile >>> fsm[" << i << "] == 0" << endl;
    }
}
