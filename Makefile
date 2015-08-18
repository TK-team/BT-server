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

bin_server = $(SRC)/udp_server
server_object = $(SRC)/udp_server.o
bin_client = $(SRC)/udp_client
client_object = $(SRC)/udp_client.o
trace_object = $(SRC)/bttrace.o
bin_seed_parse = $(SRC)/b_parse
seed_parse_object = $(SRC)/b_parse.o

CFLAGS += -Wall -Iinclude -D_BTDEBUG

.PHONY: all 
all: $(server_object) $(client_object) $(trace_object) $(seed_parse_object)
	cc -o $(bin_server) $(trace_object) $(server_object) $(LD_FLAGS)
	cc -o $(bin_client) $(trace_object) $(client_object)
	cc -o $(bin_seed_parse) $(seed_parse_object) $(trace_object) $(LD_FLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(trace_object) $(server_object) $(client_object) $(bin_client) $(bin_server) $(seed_parse_object) $(bin_seed_parse)

