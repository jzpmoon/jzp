bin = libl.$(suf_so)
obj = lattr.o lcfun.o lobj.o leval.o
temp_attr_file = _ltemp.attr
temp_cfun_file = _ltemp.cfun

libu_path=../u/
libv_path=../v/
v_sobj=v
v_somk=makefile
u_sobj=u
u_somk=makefile
CFLAGS=-std=c89 -Wall -Wextra -Wno-unused-parameter $(DEBUG_MODE)

ATTR = mod,proc,base
CFUN = base

define gen_attr_file
	$(libv_path)/attr.sh --attr=$(ATTR) --out=$(temp_attr_file) \
	--callback=lattr_file_concat_init
endef

define gen_cfun_file
	$(libv_path)/cfun.sh --cfun=$(CFUN) --out=$(temp_cfun_file) \
	--callback=lcfun_file_concat_init
endef

$(bin):$(temp_attr_file) $(temp_cfun_file) $(obj) $(v_sobj)
	$(CC) $(obj) -L$(libv_path) -l$(v_sobj) -L$(libu_path) -l$(u_sobj) \
	-o $(bin) -shared
.c.o:
	$(CC) -c -o $@ $< -I $(libv_path) -I $(libu_path) $(CFLAGS) -fPIC
$(temp_attr_file):
	$(call gen_attr_file)
$(temp_cfun_file):
	$(call gen_cfun_file)
$(v_sobj):
	cd $(libv_path); \
	./configure.sh --prefix=$(prefix) --envc=$(envc) --thw=$(thw); \
	cd $(currdir); \
	make -C $(libv_path) -f $(v_somk) DEBUG_MODE=$(DEBUG_MODE)
install:
	cp $(bin) $(prefix)/
uninstall:
	rm $(prefix)/$(bin)
clean:
	make -C $(libv_path) -f $(v_somk) clean; \
	rm -f $(bin) $(obj) $(temp_attr_file) $(temp_cfun_file)
