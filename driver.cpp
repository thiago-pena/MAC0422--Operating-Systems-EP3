#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#include "driver.hpp"
#include "tools.hpp"


extern int fat[NUMBLOCKS];
extern int fsm[NUMBLOCKS];
extern bool mountedFs;

Driver::Driver()
{
    if (DEBUG) printf("Driver inicializado!\n");
    diskName = NULL;
}

Driver::~Driver()
{
  if (mountedFS) umount();
}

void Driver::mount(char *nomeArq, bool existe) //Inicializa o parser sobre um arquivo txt
{
    diskName = nomeArq; //Nomeia arquivo disco;
    if (existe) {
        if (DEBUG) printf("Driver inicializado!\n");
        // Variáveis________________________________________________________
        string arquivo; string linha; string palavra;
        string token1; string token2; string token3;
        string token4; string token5; string token6;
        // int lFinal;
        ifstream inFile;

        inFile.open(nomeArq);

        if (!inFile) { // Verifica falha
            printf("Falha na leitura de %s\n", nomeArq);
        }


        // Gerencimanto de Espaço Livre
        getline(inFile, linha);
        for (int i = 0; i < NUMBLOCKS; i++)
            fsm[i] = linha[i] - '0';

        // FAT______________________________________________________________
        getline(inFile, linha);
        istringstream iss(linha);
        for (int i = 0; i < NUMBLOCKS; i++) {
            getline( iss, palavra , '|');
            fat[i] = atoi(palavra.c_str());
        }

        inFile.close();
        if (DEBUG) std::cout << " Terminou mount" << '\n';

    } else { //Cria arquivo.txt caso ele não exista
        FILE *fp;
        fp = fopen (nomeArq, "w");
        if(DEBUG) printf("Arquivo %s criado.\n", nomeArq);
        if (fp == NULL) {
            printf("Falha na criação de %s\n", nomeArq);
            fclose (fp);
        }

        //free space
        fprintf(fp, "1");
        for (int i = 0; i < NUMBLOCKS - 1; i++) fprintf(fp, "0");
        for (int i = NUMBLOCKS%BLOCKSIZE; i < BLOCKSIZE - 1; i++) // completa o último bloco com '@'s
            fprintf(fp, "@");
        fprintf(fp,"\n");

        for (int i = 1; i < NUMBLOCKS; i++) fsm[i] = 0; //Carrega FSM na memória
        fsm[0] = 1;

        //Cria FAT registro
        string fatStr = "";
        fatStr += "-0001|";
        for (int i = 0; i < NUMBLOCKS - 2; i++) fatStr += "00000|";
        fatStr += "00000";
        for (int i = fatStr.size()%BLOCKSIZE; i < BLOCKSIZE - 1; i++) // completa o último bloco com '@'s
            fatStr += "@";
        fatStr += "\n";
        fprintf(fp, "%s", fatStr.c_str());

        //Carrega FAT na memória
        fat[0] = -1;
        for (int i = 1; i < NUMBLOCKS; i++) fat[i] = 0;

        // para registro de data https://linux.die.net/man/3/localtime
        // time_t t = time(NULL);
        // struct tm tm = *localtime(&t);

        unsigned long long dataInt = datainfo();
        // fprintf(fp,"root/|0|%lld|%lld|%lld|root/|0|\n", dataInt, dataInt, dataInt);
        fprintf(fp, "root/|0|%lld|%lld|%lld|root/|0|", dataInt, dataInt, dataInt);
        int len = strlen("root/|0|20201121084741|20201121084741|20201121084741|root/|0|");
        for (int i = 0; i < 100 - len - 1; i++)
            fprintf(fp, "@");
        fprintf(fp, "\n");

        // Cria os blocos restantes preenchidos com "@"
        string s = "";
        for (int i = 0; i < BLOCKSIZE - 1; i++)
            s += "@";
        for (int i = 1; i < NUMBLOCKS; i++) {
            fprintf(fp, "%s", s.c_str());
            fprintf(fp, "\n");
        }

        fclose (fp);
        if (DEBUG) std::cout << " Terminou mount" << '\n';
    }

}

void Driver::umount()
{
    cout << "driver.umount" << endl;
    std::cout << diskName << " desmontado." << '\n';
    diskName = nullptr;
}

bool Driver::SearchFile(string absoluteDirName, bool remove, bool LowLevelFormat)
{
  string fileName;
  string token;
  string absolutePathName;
  string bloco, bloco2;

  int pLength, tLength, nFat, nFat2;

  absoluteDirName += "//"; //cambiarra para ler um vazio
  istringstream iss1(absoluteDirName);

  // procura pelo nome do arquivo ou diretório
  getline(iss1, token, '/');
  while (!token.empty()) {
      absolutePathName += token + "/";
      fileName = token + "/";
      getline(iss1, token, '/');
  }

  // separa path do nome do arquivo
  pLength = absolutePathName.length();
  tLength = fileName.length();
  absolutePathName = absolutePathName.substr(0, pLength - tLength);
  fileName = fileName.substr(0, tLength - 1);

  // Acha Fat da pasta pai e seu bloco
  nFat = absolutePath(absolutePathName);
  bloco = loadBlock(nFat);
  bloco += "|";

  // procura arquivo
  istringstream iss(bloco);
  for (int i = 0; i <= METADIR; i++) getline(iss, token, '|');
  while (token != fileName && token[0] != '@') getline(iss, token, '|');

  if (token == fileName) {
      getline(iss, token, '|');

  } else {
      std::cout << "cat: " << fileName <<": Arquivo ou diretório não encontrado" << '\n';
      return false;
  }

  nFat2 = atoi(token.c_str()); //Fat do arquivo de fato
  accessedAtUpdater(nFat2, true);

  // Caso flag remove remove arquivo
  if (remove) {
      fstream fs(diskName);
      bloco.erase(0, BLOCKSIZE - 1);
      bloco = loadBlock(nFat);
      bloco = metaRemover(bloco, fileName);
      fs.seekg(ROOT + nFat*BLOCKSIZE, ios::beg);
      fs << bloco;

      if (LowLevelFormat) { // Para efeitos de DEBUG remove com "zeros"
          //bloco2 = loadBlock(nFat2);
          //bloco2.erase(0, BLOCKSIZE - 1);
          if (bloco2.size() < BLOCKSIZE) {
              while (bloco2.size() < BLOCKSIZE - 1)
                  bloco2 += "@"; // completa o espaço desperdiçado com "@"
          }
          fs.seekg(ROOT + nFat2*BLOCKSIZE, ios::beg);
          fs << bloco2;
      }

      fs.close();
      fat[nFat2] = 0;
      fsm[nFat2] = 0;
      saveFat();
      saveFsm();

  // Carrega arquivo e imprime info.
  } else {
      bloco2 = loadBlock(nFat2);
      ImprimeArquivo(bloco2, true);// Começa a chamar pela impressão
      while (fat[nFat2] != -1) {
          nFat2 = fat[nFat2];
          bloco2 = loadBlock(nFat2);
          ImprimeArquivo(bloco2, false);
      }
      return true;
    }
}

void Driver::ListDir(string absoluteDirName)
{
    int nFat;
    string bloco, token;

    // carrega bloco do diretorio seguindo o caminho.
    nFat = absolutePath(absoluteDirName);
    bloco = loadBlock(nFat);
    bloco += "|";

    // segue procurando pastas e arquivos
    istringstream iss(bloco);
    for (int i = 0; i <= METADIR; i++) getline(iss, token, '|');
    while (!token.empty() && token[0] != '@') {
        getline(iss, token, '|');
        nFat = atoi(token.c_str());
        listener(nFat);
        getline(iss, token, '|');
    }
}

void Driver::listener(int nFat)
{
    string bloco, token, lastC;
    int l;
    bloco = loadBlock(nFat);
    istringstream iss(bloco);

    getline(iss, token, '|');
    std::cout << "Nome: " << token;
    l = token.length();
    lastC = token.substr(l - 1, l);

    // verefica se é diretório "/" ou arquivo.
    if (lastC == "/") {
        getline(iss, token, '|');
        getline(iss, token, '|');
        getline(iss, token, '|');
        std::cout << " Modificado em: " << token << '\n';
    } else {
        getline(iss, token, '|');
        getline(iss, token, '|');
        std::cout << " Tamanho: " << token;
        getline(iss, token, '|');
        getline(iss, token, '|');
        std::cout << " Modificado em: " << token << '\n' ;
    }
}

void Driver::finder(string absoluteDirName, string file)
{
    int nFat;
    // int l;
    string bloco, token, lastC;
    bool achou = false;

    nFat = absolutePath(absoluteDirName);
    bloco = loadBlock(nFat);
    bloco += "|";

    istringstream iss(bloco);
    for (int i = 0; i <= METADIR; i++) getline(iss, token, '|');
    while (!token.empty() && token[0] != '@') {
        // caso nome do arquivo é encontrado imprime o nome dele
        // concatenado com o caminho.
        if (token == file) {
            std::cout << absoluteDirName << file<< '\n';
            achou = true;
        }
        getline(iss, token, '|');
        getline(iss, token, '|');
    }
    // Caso não achou imprime essa informação.
    if (!achou) std::cout << "find: \'"<< file <<"\': Arquivo ou diretório não encontrado" << '\n';
}

void Driver::ImprimeArquivo(string bloco, bool isInit)
{
    string token;
    if (isInit) {
        bloco += "|";
        istringstream iss(bloco);
        std::cout << "------------------------------------------------------" << '\n';
        getline(iss, token, '|');
        std::cout << "Arquivo: " << token << '\n'<< '\n';
        std::cout << "Meta:" << '\n';
        getline(iss, token, '|');
        getline(iss, token, '|');
        std::cout << "    tamanho: " << token << '\n';
        getline(iss, token, '|');
        std::cout << "    Criado em: " << token << '\n';
        getline(iss, token, '|');
        std::cout << "    Modificado em: " << token << '\n';
        getline(iss, token, '|');
        std::cout << "    Acessado em: " << token << '\n';
        getline(iss, token, '|');
        std::cout << "    Pasta pai: " << token << '\n';
        getline(iss, token, '|');
        getline(iss, token, '@');
        std::cout << "Conteúdo:" << '\n';
        std::cout << token << '\n';
    } else {
        bloco += "|";
        istringstream iss(bloco);
        getline(iss, token, '@');
        std::cout << token << '\n';
    }
}

void Driver::df()
{
    int nDir, nFil, nFree, nWaste, l;
    string lastC, bloco, token;
    ifstream disk(diskName);

    // inicializa variáveis contadoras
    nDir = nFil = nFree = nWaste = 0;

    getline (disk, bloco); //Pula freespace e FAT
    getline (disk, bloco);
    for (int i = 0; i < FATSIZE; i++) {

        // verifica no vetor free se bloco está sendo usado ou free.
        if (fsm[i] == 1) {

            // acrescenta um tamanho de bloco no número de waste
            // para subtrair depois
            nWaste += BLOCKSIZE - 1;

            getline (disk, bloco);
            bloco += "|";
            istringstream iss(bloco);
            getline(iss, token, '|');

            // verifica se é arquivo ou dir e soma.
            l = token.length();
            nWaste -= (l + 1);
            lastC = token.substr(l - 1, l);
            if (lastC == "/") nDir++;
            else nFil++;

            getline(iss, token, '|');
            while (!token.empty() && token[0] != '@') {
                l = token.length() + 1;
                nWaste -= l;
                getline(iss, token, '|');
            }
        } else {
            nFree += BLOCKSIZE;
        }
    }
    // imprime informação.
    std::cout << "------------------------------------------------------" << '\n';
    std::cout << "df Info:" << '\n';
    std::cout << "  Quantidade de diretórios: " << nDir << '\n';
    std::cout << "  Quantidade de arquivos: " << nFil << '\n';
    std::cout << "  Espaço livre: " << nFree << '\n';
    std::cout << "  Espaço desperdiçado: " << nWaste << '\n';
}

void Driver::mkDirAndTouch(string absoluteDirName, bool isFile)
{
    if (SearchFile(absoluteDirName, 0, false)) return;

    string dirName;
    string token;
    string absolutePathName;
    string bloco;
    string bInit;

    int pLength, tLength, nFat, freeNFat;
    if (absoluteDirName[absoluteDirName.size() - 1] != '/')
        absoluteDirName += "/";
    istringstream iss1(absoluteDirName);

    // procura pelo nome do arquivo ou diretório
    getline(iss1, token, '/');
    while (!token.empty()) { // (Pena) pega o último diretório e o nome da pasta/arquivo (no token)
        absolutePathName += token + "/";
        dirName = token + "/";
        getline(iss1, token, '/');
    }
    // separa caminho de local final.
    pLength = absolutePathName.length();
    tLength = dirName.length();
    absolutePathName = absolutePathName.substr(0, pLength - tLength);
    if (isFile) dirName = dirName.substr(0, tLength - 1);
    nFat = absolutePath(absolutePathName);

    ifstream disk(diskName);
    getline (disk, bloco); //Pula freespace e FAT
    getline (disk, bloco);

    for (int i = 0; i <= nFat; i++) getline (disk, bloco);
    bloco += "|";
    istringstream iss2(bloco);

    getline(iss2, token, '|');
    while (token[0] != '@') {
        bInit += token + "|";
        getline(iss2, token, '|');
    }
    bInit += dirName + "|";
    freeNFat = firstFit();
    bInit += to_string(freeNFat) + "|";

    if (bInit.size() < BLOCKSIZE) {
        while (bInit.size() < BLOCKSIZE - 1)
            bInit += "@"; // completa o espaço desperdiçado com "@"
    }

    fstream fs(diskName);
    fs.seekg(ROOT + nFat*BLOCKSIZE, ios::beg);
    bloco.erase(0, BLOCKSIZE - 1);
    fs << bInit;

    fs.seekg(ROOT + freeNFat*BLOCKSIZE, ios::beg);
    istringstream iss3(bInit);
    string createdAt = datainfoString();
    string updatedAt = datainfoString();
    string accessedAt = datainfoString();

    getline(iss3, token, '|');
    string insideDirName = token;
    getline(iss3, token, '|');
    string insideDirNameFat =  token;
    if(isFile) {
        string size = "0";
        string newFile = dirName + "|" + to_string(freeNFat) + "|" + size;
        newFile += "|" + createdAt + "|" + updatedAt + "|" + accessedAt;
        newFile += "|" + insideDirName + "|" + insideDirNameFat + "|";
        if (newFile.size() < BLOCKSIZE) {
            while (newFile.size() < BLOCKSIZE - 1)
                newFile += "@"; // completa o espaço desperdiçado com "@"
        }
        fs << newFile;
    } else {
        string newBloco = dirName + "|" + to_string(freeNFat);
        newBloco += "|" + createdAt + "|" + updatedAt + "|" + accessedAt;
        newBloco += "|" + insideDirName + "|" + insideDirNameFat + "|";
        if (newBloco.size() < BLOCKSIZE) {
            while (newBloco.size() < BLOCKSIZE - 1)
                newBloco += "@"; // completa o espaço desperdiçado com "@"
        }
        fs << newBloco;
    }

    disk.close();
    accessedAtUpdater(nFat, false); // Atualiza tempo de acesso
    fat[freeNFat] = -1;
    fsm[freeNFat] = 1;
    // saveFat();
    // saveFsm();
}

void Driver::mkDirAndTouch2(string absoluteDirName, bool isFile)
{
    if (SearchFile(absoluteDirName, 0, false)) return;

    string dirName;
    string token;
    string absolutePathName;
    string bloco;
    string bInit;

    int pLength, tLength, nFat, freeNFat;
    if (absoluteDirName[absoluteDirName.size() - 1] != '/')
        absoluteDirName += "/";
    istringstream iss1(absoluteDirName);

    //////////////////////////////////////////

    //////////////////////////////////////////

    // procura pelo nome do arquivo ou diretório
    getline(iss1, token, '/');
    while (!token.empty()) { // (Pena) pega o último diretório e o nome da pasta/arquivo (no token)
        absolutePathName += token + "/";
        dirName = token + "/";
        getline(iss1, token, '/');
    }
    // separa caminho de local final.
    pLength = absolutePathName.length();
    tLength = dirName.length();
    absolutePathName = absolutePathName.substr(0, pLength - tLength);
    if (isFile) dirName = dirName.substr(0, tLength - 1);
    nFat = absolutePath(absolutePathName);

    ifstream disk(diskName);
    getline (disk, bloco); //Pula freespace e FAT
    getline (disk, bloco);

    for (int i = 0; i <= nFat; i++) getline (disk, bloco);
    bloco += "|";
    istringstream iss2(bloco);

    getline(iss2, token, '|');
    while (token[0] != '@') {
        bInit += token + "|";
        getline(iss2, token, '|');
    }
    bInit += dirName + "|";
    freeNFat = firstFit();
    bInit += to_string(freeNFat) + "|";

    if (bInit.size() < BLOCKSIZE) {
        while (bInit.size() < BLOCKSIZE - 1)
            bInit += "@"; // completa o espaço desperdiçado com "@"
    }

    fstream fs(diskName);
    fs.seekg(ROOT + nFat*BLOCKSIZE, ios::beg);
    bloco.erase(0, BLOCKSIZE - 1);
    fs << bInit;

    fs.seekg(ROOT + freeNFat*BLOCKSIZE, ios::beg);
    istringstream iss3(bInit);
    string createdAt = datainfoString();
    string updatedAt = datainfoString();
    string accessedAt = datainfoString();

    getline(iss3, token, '|');
    string insideDirName = token;
    getline(iss3, token, '|');
    string insideDirNameFat =  token;
    if(isFile) {
        string size = "0";
        string newFile = dirName + "|" + to_string(freeNFat) + "|" + size;
        newFile += "|" + createdAt + "|" + updatedAt + "|" + accessedAt;
        newFile += "|" + insideDirName + "|" + insideDirNameFat + "|";
        if (newFile.size() < BLOCKSIZE) {
            while (newFile.size() < BLOCKSIZE - 1)
                newFile += "@"; // completa o espaço desperdiçado com "@"
        }
        fs << newFile;
    } else {
        string newBloco = dirName + "|" + to_string(freeNFat);
        newBloco += "|" + createdAt + "|" + updatedAt + "|" + accessedAt;
        newBloco += "|" + insideDirName + "|" + insideDirNameFat + "|";
        if (newBloco.size() < BLOCKSIZE) {
            while (newBloco.size() < BLOCKSIZE - 1)
                newBloco += "@"; // completa o espaço desperdiçado com "@"
        }
        fs << newBloco;
    }

    disk.close();
    accessedAtUpdater(nFat, false); // Atualiza tempo de acesso
    fat[freeNFat] = -1;
    fsm[freeNFat] = 1;
    // saveFat();
    // saveFsm();
}

int Driver::rmDir(string absoluteDirName, bool LowLevelFormat)
{
    string dirName;
    string token;
    string absolutePathName;
    string bloco, blocoP, newBlocoP, lastC;

    int pLength, tLength, nFat, nFat2;
    // int l;

    absoluteDirName += "/"; //cambiarra para ler um vazio

    istringstream iss1(absoluteDirName);

    getline(iss1, token, '/'); //procura pelo nome do arquivo ou diretório
    while (!token.empty()) {
        absolutePathName += token + "/";
        dirName = token + "/";
        getline(iss1, token, '/');
    }

    pLength = absolutePathName.length();
    tLength = dirName.length();
    absolutePathName = absolutePathName.substr(0, pLength - tLength);

    nFat2 = absolutePath(absolutePathName);
    nFat = absolutePath(absoluteDirName);
    std::cout << "absolutePathName: " <<absolutePathName<<" nFat:"<< nFat2<< '\n';
    std::cout << "absoluteDirName: " <<absoluteDirName<<" nFat:"<< nFat<< '\n';
    remover(nFat, LowLevelFormat);

    blocoP = loadBlock(nFat2);
    newBlocoP = metaRemover(blocoP, dirName);

    fstream fs(diskName);

    if (LowLevelFormat) { // Para efeitos de DEBUG remove com "zeros"
        fs.seekg(ROOT + nFat*BLOCKSIZE, ios::beg);
        bloco.erase(0, BLOCKSIZE - 1);
        if (bloco.size() < BLOCKSIZE) {
            while (bloco.size() < BLOCKSIZE - 1)
                bloco += "@"; // completa o espaço desperdiçado com "@"
        }
        fs << bloco;
    }
    fs.seekg(ROOT + nFat2*BLOCKSIZE, ios::beg);
    fs << newBlocoP;
    fs.close();
}

void Driver::remover(int nFat, bool LowLevelFormat)
{
  std::cout << "remover nFat: " << nFat << '\n';
  int newFat, l;
  string bloco, bloco2, token, lastC;
  ifstream disk(diskName);
  // bool isFile = false;

  getline (disk, bloco); //Pula freespace e FAT
  getline (disk, bloco);
  for (int i = 0; i <= nFat; i++) getline (disk, bloco);
  bloco += "||";

  istringstream iss(bloco);
  getline(iss, token, '|');
  l = token.length();
  lastC = token.substr(l - 1, l);
  std::cout << "lastC: " << lastC << '\n';
  if (lastC == "/") { //verifica se é arquivo
      for (int i = 1; i < METADIR; i++) getline(iss, token, '|');
      getline(iss, token, '|');
      getline(iss, token, '|');
      while (!token.empty() && token[0] != '@') {
          newFat = atoi(token.c_str());
          remover(newFat, LowLevelFormat);
          getline(iss, token, '|');
          getline(iss, token, '|');
      }
  }
  disk.close();

  if (LowLevelFormat) { // Para efeitos de DEBUG remove com "zeros"
      fstream fs(diskName);
      fs.seekg(ROOT + nFat*BLOCKSIZE, ios::beg);
      if (bloco2.size() < BLOCKSIZE) {
          while (bloco2.size() < BLOCKSIZE - 1)
              bloco2 += "@"; // completa o espaço desperdiçado com "@"
      }
      fs << bloco2;
      fs.close();
  }
  fat[nFat] = 0;
  fsm[nFat] = 0;
  saveFat();
  saveFsm();
}

string Driver::metaRemover(string bloco, string name)
{
    string newBloco, token;
    // int l;
    bloco += "||";

    istringstream iss(bloco);

    for (int i = 0; i < METADIR; i++) {
        getline(iss, token, '|');
        newBloco += token + "|";
    }

    getline(iss, token, '|');
    while (token != name && token[0] != '@') {
        newBloco += token + "|";
        getline(iss, token, '|');
    }

    getline(iss, token, '|');
    getline(iss, token, '|');

    while (!token.empty() && token[0] != '@') {
        newBloco += token + "|";
        getline(iss, token, '|');
    }

    if (newBloco.size() < BLOCKSIZE) {
        while (newBloco.size() < BLOCKSIZE - 1)
            newBloco += "@"; // completa o espaço desperdiçado com "@"
    }

    return newBloco;
}

int Driver::absolutePath(string dirPath)
{
    string dirNext;
    string bloco;
    int nFat;

    dirPath += "/"; //cambiarra para ler um vazio
    istringstream iss1(dirPath);
    getline(iss1, dirNext, '/');

    bloco = loadBlock(0);
    dirNext += "/";
    nFat = cdDir(bloco, dirNext);

    getline(iss1, dirNext, '/');
    while (!dirNext.empty()) {
        bloco = loadBlock(nFat);
        dirNext += "/";
        nFat = cdDir(bloco, dirNext);
        getline(iss1, dirNext, '/');
    }
    return nFat;

}

string Driver::loadBlock(int nFat)
{
    string bloco;
    ifstream disk(diskName);

    getline (disk, bloco); //Pula freespace e FAT
    getline (disk, bloco);
    for (int i = 0; i <= nFat; i++) getline (disk, bloco);

    disk.close();
    return bloco;
}

int Driver::cdDir(string bloco, string dirName)
{
    if (DEBUG) std::cout << "[DEBUG] cd ./"<< dirName << '\n';
    if (dirName == "root/") {
      accessedAtUpdater(0, false);
      return 0;
    }
    string token;
    // int nFat;

    bloco += "|";
    istringstream iss(bloco);

    for (int i = 0; i <= METADIR; i++) getline(iss, token, '|');
    while (token != dirName && token[0] != '@') getline(iss, token, '|');

    if (token[0] == '@') {
      std::cout << "ERRO: caminho inválido" << '\n';
    } else {
      getline(iss, token, '|');
      int nFat = atoi(token.c_str());
      accessedAtUpdater(nFat, false); // Atualiza tempo de acesso
      return nFat;
    }
}

string Driver::getDiskName()
{
    return diskName;
}

// cp origem destino: cria dentro do sistema de arquivos simulado uma cópia do arquivo
// origem que está em algum sistema de arquivos real dentro do seu computador. No sistema de
// arquivos simulado a cópia de origem será salva em destino. Tanto origem quanto destino
// devem ser informados com o caminho completo tanto dentro do sistema de arquivos real quanto no
// simulado 1 . Esse comando será executado apenas para copiar arquivos em texto puro.
void Driver::copy(string origem, string destino)
{
    // exemplo
    // origem: ~/Documents/SO-ep3/so-ep3/arquivos_para_cp/teste1.txt
    // destino: /
    // std::ifstream arqOrigem("/home/bob/stuff.txt");

    ifstream arqOrigem(origem);
    fstream fs(diskName);

    if (!arqOrigem.is_open()) {
        cout << "[ERRO] Não foi possível abrir o arquivo " << origem << endl;
        return;
    }
    else
        cout << "Arquivo " << origem << " foi aberto com sucesso." << endl;

    string buffer;
    getline(arqOrigem, buffer);
    cout << "Teste buffer: " << buffer << endl;
    // arquivo1.txt|-1|8|162115102020|162215102020|162215102020|dir1/|conteudo
    // arquivo1.txt|-1|@|123|456|789

    // busca fat do diretório de destino (por enquanto é o root)
    // int blocoDir = 0;

    // Encontra primeiro bloco livre
    int k = firstFit();
    cout << "[DEBUG] Primeiro bloco livre: " << k << endl;
    cout << "Buffer size: " << buffer.size() << endl;
    // Verifica tamanho da entrada
    // Calcula número de blocos
    // como percorrer o arquivo de x em x caracteres? --> Seek
        // Pode ser com istream tb
        // No primeiro bloco, descontar o espaço dos atributos do arquivo
    string nomeOrigem = origem;
    // primeiro bloco incluir
    string size = "123";
    string createdAt = datainfoString();
    string updatedAt = datainfoString();
    string accessedAt = datainfoString();

    // Pasta Pai
    string insideDirName; string insideDirNameFat;
    if(destino == diskName) {
        insideDirName = "root/";
        insideDirNameFat = "0";
    }

    string bloco = "\"" + nomeOrigem + "\"" + "|" + "-1" + "|" + size;
    bloco += "|" + createdAt + "|" + updatedAt + "|" + accessedAt;
    bloco += "|" + insideDirName + "|" + insideDirNameFat;
    bloco += "|" + buffer;
    cout << bloco << endl;

    // --> verificar se o arquivo cabe no sistema de arquivos

    while (bloco.size() > BLOCKSIZE - 1) {
        fs.seekg(ROOT + k*BLOCKSIZE, ios::beg); // Seek base do bloco k
        fs << bloco.substr(0, BLOCKSIZE - 1);
        bloco.erase(0, BLOCKSIZE - 1);
        fsm[k] = 1; // Marca o bloco agora em uso no gerenciamento de espaço livre
        int kAnt = k;
        k = nextFit(k);
        fat[kAnt] = k;
        // insere no disco
        // remove um bloco do início
    }
    fs.seekg(ROOT + k*BLOCKSIZE, ios::beg); // Seek base do bloco k
    if (bloco.size() < BLOCKSIZE) {
        while (bloco.size() < BLOCKSIZE - 1)
            bloco += "@"; // completa o espaço desperdiçado com "@"
    }
    fs << bloco;
    fat[k] = -1; // Marca o fat como final do arquivo
    fsm[k] = 1; // Marca o bloco agora em uso no gerenciamento de espaço livre
    // Escrever no Root os arquivos inseridos e o bloco inicial de cada um
    fs.seekg(ROOT, ios::beg); // Seek base do Root

    // Preparar para funcionar em outros diretórios
}

//Atualiza a FAT
void Driver::saveFat()
{
    fstream fs(diskName);
    fs.seekg(FATPOS, ios::beg); // Seek FAT
    string fat_string = intToString(fat[0]);;
    for (int i = 1; i < NUMBLOCKS; i++)
        fat_string += "|" + intToString(fat[i]);
    fs << fat_string;
    fs.close();
    cout << "FAT foi atualizada no disco." << endl;
}

//Atualiza o registro de espaço livre
void Driver::saveFsm()
{
    fstream fs(diskName);
    fs.seekg(BITMAP, ios::beg); // Seek BITMAP
    string fsm_string;
    for (int i = 0; i < NUMBLOCKS; i++)
        fsm_string += to_string(fsm[i]);
    fs << fsm_string;
    fs.close();
    cout << "FSM foi atualizada no disco." << endl;
}

// Usa timeUpdater para atualizar a data de acesso
void Driver::accessedAtUpdater(int nFat, bool isFile)
{
    if (isFile) {
        timeUpdater(nFat, 5);
    } else {
        timeUpdater(nFat, 4);
    }
}

// Usa timeUpdater para atualizar a data de modificação
void Driver::updateAtUpdater(int nFat)
{
    timeUpdater(nFat, 3);
}

void Driver::timeUpdater(int nFat, int pos)
{
    ifstream disk(diskName);
    string token;
    string bloco;
    string bInit; string bEnd;
    //disk.open(diskName);
    getline (disk, bloco); //Pula freespace e FAT
    getline (disk, bloco);

    for (int i = 0; i <= nFat; i++) getline (disk, bloco);// Vai até a linha FAT
    istringstream iss(bloco);

    for (int j = 0; j < pos; j++) { // Pega o inicio e grava em um buffer inicio
        getline(iss, token, '|');
        bInit += token + "|";
    }
    getline(iss, token, '|');
    getline(iss, bEnd, '\n');
    token = datainfoString();

    bInit += token + "|" + bEnd; // Concatena com o buffer do fim do bloco
    disk.close();

    fstream fs(diskName);
    fs.seekg(ROOT + nFat*BLOCKSIZE, ios::beg);
    fs << bInit;
}
