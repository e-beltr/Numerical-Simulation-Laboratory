CC = g++
CFLAGS = -Wall -O3 --std=c++20 -I../../PRNG/

main.exe : main.o random.o
	$(CC) random.o main.o -o main.exe
main.o : main.cpp ../../PRNG/DataBlocking.h Metro.h ../../PRNG/random.h
	$(CC) -c main.cpp -o main.o $(CFLAGS)
random.o : ../../PRNG/random.cpp ../../PRNG/random.h
	$(CC) -c ../../PRNG/random.cpp -o random.o $(CFLAGS)

clean :
	rm -f *.o