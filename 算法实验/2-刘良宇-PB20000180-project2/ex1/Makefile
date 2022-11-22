.PHONY: all

all: build/main

build/main: src/main.cpp
	g++ -O2 src/main.cpp -o build/main
	./build/main

clean:
	rm -f build/main
	rm -f output/*.txt