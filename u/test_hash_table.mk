bin=test_hash_table
obj=test_hash_table.o \
	uhash_table.o \
	ustring.o

$(bin):$(obj)
	$(CC) $(obj) -o $(bin)
.c.o:
	$(CC) -std=c89 -Wall -c -o $@ $<
clear:
	rm -f $(bin) $(obj)
