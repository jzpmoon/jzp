bin=test_list
obj=test_list.o ulist.o

$(bin):$(obj)
	$(CC) $(obj) -o $(bin)
.c.o:
	$(CC) -std=c89 -Wall -c -o $@ $<
clean:
	rm -f $(bin) $(obj)
