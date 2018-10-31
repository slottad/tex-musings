CC=g++
CFLAGS=-I.
LIBS = -lplotter -lXaw -lXmu -lXt -lSM -lICE -lXext -lX11 -lpng -lz -lm
DEPS = hersheyplot.hpp
OBJ = hersheyplot.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: hersheyplot test

hersheyplot: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o test.* *~ core hersheyplot

test:
	./hersheyplot -p a > test.ps

.PHONY: clean test all
