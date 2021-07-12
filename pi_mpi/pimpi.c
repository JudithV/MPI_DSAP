#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

//Judith Vilella Cantos

double reglaRectangulo(double punto){
	return (4/(1+(punto*punto)));
}

int main(int argc, char **argv){
	int rango, tam, n, tamparcial, indice, k;
	MPI_Status estado;
	double start_paral, start_seq, end_paral, end_seq, h, x, pi = 0.0, pi_paral = 0.0, pi_seq = 0.0, error, speedup, eficiencia;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rango);
	MPI_Comm_size(MPI_COMM_WORLD, &tam);

	if(rango == 0){
		printf("Introduzca un número n de intervalos a tener en cuenta para el cálculo de pi.\n");
		scanf("%d", &n);
		h = (double)1/n;
		for(int i = 1; i < tam; i++){
			MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&h, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
		}
	}
	else{
		MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &estado);
		MPI_Recv(&h, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &estado);

	}
	start_paral = MPI_Wtime();

	if(rango == 0){
		k = n/tam;
		tamparcial = k + n%tam;
		indice = tamparcial;
		for(int j = 1; j < tam; j++){
			MPI_Send(&indice, 1, MPI_INT, j, 2, MPI_COMM_WORLD);
			indice += k;
		}
		for(int i = 1; i <= tamparcial; i++){
			x=(i-1)*h;
			pi += h *((reglaRectangulo(x) +reglaRectangulo(x+h))/2);
		}
	}else{
		MPI_Recv(&indice, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &estado);
		tamparcial = (n/tam);
		for(int i = indice; i <= (indice+tamparcial); i++){
			x=(i-1)*h;
			pi += h *((reglaRectangulo(x) +reglaRectangulo(x+h))/2);
		}
	}
	MPI_Reduce(&pi, &pi_paral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	end_paral = MPI_Wtime();
	if(rango == 0){
		start_seq = MPI_Wtime();
		for(int i = 1; i <= n; i++){
			x=(i-1)*h;
			pi_seq += h *((reglaRectangulo(x) +reglaRectangulo(x+h))/2);
		}
		end_seq = MPI_Wtime();
		printf("El valor obtenido para pi en paralelo ha sido: %f\n", pi_paral);
		printf("El tiempo requerido para obtener el valor de pi en paralelo ha sido: %f\n", end_paral - start_paral);
		error = fabs(pi_paral - 3.141592653589793238462643);
		printf("El error para esta aproximación ha sido: %f \n\n", error);

		printf("El valor obtenido para pi en secuencial ha sido: %f\n", pi_seq);
		printf("El tiempo requerido para obtener el valor de pi en secuencial ha sido: %f\n", end_seq - start_seq);
		error = fabs(pi_seq - 3.141592653589793238462643);
		printf("El error para esta aproximación ha sido: %f \n\n", error);
		speedup = (end_seq - start_seq)/(end_paral - start_paral);
		eficiencia = speedup / tam;
		printf("El speed-up es %f y la eficiencia %f\n", speedup, eficiencia);
	}
	MPI_Finalize();
}