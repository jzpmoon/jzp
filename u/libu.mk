lib_name=libu.so
obj_list=uhash_table.o   \
         ulist.o         \
         ustack.o      	 \
         ustack_int.o    \
         ustring.o       \
         ustring_table.o \
	 uerror.o        \
	 ubuffer.o       \
	 udbuffer.o	 \
	 ustream.o

CFLAGS=-std=c89 -Wall $(DEBUG_MODE)
$(lib_name):$(obj_list)
	$(CC) $(obj_list) -o $(lib_name) -shared
.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) -fPIC
clean:
	rm -f $(lib_name) $(obj_list)
