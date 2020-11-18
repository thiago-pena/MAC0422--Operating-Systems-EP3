CC = g++
CFLAGS = -Wall -pedantic -g
RM = rm

ep3: ep3.o diretorio.o parser.o tools.o
	$(CC) ep3.o diretorio.o parser.o tools.o -o ep3 -lm -lreadline

ep3.o: ep3.cpp parser.cpp diretorio.hpp tools.hpp
	$(CC) $(CFLAGS) -c ep3.cpp

parser.o: parser.cpp diretorio.hpp tools.hpp
	$(CC) $(CFLAGS) -c parser.cpp

diretorio.o: diretorio.cpp diretorio.hpp  tools.hpp
	$(CC) $(CFLAGS) -c diretorio.cpp

tools.o: tools.cpp tools.hpp
	$(CC) $(CFLAGS) -c tools.cpp

clean:
	$(RM) *.o $(objects) ep3
