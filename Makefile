# Use to compile the simple project.

TOPDIR = $(shell /bin/pwd)
SRC = $(TOPDIR)/src

x86=$(shell uname -a | grep -io "x86_64" | head -1)
ifneq "${x86}" "x86_64"
x86=x86_32
endif

CONFIG_DEBUG=y

ifeq "${CONFIG_DEBUG}" "y"
CFLAGS += -g
endif

CONFIG_UNIT_TEST=y
ifeq "${CONFIG_UNIT_TEST}" "y"
CFLAGS += -D_UNIT_TEST
LD_FLAGS += -L$(TOPDIR)/cmockery/${x86} -lcmockery
endif

trace_object = $(SRC)/bttrace.o
bin_b_parse = $(SRC)/b_parse
b_parse_object = $(SRC)/b_parse.o
bin_torrent_parse = $(SRC)/torrent_parse
torrent_parse_object = $(SRC)/torrent_parse.o
bin_peer_message = $(SRC)/peer_message
peer_message_object = $(SRC)/peer_message.o
timer_object = $(SRC)/bt_timer.o
bin_timer = $(SRC)/bt_timer

CFLAGS += -Wall -Iinclude -D_BTDEBUG

.PHONY: all 
all: $(server_object) $(client_object) $(trace_object) $(b_parse_object) $(torrent_parse_object) $(peer_message_object) $(timer_object)
	#cc -o $(bin_b_parse) $(b_parse_object) $(trace_object) $(LD_FLAGS)
	cc -o $(bin_torrent_parse) $(torrent_parse_object) $(b_parse_object) $(trace_object) $(LD_FLAGS)
	cc -o $(bin_peer_message) $(peer_message_object) $(trace_object) $(b_parse_object) $(timer_object) $(LD_FLAGS)
	#cc -o $(bin_timer) $(timer_object) $(trace_object) $(LD_FLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

build_libevent:
	@cd libevent && ./configure && make

clean_libevent:
	@cd libevent && make clean

.PHONY: clean
clean:
	rm -rf $(trace_object) $(server_object) $(client_object) $(bin_client) $(bin_server) $(b_parse_object) $(bin_b_parse) $(torrent_parse_object) $(bin_torrent_parse) $(peer_message_object) $(bin_peer_message) $(timer_object) $(bin_timer)

