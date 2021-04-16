# CC			= arm-none-linux-gnueabi-gcc
# CFLAGS	= -static
CC			= gcc
CFLAGS	= 
TARGET	= app
OBJS		= main.o clock.o counter.o text_editor.o draw_board.o device_status.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o app main.o clock.o counter.o text_editor.o draw_board.o device_status.o -lrt

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c -lrt

clock.o: clock.c
	$(CC) $(CFLAGS) -c -o clock.o clock.c -lrt

counter.o: counter.c
	$(CC) $(CFLAGS) -c -o counter.o counter.c -lrt

text_editor.o: text_editor.c
	$(CC) $(CFLAGS) -c -o text_editor.o text_editor.c -lrt

draw_board.o: draw_board.c
	$(CC) $(CFLAGS) -c -o draw_board.o draw_board.c -lrt

device_status.o: device_status.c
	$(CC) $(CFLAGS) -c -o device_status.o device_status.c -lrt

clean:
	rm -f $(OBJS) $(TARGET)