# Makefile for SDL2 projects using SDL2, SDL2_image, and SDL2_ttf

# Compiler
CC := gcc

# Get the compiler flags from pkg-config for SDL2, SDL2_image, and SDL2_ttf
CFLAGS := $(shell pkg-config --cflags sdl2 sdl2_ttf) -Wall -O2

# Get the linker flags from pkg-config for SDL2, SDL2_image, and SDL2_ttf
LDFLAGS := $(shell pkg-config --libs sdl2 sdl2_ttf)

# Source files
SRCS := main.c

# Object files generated from the source files
OBJS := $(SRCS:.c=.o)

# Name of the final executable
TARGET := main

# Uncomment the next line to suppress all command output
#.SILENT:

# Default target: build the executable
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	@$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files into object files
%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	@rm -f $(TARGET) $(OBJS)

.PHONY: all clean
