# Usage:
# make        # compile all binaries
# make clean  # remove ALL binaries and objects

# Tells make that these aren't file targets.
.PHONY = all clean

# Compiler
CC = gcc

# Linking flags
LDFLAGS = -lm

# Read folder for source files to compile and create variables so we always
# compile all
SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

# All target requires all binaries to be built.
all: ${BINS}

# All .c files will use this command to be built into binaries.
%: %.c
	${CC} $< ${LDFLAGS} -o $@

# Clean will remove all binaries.
clean:
	rm ${BINS}