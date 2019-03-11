lib_name=libu.so
obj_list=uhash_table.o \
         ulist.o       \
         ustack.o      \
         ustring.o

&(lib_name):$(obj_list)
	$(CC) $(obj_list) -o $(lib_name) -fpic -shared
.c.o:
	$(CC) -c -o $@ $< -std=c89 -Wall -g
clean:
	rm -f $(lib_name) $(obj_list)
