CC=gcc
DEBUG=-g -DDEBUG
OFLAGS=-Wall -Wshadow -c
OBJ=./obj/
OUT=./out/
SRC=./src/
HEADERS=-Iheaders

dev_server: common.o io.o protocol.o dev_server.o network.o
	$(CC) common.o io.o dev_server.o protocol.o network.o -o dev_server
	mv ./*.o $(OBJ)
	mv ./dev_server $(OUT)

dev_protocol: common.o io.o protocol.o dev_console.o
	$(CC) common.o io.o dev_console.o protocol.o -o dev_protocol
	mv ./*.o $(OBJ)
	mv ./dev_protocol $(OUT)

dev_network: common.o io.o network.o protocol.o dev_console.o
	$(CC) common.o io.o protocol.o dev_console.o network.o -o dev_network
	mv ./*.o $(OBJ)
	mv ./dev_network $(OUT)

io.o:
	$(CC) $(OFLAGS) -g $(HEADERS) -DDEBUG $(SRC)io.c
common.o:
	$(CC) $(OFLAGS) -g $(HEADERS) -DDEBUG $(SRC)common.c
network.o:
	$(CC) $(OFLAGS) -g $(HEADERS) -DDEBUG $(SRC)network.c
dev_console.o:
	$(CC) $(OFLAGS) -g $(HEADERS) -DDEBUG $(SRC)dev_console.c
dev_server.o:
	$(CC) $(OFLAGS) -g $(HEADERS) -DDEBUG $(SRC)dev_server.c
protocol.o:
	$(CC) $(OFLAGS) -g $(HEADERS) -DDEBUG $(SRC)protocol.c

clean:
	rm $(OBJ)* $(OUT)* *.o