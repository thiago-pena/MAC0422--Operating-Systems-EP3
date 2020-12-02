#ifndef _DRIVER_HPP
#define _DRIVER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#include "tools.hpp"

extern bool mountedFS;

class Driver {
  private:
    char *diskName;
    // Atualiza a FAT.
    void saveFat();
    // Atualiza o registro de espaço livre.
    void saveFsm();
    // Usa timeUpdater para atualizar a data de acesso.
    void accessedAtUpdater(int nFat, bool isFile);
    // Usa timeUpdater para atualizar a data de modificação.
    void updateAtUpdater(int nFat);
    // Atualizador de tempo.
    void timeUpdater(int nFat, int pos);
    // Recebe número de fat e carrega bloco
    string loadBlock(int nFat);
    // Percorre caminho de diretório e devolve número fat da pasta pai do arquivo/pasta
    int absolutePath(string dirPath);
    // Ajuda recursivamente absolutePath
    int cdDir(string bloco, string dirName);
    // Remover um bloco
    void remover(int nFat, bool LowLevelFormat, string absoluteDirName);
    // Recebe uma string com um diretório e o nome de um diretório a ser
    // removido. Retorna a string do diretório original com a remoção feita.
    string metaRemover(string bloco, string name);
    // Lê informações de um bloco para o ls
    void listener(int nFat);
    // Recebe um inteiro k representando um bloco de início de um arquivo ou
    // diretório e retorna uma string com o conteúdo de todos os seus blocos
    string readFile(int k);
    // Recebe um Driver, uma string s a ser inserida no disco e um inteiro k
    // representando um bloco de início ou -1 se for um arquivo novo. Escreve a
    // string s no disco alocando os
    // blocos conforme necessário (livres se o bloco k está livre, caso contrário,
    // ajusta os blocos utilizados conforme necessidade.
    void writeFile(string s, int k);
    // Recebe um nFat e um caminho completo de um arquivo, e limpa fsm[] de
    // todo o conteúdo do arquivo
    void removerArq(int nFat, string filePath);
    // Recebe um bloco de um diretório, o nome de um arquivo/pasta, percorre os meta-
    // dados do diretório, retorna o nFat do arquivo/pasta
    int getFileFat(string bloco, string name);
  public:
      // Recebe o caminho de um arquivo, remove o arquivo, remove-o dos metadados
      // da pasta mai, atualiza os tempos de acesso no caminho
      void rm(string absoluteDirName);
      // Para todos os blocos que estiverem livres em fsm, limpa os blocos e
      // reverte o fat[] para -2.
      void lowLevelFormat2(); // tornar privado
    Driver();
    ~Driver();
    // Monta um arquivo como estrutura de disco
    void mount(char *nomeArq, bool existe);
    // Desmonta arquivo aberto;
    void umount();
    // procura arquivo por nome ( Busca linear )
    bool SearchFile(string absoluteDirName, bool remove, bool LowLevelFormat, bool printContent);
    // Imprime informação e conteúdo de arquivo
    void ImprimeArquivo(string bloco, bool isInit);
    // Remove pasta usando...
    void rmDir(string absoluteDirName, bool LowLevelFormat);
    // Cria um diretorio no caminho especificado, caso for chamado com flag
    // isFile funciona como touch e cria/atualiza arquivo.
    void mkDirAndTouch(string absoluteDirName, bool isFile);
    // cp origem destino: cria dentro do sistema de arquivos simulado uma cópia do arquivo
    // origem que está em algum sistema de arquivos real dentro do seu computador. No sistema de
    // arquivos simulado a cópia de origem será salva em destino. Tanto origem quanto destino
    // devem ser informados com o caminho completo tanto dentro do sistema de arquivos real quanto no
    // simulado 1 . Esse comando será executado apenas para copiar arquivos em texto puro.
    void copy(string origem, string destino); // EM PROCESSO (Pena)
    // Aplica ls em um diretório listando seus diretorio e arquivos abaixo.
    void ListDir(string absoluteDirName);
    // Verifica se um determinado arquivo está dentro de uma pasta.
    bool finder(string absoluteDirName, string file);
    // imprime na tela informações seguindo o enunciado.
    void df();

    string getDiskName();

};

#endif
