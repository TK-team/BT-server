# Use to compile the simple project.

TOPDIR = $(shell /bin/pwd)
SRC = $(TOPDIR)/src

all: build_libevent
	@cd $(SRC) && make all

bt_socket:
	gcc -o bt_socket $(SRC)/bt_socket.o -L./libevent/.libs/ -levent 

clean: clean_libevent
	@cd $(SRC) && make clean

clean-all: clean_libevent
	@cd $(SRC) && make clean-all
	
build_libevent:
	[ -d ./libevent/.libs ] || (cd libevent && ./configure && make)
	exit 0

clean_libevent:
	[ -d ./libevent/.libs ] && (cd libevent && make clean)
	exit 0
