bin=libv.so
obj=vm.o vcontext.o vgc.o vgc_obj.o vpass.o vgenbc.o
lib_path=../u/
sobj=u
somk=libu.mk
CFLAGS=-std=c89 -Wall $(DEBUG_MODE)

$(bin):$(obj) $(sobj)
	$(CC) $(obj) -L$(lib_path) -l$(sobj) -Wl,-rpath='$$ORIGIN/$(lib_path)' -o $(bin) -shared
.c.o:
	$(CC) -c -o $@ $< -I $(lib_path) $(CFLAGS) -fPIC
$(sobj):
	make -C $(lib_path) -f $(somk) DEBUG_MODE=$(DEBUG_MODE)
install:
	cp $(bin) ~/../usr/lib/
uninstall:
	rm ~/../usr/lib/$(bin)
clean:
	make -C $(lib_path) -f $(somk) clean;rm -f $(bin) $(obj)
