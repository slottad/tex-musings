CC=gcc
CFLAGS=-I.
LIBS = -lplot -lXaw -lXmu -lXt -lSM -lICE -lXext -lX11 -lpng -lz -lm
DEPS = hersheyplot.h
OBJ = hersheyplot.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: hersheyplot test

hersheyplot: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o *.ps *.pdf *~ core hersheyplot

test:
	./hersheyplot > test.ps
	ps2pdf -dEPSCrop test.ps

.PHONY: clean test all
