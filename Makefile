CPP = g++
CPPFLAGS = -Wall

OBJ = main.o metodos.o

main: $(OBJ)
	$(CPP) $(CPPFLAGS) $(OBJ) -o main

main.o: main.cpp
	$(CPP) $(CPPFLAGS) -c main.cpp -o main.o

metodos.o: metodos.cpp
	$(CPP) $(CPPFLAGS) -c metodos.cpp -o metodos.o

all: main

clean:
	rm -rf *.o
