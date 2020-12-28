bin=test_hstb
obj=test_hstb.o \
	ustring.o \
	ustring_table.o \
	uhstb.o

$(bin):$(obj)
	$(CC) $(obj) -o $(bin)
.c.o:
	$(CC) -std=c89 -Wall -c -o $@ $<
clean:
	rm -f $(bin) $(obj)
