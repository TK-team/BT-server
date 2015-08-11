# Use to compile the simple project.

TOPDIR = $(shell /bin/pwd)
SRC = $(TOPDIR)/src

bin_server = $(SRC)/udp_server
server_object = $(SRC)/udp_server.o $(SRC)/seed_parse.o 
bin_client = $(SRC)/udp_client
client_object = $(SRC)/udp_client.o
trace_object = $(SRC)/bttrace.o

CFLAGS += -Wall -Iinclude -D_BTDEBUG
LD_FLAGS += -L$(TOPDIR)/cmockery -lcmockery

.PHONY: all
all: $(server_object) $(client_object) $(trace_object)
	cc -o $(bin_server) $(server_object) $(LD_FLAGS)
	cc -o $(bin_client) $(trace_object) $(client_object)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(trace_object) $(server_object) $(client_object) $(bin_client) $(bin_server)

