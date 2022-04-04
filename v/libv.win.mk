bin = libv$(suf_so)
obj = vm.obj       \
      vcontext.obj \
	  vgc.obj      \
	  vgc_obj.obj  \
	  vpass.obj    \
	  vgenbc.obj   \
      vparser.obj  \
	  vreader.obj  \
	  vclosure.obj

ag_exec = vag$(suf_pg)

lib_path = ..\u\

somk = makefile
agmk = makefile_vag

CFLAGS = $(STDC98) $(WALL) $(WEXTRA) $(WNO_UNUSED_PARAMETER) $(DEBUG_MODE)

$(bin):$(ag_exec) libu.lib $(obj)
	$(LINK) $(obj) $(L)$(lib_path) libu.lib $(OUT)$(bin) $(SHARED)
.c.obj:
	$(CC) $(C) $(COUT)$@ $< $(I) $(lib_path) $(CFLAGS) $(FPIC) $(D)_EXPORT_LIBV_
$(ag_exec):
	.\vag.bat --prefix=$(currdir) --envc=$(envc) --thw=$(thw) --smk=vag.win.mk --dmk=$(agmk)
	nmake /f $(agmk)
	set path=%path%;$(currdir)\$(lib_path)
	.\$(ag_exec) autogen.irm
	nmake /f $(agmk) clean
libu.lib:
	cd $(lib_path)
	nmake /f $(somk) DEBUG_MODE=$(DEBUG_MODE)
	cd $(currdir)
install:
	copy $(bin) $(prefix)
uninstall:
	del $(prefix)\$(bin)
clean:
	cd $(lib_path)
	nmake /f $(somk) clean
	cd $(currdir)
	del $(bin) $(obj) *.lib *.exp
