CC			= arm-none-linux-gnueabi-gcc
CFLAGS	= -static
SOURCES	= main.c clock.c counter.c text_editor.c draw_board.c device_status.c
TARGET	= test
RM			= rm

all: build

build:
	@$(CC) $(CFLAGS) -g -o $(TARGET) $(SOURCES)

test:
	@gcc -g -o $(TARGET) $(SOURCES)
	@./$(TARGET)
	@$(RM) -f $(TARGET)