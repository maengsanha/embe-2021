CC      = arm-none-linux-gnueabi-gcc
CFLAGS  = -static
TARGET  = app
OBJS    = main.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o app main.o

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c -lrt

clean:
	rm -f $(OBJS) $(TARGET)
