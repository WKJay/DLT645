CFLAGS = -Wall -I./inc -I./port
LIB = -lm
LINK = 
CC = gcc
BINDIR = build

TARGET = application
DLT645 = $(wildcard ./src/*.c)
PORT = $(wildcard ./port/*.c)
SAMPLE = $(wildcard ./sample/*.c)

SRC = $(DLT645) $(PORT) $(SAMPLE)
OBJS = $(patsubst %.c,%.o,$(SRC))

$(shell mkdir -p $(BINDIR))

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIB)

$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
