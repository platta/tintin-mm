# Usage:
# make        # compile all binaries
# make clean  # remove all binaries and objects

# Tells make that these aren't file targets.
.PHONY = all clean

# Compiler
CC = gcc

# Linking flags
LDFLAGS = -lm

# Read folder for source files to compile and create variables so we always
# compile all

SOURCES := $(wildcard *.c)
BINS := $(SOURCES:%.c=%)

LIB_SOURCES := $(wildcard lib/*.c)
LIB_MODULES := $(LIB_SOURCES:%.c=%.o)
LIB_ARCHIVES := $(LIB_SOURCES:lib/%.c=lib/lib%.a)


# All target requires all binaries to be built.
all: ${LIB_ARCHIVES} ${BINS}

lib/lib%.a: lib/%.o
	@echo "Archiving $< as $@"
	ar rcs $@ $<

lib/%.o: lib/%.c
	@echo "Compiling $< into $@"
	gcc -c ${LDFLAGS} $< -o $@

# All .c files will use this command to be built into binaries.
%: %.c
	@echo "Compiling $< into $@"
	${CC} $< ${LDFLAGS} -L./lib ${LIB_SOURCES:lib/%.c=-l%} -o $@

# Clean will remove all binaries.
clean:
	@echo "Building target clean"
	rm -f ${BINS}
	rm -f ${LIB_MODULES}
	rm -f ${LIB_ARCHIVES}
