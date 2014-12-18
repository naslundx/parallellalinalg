#Makefile for dense matrix vector

all: host

host: kernel
	gcc -o dmv dmvhost.c dmvkernel.o -I/usr/local/browndeer/include -L/usr/local/browndeer/lib -lstdcl -locl

kernel: dmvkernel.cl
	clcc -c -k dmvkernel.cl

clean:
	rm -rf dmv dmvkernel.o