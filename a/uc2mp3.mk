bin_name = uc2mp3
obj_list = uc2mp3.o
u_lib    = u
libu_path = ../u/
u_somk   = $(libu_path)libu.mk
CFLAGS   = -std=c89 -Wall $(DEBUG_MODE)

&(lib_name):$(obj_list) $(u_lib)
	$(CC) $(obj_list) -L$(libu_path) -l$(u_lib) -o $(bin_name)
.c.o:
	$(CC) -c -o $@ $< -I $(libu_path) $(CFLAGS)
$(u_lib):
	make -C $(libu_path) -f $(u_somk)
install:
	cp $(bin_name) ~/../usr/bin/;make -C $(libu_path) -f $(u_somk) install
clean:
	make -C $(libu_path) -f $(u_somk) clean;rm -f $(bin_name) $(obj_list)
