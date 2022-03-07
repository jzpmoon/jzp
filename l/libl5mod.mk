bin = libl5mod.$(suf_so)
obj = l5mod.o lobj.o
temp_cfun_file = _l5temp.cfun

libu_path=../u/
libv_path=../v/
v_sobj=v
v_somk=makefile
u_sobj=u
u_somk=makefile
CFLAGS=-std=c89 -Wall -Wextra -Wno-unused-parameter $(DEBUG_MODE)

CFUN = l3base l5base

define gen_cfun_file
	$(libv_path)/cfun.sh --cfun=$(CFUN) --out=$(temp_cfun_file) \
	--callback=l5cfun_file_concat_init
endef

$(bin):$(temp_attr_file) $(temp_cfun_file) $(obj) $(v_sobj)
	$(CC) $(obj) -L$(libv_path) -l$(v_sobj) -L$(libu_path) -l$(u_sobj) \
	-o $(bin) -shared -ldl
.c.o:
	$(CC) -c -o $@ $< -I $(libv_path) -I $(libu_path) $(CFLAGS) -fPIC
$(temp_cfun_file):
	$(call gen_cfun_file)
$(v_sobj):
	make -C $(libv_path) -f $(v_somk) DEBUG_MODE=$(DEBUG_MODE)
install:
	cp $(bin) $(prefix)/
uninstall:
	rm $(prefix)/$(bin)
clean:
	make -C $(libv_path) -f $(v_somk) clean; \
	rm -f $(bin) $(obj) $(temp_cfun_file)
