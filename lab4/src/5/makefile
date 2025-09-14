CC=gcc
CFLAGS=-I.


psum: psum.o lib/libsum.a utils.o
	$(CC)   -o psum psum.o utils.o -Llib -lsum $(CFLAGS)

psum.o :  utils.h parallel_sum.c 
	$(CC) -c -o psum.o parallel_sum.c $(CFLAGS)


lib/libsum.a: lib/sum_lib.o
	ar rcs lib/libsum.a lib/sum_lib.o

lib/sum_lib.o: lib/sum_lib.c lib/sum_lib.h
	$(CC) -c -o lib/sum_lib.o lib/sum_lib.c $(CFLAGS)




utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)


clean :
	rm utils.o psum psum.o lib/sum_lib.o lib/libsum.a
