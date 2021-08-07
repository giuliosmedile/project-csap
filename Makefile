CC=gcc
OPTS=-o

client: ./client/*
	cd client; \
	$(CC) $(OPTS) main main.c
server: ./server/*
	cd server; \
	$(CC) $(OPTS) main main.c
datarepo: ./datarepo/*
	cd datarepo; \
	$(CC) $(OPTS) main main.c
all: ./*
	make client
	make server
	make datarepo