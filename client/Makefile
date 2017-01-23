CC = gcc
TARGET = myftpc.out
SRCS = myftpc.c
OBJS = myftpc.o
DEPS = tcpmodule.h myftpc.h

LDFLAGS = 
CFLAGS = -g -O
DFLAGS = -D DEBUG -D ANYADDR -std=c99

RM = rm -f

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(DFLAGS) $^ -o $@

.c.o:
	$(CC) -c $(DFLAGS) $<

clean:
	$(RM) $(TARGET)

clean_all:
	$(RM) $(TARGET) $(OBJS)
