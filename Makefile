CC=gcc
DEBUG=-g -DDEBUG
OFLAGS=-Wall -Wshadow -c
HEADERS=-Iinclude

OBJ=./obj/
OUT=./out/

LIB_SRC=./src/lib/

LIB=$(LIB_SRC)common.c $(LIB_SRC)IO.c $(LIB_SRC)protocol.c $(LIB_SRC)network.c

SERVER_SRC=./src/server/
SERVER=$(SERVER_SRC)app.c $(SERVER_SRC)flow.c $(SERVER_SRC)server_IO.c

LIB_OBS=common.o IO.o protocol.o network.o
SERVER_OBJS=$(LIB_OBS) app.o flow.o server_IO.o


server_app: lib.o server.o
	$(CC) $(SERVER_OBJS) -o app
	mv ./*.o $(OBJ)
	mv ./app $(OUT)
run_server:
	$(OUT)app 5050

server.o:
	$(CC) $(OFLAGS) $(HEADERS) $(DEBUG) $(SERVER)
lib.o:
	$(CC) $(OFLAGS) $(HEADERS) $(DEBUG) $(LIB)

clean:
	rm $(OBJ)* $(OUT)* *.o

dev_client: lib.o dev_client.o
	$(CC) $(LIB_OBS) dev_client.o -o dev_client
	mv ./*.o $(OBJ)
	mv ./dev_client $(OUT)

dev_client.o:
	$(CC) $(OFLAGS) $(HEADERS) $(DEBUG) ./src/dev_client.c

run_dev_client:
	$(OUT)dev_client


