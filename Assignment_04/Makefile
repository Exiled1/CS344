
# THIS IS WHERE YOU SPECIFY THE C SOURCE FILES.
SOURCES = uniqify.c

# Target is the final executable name.

TARGET = uniqify

# VPATH is to set the source code path, use this if the path isn't src.
VPATH =./src/

# Define any header directories here (As long as they're in the root directory).
DIRECTORIES = include

# I made this ftmacros variable to expand and include feature test macros automatically.
FTMACROS = _POSIX_SOURCE _GNU_SOURCE _POSIX_C_SOURCE

# --------------------------------------- Below is config stuff ------------------------------------------------

# LDFLAGS: Extra flags to give to compilers when they are supposed to invoke the linker

# This conditional snippet checks to see if the feature test macros variable is set to anything, if it is then we add prefixes to the expanded
# variable list, if it's not defined, then we leave the EXP variable blank so that we can add it to the compile flags.
ifdef FTMACROS
EXP= $(addprefix -D, $(FTMACROS))
else
EXP=
endif

# A conditional snippet I made to repeatedly expand pre-fixed directories (If any have been added in the directories variable).
ifdef DIRECTORIES
HDIR:= $(addprefix -I, $(DIRECTORIES))
else
HDIR:=
endif


CC=gcc
CFLAGS= -std=c11 -g3 -pthread -O0 $(HDIR) $(EXP)

# This default goal is to make your main target, it'll go and create an executable file in the specified VPATH 
.DEFAULT_GOAL := $(TARGET)

objects = $(SOURCES:.c=.o) # objects is to be able to take all of our sources and turn them into the correct fallback rule (in this case .exe/.o)


# falls down to the $(objects) call.
all: $(objects) 

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

