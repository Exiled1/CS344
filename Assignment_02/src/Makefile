#LDFLAGS: Extra flags to give to compilers when they are supposed to invoke the linker


CC=gcc
CFLAGS=-std=c11 -Wall -Werror -g3 -O0 -Iinclude

objects = main.o

binary = main

all: $(objects) # falls down to the $(objects) call.

# These files compile via implicit rules
# Syntax - targets ...: target-pattern: prereq-patterns ...
# In the case of the first target, foo.o, the target-pattern matches foo.o and sets the "stem" to be "foo".
# It then replaces the '%' in prereq-patterns with that stem

$(objects): %.o: %.c



clean:
	rm -f *.o $(binary)

