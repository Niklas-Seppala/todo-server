CC=gcc
DEBUG=-g -DDEBUG
STD=-std=g11
OFLAGS=-Wall -Wshadow -c
HEADERS=-Iinclude

OBJ=./obj/
OUT=./out/

LIB_SRC=./src/lib/
LIB=$(LIB_SRC)common.c $(LIB_SRC)IO.c $(LIB_SRC)protocol.c $(LIB_SRC)network.c

SERVER_SRC=./src/server/
SERVER=$(SERVER_SRC)app.c $(SERVER_SRC)threads.c $(SERVER_SRC)todo.c $(SERVER_SRC)queue.c 

LIB_OBS=common.o IO.o protocol.o network.o
SERVER_OBJS=$(LIB_OBS) app.o threads.o server_IO.o todo.o queue.o server_network.o

server_app: lib.o server_IO.o server_nw.o server.o 
	$(CC) $(SERVER_OBJS) -pthread -o app
	mv ./*.o $(OBJ)
	mv ./app $(OUT)

debug_server:
	valgrind $(OUT)app 5050


server_IO.o:
	$(CC) $(OFLAGS) $(HEADERS) $(DEBUG) $(SERVER_SRC)IO.c -o server_IO.o
server_nw.o:
	$(CC) $(OFLAGS) $(HEADERS) $(DEBUG) $(SERVER_SRC)network.c -o server_network.o


server.o:
	$(CC) $(OFLAGS) $(HEADERS) $(DEBUG) $(SERVER)
lib.o:
	$(CC) $(OFLAGS) $(HEADERS) $(DEBUG) $(LIB)

dev_client: lib.o dev_client.o
	$(CC) $(LIB_OBS) dev_client.o -o dev_client
	mv ./*.o $(OBJ)
	mv ./dev_client $(OUT)

dev_client.o:
	$(CC) $(OFLAGS) $(HEADERS) $(DEBUG) ./src/dev_client.c
run_dev_client:
	$(OUT)dev_client

clean:
	rm $(OBJ)* $(OUT)* *.o

