bin = libl.so
obj = lparser.o lattr.o
temp_attr_file = _ltemp.attr

libu_path=../u/
libv_path=../v/
v_sobj=v
v_somk=libv.mk
u_sobj=u
u_somk=libu.mk
CFLAGS=-std=c89 -Wall $(DEBUG_MODE)

define gen_attr_file
	cat $(shell echo ${ATTR}.attr | sed 's: :.attr :g') > $(temp_attr_file)
	echo "void lattr_file_concat_init(ltoken_state* ts){\
	_lattr_file_init_"$(shell echo ${ATTR} | \
	sed 's: :(ts);_lattr_file_init:g')"(ts);\
	}" >> $(temp_attr_file)
endef

$(bin):$(temp_attr_file) $(obj) $(v_sobj)
	$(CC) $(obj) -L$(libv_path) -l$(v_sobj) -L$(libu_path) -l$(u_sobj) \
	-Wl,-rpath='$$ORIGIN/$(libv_path):$$ORIGIN/$(libu_path)' -o $(bin) -shared
.c.o:
	$(CC) -c -o $@ $< -I $(libv_path) -I $(libu_path) $(CFLAGS) -fPIC
$(temp_attr_file):
	$(call gen_attr_file)
$(v_sobj):
	make -C $(libv_path) -f $(v_somk) DEBUG_MODE=$(DEBUG_MODE)
clean:
	make -C $(libv_path) -f $(v_somk) clean;rm -f $(bin) $(obj) $(temp_attr_file)
