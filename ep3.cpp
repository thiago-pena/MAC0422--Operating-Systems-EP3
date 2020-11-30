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
// Escreve o conteúdo da FAT da memória para o disco
void saveFat2(Driver *d);
// Escreve o conteúdo do Gerenciamento de Espaço Livre da memória para o disco
void saveFsm2(Driver *d);
void cleanFsmFile2(int k);
string readFile2(Driver *d, int k);
void writeFile2(Driver *d, string s, int k);
void accessedAtUpdater(int nFat, Driver *d);
void lowLevelFormat(Driver *d);

int fat[NUMBLOCKS];
int fsm[NUMBLOCKS]; // Free Space Management
bool mountedFS = false;

int main() {
    char prompt[MAXLEN];
    Driver *driver; // inicializa o driver

    using_history();
    strcpy(prompt, "[ep3] $ ");
    while (true) {
        char *strChar = readline(prompt);
        add_history(strChar);

        char *c = strtok(strChar, " ");
        char *arg1 = strtok(NULL, " ");
        char *arg2 = strtok(NULL, " ");

        if (DEBUG) {
            cout << "\t[DEBUG] c: " << c << endl;
            if (arg1 != NULL)
                cout << "\t[DEBUG] arg1: " << arg1 << endl;
            if (arg2 != NULL)
                cout << "\t[DEBUG] arg2: " << arg2 << endl;
        }

        // mount arquivo
        if (strcmp(c, "mount") == 0) {
            if (!mountedFS) {
                driver = new Driver(); // inicializa o driver
                FILE *fp;

                if ((fp = fopen (arg1,"r"))) { //Caso o arquivo exista (read)
                    fclose (fp);
                    driver->mount(arg1, 1);
                } else {
                    driver->mount(arg1, 0);
                }
                mountedFS = true;
                // Atualiza prompt com o nome do sistema de arquivos
                strcpy(prompt, "[");
                strcat(prompt, arg1);
                strcat(prompt, "] $ ");
            }
            else
                cout << "Já há um sistema de arquivos montado." << endl;

        }
        // cp origem destino
        else if (strcmp(c, "cp") == 0) {
            driver->copy(arg1, arg2);
        }
        // mkdir diretorio
        else if (strcmp(c, "mkdir") == 0) {
            driver->mkDirAndTouch(arg1, 0);
            cout << "3" << endl;
        }
        else if (strcmp(c, "cat2") == 0) {
            string s = readFile2(driver, atoi(arg1));
            cout << "\t[DEBUG] " << s << endl;
        }
        // rmdir diretorio
        else if (strcmp(c, "rmdir") == 0) {
            driver->rmDir(arg1, LOWLEVELFORMAT);
            cout << "4" << endl;
        }
        // cat arquivo
        else if (strcmp(c, "cat") == 0) {
            driver->SearchFile(arg1, 0, LOWLEVELFORMAT);
            cout << "5" << endl;
        }
        // touch arquivo
        else if (strcmp(c, "touch") == 0) {
            driver->mkDirAndTouch(arg1, 1);
            cout << "6" << endl;
        }
        // rm arquivo
        else if (strcmp(c, "rm") == 0) {
            driver->SearchFile(arg1, 1, LOWLEVELFORMAT);
            cout << "7" << endl;
        }
        // ls diretorio
        else if (strcmp(c, "ls") == 0) {
            driver->ListDir(arg1);
            cout << "8" << endl;
        }
        // find diretorio arquivo
        else if (strcmp(c, "find") == 0) {
            bool achou = driver->finder(arg1, arg2);
            if (!achou) cout << "find: \'" << arg2 << "\': Arquivo ou diretório não encontrado" << '\n';
            cout << "9" << endl;
        }
        // df
        else if (strcmp(c, "df") == 0) {
            driver->df();
            cout << "10" << endl;
        }
        // umount
        else if (strcmp(c, "umount") == 0) {
            if (DEBUG) cout << "\t[DEBUG] comando umount" << endl;
            if (mountedFS) driver->umount();
            mountedFS = false;

            // dar free? ou preparar para poder montar outro arquivo
        }
        // sai
        else if (strcmp(c, "sai") == 0) {
            break;
        }
        else if (strcmp(c, "fat") == 0) {
            for (int i = 0; i < NUMBLOCKS; i++)
                cout << i << "\t";
            cout << endl;
            for (int i = 0; i < NUMBLOCKS; i++)
                cout << fat[i] << "\t";
            cout << endl;
        }
        else if (strcmp(c, "fsm") == 0) {
            for (int i = 0; i < NUMBLOCKS; i++)
                cout << i << "\t";
            cout << endl;
            for (int i = 0; i < NUMBLOCKS; i++)
                cout << fsm[i] << "\t";
            cout << endl;
        }
        else if (strcmp(c, "saveFat") == 0) {
            saveFat2(driver);
            cout << "Fim save fat" << endl;
            // driver->saveFat();
        }
        else if (strcmp(c, "saveFsm") == 0) {
            saveFsm2(driver);
        }
        else if (strcmp(c, "writeNewFile") == 0) {
            writeFile2(driver, arg1, -1);
        }
        else if (strcmp(c, "writeFile") == 0) {
            if (arg1 == NULL || arg2 == NULL) continue;
            writeFile2(driver, arg1, atoi(arg2));
        }
        else if (strcmp(c, "readFile") == 0) {
            if (arg1 == NULL) continue;
            cout << readFile2(driver, atoi(arg1)) << endl;
        }
        else if (strcmp(c, "format") == 0) {
            driver->lowLevelFormat2();
        }
        else if (strcmp(c, "accessedAtUpdater") == 0) {
            accessedAtUpdater(atoi(arg1), driver);
        }
        else if (strcmp(c, "teste") == 0) {
            cout << "Para criar funções de debug." << endl;
        }
        else
            cout << "Comando não reconhecido." << endl;
        if (LOWLEVELFORMAT) driver->lowLevelFormat2();
    }
}


void saveFat2(Driver *d) {
    fstream fs(d->getDiskName());
    fs.seekg(FATPOS, ios::beg); // Seek FAT
    string fat_string = intToString(fat[0]);;
    for (int i = 1; i < NUMBLOCKS; i++)
        fat_string += "|" + intToString(fat[i]);
    fs << fat_string;
    fs.close();
    cout << "FAT foi atualizada no disco." << endl;
    return;
}

void saveFsm2(Driver *d) {
    fstream fs(d->getDiskName());
    fs.seekg(BITMAP, ios::beg); // Seek BITMAP
    string fsm_string;
    for (int i = 0; i < NUMBLOCKS; i++)
        fsm_string += to_string(fsm[i]);
    fs << fsm_string;
    fs.close();
    cout << "FSM foi atualizada no disco." << endl;
}

void accessedAtUpdater(int nFat, Driver *d) {
    ifstream disk(d->getDiskName());
    string token;
    string bloco;
    string bInit; string bEnd;

    //disk.open(diskName);
    getline (disk, bloco); //Pula freespace e FAT
    getline (disk, bloco);

    for (int i = 0; i <= nFat; i++) getline (disk, bloco);// Vai até a linha FAT
    istringstream iss(bloco);

    // int jump = 4; // Diferencia local da data de arquivo e diretorio
    // if (bloco[0] == '\"') jump = 5;

    for (int j = 0; j < 4; j++) { // Pega o inicio e grava em um buffer inicio
        getline(iss, token, '|');
        bInit += token + "|";
    }
    getline(iss, token, '|');
    getline(iss, bEnd, '\n');
    token = datainfoString();
    bInit += token + "|" + bEnd; // Concatena com o buffer do fim do bloco
    disk.close();

    fstream fs(d->getDiskName());
    fs.seekg(ROOT + nFat*BLOCKSIZE, ios::beg);
    fs << bInit;
}

// Recebe um Driver e um inteiro k representando um bloco de início de um
// arquivo ou diretório e retorna uma string com o conteúdo de todos os seus
// blocos
string readFile2(Driver *d, int k) {
    fstream fs(d->getDiskName());
    string fileContent = "";
    string buffer;
    while (fat[k] != -1) {
        fs.seekg(ROOT + k*BLOCKSIZE, ios::beg);
        getline (fs, buffer);
        fileContent += buffer;
        k = fat[k];
        // Testar seek pra trás (qdo chegar no EOF) ---> funcionou, depois pesquisar mais, qdo pode dar problema?
    }
    fs.seekg(ROOT + k*BLOCKSIZE, ios::beg); // Lê o bloco final (fat -1)
    getline (fs, buffer, '@');
    fileContent += buffer;
    fs.close();
    return fileContent;
    // descontar os @@@@@@@@@
}

// Recebe um inteiro k, indicando o bloco de início de um arquivo no disco e
// marca como livres no vetor fsm[] as posições do arquivo dadas pelo vetor
// fat[].
void cleanFsmFile2(int k) {
    for (int i = k; i >= 0 && fsm[i] == 1; i = fat[i])
        fsm[i] = 0;
}

// Recebe um Driver, uma string s a ser inserida no disco e um inteiro k
// representando um bloco de início ou -1 se for um arquivo novo. Escreve a
// string s no disco alocando os
// blocos conforme necessário (livres se o bloco k está livre, caso contrário,
// ajusta os blocos utilizados conforme necessidade.
void writeFile2(Driver *d, string s, int k) {
    cleanFsmFile2(k);
    fstream fs(d->getDiskName());
    if (k == -1) k = firstFit();
    cout << "\t\t[P] fsm[0]: " << fsm[0] << endl;
    while (s.size() > BLOCKSIZE - 1) {
        fs.seekg(ROOT + k*BLOCKSIZE, ios::beg); // Seek base do bloco k
        fs << s.substr(0, BLOCKSIZE - 1);
        s.erase(0, BLOCKSIZE - 1);
        fsm[k] = 1;
        int kAnt = k;
        k = nextFit(k);
        fat[kAnt] = k;
    }
    cout << "\t\t[P] fsm[0]: " << fsm[0] << endl;
    fs.seekg(ROOT + k*BLOCKSIZE, ios::beg);
    if (s.size() < BLOCKSIZE) {
        while (s.size() < BLOCKSIZE - 1)
            s += "@";
    }
    fs << s;
    fs.close();
    fsm[k] = 1;
    fat[k] = -1;
    cout << "\t\t[P] fsm[0]: " << fsm[0] << endl;
}

void lowLevelFormat(Driver *d) {
    fstream fs(d->getDiskName());
    string s = "";
    for (int i = 0; i < BLOCKSIZE - 1; i++)
        s += "@";
    for (int i = 0; i < NUMBLOCKS; i++) {
        if (fsm[i] == 0) {
            fat[i] = -2;
            fs.seekg(ROOT + i*BLOCKSIZE, ios::beg);
            fs << s;
        }
    }
    fs.close();
}
