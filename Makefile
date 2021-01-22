CC=gcc
DEBUG=-g -DDEBUG
OFLAGS=-Wall -Wshadow -c
OBJ=./obj/
OUT=./out/
SRC=./src/
HEADERS=-Iheaders


dev_protocol: common.o protocol.o dev_console.o
	$(CC) $(DEBUG) common.o dev_console.o protocol.o -o dev
	mv ./*.o $(OBJ)
	mv ./dev $(OUT)

dev_network: common.o network.o dev_console.o
	$(CC) $(DEBUG) common.o dev_console.o network.o -o dev
	mv ./*.o $(OBJ)
	mv ./dev $(OUT)

common.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)common.c
network.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)network.c
dev_console.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)dev_console.c
protocol.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)protocol.c