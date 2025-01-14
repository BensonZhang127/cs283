# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g 
# -Wall enable all warnings
# -Wextra enable more warnings
# -g enable debugging information

# Target executable name
TARGET = stringfun

# Default target
all: $(TARGET)

# Compile source to executable
$(TARGET): stringfun.c
	$(CC) $(CFLAGS) -o $(TARGET) $^

# Clean up build files
clean:
	rm -f $(TARGET)
# make clean = will remove the executable

# Phony targets
.PHONY: all clean