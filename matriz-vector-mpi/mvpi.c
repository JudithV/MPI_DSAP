#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
//Judith Vilella Cantos
#define maxm 500 // maximo filas
#define maxn 500 // maximo columnas
#define maxnumprocs 8


/* Allocate un puntero a un array 2d */
double **allocate_array(int row_dim, int col_dim){
    double **result;
    int i;
    /* Necesitamos ir con cuidado: el array debe ser asignado a un trozo contiguo de memoria, para que MPI pueda distribuirlo correctamente. */
    result=(double **)malloc(row_dim*sizeof(double *));
    result[0]=(double *)malloc(row_dim*col_dim*sizeof(double));
    for(i=1; i<row_dim; i++)
         result[i]=result[i-1]+col_dim;
    return result;
 }

/* Desasignamos un puntero a un array 2d*/
void deallocate_array(double **array, int row_dim) {
   int i;
   for(i=1; i<row_dim; i++)
       array[i]=NULL;
   free(array[0]);
   free(array);
}

void matvec(double **mat, double *vec, double *res, int row, int col){
	for(int i = 0; i < row; i++){
		res[i] = 0;
		for(int j = 0; j < col; j++){
			res[i] = res[i] + (mat[i][j]*vec[j]);
		}
	}

}

void vermatriz(double **a, int m, int n, char nombre[])
{
 int i,j;
 printf("%4s =",nombre);
 printf("%6d ",0);
 for (j=1;j<n;j++){
     printf("%7d ",j);
 }
 printf("\n");
 for (i=0;i<m;i++){
     printf("%8d:",i);
     for (j=0;j<n;j++){
         printf("%7.3f ",a[i][j]);
     }
     printf("\n");
 }
}

void vervector(double *a, int m, char nombre[])
{
 int i;
 printf("%6s = ",nombre);
 for (i=0;i<m;i++){
     printf("%7.3f ",a[i]);
     if ((i+1)%5  == 0) printf("\n         ");
 }
 printf("\n");
}


int main(int argc, char **argv){
	int rango, tam, m = 0, n = 0, root = 0, slice, resto, lm = 0;
	MPI_Status estado;
	int auxdespl = 0;
	double aux = 0;
	double **matriz;
	double *vector, *y;
	int *recvcount, *despl;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rango);
	MPI_Comm_size(MPI_COMM_WORLD, &tam);

	if(rango == root){
		printf("Número de filas (1-500):\n");
		scanf("%d", &m);
		if(m > maxm){
			printf("Número de filas no válido, debe ser menor que 500.\n");
		}
		printf("Número de columnas (1-500):\n");
		scanf("%d", &n);
			for(int i = 1; i < tam; i++){
				MPI_Send(&m, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&n, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			}

		if(n > maxn){
			printf("Número de columnas no válido, debe ser menor que 500.\n");				
		}
	}
	else{

		MPI_Recv(&m, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &estado);
		MPI_Recv(&n, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &estado);

	}
	if(m > maxm || n > maxn){
		MPI_Finalize();
		return 0;
	}
	slice = m / tam;
	if(rango == 0){
		matriz = allocate_array(m,n); // reservar memoria matriz
		vector = malloc(n*sizeof(double));// reservar memoria vector
		resto = m % tam;
		lm = slice + resto;
		for(int i = 0; i < m; i++){
			for(int j = 0; j < n; j++){ // inicializar matriz
				matriz[i][j] = (double)(i+j);
			}
		}
		for(int k = 0; k < n; k++){ //inicializar vector
			vector[k] = k;
		}
		for(int a = 1; a < tam; a++){
			MPI_Send(&vector[0], n, MPI_DOUBLE, a, 2, MPI_COMM_WORLD);
		}
		vervector(vector,n,"x");
		vermatriz(matriz,m,n,"A");

	}
	else{
		resto = 0;
		lm = slice + resto;
		matriz = allocate_array(lm, n);
		vector = malloc(n*sizeof(double));
        //printf("Soy el proceso %d de un total de %d.\n", rango, tam);
        MPI_Recv(&vector[0], n, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &estado);

        //vervector(vector,n,"x");

	}
	if(rango != root){
		MPI_Scatter(&matriz[resto][0], slice*n, MPI_DOUBLE, &matriz[resto][0], slice*n, MPI_DOUBLE, root, MPI_COMM_WORLD);

	}else{
		MPI_Scatter(&matriz[resto][0], slice*n, MPI_DOUBLE, MPI_IN_PLACE, slice*n, MPI_DOUBLE, root, MPI_COMM_WORLD);

	}
	if(rango != root){ //multiplicacion y almacenar en y
		printf("Soy el proceso %d de un total de %d.\n", rango, tam);
		vermatriz(matriz,lm,n,"A");
		y = malloc(lm * sizeof(double));
		matvec(matriz, vector, y, lm, n);
		vervector(y,lm, "y");

	}
	
	y = malloc(n * sizeof(double));
	matvec(matriz, vector, y, lm, n);
	/*if(rango == 0){
		recvcount = malloc(tam*sizeof(int));
		for(int i = 0; i < tam; i++){
			if(i == 0){
				recvcount[i] = lm;
			}else{
				resto = 0;
				lm = slice + resto;
				recvcount[i] = lm;
			}
			
		}
		resto = m % tam;
		lm = slice + resto;
		auxdespl = lm;
		for(int j = 0; j < tam; j++){
			despl[i] = auxdespl;
			auxdespl += slice;
		}
		for(int a = 1; a < tam; a++){
			MPI_Send(&recvcount[0], tam, MPI_INT, a, 3, MPI_COMM_WORLD);
			MPI_Send(&despl[0], tam, MPI_INT, a, 4, MPI_COMM_WORLD);
		}

	}else{
		MPI_Recv(&recvcount[0], tam, MPI_INT, 0, 3, MPI_COMM_WORLD, &estado);
		MPI_Recv(&despl[0], tam, MPI_INT, 0, 4, MPI_COMM_WORLD, &estado);
	}
	MPI_Gatherv(y, recvcount[rango], MPI_DOUBLE, y, recvcount, despl, MPI_DOUBLE, 0, MPI_COMM_WORLD);*/
	if(rango != root){
		MPI_Gather(&y[resto], slice, MPI_DOUBLE, &y[resto], slice, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}else{
		MPI_Gather(MPI_IN_PLACE, slice, MPI_DOUBLE, &y[resto], slice, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}
	if(rango == 0){
		vervector(y, n, "A*x");
	}
	free(vector);
	free(matriz);
	free(y);
	MPI_Finalize();
	return 0;
}





