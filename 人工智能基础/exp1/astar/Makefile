CXXFLAGS += -std=c++2a -Ofast -Wall -Wno-sign-compare

astar: astar.o
	g++ ${CXXFLAGS} -o astar astar.o
astar.o: src/astar.cpp
	g++ ${CXXFLAGS} -c src/astar.cpp
validate: validate.o
	g++ ${CXXFLAGS} -o validate validate.o
validate.o: src/validate.cpp
	g++ ${CXXFLAGS} -c src/validate.cpp
astar_debug: astar_debug.o
	g++ -g -std=c++2a -o astar_debug astar_debug.o
astar_debug.o: src/astar.cpp
	g++ -g -std=c++2a -c src/astar.cpp -o astar_debug.o
clean:
	rm -f astar astar.o validate validate.o astar_debug astar_debug.o
