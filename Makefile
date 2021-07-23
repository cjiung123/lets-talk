CC = gcc

CFLAGS = -pthread

TARGET = lets-talk

SRCS = server.c client.c list.c encryption.c

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) -o $(TARGET) $(TARGET).c $(SRCS) $(CFLAGS)

valgrind:
	valgrind --leak-check=full ./$(TARGET) $(arg1) $(arg2) $(arg3)


clean: 
	$(RM) $(TARGET)