bin_name = jzpl
obj_list = jzpl.o

l_lib = l
v_lib = v
u_lib = u

libl_path = ../l/
libv_path = ../v/
libu_path = ../u/

l_somk   = $(libl_path)libl.mk
v_somk   = $(libv_path)libv.mk
u_somk   = $(libu_path)libu.mk
CFLAGS   = -std=c89 -Wall $(DEBUG_MODE)

&(bin_name):$(obj_list) $(l_lib)
	$(CC) $(obj_list) -L$(libl_path) -l$(l_lib) -L$(libv_path) -l$(v_lib) -L$(libu_path) -l$(u_lib) -Wl,-rpath='$$ORIGIN/$(libl_path):$$ORIGIN/$(libv_path):$$ORIGIN/$(libu_path)' -o $(bin_name)
.c.o:
	$(CC) -c -o $@ $< -I $(libl_path) -I $(libv_path) -I $(libu_path) $(CFLAGS)
$(l_lib):
	make -C $(libl_path) -f $(l_somk)
install:
	cp $(bin_name) ~/../usr/bin/; \
	make -C $(libl_path) -f $(l_somk) install; \
	make -C $(libv_path) -f $(v_somk) install; \
	make -C $(libu_path) -f $(u_somk) install
uninstall:
	rm ~/../usr/bin/$(bin_name); \
	make -C $(libl_path) -f $(l_somk) uninstall; \
	make -C $(libv_path) -f $(v_somk) uninstall; \
	make -C $(libu_path) -f $(u_somk) uninstall
clean:
	make -C $(libl_path) -f $(l_somk) clean;rm -f $(bin_name) $(obj_list)
