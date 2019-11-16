bin_name = test_stack
obj_list = test_ustack.o
u_lib    = u
CFLAGS   = -std=c89 -Wall $(DEBUG_MODE)

$(bin_name):$(obj_list) $(u_lib)
	$(CC) $(obj_list) -L./ -l$(u_lib) -Wl,-rpath='$$ORIGIN' -o $(bin_name)
.c.o:
	$(CC) -c -o $@ $< $(CFLAGS)
$(u_lib):
	make -f libu.mk
clean:
	make -f libu.mk clean;rm -f $(bin_name) $(obj_list)
