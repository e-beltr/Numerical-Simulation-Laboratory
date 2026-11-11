CC = mpicxx
CFLAGS = -O3 --std=c++20
LDFLAGS = -L/usr/local/lib -larmadillo -Wl,-rpath,/usr/local/lib

main.exe : main.o random.o
	$(CC) main.o random.o -o main.exe $(LDFLAGS)

main.o : main.cpp Map.h Individual.h GeneticAlg.h ../PRNG/random.h
	$(CC) -c main.cpp -o main.o $(CFLAGS)

random.o : ../PRNG/random.cpp ../PRNG/random.h
	$(CC) -c ../PRNG/random.cpp -o random.o $(CFLAGS)

clean :
	rm *.o main.exe

remove:
	rm *.dat seed.out