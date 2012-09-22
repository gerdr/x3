.PHONY : build clean test
.DEFAULT_GOAL := build

TESTS := t-basic
OBJECTS := core.o
DEPS := $(OBJECTS:%.o=%.d)

CLANG := clang -std=c99 -Werror -Weverything
GCC := gcc -std=c99 -Werror -Wall -Wextra
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

$(TESTS) : % : %.c $(OBJECTS)
	$(CHECK_SYNTAX)
	$(BUILD)

core.o : NOWARN := gnu
$(OBJECTS) : %.o : %.c
	$(CHECK_SYNTAX)
	$(COMPILE)
