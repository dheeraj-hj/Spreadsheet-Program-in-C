# Compiler
CC = gcc

# Compiler flags
# CFLAGS = -Wall -Wextra -std=c11 -g -D_GNU_SOURCE	
CFLAGS = -g	

# Source files
SRC = main.c src/spreadsheet.c src/utils.c src/command_handler.c src/spreadsheet_display.c

# Object files
OBJ = $(SRC:.c=.o)

# Executable name
TARGET = sheet

# Default rule: Build the sheet executable
all: $(TARGET)

# Compile the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm

# Compile each C file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run test cases
test: $(TARGET)
	./tests/run_tests.sh  # Replace with your test script

# Generate report
report:
	pdflatex report.tex   # Assumes you have a LaTeX report source file

# Clean build files
clean:
	rm -f $(OBJ) $(TARGET)

# Clean everything, including report files
distclean: clean
	rm -f report.pdf
