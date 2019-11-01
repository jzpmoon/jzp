bin=test
obj=test.o lobj.o lparser.o leval.o
libu_path=../u/
libv_path=../v/
v_sobj=v
v_somk=libv.mk
u_sobj=u
u_somk=libu.mk
CFLAGS=-std=c89 -Wall $(DEBUG_MODE)

$(bin):$(obj) $(v_sobj)
	$(CC) $(obj) -L$(libv_path) -l$(v_sobj) -L$(libu_path) -l$(u_sobj) -Wl,-rpath='$$ORIGIN/$(libv_path):$$ORIGIN/$(libu_path)' -o $(bin)
.c.o:
	$(CC) -c -o $@ $< -I $(libv_path) -I $(libu_path) $(CFLAGS)
$(v_sobj):
	make -C $(libv_path) -f $(v_somk) DEBUG_MODE=$(DEBUG_MODE)
clean:
	make -C $(libv_path) -f $(v_somk) clean;rm -f $(bin) $(obj)
