CC=gcc
OPTS=

all: FORCE client server datarepo
FORCE: ;

clean:
	rm -rf ./*/main

client: ./client/* ./shared/*
	cd client; \
	$(CC) $(OPTS) -o main main.c
server: ./server/* ./shared/*
	cd server; \
	$(CC) $(OPTS) -o main main.c
datarepo: ./datarepo/* ./shared/*
	cd datarepo; \
	$(CC) $(OPTS) -o main main.c
