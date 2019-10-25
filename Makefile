all:
	$(MAKE) -C client
	$(MAKE) -C proxy
	$(MAKE) -C server

client:
	$(MAKE) -C client

server:
	$(MAKE) -C server

proxy:
	$(MAKE) -C proxy