# Use to compile the simple project.

bin_server = udp_server
server_object = udp_server.o
bin_client = udp_client
client_object = udp_client.o

.PHONY: server
server: $(server_object) 
	cc -o $(bin_server) $(server_object)

.PHONY: client
client: $(client_object)
	cc -o $(bin_client) $(client_object)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(bin) $(object)
