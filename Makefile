CC=gcc
OPTS=

client: ./client/* ./shared/*
	cd client; \
	$(CC) $(OPTS) -o main main.c
server: ./server/* ./shared/*
	cd server; \
	$(CC) $(OPTS) -o main main.c
datarepo: ./datarepo/* ./shared/*
	cd datarepo; \
	$(CC) $(OPTS) -o main main.c
all: ./*
	make client
	make server
	make datarepo