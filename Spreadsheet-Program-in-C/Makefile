# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -std=c11 # Use C11 standard for reliability

# Source files and object files
SRCS = main.c src/spreadsheet.c src/spreadsheet_display.c src/utils.c src/command_handler.c
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = spreadsheet

# Default rule: Build the target
all: $(TARGET)

# Rule to create the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile each .c file into .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule: Remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
