bin=test_vm
obj=test.o lobj.o
libu_path=../u/
libv_path=../v/
sobj=v
somk=libv.mk
CFLAGS=-std=c89 -Wall $(DEBUG_MODE)

$(bin):$(obj) $(sobj)
	$(CC) $(obj) -L$(libv_path) -l$(sobj) -Wl,-rpath='$$ORIGIN/$(libv_path)' -o $(bin)
.c.o:
	$(CC) -c -o $@ $< -I $(libv_path) -I $(libu_path) $(CFLAGS)
$(sobj):
	make -C $(libv_path) -f $(somk) DEBUG_MODE=$(DEBUG_MODE)
clean:
	make -C $(libv_path) -f $(somk) clean;rm -f $(bin) $(obj)
