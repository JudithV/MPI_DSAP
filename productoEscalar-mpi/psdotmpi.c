#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define maxnprocs 8
#define maxn 100000000
// Práctica 2 DSAP: Judith Vilella Cantos
int main(int argc, char **argv){
	double *x, *y;
	double productoescalar = 0, subproductoescalar = 0;
	int rango, tam, tamarray, j, ln, indice;
	MPI_Status estado;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rango);
	MPI_Comm_size(MPI_COMM_WORLD, &tam);
	if(rango == 0){
		printf("Introduce el tamaño del array\n");
		scanf("%d", &tamarray);
		for(int i = 1; i < tam; i++){
			MPI_Send(&tamarray, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}else{
		MPI_Recv(&tamarray, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &estado);
	}


	if(tamarray > maxn){
		if(rango == 0)
			printf("Introduce un tamaño de array menor que 100000000, por favor.\n");
		MPI_Finalize();
		return 0;
	}
	else{
		if(rango == 0){
			x = malloc(tamarray * sizeof(double));
			y = malloc(tamarray * sizeof(double));
			for(int i = 0; i < tamarray; i++){ // inicializo los dos vectores
				y[i] = i + 1;
				x[i] = (double)1/(i+1);
			}
			j = tamarray/tam; // definimos el tamaño de cada parte, en función al tamaño de array introducido y el total de procesos
			ln = j + tamarray%tam; // este será el tamaño de la parte del padre, el cuál contendrá los sobrantes. Los hijos tendrán tamaño equitativo
			indice = ln;
			for(int i = 1; i < tam; i++){
				MPI_Send(&x[indice], j, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
				MPI_Send(&y[indice], j, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
				indice += j;
			}
			for(int i = 0; i < ln; i++){
				subproductoescalar += x[i]*y[i];
			}
		}
		else{
				ln = tamarray/tam;
				x = malloc(ln * sizeof(double));
				y = malloc(ln * sizeof(double));
				MPI_Recv(x, ln, MPI_DOUBLE, 0, rango, MPI_COMM_WORLD, &estado);
				MPI_Recv(&y[0], ln, MPI_DOUBLE, 0, rango, MPI_COMM_WORLD, &estado);
				for(int i = 0; i < ln; i++){
					subproductoescalar += x[i]*y[i];
				}

		}
		MPI_Reduce(&subproductoescalar, &productoescalar, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	}
	
	if(rango == 0){
		if(tamarray > maxn){
			printf("Introduce un tamaño de array menor que 100000000, por favor.\n");
		}else{
			printf("Soy el proceso %d, el resultado del producto escalar es %f\n", rango, productoescalar);
		}
		
	}
	free(x);
	free(y);
	MPI_Finalize();

	return 0;
}