bin = liblmod$(suf_so)
obj = lmod.obj lobj.obj
temp_cfun_file = _ltemp.cfun

libu_path=..\u\

libv_path=..\v\

v_somk=makefile

CFLAGS = $(STDC98) $(WALL) $(WEXTRA) $(WNO_UNUSED_PARAMETER) $(DEBUG_MODE)

CFUN = l3base-l5base

$(bin):$(temp_cfun_file) libv.lib $(obj)
	$(LINK) $(obj) $(L)$(libv_path) libv.lib $(L)$(libu_path) libu.lib $(OUT)$(bin) $(SHARED)
.c.obj:
	$(CC) $(C) $(COUT)$@ $< $(I) $(libv_path) $(I) $(libu_path) $(CFLAGS) $(FPIC) $(D)_EXPORT_LIBLM_
$(temp_cfun_file):
	$(libv_path)\cfun.bat --cfun=$(CFUN) --out=$(temp_cfun_file) --callback=lcfun_file_concat_init
libv.lib:
	cd $(libv_path)
	nmake /f $(v_somk) DEBUG_MODE=$(DEBUG_MODE)
	cd $(currdir)
install:
	copy $(bin) $(prefix)
uninstall:
	del $(prefix)\$(bin)
clean:
	cd $(libv_path)
	nmake /f $(v_somk) clean
	cd $(currdir)
	del $(bin) $(obj) $(temp_cfun_file) *.lib *.exp
