#include <stdio.h>
#include <mpi.h>
// Práctica 1 DSAP: Judith Vilella Cantos
int main(int argc, char **argv){
	int rango, tam, entero = 0;

	MPI_Status estado;
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rango);
	MPI_Comm_size(MPI_COMM_WORLD, &tam);
	if(rango != 0){
		MPI_Recv(&entero, 1, MPI_INT, (rango-1), 2, MPI_COMM_WORLD, &estado);
		printf("Soy el proceso %d. El entero que he recibido es: %d\n", rango, entero);
	}else{
		printf("Introduce un entero para transmitir:\n");
		scanf("%d", &entero);
		printf("El entero introducido es %d\n", entero);
		printf("Soy el proceso %d. El entero que he recibido es: %d\n", rango, entero);
	}
	if(rango < (tam - 1)){
		entero++;
		MPI_Send(&entero, 1, MPI_INT, (rango+1), 2, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}