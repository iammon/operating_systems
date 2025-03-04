# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c11

# Executable name
TARGET = simpleShell

# Source files
SRCS = simpleShell.c

# Object files (compiled versions of source files)
OBJS = $(SRCS:.c=.o)

# Default target: Build the shell
all: $(TARGET)

# Compile source files into object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the shell
run: $(TARGET)
	./$(TARGET)

# Clean up compiled files
clean:
	rm -f $(TARGET) $(OBJS)
