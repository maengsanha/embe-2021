CC			= arm-none-linux-gnueabi-gcc
SOURCE	= main.c
TARGET	= test
RM			= rm

all: run

run:
	@$(CC) -static -g -O -o $(TARGET) $(SOURCE)
	@./$(TARGET)
	@$(RM) -f $(TARGET)

test:
	@gcc -g -O -o $(TARGET) $(SOURCE)
	@./$(TARGET)
	@$(RM) -f $(TARGET)
