CC=gcc

client: ./client/*
	$(CC) -o ./client/main ./client/main.c
server: ./server/*
	$(CC) -o ./server/main ./server/main.c
datarepo: ./datarepo/*
	$(CC) -o ./datarepo/main ./datarepo/main.c
all: ./*
	make client
	make server
	make datarepo