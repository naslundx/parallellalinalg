#Makefile for dense matrix matrix

all: host

host: kernel
	gcc -o dmm dmmhost.c dmmkernel.o -I/usr/local/browndeer/include -L/usr/local/browndeer/lib -lstdcl -locl

kernel: dmmkernel.cl
	clcc -c -k dmmkernel.cl

clean:
	rm -rf dmm dmmkernel.o