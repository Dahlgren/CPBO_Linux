CFLAGS=-I/usr/local/include
LFLAGS=-L/usr/local/lib/ -lboost_system -lboost_filesystem

all: pbo sha1 main
	g++ $(LFLAGS) pbo.o sha1.o main.o -o cpbo
pbo: pbo.cpp pbo.h
	g++ $(CFLAGS) -c pbo.cpp -o pbo.o
sha1: sha1.cpp sha1.h
	g++ $(CFLAGS) -c sha1.cpp -o sha1.o
main: main.cpp
	g++ $(CFLAGS) -c main.cpp -o main.o

clean:
	rm -f cpbo
	rm -f main.o
	rm -f pbo.o
	rm -f sha1.o
