# Use to compile the simple project.

bin = main
object = udp_server.o

cli = client
cli_obj = udp_client.o

.PHONY: all
all: $(object) $(cli_obj)
	cc -o $(bin) $(object)
	cc -o $(cli) $(cli_obj)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(bin) $(object) $(cli) $(cli_obj)
