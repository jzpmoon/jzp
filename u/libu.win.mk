lib_name = libu$(suf_so)
obj_list = ulist.obj         \
           ustack.obj        \
           ustring.obj       \
           ustring_table.obj \
           uerror.obj        \
           ubuffer.obj       \
           udbuffer.obj	     \
           ustream.obj       \
           uhstb.obj         \
           umempool.obj      \
           ualloc.obj        \
           ugraph.obj        \
           ulr.obj           \
           ulib.obj          \
           utime.obj

CFLAGS = $(STDC98) $(WALL) $(WEXTRA) $(WNO_UNUSED_PARAMETER) $(DEBUG_MODE)

$(lib_name):$(obj_list)
	$(LINK) $(obj_list) $(OUT)$(lib_name) $(SHARED)
.c.obj:
	$(CC) $(C) $< $(COUT)$@ $(CFLAGS) $(FPIC) $(D)_EXPORT_LIBU_
install:
	copy $(lib_name) $(prefix)
uninstall:
	del $(prefix)\$(lib_name)
clean:
	del $(lib_name) $(obj_list) *.lib *.exp
