BIN = binar
OBJ = object

.PHONY: all clean

all: pull_inet_dgram_server pull_inet_dgram_client

clean:
	rm -f $(OBJ)/*.o $(BIN)/*

pull_inet_dgram_server.o: pull_inet_dgram_server.c
	gcc -c -o $(OBJ)/pull_inet_dgram_server.o pull_inet_dgram_server.c -pthread -g

pull_inet_dgram_server: pull_inet_dgram_server.o 
	gcc -o $(BIN)/pull_inet_dgram_server $(OBJ)/pull_inet_dgram_server.o -pthread -g

pull_inet_dgram_client.o: pull_inet_dgram_client.c 
	gcc -c -o $(OBJ)/pull_inet_dgram_client.o pull_inet_dgram_client.c -pthread -g

pull_inet_dgram_client: pull_inet_dgram_client.o 
	gcc -o $(BIN)/pull_inet_dgram_client $(OBJ)/pull_inet_dgram_client.o -pthread -g
