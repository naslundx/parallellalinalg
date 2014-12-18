#Makefile for sparse matrix matrix

all: host

host: kernel
	gcc -o spmm spmmhost.c spmmkernel.o -I/usr/local/browndeer/include -L/usr/local/browndeer/lib -lstdcl -locl

kernel: spmmkernel.cl
	clcc -c -k spmmkernel.cl

clean:
	rm -rf spmm spmmkernel.o