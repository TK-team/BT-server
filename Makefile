# Use to compile the simple project.

bin_server = udp_server
server_object = udp_server.o seed_parse.o 
bin_client = udp_client
client_object = udp_client.o

CFLAGS += -Wall

.PHONY: all
all: $(server_object) $(client_object)
	cc -o $(bin_server) $(server_object)
	cc -o $(bin_client) $(client_object)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(server_object) $(client_object) $(bin_client) $(bin_server)
