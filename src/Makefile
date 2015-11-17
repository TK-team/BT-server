# Use to compile the simple project.
PROJECT = bt_utility 

# Compiler
CC = gcc

# x86=$(shell uname -a | grep -io "x86_64" | head -1)
# ifneq "${x86}" "x86_64"
# x86=x86_32
# endif
# 
# CONFIG_DEBUG=y
# 
# ifeq "${CONFIG_DEBUG}" "y"
# CFLAGS += -g
# endif
# 
# CONFIG_UNIT_TEST=y
# ifeq "${CONFIG_UNIT_TEST}" "y"
# UNIT_TEST_CFLAGS += -D_UNIT_TEST -Wall -Iinclude -D_BTDEBUG
# LD_FLAGS += -L$(TOPDIR)/cmockery/${x86} -lcmockery
# endif

# Use to build the lib
CFLAGS += -I../include -D_BTDEBUG

# Compiler options during compilation -pedantic
COMPILE_OPTIONS = -Wall $(CFLAGS)

# Dependency options
DEPENDENCY_OPTIONS = -MM $(CFLAGS)


#----------------------- Do not edit below this line --------------------------


# Subdirs to search for additional source files
SUBDIRS := $(shell ls -F | grep "\/" )
DIRS := ./ $(SUBDIRS)
SOURCE_FILES := $(foreach d, $(DIRS), $(wildcard $(d)*.c) )

# Create an object file of every c file
OBJECTS = $(patsubst %.c, %.o, $(SOURCE_FILES))

# Dependencies
DEPENDENCIES = $(patsubst %.c, %.d, $(SOURCE_FILES))

# Create .d files
%.d: %.c
	$(CC) $(DEPENDENCY_OPTIONS) $< -MT "$*.o $*.d" -MF $*.d


# Make $(PROJECT) the default target
all: $(DEPENDENCIES) $(PROJECT)


$(PROJECT): $(OBJECTS)
	$(CC) -o $(PROJECT) $(OBJECTS) $(LIBS)

# Include dependencies (if there are any)
ifneq "$(strip $(DEPENDENCIES))" ""
include $(DEPENDENCIES)
endif

# Compile every c file to an object
%.o: %.c
	$(CC) -c $(COMPILE_OPTIONS) -o $@ $< $(HEADERS)

.PHONY: clean
clean:
	rm -f $(PROJECT) $(OBJECTS)

.PHONY: depclean
depclean:
	rm -f $(DEPENDENCIES)

clean-all: clean depclean



