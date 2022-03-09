lib_name := libi$(suf_so)
obj_list := ibmp.o

libu_path := ../u/
libu_sobj := u

CFLAGS := -std=c89 -Wall -Wextra -Wno-unused-parameter $(DEBUG_MODE)

$(lib_name):$(obj_list) $(libu_sobj)
	$(CC) $(obj_list) -L$(libu_path) -l$(libu_sobj) -o $(lib_name) -shared
.c.o:
	$(CC) -c -o $@ $< -I $(libu_path) $(CFLAGS) -fPIC
$(libu_sobj):
	cd $(libu_path); \
	./configure.sh --prefix=$(prefix) --envc=$(envc) --thw=$(thw); \
	cd $(currdir); \
	make -C $(libu_path) -f makefile DEBUG_MODE=$(DEBUG_MODE)
install:
	cp $(lib_name) $(prefix)/; \
	make -C $(libu_path) -f makefile install
uninstall:
	rm $(prefix)/$(lib_name); \
	make -C $(libu_path) -f makefile uninstall
clean:
	make -C $(libu_path) -f makefile clean; \
	rm -f $(lib_name) $(obj_list)
