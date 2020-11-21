// g++ ep3.cpp -lreadline
#include <iostream>
#include <string.h>
    // strcpy
#include <readline/readline.h>
    // readline
#include <readline/history.h>
    // add_history
#include <fstream>
#include <sstream>
    // ofstream -> write
#include <vector>
    // vetor da STL
#include "tools.hpp"
    // Ferramentas simples
#include "driver.hpp"
    // Parser & Writer

void mount(FILE *fp, const char *arg1);


int fat[NUMBLOCKS];
int fsm[NUMBLOCKS]; // Free Space Management
bool mountedFS = false;

int main() {
    char prompt[MAXLEN];
    Driver driver; // inicializa o driver

    using_history();
    strcpy(prompt, "[ep3] $ ");
    while (true) {
        char *strChar = readline(prompt);
        add_history(strChar);

        char *c = strtok(strChar, " ");
        char *arg1 = strtok(NULL, " ");
        char *arg2 = strtok(NULL, " ");

        if (DEBUG) {
            cout << "[DEBUG] c: " << c << endl;
            if (arg1 != NULL)
                cout << "[DEBUG] arg1: " << arg1 << endl;
            if (arg2 != NULL)
                cout << "[DEBUG] arg2: " << arg2 << endl;
        }

        // mount arquivo
        if (strcmp(c, "mount") == 0) {
            if (!mountedFS) {
                FILE *fp;

                if ((fp = fopen (arg1,"r"))) { //Caso o arquivo exista (read)
                    fclose (fp);
                    driver.mount(arg1, 1);
                } else {
                    driver.mount(arg1, 0);
                }
                mountedFS = true;
            }
            else
                cout << "Já há um sistema de arquivos montado." << endl;

        }
        // cp origem destino
        else if (strcmp(c, "cp") == 0) {
            driver.copy(arg1, arg2);
        }
        // mkdir diretorio
        else if (strcmp(c, "mkdir") == 0) {

            cout << "3" << endl;
        }
        // rmdir diretorio
        else if (strcmp(c, "rmdir") == 0) {
            cout << "4" << endl;
        }
        // cat arquivo
        else if (strcmp(c, "cat") == 0) {
            driver.SearchFile(arg1);
            cout << "5" << endl;
        }
        // touch arquivo
        else if (strcmp(c, "touch") == 0) {
            cout << "6" << endl;
        }
        // rm arquivo
        else if (strcmp(c, "rm") == 0) {
            cout << "7" << endl;
        }
        // ls diretorio
        else if (strcmp(c, "ls") == 0) {
            cout << "8" << endl;
        }
        // find diretorio arquivo
        else if (strcmp(c, "find") == 0) {
            cout << "9" << endl;
        }
        // df
        else if (strcmp(c, "df") == 0) {
            cout << "10" << endl;
        }
        // umount
        else if (strcmp(c, "umount") == 0) {
            mountedFS = false;
            // dar free? ou preparar para poder montar outro arquivo
        }
        // sai
        else if (strcmp(c, "sai") == 0) {
            break;
        }
        else if (strcmp(c, "fat") == 0) {
            for (int i = 0; i < NUMBLOCKS; i++)
                cout << fat[i] << " ";
            cout << endl;
        }
        else if (strcmp(c, "fsm") == 0) {
            for (int i = 0; i < NUMBLOCKS; i++)
                cout << fsm[i] << " ";
            cout << endl;
        }
        else if (strcmp(c, "teste") == 0) {
            cout << "Para criar funções de debug." << endl;
        }
        else
            cout << "Comando não reconhecido." << endl;
    }
}
