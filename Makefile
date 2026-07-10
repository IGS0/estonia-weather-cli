# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99

# Libraries
LIBS = -lcurl -lxml2

# Include directories
INC_DIRS = -I/usr/include/libxml2

# Source file
SRCS = K3_Igor_Solodov_233376IACB.c

# Object file
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = K3_Igor_Solodov_233376IACB

# Default rule
all: $(EXEC)

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

# Link object files into executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(EXEC)
