CC=gcc
DEBUG=-g -DDEBUG
OFLAGS=-Wall -Wshadow -c
OBJ=./obj/
OUT=./out/
SRC=./src/
HEADERS=-Iheaders


dev: protocol.o dev_console.o
	$(CC) $(DEBUG) dev_console.o protocol.o -o dev
	mv ./*.o $(OBJ)
	mv ./dev $(OUT)

dev_console.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)dev_console.c

protocol.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)protocol.c