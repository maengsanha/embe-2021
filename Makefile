CC			= arm-none-linux-gnueabi-gcc
CFLAGS	= -static
SOURCE	= main.c
TARGET	= test
RM			= rm

all: run

run:
	@$(CC) $(CFLAGS) -g -O -o $(TARGET) $(SOURCE)
	@./$(TARGET)
	@$(RM) -f $(TARGET)

test:
	@gcc -g -O -o $(TARGET) $(SOURCE)
	@./$(TARGET)
	@$(RM) -f $(TARGET)