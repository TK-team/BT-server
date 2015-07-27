# Use to compile the simple project.

bin = main
object = udp_server.o

.PHONY: all
all: $(object) 
	cc -o $(bin) $(object)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(bin) $(object)
