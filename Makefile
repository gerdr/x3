.PHONY : build clean test
.DEFAULT_GOAL := build

TESTS := t-murmur3 t-symtable t-cxx-header
OBJECTS := core.o symtable.o heap.o
DEPS := $(OBJECTS:%.o=%.d)

CLANG := clang -std=c99 -Werror -Weverything
CLANGXX := clang++ -std=c++98 -Werror -Weverything
GCC := gcc -std=c99 -Werror -Wall -Wextra
GXX := g++ -std=c++98 -Werror -Wall -Wextra
CFLAGS := -O3 -ggdb3

CHECK_SYNTAX = $(CLANG) -fsyntax-only $(NOWARN:%=-Wno-%) $<
COMPILE = $(GCC) -c -MMD $(CFLAGS) -o $@ $<
BUILD = $(GCC) $(CFLAGS) -o $@ $^
CLEAN = rm -f $(GARBAGE)
RUN = @set -e; for BIN in $(BINARIES); do echo ./$$BIN; ./$$BIN; done

-include $(DEPS)

build : $(OBJECTS)

clean : GARBAGE := $(OBJECTS) $(DEPS) $(TESTS)
clean :
	$(CLEAN)

test : BINARIES := $(TESTS)
test : $(TESTS)
	$(RUN)

t-cxx-header : CLANG := $(CLANGXX) -xc++
t-cxx-header : GCC := $(GXX)
$(TESTS) : % : %.c $(OBJECTS)
	$(CHECK_SYNTAX)
	$(BUILD)

core.o : NOWARN := gnu
heap.o : NOWARN := unreachable-code
$(OBJECTS) : %.o : %.c
	$(CHECK_SYNTAX)
	$(COMPILE)
