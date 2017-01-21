CC = gcc
TARGET = myftpd.out
SRCS = myftpd.c
OBJS = myftpd.o
DEPS = tcpdmodule.h myftpd.h

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
