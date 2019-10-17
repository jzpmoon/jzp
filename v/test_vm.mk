bin=test_vm
obj=vvm.o vgc.o vgc_obj.o test_vm.o
lib_path=../u/
sobj=u
CFLAGS=-std=c89 -Wall $(DEBUG_MODE)

$(bin):$(obj) $(sobj)
	$(CC) $(obj) -L$(lib_path) -l$(sobj) -Wl,-rpath='$$ORIGIN/$(lib_path)' -o $(bin)
.c.o:
	$(CC) -c -o $@ $< -I $(lib_path) $(CFLAGS)
$(sobj):
	make -C $(lib_path) -f libu.mk DEBUG_MODE=$(DEBUG_MODE)
clean:
	make -C $(lib_path) -f libu.mk clean;rm -f $(bin) $(obj)
