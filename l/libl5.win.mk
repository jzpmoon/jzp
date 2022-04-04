bin = libl5$(suf_so)
obj = leval.obj l5eval.obj

temp_attr_file = _l5temp.attr

libu_path = ..\u\

libv_path = ..\v\

v_somk = makefile

CFLAGS = $(STDC98) $(WALL) $(WEXTRA) $(WNO_UNUSED_PARAMETER) $(DEBUG_MODE)

ATTR = l5base

$(bin):$(temp_attr_file) libv.lib $(obj)
	$(LINK) $(obj) $(L)$(libv_path) libv.lib $(L)$(libu_path) libu.lib $(OUT)$(bin) $(SHARED)
.c.obj:
	$(CC) $(C) $(COUT)$@ $< $(I) $(libv_path) $(I) $(libu_path) $(CFLAGS) $(FPIC) $(D)_EXPORT_LIBU_ $(D)_EXPORT_LIBV_
$(temp_attr_file):
	$(libv_path)\attr.bat --attr=$(ATTR) --out=$(temp_attr_file) --callback=l5attr_file_concat_init
libv.lib:
	cd $(libv_path)
	nmake /f $(v_somk) DEBUG_MODE=$(DEBUG_MODE)
	cd $(currdir)
install:
	copy $(bin) $(prefix)
	copy sysmod.l5 $(prefix)
	copy usrmod.l5 $(prefix)
uninstall:
	del $(prefix)\$(bin)
	del $(prefix)\sysmod.l5
	del $(prefix)\usrmod.l5
clean:
	cd $(libv_path)
	nmake /f $(v_somk) clean
	cd $(currdir)
	del $(bin) $(obj) $(temp_attr_file)
