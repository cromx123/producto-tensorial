/***************************************************************************************
 * 
 * tensproductmatrix.c:  
 *
 * Programmer: Cristobal Gallardo Cubillos
 *
 * Santiago de Chile, 10/10/2023
 *
 **************************************************************************************/


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

float **Matrix1,**Matrix2,**Resultado;

typedef struct {
    int id; // ID del hilo
    int num_threads; // Número total de hilos
    int partition_type; // Tipo de particion V o H
    int m, k, n;
    float **Matrix1, **Matrix2, **resultado;
} ThreadArgs;

void Usage(char *mess) {
    
    printf("\nUsage: %s K -P -O < data.txt\n",mess);
    printf("K = numero de threads\n");
    printf("P = {V: particion vertical, H: particion horizontal}\n");
    printf("O = {S (Silent), V (Verbose)}\n\n");
}

float **FillMatrix(unsigned int r, unsigned int c) {
    unsigned int i, j;
    float **mat;

    mat = calloc(r, sizeof(float *));
    for (i = 0; i < r; i = i + 1)
        mat[i] = calloc(c, sizeof(float));
    for (i = 0; i < r; i = i + 1)
        for (j = 0; j < c; j = j + 1)
            scanf("%f", &mat[i][j]);
    return mat;
}
//Funcion que realiza el producto tensorial secuencial
float **multi(unsigned int m,unsigned int k,unsigned int n, float **Matrix1,float **Matrix2,float **resultado){

    unsigned int id, i, j, l, p;

    // Realiza el producto tensorial las matrices
    for ( i = 0; i < m; i++) {
        for ( j = 0; j < k; j++) {
            for ( l = 0; l < k; l++) {
                for ( p = 0; p < n; p++) {
                    resultado[i * k + j][l * n + p] = Matrix1[i][l] * Matrix2[j][p];
                }
            }
        }
    }
    return resultado;
}
// Función que realiza el producto tensorial en un hilo
void* tensorial_product(void* args) {
    ThreadArgs* targs = (ThreadArgs*)args;
    int id = targs->id;
    int num_threads = targs->num_threads;
    int partition_type = targs->partition_type;
    int m = targs-> m;
    int k = targs->k;
    int n = targs->n;
    float **Matrix1 = targs->Matrix1;
    float **Matrix2 = targs->Matrix2;
    float **resultado = targs->resultado;

    unsigned i,j,l,p;
    if ( partition_type == 0) { // Partición horizontal
        for (i = id; i < m; i += num_threads) {
            for (j = 0; j < k; j++) {
                for (l = 0; l < k; l++) {
                    for (p = 0; p < n; p++) {
                        resultado[i * k + j][l * n + p] = Matrix1[i][l] * Matrix2[j][p];
                    }
                }
            }
        }
    } else if ( partition_type == 1) { // Partición vertical
        for (j = id; j < k; j += num_threads) {
            for (i = 0; i < m; i++) {
                for (l = 0; l < k; l++) {
                    for (p = 0; p < n; p++) {
                        resultado[i * k + j][l * n + p] = Matrix1[i][l] * Matrix2[j][p];
                    }
                }
            }
        }
    }

    pthread_exit(NULL);
}
void PrintMatrix(unsigned int r, unsigned int c, float **mat) {
    
   unsigned int i, j;
   
   for (i = 0; i < r; i = i + 1) {
      for (j = 0; j < c; j = j + 1)
         printf(" %.2f ",mat[i][j]);
      printf("\n");
   }
}
void FreeMatrix(unsigned int r, float **mat) {

   unsigned int i;
   
   for (i = 0; i < r; i = i + 1)
      free(mat[i]);
   free(mat);
}




int main(int argc, char **argv) {
    
    //definicion de Matrices
    
    int m,k,n,num_threads;
    int tipo;
    sscanf(argv[1], "%d", &num_threads);
    if (strcmp(argv[2],"-V") == 0) {
            tipo=0;
    }else if (strcmp(argv[2],"-H") == 0){
        tipo = 1;
    }else{
        argc = 3;
    }
    pthread_t threads[num_threads];
    ThreadArgs thread_args[num_threads];

    //definicion Variables para calcular el tiempo
    float  E_cpu;
    long E_wall;
    time_t  ts, te;
    clock_t cs, ce; 

    if(argc == 4){
        // Leer las dimensiones de la matriz
        scanf("%d",&m); //filas A
        scanf("%d",&k); //columnas A y filas B
        scanf("%d",&n); //columnas B

        // Leer e inicializar las matrices
        Matrix1= FillMatrix(m,k);
        Matrix2= FillMatrix(k,n);
        int mk= m*k, kn= k*n,i;

        Resultado = (float **)calloc(mk, sizeof(float *));
        for (i = 0; i < mk; i++) {
            Resultado[i] = (float *)calloc(kn, sizeof(float));
        }
        
        // Muestra las matrices si se desean ver
        if (strcmp(argv[3],"-V") == 0) {
            printf(" Matriz A(%d,%d):\n\n", m,k);
            PrintMatrix(m,k,Matrix1);
            printf("\n");
            printf(" Matriz B(%d,%d):\n\n", k,n);
            PrintMatrix(k,n,Matrix2);
        }
        cs = clock();
        ts = time(NULL);

        //multiplica las matrices
        
        //Creación de threads
        for ( i = 0; i < num_threads; i++) {
            thread_args[i].id = i;
            thread_args[i].num_threads = num_threads;
            thread_args[i].m = m;
            thread_args[i].k = k;
            thread_args[i].n = n;
            thread_args[i].Matrix1 = Matrix1;
            thread_args[i].Matrix2 = Matrix2;
            thread_args[i].resultado = Resultado;
            thread_args[i].partition_type = tipo;
            pthread_create(&threads[i], NULL, tensorial_product, (void*)&thread_args[i]);
        }
        //Espera el termino de los threads
        for (i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
            //printf("Hilo %d terminado\n", i);
        }
        ce = clock();
        te = time(NULL);
        E_wall = (long)(te - ts);
        E_cpu = (float)(ce - cs) / CLOCKS_PER_SEC; 

        if (strcmp(argv[3],"-V") == 0) {
            //Muestra la matriz resultante
            printf("\n Matriz Resultado:\n\n");
            PrintMatrix(mk,kn,Resultado);
        }

        printf("\nm = %d k = %d n = %d Elapsed Time CPU Time %f Wall Time %ld\n",m,k,n,E_cpu,E_wall);
        printf("num_threads = %d \n\n",num_threads);
        //Limpia la memoria utilizada para las matrices
        FreeMatrix(m,Matrix1);
        FreeMatrix(k,Matrix2);
        FreeMatrix(mk,Resultado);
    }else
        Usage(argv[0]);
    return 0;
}