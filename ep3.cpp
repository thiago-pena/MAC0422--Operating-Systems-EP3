// g++ ep3.cpp -lreadline
#include <iostream>
#include <string.h>
    // strcpy
#include <readline/readline.h>
    // readline
#include <readline/history.h>
    // add_history
using namespace std;

#define DEBUG 1
#define MAXLEN 255

string charToString(char *a);

int main() {
    char prompt[MAXLEN];
    using_history();
    strcpy(prompt, "teste prompt > ");
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
            cout << "1" << endl;
        }
        // cp origem destino
        else if (strcmp(c, "cp") == 0) {
            cout << "2" << endl;
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
            cout << "11" << endl;
        }
        // sai
        else if (strcmp(c, "sai") == 0) {
            cout << "12" << endl;
        }
        else
            cout << "Comando não reconhecido." << endl;
    }
}

// Recebe um char* e converte para string
string charToString(char *a)
{
    string s = "";
    int size = strlen(a);
    for (int i = 0; i < size; i++)
        s = s + a[i];
    return s;
}
