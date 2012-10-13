.PHONY : build clean tests check
.DEFAULT_GOAL := build

TESTS := t-murmur3 t-bithacks t-symtable t-heap t-core t-cplusplus
OBJECTS := vm.o core.o symtable.o heap.o
TESTOBJECTS := $(TESTS:%=%.o)
DEPS := $(OBJECTS:%.o=%.d) $(TESTOBJECTS:%.o=%.d)

CLANG := clang -std=c99 -Werror -Weverything
CLANGXX := clang++ -std=c++98 -Werror -Weverything
GCC := gcc -std=c99 -Werror -Wall -Wextra
GXX := g++ -std=c++98 -Werror -Wall -Wextra
CFLAGS := -O3 -ggdb3
NOWARN := padded

CHECK_SYNTAX = $(CLANG) -fsyntax-only $(NOWARN:%=-Wno-%) $<
COMPILE = $(GCC) -c -MMD $(CFLAGS) -o $@ $<
BUILD = $(GCC) $(CFLAGS) -o $@ $^
CLEAN = rm -f $(GARBAGE)
RUN = @set -e; for BIN in $(BINARIES); do echo ./$$BIN; ./$$BIN; done

-include $(DEPS)

build : $(OBJECTS)

clean : GARBAGE := $(OBJECTS) $(TESTOBJECTS) $(DEPS) $(TESTS)
clean :
	$(CLEAN)

tests : $(TESTOBJECTS)

check : BINARIES := $(TESTS)
check : $(TESTS)
	$(RUN)

t-cplusplus : GCC := $(GXX)
t-symtable : symtable.o vm.o
t-heap : heap.o vm.o
$(TESTS) : % : %.o
	$(BUILD)

core.o : NOWARN += gnu
heap.o : NOWARN += unreachable-code
t-cplusplus.o : CLANG := $(CLANGXX) -xc++
t-cplusplus.o : GCC := $(GXX) -xc++
$(OBJECTS) $(TESTOBJECTS) : %.o : %.c
	$(CHECK_SYNTAX)
	$(COMPILE)
