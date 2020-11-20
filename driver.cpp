#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#include "driver.hpp"
#include "tools.hpp"


extern int FAT[FATSIZE];

Driver::Driver() {
    if (DEBUG) printf("Driver inicializado!\n");
}

Driver::~Driver() {

}

void Driver::mount(char *nomeArq, bool existe) { //Inicializa o parser sobre um arquivo txt
    diskName = nomeArq; //Nomeia arquivo disco;
    if (existe) {
        if (DEBUG) printf("Driver inicializado!\n");
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
        getline(inFile, linha);
        istringstream iss(linha);
        for (int i = 0; i < FATSIZE; i++) {
            getline( iss, palavra , '|');
            FAT[i] = atoi(palavra.c_str());
        }
        if (DEBUG) {
          printf("memory FAT: [");
          for (int i = 0; i < FATSIZE; i++) printf(" %02d", FAT[i]);
          printf("]\n");
        }
        inFile.close();
        if (DEBUG) std::cout << " Terminou mount" << '\n';

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
            fprintf(fp,"0|"); //Cria FAT registro
        }

        // para registro de data https://linux.die.net/man/3/localtime
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        unsigned long long dataInt = datainfo();
        fprintf(fp,"root/|0|%lld|%lld|%lld|root/|0|\n", dataInt, dataInt, dataInt);

        fclose (fp);
        fp = fopen (nomeArq,"a"); //ao invés de pular é melhor reabrir e append
        for (int i = 0; i < FATSIZE; i++) fprintf(fp,"|\n");
        fclose (fp);
        if (DEBUG) std::cout << " Terminou mount" << '\n';
    }

}


arq *Driver::SearchFile(string name)
{
  //Variáveis___________________________________________________________________
  ifstream disk;
  string nameTyped;
  string linha; string palavra;
  string token1; string conteudo;
  int l;
  //Search______________________________________________________________________
  nameTyped = "\""; //Prepara string tipificada com aspas para procura em disco
  nameTyped.append(name);
  nameTyped.append("\"");
  if (DEBUG) cout << "Procurando: " << nameTyped << '\n';

  disk.open(diskName);
  getline (disk, linha); //Pula freespace e FAT
  getline (disk, linha);

  for (int i = 0; i < FATSIZE; i++) {
      getline (disk, linha);
      istringstream iss(linha);
      getline(iss, palavra, '|');
      if (palavra == nameTyped) {
          arq *file = new arq; // Começa a preencher a struct arquivo
          file->name = palavra;
          getline(iss, token1, '|');
          file->localFAT = atoi(token1.c_str());
          getline(iss, token1, '|');
          file->size = atoi(token1.c_str());
          getline(iss, token1, '|');
          file->createdAt = atol(token1.c_str());
          getline(iss, token1, '|');
          file->updatedAt = atol(token1.c_str());
          getline(iss, token1, '|');
          file->accessedAt = atol(token1.c_str());
          getline(iss, token1, '|');
          file->insideDirName = token1;

          //Conteudo____________________________________________________________
          getline(iss, conteudo, '|');
          file->content.push_back(conteudo);
          disk.close();

          int nFat = FAT[file->localFAT];
          while (nFat != -1){
              disk.open(diskName);   //reabre para procura;
              getline (disk, linha); //Pula freespace e FAT
              getline (disk, linha);
              for (int i = 0; i <= nFat; i++) getline (disk, linha); //conta FAT
              istringstream iss(linha);
              getline(iss, conteudo, '|'); //Pula primeiro pipeline
              getline(iss, conteudo, '|');
              file->content.push_back(conteudo);
              nFat = FAT[nFat]; //anda o número de FAT;
              disk.close(); //fecha disco
          }

          if (DEBUG) ImprimeArquivo(file);

      }
  }
}

// imprime informação e conteúdo de arquivo
void Driver::ImprimeArquivo(arq *file)
{
  cout << "--------------------------------------------------------------------------------"<<'\n';
  cout << "Nome: " << file->name << '\n';
  cout << "FAT: " << file->localFAT << '\n';
  cout << "Tamanho: " << file->size << '\n';
  cout << "Criado em: " << file->createdAt << '\n';
  cout << "Atualizado em: " << file->createdAt << '\n';
  cout << "Acessado em: " << file->accessedAt << '\n';
  cout << "Diretório: " << file->insideDirName << '\n';
  cout << '\n';
  cout << "Conteúdo: " << '\n';

  for (auto& i : file->content) { // itera vetor
        // Print the values
        cout << i << ' ';
  }
  cout <<'\n'<< "------------------------------------------------------------------------------"<<'\n';
}

// aponta próx bloco free
int Driver::whereIsFree()
{
  ifstream disk;
  string linha;
  string token1;
  disk.open(diskName);
  getline (disk, linha); //Pula freespace e FAT
  getline (disk, linha);
  for (int i = 0; i < FATSIZE; i++) {
    getline (disk, linha);
    istringstream iss(linha);
    getline(iss, token1, '|');
    if (token1.empty()) return i; //devolve posição do FAT
  }
}

string Driver::nFAT(int nFat)
{
    ifstream disk;
    string bloco;
    disk.open(diskName);
    getline (disk, bloco); //Pula freespace e FAT
    getline (disk, bloco);
    for (int i = 0; i < nFat; i++) getline (disk, bloco);
    return bloco;
}

int Driver::nFAT(string name, string bloco)
{
    string token1;
    istringstream iss(bloco);
    while (token1 != name) {
        getline(iss, token1, '|');
    }
    getline(iss, token1, '|');
    return atoi(token1.c_str());
}

void Driver::mkDir(string dirName)
{
    string token1;
    string newName;
    int nFat;
    int nFatAtual = 0;
    istringstream iss(dirName);
    vector <string> caminho;
    getline(iss, token1, '/');
    caminho.push_back(token1.append("/"));
    nFat = nFAT(caminho.back(), nFAT(nFatAtual));
    newName = token1;
    while (!token1.empty()) {
        newName = token1;
        getline(iss, token1, '/');
        if(!token1.empty()) caminho.push_back(token1.append("/"));
    }
    // for(auto& i : caminho) {
    //     nFat = (string newName, nFAT(0))
    // }


}

dir *Driver::dirStruct(string linha)
{

  string token1;
  istringstream iss(linha);
  int l;

  dir *diretorio = new dir; // Começa a preencher a struct diretorio
  getline(iss, token1, '|');
  diretorio->name = token1;
  getline(iss, token1, '|');
  diretorio->localFAT = atoi(token1.c_str());
  getline(iss, token1, '|');
  diretorio->createdAt = atol(token1.c_str());
  getline(iss, token1, '|');
  diretorio->updatedAt = atol(token1.c_str());
  getline(iss, token1, '|');
  diretorio->accessedAt = atol(token1.c_str());
  getline(iss, token1, '|');
  diretorio->dotdotBar = token1;

  getline(iss, token1, '|'); //Pastas e arquivos dentro.
  l = token1.length();
  while (!token1.empty()) {
      if (token1[0] == '\"') {
        diretorio->arqPont.push_back(token1);
      } else if (token1[l - 1] == '/'){
        diretorio->dotBar.push_back(token1);
      }
      getline(iss, token1, '|');
      l = token1.length();
  }
  return diretorio;
}
