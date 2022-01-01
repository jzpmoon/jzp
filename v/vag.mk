bin=vag$(suf_pg)
obj=vparser.o vreader.o vag.o
temp_attr_file=_vtemp.attr

lib_path=../u/
sobj=u
somk=makefile
CFLAGS=-std=c89 -Wall -Wextra -Wno-unused-parameter $(DEBUG_MODE)

ATTR=autogen

define gen_attr_file
	./attr.sh --attr=$(ATTR) --out=$(temp_attr_file) \
	--callback=vattr_file_concat_init
endef

$(bin):$(temp_attr_file) $(obj) $(sobj)
	$(CC) $(obj) -L$(lib_path) -l$(sobj) -o $(bin)
.c.o:
	$(CC) -c -o $@ $< -I $(lib_path) $(CFLAGS) -fPIC
$(temp_attr_file):
	$(call gen_attr_file)
$(sobj):
	cd $(lib_path); \
	./configure.sh --prefix=$(prefix) --envc=$(envc) --thw=$(thw); \
	cd $(currdir); \
	make -C $(lib_path) -f $(somk) DEBUG_MODE=$(DEBUG_MODE)
install:
	cp $(bin) $(prefix)/; \
	make -C $(lib_path) -f $(somk) install
uninstall:
	rm $(prefix)/$(bin); \
	make -C $(lib_path) -f $(somk) uninstall
clean:
	make -C $(lib_path) -f $(somk) clean; \
	rm -f $(bin) $(obj) $(temp_attr_file)
