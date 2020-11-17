#ifndef _PARSE_CPP
#define _PARSE_CPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#include "tools.hpp"
#include "diretorio.hpp"

extern int FAT[FATSIZE];

class Parser {
  private:
  public:
    Parser(char *nomeArq, bool existe) { //Inicializa o parser sobre um arquivo txt
        if (existe) {
            if (DEBUG) printf("Parser inicializado!\n");
            string arquivo; string linha; string palavra;
            ifstream inFile;
            inFile.open(nomeArq);
            if (!inFile) {
                printf("Falha na leitura de %s\n", nomeArq);
            }
            getline(inFile, linha); //pula free space
            for (int i = 0; i < FATSIZE; i++) {
                getline(inFile, linha);
                stringstream ss(linha);
                while (ss >> palavra) {
                    FAT[i] = atoi(palavra.c_str());
                }
            }
            if (DEBUG) {
              printf("memory FAT: [");
              for (int i = 0; i < FATSIZE; i++) printf(" %02d", FAT[i]);
              printf("]\n");
            }
            vector <string> tokens;


        } else {
            FILE *fp;
            fp = fopen (nomeArq,"w");
            if(DEBUG) printf("Arquivo %s criado.\n", nomeArq);
            if (fp == NULL) {
                printf("Falha na criação de %s\n", nomeArq);
                fclose (fp);
            }
            for (int i = 0; i < FATSIZE; i++) fprintf(fp,"0|"); //free space
            fprintf(fp,"\n");
            for (int i = 0; i < FATSIZE; i++) {
                FAT[i] = 0; //Carrega FAT na memória
                fprintf(fp,"0\n"); //Cria FAT registro
            }

            // para registro de data https://linux.die.net/man/3/localtime
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            int dataInt = datainfo();
            int clockInt = clockinfo();
            fprintf(fp,"root/|0|%d%d|%d%d|%d%d|root/|0|\n", dataInt, clockInt,
                                                            dataInt, clockInt,
                                                            dataInt, clockInt);

            fclose (fp);
        }

    }

    ~Parser() {

    }
};

#endif
