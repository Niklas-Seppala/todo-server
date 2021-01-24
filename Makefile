CC=gcc
DEBUG=-g -DDEBUG
OFLAGS=-Wall -Wshadow -c
HEADERS=-Iheaders

OBJ=./obj/
OUT=./out/

LIB_SRC=./src/lib/

LIB=$(LIB_SRC)common.c $(LIB_SRC)io.c $(LIB_SRC)protocol.c $(LIB_SRC)network.c

SERVER_SRC=./src/server/
SERVER=$(SERVER_SRC)app.c $(SERVER_SRC)flow.c $(SERVER_SRC)server_IO.c
SERVER_OBJS=common.o io.o protocol.o network.o app.o flow.o server_IO.o

server_app: lib.o server.o
	$(CC) $(SERVER_OBJS) -o app
	mv ./*.o $(OBJ)
	mv ./app $(OUT)

server.o:
	$(CC) $(OFLAGS) $(HEADERS) $(DEBUG) $(SERVER)
lib.o:
	$(CC) $(OFLAGS) $(HEADERS) $(DEBUG) $(LIB)

clean:
	rm $(OBJ)* $(OUT)* *.o