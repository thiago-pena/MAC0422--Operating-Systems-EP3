#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#include "driver.hpp"
#include "tools.hpp"


extern int fat[NUMBLOCKS];
extern int fsm[NUMBLOCKS];

int firstFit();
int nextFit(int b);

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


        // Gerencimanto de Espaço Livre
        getline(inFile, linha);
        cout << "Teste FSM\n";
        cout << linha << endl;
        cout << linha[0] << endl;
        cout << linha[2] << endl;
        for (int i = 0; i < NUMBLOCKS; i++)
            fsm[i] = linha[i] - '0';
        cout << "FIM teste\n";

        // FAT______________________________________________________________
        getline(inFile, linha);
        istringstream iss(linha);
        for (int i = 0; i < NUMBLOCKS; i++) {
            getline( iss, palavra , '|');
            fat[i] = atoi(palavra.c_str());
        }



        if (DEBUG) {
          printf("memory FAT: [");
          for (int i = 0; i < NUMBLOCKS; i++) printf(" %02d", fat[i]);
          printf("]\n");
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
        fprintf(fp,"1");
        for (int i = 0; i < NUMBLOCKS - 1; i++) fprintf(fp,"0");
        fprintf(fp,"\n");

        for (int i = 1; i < NUMBLOCKS; i++) fsm[i] = 0; //Carrega FSM na memória
        fsm[0] = 1;

        //Cria FAT registro
        fprintf(fp,"-0001|");
        for (int i = 0; i < NUMBLOCKS - 2; i++) fprintf(fp,"00000|");
        fprintf(fp,"00000\n");

        //Carrega FAT na memória
        fat[0] = -1;
        for (int i = 1; i < NUMBLOCKS; i++) fat[i] = 0;

        // para registro de data https://linux.die.net/man/3/localtime
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

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
            fprintf(fp, s.c_str());
            fprintf(fp, "\n");
        }

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

  for (int i = 0; i < NUMBLOCKS; i++) {
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

          int nFat = fat[file->localFAT];
          while (nFat != -1){
              disk.open(diskName);   //reabre para procura;
              getline (disk, linha); //Pula freespace e FAT
              getline (disk, linha);
              for (int i = 0; i <= nFat; i++) getline (disk, linha); //conta FAT
              istringstream iss(linha);
              getline(iss, conteudo, '|'); //Pula primeiro pipeline
              getline(iss, conteudo, '|');
              file->content.push_back(conteudo);
              nFat = fat[nFat]; //anda o número de FAT;
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


void Driver::mkDir(string absoluteDirName)
{
    string dirName;
    string token;
    string absolutePathName;
    string bloco;
    string bInit;

    int pLength, tLength, nFat, freeNFat;

    istringstream iss1(absoluteDirName);
    absoluteDirName += "/"; //cambiarra para ler um vazio
    getline(iss1, token, '/');
    while (!token.empty()) {
        absolutePathName += token + "/";
        dirName = token + "/";
        getline(iss1, token, '/');
    }
    pLength = absolutePathName.length();
    tLength = dirName.length();
    absolutePathName = absolutePathName.substr(0, pLength - tLength);

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

    string newBloco = dirName + "|" + to_string(freeNFat);
    newBloco += "|" + createdAt + "|" + updatedAt + "|" + accessedAt;
    newBloco += "|" + insideDirName + "|" + insideDirNameFat + "|";
    if (newBloco.size() < BLOCKSIZE) {
        while (newBloco.size() < BLOCKSIZE - 1)
            newBloco += "@"; // completa o espaço desperdiçado com "@"
    }
    fs << newBloco;
    disk.close();

    accessedAtUpdater(nFat); // Atualiza tempo de acesso
    fat[freeNFat] = -1;
    fsm[freeNFat] = 1;
    saveFat();
    saveFsm();
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
    if (DEBUG) std::cout << "cd ./"<< dirName << '\n';
    if (dirName == "root/") {
      accessedAtUpdater(0);
      return 0;
    }
    string token;
    int nFat;

    bloco += "|";
    istringstream iss(bloco);

    for (int i = 0; i <= METADIR; i++) getline(iss, token, '|');
    while (token != dirName && token[0] != '@') getline(iss, token, '|');

    if (token[0] == '@') {
      std::cout << "ERRO: caminho inválido" << '\n';
    } else {
      getline(iss, token, '|');
      int nFat = atoi(token.c_str());
      accessedAtUpdater(nFat); // Atualiza tempo de acesso
      return nFat;
    }
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
  while (token1[0] != '@') {
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


string Driver::getDiskName()
{
    return diskName;
}

// void Driver::newDir(string nameDir)
// {
//     fstream fs(diskName);
//
//     int k = firstFit();
//     cout << "[DEBUG] Primeiro bloco livre: " << k << endl;
//
// }

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

// Retorna o primeiro bloco livre do gerenciamento de espaço livre
int firstFit() {
    return nextFit(0);
}

// Retorna o primeiro bloco livre do gerenciamento de espaço livre, mas partindo
// de um bloco inicial b >= 0
int nextFit(int b) {
    int k = b;
    while (k < NUMBLOCKS && fsm[k] != 0)
        k++;
    if (k >= NUMBLOCKS) cout << "[ERRO] Não há espaço livre no disco." << endl;
    return k;
}

//Atualiza a FAT
void Driver::saveFat() {
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
void Driver::saveFsm() {
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
void Driver::accessedAtUpdater(int nFat) {
    timeUpdater(nFat, 4);
}
// Usa timeUpdater para atualizar a data de modificação
void Driver::updateAtUpdater(int nFat) {
    timeUpdater(nFat, 3);
}

void Driver::timeUpdater(int nFat, int pos) {
    ifstream disk(diskName);
    string token;
    string bloco;
    string bInit; string bEnd;
    //disk.open(diskName);
    getline (disk, bloco); //Pula freespace e FAT
    getline (disk, bloco);

    for (size_t i = 0; i <= nFat; i++) getline (disk, bloco);// Vai até a linha FAT
    istringstream iss(bloco);

    int jump = pos; // Diferencia local da data de arquivo e diretorio
    if (bloco[0] == '\"') jump = pos + 1;

    for (int j = 0; j < 4; j++) { // Pega o inicio e grava em um buffer inicio
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
