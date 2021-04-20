lib_name=libu.so
obj_list=ulist.o         \
         ustack.o      	 \
         ustring.o       \
         ustring_table.o \
	 uerror.o        \
	 ubuffer.o       \
	 udbuffer.o	 \
	 ustream.o       \
	 uhstb.o         \
	 umempool.o 	 \
         ualloc.o

CFLAGS=-std=c89 -Wall $(DEBUG_MODE)
$(lib_name):$(obj_list)
	$(CC) $(obj_list) -o $(lib_name) -shared
.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) -fPIC
install:
	cp $(lib_name) $(prefix)/
uninstall:
	rm $(prefix)/$(lib_name)
clean:
	rm -f $(lib_name) $(obj_list)
