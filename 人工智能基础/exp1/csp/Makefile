all: csp

CC=g++
CFLAGS=-Ofast -Wall -Wextra -Wpedantic -Werror -std=c++2a

csp: csp.o
	$(CC) $(CFLAGS) -o csp csp.o

csp.o: src/csp.cpp
	$(CC) $(CFLAGS) -c src/csp.cpp

clean:
	rm -f csp *.o