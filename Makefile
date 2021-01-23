CC=gcc
DEBUG=-g -DDEBUG
OFLAGS=-Wall -Wshadow -c
OBJ=./obj/
OUT=./out/
SRC=./src/
HEADERS=-Iheaders

dev_server: common.o protocol.o dev_server.o network.o
	$(CC) $(DEBUG) common.o dev_server.o protocol.o network.o -o dev_server
	mv ./*.o $(OBJ)
	mv ./dev_server $(OUT)

dev_protocol: common.o protocol.o dev_console.o
	$(CC) $(DEBUG) common.o dev_console.o protocol.o -o dev_protocol
	mv ./*.o $(OBJ)
	mv ./dev_protocol $(OUT)

dev_network: common.o network.o protocol.o dev_console.o
	$(CC) $(DEBUG) common.o protocol.o dev_console.o network.o -o dev_network
	mv ./*.o $(OBJ)
	mv ./dev_network $(OUT)

common.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)common.c
network.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)network.c
dev_console.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)dev_console.c
dev_server.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)dev_server.c
protocol.o:
	$(CC) $(OFLAGS) -g $(HEADERS) $(SRC)protocol.c

clean:
	rm $(OBJ)* $(OUT)*