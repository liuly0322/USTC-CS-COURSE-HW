.PHONY: all

all: build/main
	

build/main: src/main.cpp
	g++ -O2 src/main.cpp -o build/main

test: all
	./build/main -i input/input.txt -g -n3 -n6 -n9 -n12 -n15 -n18 -mheap -mquick -mcounting -mmerge
	diff -r -xtime.txt output/counting_sort/ output/heap_sort/
	diff -r -xtime.txt output/counting_sort/ output/merge_sort/
	diff -r -xtime.txt output/counting_sort/ output/quick_sort/

clean:
	rm -f build/main
	rm -f output/counting_sort/*.txt output/quick_sort/*.txt output/merge_sort/*.txt output/heap_sort/*.txt