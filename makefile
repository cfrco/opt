CC=gcc
CFLAGS=

all: util.o hash.o opt.o

%.o : %.c %.h
	$(CC) -c $< -o $@

clean : 
	rm *.o
