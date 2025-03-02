# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g	

# Source files
SRC = src/main.c src/spreadsheet.c src/utils.c src/command_handler.c src/spreadsheet_display.c

# Object files
OBJ = $(SRC:.c=.o)

# Output directory
BUILD_DIR = target/release

# Executable name
TARGET = $(BUILD_DIR)/spreadsheet

# Default rule: Build the sheet executable
all: $(BUILD_DIR) $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm -O3

# Compile each C file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -O3

# Run test cases
test: $(TARGET)
	./tests/run_tests.sh  # Replace with your test script

# Generate report
report:
	pdflatex ./docs/report.tex   # Assumes you have a LaTeX report source file

# Clean build files
clean:
	rm -f $(OBJ) $(TARGET)

# Clean everything, including report files
distclean: clean
	rm -f report.pdf
