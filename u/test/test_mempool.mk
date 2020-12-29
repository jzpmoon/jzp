bin = test_mempool
obj = ../umempool.o \
      test_mempool.o
libu_path = ../


$(bin):$(obj)
	$(CC) $(obj) -o $(bin)
.c.o:
	$(CC) -std=c89 -Wall -I $(libu_path) -c -o $@ $<
clean:
	rm -f $(bin) $(obj)
