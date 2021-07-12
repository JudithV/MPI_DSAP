compile: psdotmpi.c
	mpicc -o psdotmpi psdotmpi.c

run: psdotmpi
	mpirun -np 4 psdotmpi

clean: 
	rm psdotmpi