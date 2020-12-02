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
    diskName = NULL;
}

Driver::~Driver()
{
  if (mountedFS) umount();
}

//Inicializa o parser sobre um arquivo txt
void Driver::mount(char *nomeArq, bool existe)
{
    if (DEBUG) cout << "\t[DEBUG] Driver inicializado!" << endl;
    diskName = (char*)malloc(strlen(nomeArq)*sizeof(char) + 1);
    // diskName = new char[strlen(nomeArq + 1)];
    strcpy(diskName, nomeArq);
    if (existe) {
        string arquivo; string linha; string palavra;
        string token1; string token2; string token3;
        string token4; string token5; string token6;
        ifstream inFile;

        inFile.open(nomeArq);

        if (!inFile) { // Verifica falha
            printf("Falha na leitura de %s\n", nomeArq);
            exit(1);
        }

        // Carrega o Gerencimento de Espaço Livre na memória
        getline(inFile, linha);
        for (int i = 0; i < NUMBLOCKS; i++)
            fsm[i] = linha[i] - '0';

        // Carrega FAT na memória
        getline(inFile, linha);
        istringstream iss(linha);
        for (int i = 0; i < NUMBLOCKS; i++) {
            getline( iss, palavra , '|');
            fat[i] = atoi(palavra.c_str());
        }
        inFile.close();
    } else { //Cria arquivo.txt caso ele não exista
        FILE *fp;
        fp = fopen (nomeArq, "w");
        if (DEBUG) printf("Arquivo %s criado.\n", nomeArq);
        if (fp == NULL) {
            printf("Falha na criação de %s\n", nomeArq);
            fclose (fp);
        }

        // Escreve o Gerencimento de Espaço Livre no disco
        fprintf(fp, "1");
        for (int i = 0; i < NUMBLOCKS - 1; i++) fprintf(fp, "0");
        for (int i = NUMBLOCKS%BLOCKSIZE; i < BLOCKSIZE - 1; i++) // completa o último bloco com '@'s
            fprintf(fp, "@");
        fprintf(fp,"\n");

        // Carrega o Gerencimento de Espaço Livre na memória
        for (int i = 1; i < NUMBLOCKS; i++) fsm[i] = 0;
        fsm[0] = 1;

        // Escreve FAT no disco
        string fatStr = "";
        fatStr += "-0001|";
        for (int i = 0; i < NUMBLOCKS - 2; i++) fatStr += "-0002|";
        fatStr += "-0002";
        for (int i = fatStr.size()%BLOCKSIZE; i < BLOCKSIZE - 1; i++) // completa o último bloco com '@'s
            fatStr += "@";
        fatStr += "\n";
        fprintf(fp, "%s", fatStr.c_str());

        // Carrega FAT na memória
        fat[0] = -1;
        for (int i = 1; i < NUMBLOCKS; i++) fat[i] = -2;

        unsigned long long dataInt = datainfo();
        fprintf(fp, "root/|0|%lld|%lld|%lld|root/|0|", dataInt, dataInt, dataInt);
        int len = strlen("root/|0|20201121084741|20201121084741|20201121084741|root/|0|");
        for (int i = 0; i < BLOCKSIZE - len - 1; i++)
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
    }
    cout << "Sistema de arquivos "  << nomeArq << " montado com sucesso." << endl;
}

void Driver::umount()
{
    cout << "O sistema de arquivos "  << diskName << " foi desmontado." << endl;
    free(diskName);
    // diskName = nullptr;
}

bool Driver::SearchFile(string absoluteDirName, bool remove, bool LowLevelFormat, bool printContent)
{
  string fileName;
  string token;
  string absolutePathName;
  string bloco, bloco2;

  int pLength, tLength, nFat, nFat2;

  absoluteDirName += "//"; // gambiarra para ler um vazio
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
  // bloco = loadBlock(nFat);
  bloco = readFile(nFat);
  bloco += "|";
  bloco += "@";

  // procura arquivo
  istringstream iss(bloco);
  for (int i = 0; i <= METADIR; i++) getline(iss, token, '|');
  while (token != fileName && token[0] != '@') getline(iss, token, '|');

  if (token == fileName) {
      getline(iss, token, '|');

  } else {
      if (DEBUG) cout << "\t[DEBUG] cat: " << fileName <<": Arquivo ou diretório não encontrado" << '\n';
      return false;
  }

  nFat2 = atoi(token.c_str()); //Fat do arquivo de fato
  accessedAtUpdater(nFat2, true);

  // Caso flag remove remove arquivo
  if (remove) {
      // fstream fs(diskName);
      // bloco.erase(0, BLOCKSIZE - 1);
      // bloco = loadBlock(nFat);
      bloco = readFile(nFat);
      bloco = metaRemover(bloco, fileName);
      // fs.seekg(ROOT + nFat*BLOCKSIZE, ios::beg);
      // fs << bloco;
      writeFile(bloco, nFat);

      // if (LowLevelFormat) { // Para efeitos de DEBUG remove com "zeros"
      //     //bloco2 = loadBlock(nFat2);
      //     //bloco2.erase(0, BLOCKSIZE - 1);
      //     if (bloco2.size() < BLOCKSIZE) {
      //         while (bloco2.size() < BLOCKSIZE - 1)
      //             bloco2 += "@"; // completa o espaço desperdiçado com "@"
      //     }
      //     fs.seekg(ROOT + nFat2*BLOCKSIZE, ios::beg);
      //     fs << bloco2;
      // }

      // fs.close();
      // fat[nFat2] = 0;
      // fsm[nFat2] = 0;
      cleanFsmFile(nFat2);
      saveFat();
      saveFsm();
      return true; // Remover warning
  // Carrega arquivo e imprime info.
  } else {
      // bloco2 = loadBlock(nFat2);
      if (printContent) {
          bloco2 = readFile(nFat2);
          ImprimeArquivo(bloco2, true);// Começa a chamar pela impressão
      }
      // while (fat[nFat2] != -1) {
      //     nFat2 = fat[nFat2];
      //     bloco2 = loadBlock(nFat2);
      //     ImprimeArquivo(bloco2, false);
      // }
      return true;
    }
}

void Driver::ListDir(string absoluteDirName)
{
    int nFat;
    string bloco, token;

    // carrega bloco do diretorio seguindo o caminho.
    nFat = absolutePath(absoluteDirName);
    // bloco = loadBlock(nFat);
    bloco = readFile(nFat);
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
    // bloco = loadBlock(nFat);
    bloco = readFile(nFat);
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
        std::cout << " Modificado em: " << dateToStr(token) << '\n';
    } else {
        getline(iss, token, '|');
        getline(iss, token, '|');
        std::cout << " Tamanho: " << token;
        getline(iss, token, '|');
        getline(iss, token, '|');
        std::cout << " Modificado em: " << dateToStr(token) << '\n' ;
    }
}

bool Driver::finder(string absoluteDirName, string file)
{
    int nFat;
    string bloco, token, lastC;
    bool achou = false;

    if (absoluteDirName[absoluteDirName.size() - 1] != '/')
        absoluteDirName += "/";

    nFat = absolutePath(absoluteDirName);
    // bloco = loadBlock(nFat);
    bloco = readFile(nFat);
    bloco += "@"; // Pena teste
    bloco += "|";

    istringstream iss(bloco);
    for (int i = 0; i <= METADIR; i++) getline(iss, token, '|');
    while (!token.empty() && token[0] != '@') {
        // caso nome do arquivo é encontrado imprime o nome dele
        // concatenado com o caminho.
        if (token == file) {
            cout << absoluteDirName << file << '\n';
            achou = true;
        }
        else { // Verifica se é uma pasta, se for, chama finder recursivamente
            if (token[token.size() - 1] == '/') {
                achou = finder(absoluteDirName + token, file) || achou;
            }
        }
        getline(iss, token, '|');
        getline(iss, token, '|');
    }
    // Caso não achou imprime essa informação.
    return achou;
}

void Driver::ImprimeArquivo(string bloco, bool isInit)
{
    string token;
    if (isInit) {
        bloco += "|";
        istringstream iss(bloco);
        if (DEBUG) cout << "------------------------------------------------------" << '\n';
        getline(iss, token, '|');
        if (DEBUG) cout << "\t[DEBUG] Arquivo: " << token << '\n'<< '\n';
        if (DEBUG) cout << "\t[DEBUG] Meta:" << '\n';
        getline(iss, token, '|');
        getline(iss, token, '|');
        if (DEBUG) cout << "\t\t[DEBUG] tamanho: " << token << '\n';
        getline(iss, token, '|');
        if (DEBUG) cout << "\t\t[DEBUG] Criado em: " << token << '\n';
        getline(iss, token, '|');
        if (DEBUG) cout << "\t\t[DEBUG] Modificado em: " << token << '\n';
        getline(iss, token, '|');
        if (DEBUG) cout << "\t\t[DEBUG] Acessado em: " << token << '\n';
        getline(iss, token, '|');
        if (DEBUG) cout << "\t\t[DEBUG] Pasta pai: " << token << '\n';
        getline(iss, token, '|');
        getline(iss, token, '@');
        if (DEBUG) cout << "\t[DEBUG] Conteúdo:" << '\n';
        cout << token.substr(0, token.size() - 1) << '\n';
    } else {
        bloco += "|";
        istringstream iss(bloco);
        getline(iss, token, '@');
        cout << token << '\n';
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
    for (int i = 0; i < NUMBLOCKS; i++) {
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
    if (DEBUG) cout << "------------------------------------------------------" << '\n';
    if (DEBUG) cout << "\t[DEBUG] df Info:" << '\n';
    cout << "Quantidade de diretórios: " << nDir << '\n';
    cout << "Quantidade de arquivos: " << nFil << '\n';
    cout << "Espaço livre: " << nFree << '\n';
    cout << "Espaço desperdiçado: " << nWaste << '\n';
}

void Driver::mkDirAndTouch(string absoluteDirName, bool isFile)
{
    if (isFile && SearchFile(absoluteDirName, 0, false, false)) return; // para cat, só cria arquivo se não existir
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
    ///////////////////////////////////////////////
    nFat = absolutePath(absolutePathName);

    freeNFat = firstFit();
    fsm[freeNFat] = 1;
    bloco = readFile(nFat);
    bloco += dirName + "|";
    bloco += to_string(freeNFat) + "|";
    writeFile(bloco, nFat); // reescreve pasta pai
    fstream fs(diskName);

    fs.seekg(ROOT + freeNFat*BLOCKSIZE, ios::beg);
    istringstream iss3(bloco);
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
        // if (newFile.size() < BLOCKSIZE) {
        //     while (newFile.size() < BLOCKSIZE - 1)
        //         newFile += "@"; // completa o espaço desperdiçado com "@"
        // }
        writeFile(newFile, freeNFat);
        // fs << newFile;
    } else {
        string newBloco = dirName + "|" + to_string(freeNFat);
        newBloco += "|" + createdAt + "|" + updatedAt + "|" + accessedAt;
        newBloco += "|" + insideDirName + "|" + insideDirNameFat + "|";
        writeFile(newBloco, freeNFat);
    }

    accessedAtUpdater(nFat, false); // Atualiza tempo de acesso
    // fat[freeNFat] = -1;
    fsm[freeNFat] = 1;
    saveFat();
    saveFsm();
}

void Driver::rmDir(string absoluteDirName, bool LowLevelFormat)
{
    string dirName;
    string token;
    string absolutePathName;
    string bloco, blocoP, newBlocoP, lastC;
    int pLength, tLength, nFat, nFat2;

    absoluteDirName += "/"; // gambiarra para ler um vazio
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

    nFat2 = absolutePath(absolutePathName); // dir pai
    nFat = absolutePath(absoluteDirName); // path completo
    remover(nFat, LowLevelFormat, absolutePathName);

    // Ajuste bloco diretório pai
    blocoP = readFile(nFat2);
    newBlocoP = metaRemover(blocoP, dirName);
    writeFile(newBlocoP, nFat2);

    saveFat();
    saveFsm();
}

void Driver::rm(string absolutePathName)
{
    string fileName;
    string token;
    string bloco, blocoP, newBlocoP, lastC;
    int pLength, tLength, nFat, nFat2;
    if (DEBUG) cout << "\t[DEBUG] rm(" << absolutePathName << ")" << endl;

    absolutePathName += "/"; // gambiarra para ler um vazio
    istringstream iss1(absolutePathName);
    getline(iss1, token, '/'); //procura pelo nome do arquivo ou diretório
    while (!token.empty()) {
        fileName = token;
        getline(iss1, token, '/');
    }

    if (DEBUG) cout << "\t[DEBUG] rm | fileName: " << fileName << endl;

    if (DEBUG) cout << "\t[DEBUG] rm | absolutePathName: " << absolutePathName << endl;
    pLength = absolutePathName.length();
    tLength = fileName.length();
    absolutePathName = absolutePathName.substr(0, pLength - tLength - 1);
    if (DEBUG) cout << "\t[DEBUG] rm | absolutePathName: " << absolutePathName << endl;

    nFat2 = absolutePath(absolutePathName); // dir pai
    if (DEBUG) cout << "\t[DEBUG] rm | check1" << endl;

    // Ajuste bloco diretório pai
    blocoP = readFile(nFat2);
    nFat = getFileFat(blocoP, fileName);
    if (DEBUG) cout << "\t[DEBUG] rm | nFat: " << nFat << endl;

    newBlocoP = metaRemover(blocoP, fileName);
    if (DEBUG) cout << "\t[DEBUG] rm | newBlocoP: " << newBlocoP << endl;
    writeFile(newBlocoP, nFat2);
    removerArq(nFat, absolutePathName + "/" + fileName);
    saveFat();
    saveFsm();
}

void Driver::remover(int nFat, bool LowLevelFormat, string absolutePathName)
{
  int newFat, l;
  string name, bloco, bloco2, token, lastC;

  cleanFsmFile(nFat);
  bloco = readFile(nFat);
  bloco += "||";

  istringstream iss(bloco);
  getline(iss, token, '|');
  name = token;
  l = token.length();
  lastC = token.substr(l - 1, l);
  if (lastC == "/") { // verifica se é diretório
      for (int i = 1; i < METADIR; i++) getline(iss, token, '|');
      getline(iss, token, '|');
      getline(iss, token, '|');
      while (!token.empty() && token[0] != '@') {
          newFat = atoi(token.c_str());
          remover(newFat, LowLevelFormat, absolutePathName + name);
          getline(iss, token, '|');
          getline(iss, token, '|');
      }
      cout << absolutePathName << name << " foi removido." << endl;
  }
  else cout << absolutePathName << name << " foi removido." << endl;
}

void Driver::removerArq(int nFat, string filePath)
{
  // int newFat, l;
  // string name, bloco, bloco2, token, lastC;
  if (DEBUG) cout << "\t[DEBUG] removerArq(" << nFat << ", " << filePath << ")" << endl;
  cleanFsmFile(nFat);
  // bloco = readFile(nFat);
  // bloco += "||";
  //
  // istringstream iss(bloco);
  // getline(iss, token, '|');
  // name = token;
  // l = token.length();
  // lastC = token.substr(l - 1, l);

  cout << filePath << " foi removido." << endl;
}

string Driver::metaRemover(string bloco, string name)
{
    string newBloco, token;
    bloco += "||";

    istringstream iss(bloco);
    for (int i = 0; i < METADIR; i++) { // Atributos do diretório
        getline(iss, token, '|');
        newBloco += token + "|";
    }

    getline(iss, token, '|');
    while (token != name && token[0] != '@') { // pega todo o restante anterior ao diretório
        newBloco += token + "|";
        getline(iss, token, '|');
    }

    // Pula o diretório a ser removido dos metadados
    getline(iss, token, '|');
    getline(iss, token, '|');
    while (!token.empty() && token[0] != '@') { // pega todo o restante posterior ao diretório
        newBloco += token + "|";
        getline(iss, token, '|');
    }
    return newBloco;
}

// Recebe um bloco de um diretório, o nome de um arquivo/pasta, percorre os meta-
// dados do diretório, retorna o nFat do arquivo/pasta
int Driver::getFileFat(string bloco, string name)
{
    string newBloco, token;
    bloco += "||";

    if (name.length() > 0 && name[name.length() - 1] == '/')
        name = name.substr(0, name.length() - 1);

    istringstream iss(bloco);
    for (int i = 0; i < METADIR; i++) { // Atributos do diretório
        getline(iss, token, '|');
    }

    getline(iss, token, '|');
    while (token != name && token[0] != '@') { // pega todo o restante anterior ao diretório
        getline(iss, token, '|');
    }
    getline(iss, token, '|');
    return stoi(token);
    //
    // // Pula o diretório a ser removido dos metadados
    // getline(iss, token, '|');
    // while (!token.empty() && token[0] != '@') { // pega todo o restante posterior ao diretório
    //     newBloco += token + "|";
    //     getline(iss, token, '|');
    // }
    // return newBloco;
}

int Driver::absolutePath(string dirPath)
{
    string dirNext;
    string bloco;
    int nFat;

    if (DEBUG) cout << "\t[DEBUG] absolutePath(" << dirPath << ")" << endl;

    dirPath += "/"; // gambiarra para ler um vazio
    istringstream iss1(dirPath);
    getline(iss1, dirNext, '/');

    bloco = readFile(0);
    dirNext += "/";
    nFat = cdDir(bloco, dirNext);

    getline(iss1, dirNext, '/');
    while (!dirNext.empty()) {
        bloco = readFile(nFat);
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
    if (DEBUG) cout << "\t[DEBUG] cd ./"<< dirName << '\n';
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
      exit(1); // (Pena) remover warning
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
    string fileName, token, absolutePathName, bloco, bInit;
    int pLength, tLength, nFat;

    if (destino.length() > 0 && destino[destino.length() -1 ] == '/') {
        cout << "Especifique o nome do arquivo." << endl;
        return;
    }

    destino += "/";
    istringstream iss1(destino);

    // procura pelo nome do arquivo ou diretório
    getline(iss1, token, '/');
    while (!token.empty()) {
        absolutePathName += token + "/";
        fileName = token + "/";
        getline(iss1, token, '/');
    }

    if (DEBUG) cout << "\t[DEBUG] fileName: " << fileName << endl;
    if (fileName + "/" == destino) {
        cout << "Especifique o nome do arquivo." << endl;
        return;
    }

    // separa caminho de local final.
    pLength = absolutePathName.length();
    tLength = fileName.length();
    absolutePathName = absolutePathName.substr(0, pLength - tLength);
    fileName = fileName.substr(0, tLength - 1);

    nFat = absolutePath(absolutePathName);

    ifstream arqOrigem(origem);
    fstream fs(diskName);

    if (!arqOrigem.is_open()) {
        cout << "[ERRO] Não foi possível abrir o arquivo " << origem << endl;
        return;
    }
    else if (DEBUG) cout << "\t[DEBUG] Arquivo " << origem << " foi aberto com sucesso." << endl;

    string buffer;
    getline(arqOrigem, buffer);

    // Encontra primeiro bloco livre
    int k = firstFit();
    if (DEBUG) cout << "\t[DEBUG] k: " << k << endl;
    fsm[k] = 1;

    // Reescreve pasta pai
    string blocoDir = readFile(nFat);
    blocoDir += fileName + "|";
    blocoDir += to_string(k) + "|";
    writeFile(blocoDir, nFat);
    // if (DEBUG) cout << "\t[DEBUG] blocoDir: " << blocoDir << endl;

    ////////////////////////////////////////////////////////////////////////////
    istringstream iss3(blocoDir);
    getline(iss3, token, '|');
    string insideDirName = token;
    getline(iss3, token, '|');
    string insideDirNameFat = token;

    string nomeOrigem = origem;
    string size = to_string(buffer.size());
    string createdAt = datainfoString();
    string updatedAt = datainfoString();
    string accessedAt = datainfoString();

    bloco = fileName + "|" + to_string(k) + "|" + size;
    bloco += "|" + createdAt + "|" + updatedAt + "|" + accessedAt;
    bloco += "|" + insideDirName + "|" + insideDirNameFat;
    bloco += "|" + buffer;
    writeFile(bloco, k);
    saveFat();
    saveFsm();
}

//Atualiza a FAT
void Driver::saveFat()
{
    fstream fs(diskName);
    fs.seekg(FATPOS, ios::beg); // Seek FAT
    string fat_string = intToString(fat[0]);
    for (int i = 1; i < NUMBLOCKS; i++)
        fat_string += "|" + intToString(fat[i]);
    fs << fat_string;
    fs.close();
    if (DEBUG) cout << "\t[DEBUG] FAT foi atualizada no disco." << endl;
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
    if (DEBUG) cout << "\t[DEBUG] FSM foi atualizada no disco." << endl;
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
    // ifstream disk(diskName);
    string token;
    string bloco;
    string bInit; string bEnd;
    //disk.open(diskName);
    // getline (disk, bloco); //Pula freespace e FAT
    // getline (disk, bloco);
    //
    // for (int i = 0; i <= nFat; i++) getline (disk, bloco);// Vai até a linha FAT
    bloco = readFile(nFat);
    istringstream iss(bloco);

    for (int j = 0; j < pos; j++) { // Pega o inicio e grava em um buffer inicio
        getline(iss, token, '|');
        bInit += token + "|";
    }
    getline(iss, token, '|');
    getline(iss, bEnd, '\n');
    token = datainfoString();

    bInit += token + "|" + bEnd; // Concatena com o buffer do fim do bloco
    // disk.close();
    writeFile(bInit, nFat);

    // fstream fs(diskName);
    // fs.seekg(ROOT + nFat*BLOCKSIZE, ios::beg);
    // fs << bInit;
}



// Recebe um inteiro k representando um bloco de início de um arquivo ou
// diretório e retorna uma string com o conteúdo de todos os seus blocos
string Driver::readFile(int k) {
    fstream fs(diskName);
    string fileContent = "";
    string buffer;
    while (fat[k] != -1) {
        fs.seekg(ROOT + k*BLOCKSIZE, ios::beg);
        getline (fs, buffer);
        fileContent += buffer;
        k = fat[k];
    }
    fs.seekg(ROOT + k*BLOCKSIZE, ios::beg); // Lê o bloco final (fat -1)
    getline (fs, buffer);
    int limite = buffer.find("@", 0);
    if (limite <= (int)buffer.length())
        buffer = buffer.substr(0, limite);
    // if (DEBUG) cout << "\t[DEBUG] readline | buffer: " << buffer << endl;
    fileContent += buffer;
    fs.close();
    return fileContent;
}

// Recebe um Driver, uma string s a ser inserida no disco e um inteiro k
// representando um bloco de início ou -1 se for um arquivo novo. Escreve a
// string s no disco alocando os
// blocos conforme necessário (livres se o bloco k está livre, caso contrário,
// ajusta os blocos utilizados conforme necessidade.
void Driver::writeFile(string s, int k) {
    cleanFsmFile(k);
    fstream fs(diskName);
    if (k == -1) k = firstFit();
    while (s.size() > BLOCKSIZE - 1) {
        fs.seekg(ROOT + k*BLOCKSIZE, ios::beg); // Seek base do bloco k
        fs << s.substr(0, BLOCKSIZE - 1);
        s.erase(0, BLOCKSIZE - 1);
        fsm[k] = 1;
        int kAnt = k;
        k = nextFit(k);
        fat[kAnt] = k;
    }
    fs.seekg(ROOT + k*BLOCKSIZE, ios::beg);
    if (s.size() < BLOCKSIZE) {
        while (s.size() < BLOCKSIZE - 1)
            s += "@";
    }
    fs << s;
    fs.close();
    fsm[k] = 1;
    fat[k] = -1;
}

// Para todos os blocos que estiverem livres em fsm, limpa os blocos e
// reverte o fat[] para -2.
void Driver::lowLevelFormat2() {
    fstream fs(diskName);
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
    saveFat();
}
