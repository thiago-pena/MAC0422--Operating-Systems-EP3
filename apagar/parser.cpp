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
    Parser(char *nomeArq, bool existe, DirTree dirTree) { //Inicializa o parser sobre um arquivo txt
        if (existe) {
            if (DEBUG) printf("Parser inicializado!\n");
            // Variáveis________________________________________________________
            string arquivo; string linha; string palavra;
            string token1; string token2; string token3;
            string token4; string token5; string token6;
            int lFinal;
            ifstream inFile;

            inFile.open(nomeArq);

            if (!inFile) { // Verifica falha
                printf("Falha na leitura de %s\n", nomeArq);
            }

            getline(inFile, linha); // pula free space

            // FAT______________________________________________________________
            for (int i = 0; i < FATSIZE; i++) {// Lê FAT
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

            // Root_____________________________________________________________
            getline(inFile, linha);
            istringstream iss(linha);
            getline( iss, palavra , '|');
            getline( iss, token1 , '|');
            getline( iss, token2 , '|');
            getline( iss, token3 , '|');
            getline( iss, token4 , '|');
            dirTree.dirRootUpdate(palavra, token1, token2, token3, token4);

            // Diretórios_______________________________________________________
            std::cout << "Terminou Root" << '\n';
            for (int i = 1; i < FATSIZE; i++) {
                getline(inFile, linha);
                istringstream iss(linha);
                getline( iss, palavra , '|');
                lFinal = palavra.length();
                if (palavra[lFinal -1] == '/') {
                  std::cout << "opa "<< palavra <<" é diretorio! " << '\n';

                }
                cout << "Palavra: " << palavra <<" de final: "<< palavra[lFinal -1] << '\n';

            }


        } else { //Cria arquivo.txt caso ele não exista
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
            fp = fopen (nomeArq,"a"); //ao invés de pular é melhor reabrir e append
            for (int i = 0; i < FATSIZE; i++) fprintf(fp,"|\n");
            fclose (fp);
        }

    }

    ~Parser() {

    }
};

#endif
