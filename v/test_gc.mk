bin=test_gc
obj=vgc.o test_gc.o vgc_obj.o
lib_path=../u/
sobj=libu.so
CFLAGS=-std=c89 -Wall $(DEBUG_MODE)

$(bin):$(obj) $(sobj)
	$(CC) $(obj) $(sobj) -Wl,-rpath=./ -o $(bin)
.c.o:
	$(CC) -c -o $@ $< -I $(lib_path) $(CFLAGS)
$(sobj):
	make -C $(lib_path) -f libu.mk DEBUG_MODE=$(DEBUG_MODE);mv $(lib_path)$(sobj) ./$(sobj)
clean:
	make -C $(lib_path) -f libu.mk clean;rm -f $(bin) $(obj) $(sobj)
