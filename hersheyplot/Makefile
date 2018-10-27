CC=gcc
CFLAGS=-I.
LIBS = -lplot -lXaw -lXmu -lXt -lSM -lICE -lXext -lX11 -lpng -lz -lm
DEPS = hersheyplot.h
OBJ = hersheyplot.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hersheyplot: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ core hersheyplot
