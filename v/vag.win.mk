bin = vag$(suf_pg)
obj = vparser.obj \
      vreader.obj \
	  vag.obj

temp_attr_file = _vtemp.attr

lib_path = ..\u\

somk = makefile

CFLAGS = $(STDC98) $(WALL) $(WEXTRA) $(WNO_UNUSED_PARAMETER) $(DEBUG_MODE)

ATTR = autogen

$(bin):libu.lib $(temp_attr_file) $(obj)
	$(LINK) $(obj) $(L)$(lib_path) libu.lib $(OUT)$(bin)
.c.obj:
	$(CC) $(C) $(COUT)$@ $< $(I) $(lib_path) $(CFLAGS) $(FPIC) $(D)_NOTEXPORT_LIBV_
$(temp_attr_file):
	.\attr.bat --attr=$(ATTR) --out=$(temp_attr_file) --callback=vattr_file_concat_init
libu.lib:
	cd $(lib_path)
	nmake /f $(somk) DEBUG_MODE=$(DEBUG_MODE)
	cd $(currdir)
install:
	cp $(bin) $(prefix)
	cd $(lib_path)
	nmake /f $(somk) install
	cd $(currdir)
uninstall:
	del $(prefix)\$(bin)
	cd $(lib_path)
	nmake -f $(somk) uninstall
	cd $(currdir)
clean:
	cd $(lib_path)
	nmake /f $(somk) clean
	cd $(currdir)
	del $(bin) $(obj) $(temp_attr_file)
