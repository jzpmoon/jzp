bin=libv.so
obj=vm.o vcontext.o vgc.o vgc_obj.o vpass.o vgenbc.o
lib_path=../u/
sobj=u
somk=makefile
CFLAGS=-std=c89 -Wall $(DEBUG_MODE)

$(bin):$(obj) $(sobj)
	$(CC) $(obj) -L$(lib_path) -l$(sobj) -o $(bin) -shared
.c.o:
	$(CC) -c -o $@ $< -I $(lib_path) $(CFLAGS) -fPIC
$(sobj):
	cd $(lib_path); \
	./configure.sh --prefix=$(prefix); \
	cd $(currdir); \
	make -C $(lib_path) -f $(somk) DEBUG_MODE=$(DEBUG_MODE)
install:
	cp $(bin) $(prefix)/
uninstall:
	rm $(prefix)/$(bin)
clean:
	make -C $(lib_path) -f $(somk) clean;rm -f $(bin) $(obj)
