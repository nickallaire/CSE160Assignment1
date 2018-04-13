# Sample Makefile for CS160
CC = cc

default: mpi-ring2 

svalidate.o : svalidate.c svalidate.h
	$(CC) -c svalidate.c
svtest: svtest.c svalidate.o
	$(CC) -o svtest svtest.c svalidate.o
mpi-ring2 : mpi-ring2.c svalidate.c svalidate.h
	mpicc mpi-ring2.c svalidate.c -lm -o mpi-ring2
clean:
	- /bin/rm svalidate.o
	- /bin/rm svtest 
	- /bin/rm mpi-ring2
