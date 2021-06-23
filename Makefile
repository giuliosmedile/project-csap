CC=gcc

client: ./client/*
	cd client; \
	$(CC) -o main main.c
server: ./server/*
	cd server; \
	$(CC) -o main main.c
datarepo: ./datarepo/*
	cd datarepo; \
	$(CC) -o main main.c
all: ./*
	make client
	make server
	make datarepo