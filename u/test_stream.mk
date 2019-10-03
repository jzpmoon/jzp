lib_name=test_stream
obj_list=ubuffer.o     \
	 udbuffer.o    \
	 ustream.o     \
	 test_stream.o

CFLAGS=-std=c89 -Wall $(DEBUG_MODE)
&(lib_name):$(obj_list)
	$(CC) $(obj_list) -o $(lib_name)
.c.o:
	$(CC) -c -o $@ $< $(CFLAGS)
clean:
	rm -f $(lib_name) $(obj_list)
