.PHONY : build clean test

TESTS := t-basic
OBJECTS := core.o

CLANG := clang -std=c99 -Werror -Weverything
GCC := gcc -std=c99 -Werror -Wall -Wextra
CFLAGS := -O3 -ggdb3

CHECK_SYNTAX = $(CLANG) -fsyntax-only $(NOWARN:%=-Wno-%) $<
COMPILE = $(GCC) -c $(CFLAGS) -o $@ $<
BUILD = $(GCC) $(CFLAGS) -o $@ $^
CLEAN = rm -f $(GARBAGE)
RUN = @set -e; for BIN in $(BINARIES); do echo ./$$BIN; ./$$BIN; done

build : $(OBJECTS)

clean : GARBAGE := $(OBJECTS) $(TESTS)
clean :
	$(CLEAN)

test : BINARIES := $(TESTS)
test : $(TESTS)
	$(RUN)

$(TESTS) : % : %.c $(OBJECTS)
	$(CHECK_SYNTAX)
	$(BUILD)

core.o : NOWARN := gnu
$(OBJECTS) : %.o : %.c x3.h
	$(CHECK_SYNTAX)
	$(COMPILE)
