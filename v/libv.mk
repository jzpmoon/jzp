bin=libv.$(suf_so)
obj=vm.o vcontext.o vgc.o vgc_obj.o vpass.o vgenbc.o \
    vparser.o vreader.o vclosure.o
ag_exec=vag$(suf_pg)

lib_path=../u/
sobj=u
somk=makefile
agmk=makefile_vag
CFLAGS=-std=c89 -Wall -Wextra -Wno-unused-parameter $(DEBUG_MODE)

define gen_ag_exec
	./vag.sh --prefix=$(currdir) --envc=$(envc) \
	--thw=$(thw) --smk=vag.mk --dmk=$(agmk); \
	make -f $(agmk); \
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(currdir)/$(lib_path); \
	./$(ag_exec) autogen.irm; \
	make -f $(agmk) clean
endef

$(bin):$(ag_exec) $(obj) $(sobj)
	$(CC) $(obj) -L$(lib_path) -l$(sobj) -o $(bin) -shared
.c.o:
	$(CC) -c -o $@ $< -I $(lib_path) $(CFLAGS) -fPIC
$(ag_exec):
	$(call gen_ag_exec)
$(sobj):
	make -C $(lib_path) -f $(somk) DEBUG_MODE=$(DEBUG_MODE)
install:
	cp $(bin) $(prefix)/
uninstall:
	rm $(prefix)/$(bin)
clean:
	make -C $(lib_path) -f $(somk) clean; \
	rm -f $(bin) $(obj)
