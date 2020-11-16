CC = g++
CFLAGS = -Wall -pedantic -g
RM = rm

ep3: ep3.o
	$(CC) ep3.o -o ep3 -lm -lreadline

ep3.o: ep3.cpp
	$(CC) $(CFLAGS) -c ep3.cpp

clean:
	$(RM) *.o ep3
