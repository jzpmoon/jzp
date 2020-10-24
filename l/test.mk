bin=test
obj=test.o
libl_path=./
libv_path=../v/
libu_path=../u/
l_sobj=l
v_sobj=v
u_sobj=u
l_somk=libl.mk
CFLAGS=-std=c89 -Wall $(DEBUG_MODE)

$(bin):$(obj) $(l_sobj)
	$(CC) $(obj) -L$(libl_path) -l$(l_sobj) -L$(libv_path) -l$(v_sobj) -L$(libu_path) -l$(u_sobj) -Wl,-rpath='$$ORIGIN/$(libl_path):$$ORIGIN/$(libv_path):$$ORIGIN/$(libu_path)' -o $(bin)
.c.o:
	$(CC) -c -o $@ $< -I $(libv_path) -I $(libu_path) $(CFLAGS)
$(l_sobj):
	make -f $(l_somk) DEBUG_MODE=$(DEBUG_MODE) ATTR=base
clean:
	make -f $(l_somk) clean;rm -f $(bin) $(obj)
