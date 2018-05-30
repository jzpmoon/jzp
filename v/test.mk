
test:test.c
	$(cc) -o test.o test.c -I ../u/
clean:
	rm *.o test
