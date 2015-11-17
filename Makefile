# Use to compile the simple project.

TOPDIR = $(shell /bin/pwd)
SRC = $(TOPDIR)/src

all: build_libevent
	@cd $(SRC) && make all

clean:
	@cd $(SRC) && make clean

clean-all: clean_libevent
	@cd $(SRC) && make clean-all
	
build_libevent:
	@cd libevent && ./configure && make

clean_libevent:
	@cd libevent && make clean
