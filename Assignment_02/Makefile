
# THIS IS WHERE YOU SPECIFY THE SOURCE FILES.
SOURCES = main.c a.c 

# Target is the final executable name.
TARGET = blob

VPATH =./src/ # VPATH is to set the source code path, use this if the path isn't 

# --------------------------------------- Below is config stuff ------------------------------------------------
#LDFLAGS: Extra flags to give to compilers when they are supposed to invoke the linker

CC=gcc
CFLAGS=-std=c11 -Wall -Werror -g3 -O0 -Iinclude

# This default goal is to make your main target, it'll go and create an executable file in the specified VPATH 

objects = $(SOURCES:.c=.o) # objects is to be able to take all of our sources and turn them into the correct fallback rule (in this case .exe/.o)

.DEFAULT_GOAL := $(TARGET)

all: $(objects) # falls down to the $(objects) call.

# These files compile via implicit rules
# Syntax - targets ...: target-pattern: prereq-patterns ...
# In the case of the first target, foo.o, the target-pattern matches foo.o and sets the "stem" to be "foo".
# It then replaces the '%' in prereq-patterns with that stem

$(TARGET) : $(objects)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(objects): %.o: %.c

.PHONY: clean

clean:
	rm -f *.o $(TARGET)

