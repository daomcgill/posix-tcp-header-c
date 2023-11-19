TARGET = server

SOURCES = server.c

CC = gcc

CFLAGS = -Wall

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
