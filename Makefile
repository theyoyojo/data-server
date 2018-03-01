CC = gcc
CFLAGS = -g -Wall
SERVER_EXEC = dataserverd
CLIENT_EXEC = dataserver-client
LIBS = -lm
CCLOG = gcc.log

SHARED_OBJECTS = logger.o qsock.o interrupt.o
SERVER_OBJECTS = servermain.o 
CLIENT_OBJECTS = clientmain.o
OBJECTS = $(SHARED_OBJECTS) $(SERVER_OBJECTS) $(CLIENT_OBJECTS) 

all: server client

server: $(SHARED_OBJECTS) $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) $(SHARED_OBJECTS) $(SERVER_OBJECTS) -o $(SERVER_EXEC) 2> $(CCLOG)

client: $(SHARED_OBJECTS) $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) $(SHARED_OBJECTS) $(CLIENT_OBJECTS) -o $(CLIENT_EXEC) 2> $(CCLOG)

%.o:%.c
	$(CC) $(CFLAGS) -c $^ 2> $(CCLOG)

clean:
	rm $(SERVER_EXEC) $(CLIENT_EXEC) $(OBJECTS) 
