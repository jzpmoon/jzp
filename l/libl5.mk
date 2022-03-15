bin = libl5$(suf_so)
obj = leval.o l5eval.o
temp_attr_file = _l5temp.attr

libu_path=../u/
libv_path=../v/
v_sobj=v
v_somk=makefile
u_sobj=u
u_somk=makefile
CFLAGS=-std=c89 -Wall -Wextra -Wno-unused-parameter $(DEBUG_MODE)

ATTR = l5base
CFUN = l3base l5base

define gen_attr_file
	$(libv_path)/attr.sh --attr=$(ATTR) --out=$(temp_attr_file) \
	--callback=l5attr_file_concat_init
endef

$(bin):$(temp_attr_file) $(obj) $(v_sobj)
	$(CC) $(obj) -L$(libv_path) -l$(v_sobj) -L$(libu_path) -l$(u_sobj) \
	-o $(bin) -shared
.c.o:
	$(CC) -c -o $@ $< -I $(libv_path) -I $(libu_path) $(CFLAGS) -fPIC
$(temp_attr_file):
	$(call gen_attr_file)
$(v_sobj):
	make -C $(libv_path) -f $(v_somk) DEBUG_MODE=$(DEBUG_MODE)
install:
	cp $(bin) $(prefix)/;    \
	cp sysmod.l5 $(prefix)/; \
	cp usrmod.l5 $(prefix)/
uninstall:
	rm $(prefix)/$(bin);    \
	rm $(prefix)/sysmod.l5; \
	rm $(prefix)/usrmod.l5
clean:
	make -C $(libv_path) -f $(v_somk) clean; \
	rm -f $(bin) $(obj) $(temp_attr_file)
