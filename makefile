# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -ansi -Wall -pedantic -g

# Define the target executable
TARGET = assembler

# Define the directories
INCDIR = include
SRCDIR = src
BUILDDIR = build

# Define source and object files
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# Define include paths
INCLUDES = -I$(INCDIR)

# Default target
all: $(TARGET)

# Link the object files to create the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

# Compile source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean the build directory
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# Phony targets
.PHONY: all clean
