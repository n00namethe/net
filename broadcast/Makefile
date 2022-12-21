BIN = binar
OBJ = object
SERVER = server
CLIENT = client
.PHONY: all clean

all: $(SERVER) $(CLIENT)

clean:
	rm -f $(OBJ)/*.o $(BIN)/*

$(SERVER).o: $(SERVER).c
	gcc -Wall -c -o $(OBJ)/$(SERVER).o $(SERVER).c -pthread -g -lrt

$(SERVER): $(SERVER).o 
	gcc -Wall -o $(BIN)/$(SERVER) $(OBJ)/$(SERVER).o -pthread -g -lrt

$(CLIENT).o: $(CLIENT).c 
	gcc -Wall -c -o $(OBJ)/$(CLIENT).o $(CLIENT).c -g

$(CLIENT): $(CLIENT).o 
	gcc -Wall -o $(BIN)/$(CLIENT) $(OBJ)/$(CLIENT).o -g
