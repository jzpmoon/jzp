bin_name = l5$(suf_pg)
obj_list = l5.obj

conf_name = l5.win.conf

libl_path = ..\l\

libv_path = ..\v\

libu_path = ..\u\

liblmod_path = ..\l\

l_somk   = makefile
v_somk   = makefile
u_somk   = makefile
lmod_somk = makefile_m

CFLAGS = $(STDC98) $(WALL) $(WEXTRA) $(WNO_UNUSED_PARAMETER) $(DEBUG_MODE)

&(bin_name):libl5.lib liblmod.lib $(obj_list)
	$(LINK) $(obj_list) $(L)$(libl_path) libl5.lib $(L)$(libv_path) libv.lib $(L)$(libu_path) libu.lib $(OUT)$(bin_name)
.c.obj:
	$(CC) $(I) $(libl_path) $(I) $(libv_path) $(I) $(libu_path) $(C) $(COUT)$@ $< $(CFLAGS)
libl5.lib:
	cd $(libl_path)
	nmake /f $(l_somk)
	cd $(currdir)
liblmod.lib:
	cd $(liblmod_path)
	nmake /f $(lmod_somk)
	cd $(currdir)
install:
	copy $(bin_name) $(prefix)
	copy $(conf_name) $(prefix)
	copy $(entry_exec) $(prefix)
	cd $(libl_path)
	nmake /f $(l_somk) install
	cd $(currdir)
	cd $(libv_path)
	nmake /f $(v_somk) install
	cd $(currdir)
	cd $(libu_path)
	nmake /f $(u_somk) install
	cd $(currdir)
	cd $(liblmod_path)
	nmake /f $(lmod_somk) install
	cd $(currdir)
uninstall:
	del $(prefix)\$(bin_name)
	del $(prefix)\$(conf_name)
	del $(prefix)\$(entry_exec)
	cd $(libl_path)
	nmake /f $(l_somk) uninstall
	cd $(currdir)
	cd $(libv_path)
	nmake /f $(v_somk) uninstall
	cd $(currdir)
	cd $(libu_path)
	nmake /f $(u_somk) uninstall
	cd $(currdir)
	cd $(liblmod_path)
	nmake /f $(lmod_somk) uninstall
	cd $(currdir)
clean:
	cd $(libl_path)
	nmake /f $(l_somk) clean
	cd $(currdir)
	cd $(liblmod_path)
	nmake /f $(lmod_somk) clean
	cd $(currdir)
	del $(bin_name) $(obj_list)
