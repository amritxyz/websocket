# Makefile for client program

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall

# Source directory
SRC_DIR = src

# Object directory
OBJ_DIR = build

# Source files
SRC = $(SRC_DIR)/client.c

# Object files
OBJ = $(OBJ_DIR)/client.o

# Executable name
EXEC = client

# Default target
all: $(EXEC)

# Rule to build the executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

# Rule to compile .c files to .o files
$(OBJ): $(SRC)
	@mkdir -p $(OBJ_DIR)  # Create build directory if it doesn't exist
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Clean rule to remove object and executable files
clean:
	rm -f $(OBJ) $(EXEC)

# Phony targets
.PHONY: all clean
