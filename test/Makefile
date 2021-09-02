SRCS=$(shell ls *.c)
OBJS=$(SRCS:.c=)
EXE=$(SRCS:.c=)
CFLAGS=-g
LDFLAGS=-lm

all: $(EXE)

clean:
	rm -f $(EXE) $(OBJS)
