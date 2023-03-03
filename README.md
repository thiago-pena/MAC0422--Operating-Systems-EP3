# MAC0422 - Operating Systems / Sistemas Operacionais (2020)
Undergraduate subject of the Bachelor's Degree in Computer Science at IME-USP.

View [course description](https://uspdigital.usp.br/jupiterweb/obterDisciplina?nomdis=&sgldis=mac0422).

## EP3 - Segundo Semestre de 2020

Nome | nUSP
--- | ---
[Pedro Fernandes](https://github.com/Pedro84skynet) | 5948611
[Thiago Benitez Pena](https://github.com/thiago-pena) | 6847829

## 1.CONTEÚDO

    Conteúdo do arquivo "ep3-pedro.thiago.tar.gz"

    Makefile
    apresentação.pdf
    LEIAME
    ep3.cpp
    driver.cpp
    driver.hpp
    tools.cpp
    tools.hpp


## 2.INSTRUÇÕES

  Observações inicias:

          A máquina cujo programa será executado dever ser compatível com
          as principais funções GNU/Linux em especial as referidas pela
          definição _GNU_SOURCE e as bibliotecas:

 		stdbool.h
		unistd.h
		sys/resource.h
		sys/time.h
		errno.h
		unistd.h
		string.h
		fstream
		sstream
        chrono



  ### 2.1   Para gerar os códigos binários abrir pasta desempacotada no shell e
        digitar "make"

          	Exemplo: ~/dir1/dir2 >$ make

  ### 2.2   Para rodar o binário digitar "./ep3"

          	Exemplo: ~/dir1/dir2 >$ ./ep3

  #### 2.2.1 Inicialmente a necessidade montar um arquivo com o comando mount, caso
  	 arquivo não exista um novo será criado com o nome oferecido.

        	Exemplo: [ep3] $  mount sistema1

  #### 2.2.2 Após isso, o usuário já pode usar o programa com todas as interações pe-
  	didas no enunciado deste EP. Lembrando somente que este programa neces-
  	sita do caminhos completos de qualquer diretório ou arquivo bem como a
  	diferenciação na escrita dos mesmos. Sendo todo nome terminado com "/"
  	um diretório e todo sem "/" um arquivo.


         	 Exemplos: mkdir root/dir1/
                       rmdir root/dir1
                       ls root/
                       cp ./arquivo.txt root/file1

        Criará um diretorio chamado dir1/ dentro da pasta root/
    OBS: Para todas as operações, é necessário referenciar o diretório raiz
    "/" como root/.


## 3.REFERÊNCIAS

   	Várias referências de uso das biblioteca fstream, fstream, assim como diferentes técnicas para manejo de strings
	inspiradas nos sites:

	https://pt.stackoverflow.com/
	https://man7.org/
	https://linux.die.net/
