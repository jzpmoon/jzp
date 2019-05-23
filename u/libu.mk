lib_name=libu.so
obj_list=uhash_table.o \
         ulist.o       \
         ustack.o      \
         ustring.o
CFLAGS=-std=c89 -Wall $(DEBUG_MODE)
&(lib_name):$(obj_list)
	$(CC) $(obj_list) -o $(lib_name) -fpic -shared
.c.o:
	$(CC) -c -o $@ $< $(CFLAGS)
clean:
	rm -f $(lib_name) $(obj_list)
