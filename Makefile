CC = g++
CFLAGS = -Wall -pedantic -g -O0
RM = rm

ep3: ep3.o driver.o tools.o
	$(CC) ep3.o driver.o tools.o -o ep3 -lm -lreadline

ep3.o: ep3.cpp driver.hpp tools.hpp
	$(CC) $(CFLAGS) -c ep3.cpp

driver.o: driver.cpp driver.hpp tools.hpp
	$(CC) $(CFLAGS) -c driver.cpp

tools.o: tools.cpp tools.hpp
	$(CC) $(CFLAGS) -c tools.cpp

clean:
	$(RM) *.o $(objects) ep3
