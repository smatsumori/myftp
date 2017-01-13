CC = gcc
TARGET = myftp.out
SRCS = myftp.c
OBJS = myftp.o
DEPS =

LDFLAGS = 
CFLAGS = -g -O
DFLAGS = -D DEBUG

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
