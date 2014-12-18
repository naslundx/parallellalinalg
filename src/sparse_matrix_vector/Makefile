#Makefile for sparse matrix vector

all: host

host: kernel
	gcc -o spmv spmvhost.c spmvkernel.o -I/usr/local/browndeer/include -L/usr/local/browndeer/lib -lstdcl -locl

kernel: spmvkernel.cl
	clcc -c -k spmvkernel.cl

clean:
	rm -rf spmv spmvkernel.o