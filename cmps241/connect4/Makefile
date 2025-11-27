# Compiler
CC = gcc

# Normal compile flags
CFLAGS = -Wall -Wextra -O2

# Debug compile flags
DEBUGFLAGS = -g -Wall -Wextra

# Source files
SRCS = board.c bot_easy.c bot_medium.c bot_hard.c evaluation.c hashing.c main.c minimax.c transposition.c utils.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable
TARGET = my_program

# --------------------
# Default build
# --------------------
all: $(TARGET)

# Link objects into executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c to .o
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Special rule for main.c (no header)
main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

# --------------------
# Run program
# --------------------
run: $(TARGET)
	./$(TARGET)

# --------------------
# Debug build for gdb
# --------------------
debug: CFLAGS = $(DEBUGFLAGS)
debug: clean all
	@echo "Debug build ready! Use gdb ./$(TARGET) to debug."

# --------------------
# Valgrind memory check
# --------------------
valgrind: debug
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

# --------------------
# Clean
# --------------------
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all run debug valgrind clean
