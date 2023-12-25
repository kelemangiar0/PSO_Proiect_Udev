CC = gcc
CFLAGS = -Wall

TARGET = network_ping

all: $(TARGET)

$(TARGET): network_ping.c
	$(CC) $(CFLAGS) -o $@ $^ -lpthread 

clean:
	rm -f $(TARGET)
