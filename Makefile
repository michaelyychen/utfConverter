CC = gcc
CFLAGS = -Werror -Wall 
BIN = utfconverter debug

SRC = $(wildcard *.c)

all: $(BIN)

utfconverter: $(SRC)
	$(CC) $(CFLAGS) utfconverter.c -o $@

debug: $(SRC)
	$(CC) $(CFLAGS) -DCSE320 -g utfconverter.c -o $@

.PHONY: clean

clean:
	rm -f *.o $(BIN)

