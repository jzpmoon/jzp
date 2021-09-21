bin_name := test$(suf_pg)
obj_list := test.o

libi_path := ../
libi_sobj := i
libu_path := ../../u
libu_sobj := u

CFLAGS := -std=c89 -Wall -Wextra -Wno-unused-parameter $(DEBUG_MODE)

$(bin_name):$(obj_list) $(libi_sobj)
	$(CC) $(obj_list) -L$(libi_path) -l$(libi_sobj) -L$(libu_path) -l$(libu_sobj) -o $(bin_name)
.c.o:
	$(CC) -c -o $@ $< -I $(libi_path) -I $(libu_path) $(CFLAGS) -fPIC
$(libi_sobj):
	cd $(libi_path); \
	./configure.sh --prefix=$(prefix) --envc=$(envc) --thw=$(thw); \
	cd $(currdir); \
	make -C $(libi_path) -f makefile DEBUG_MODE=$(DEBUG_MODE)
install:
	cp $(bin_name) $(prefix)/; \
	make -C $(libi_path) -f makefile install; \
	cp $(entry_exec) $(prefix)/
uninstall:
	rm $(prefix)/$(bin_name); \
	make -C $(libi_path) -f makefile uninstall
clean:
	make -C $(libi_path) -f makefile clean; \
	rm -f $(bin_name) $(obj_list)
