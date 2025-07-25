all: mycode

mycode: test.o Sorts.o
	g++ test.o Sorts.o -o mycode

test.o: test.cpp
	g++ -c test.cpp

Sorts.o: Sorts.cpp
	g++ -c Sorts.cpp

clean:
	rm -f *.o mycode

