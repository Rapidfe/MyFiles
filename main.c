#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "matrix.h"

void matinit(int***, int***, int***, int);
void print_matrix(int**, char*, int);
double wtime();

int main(int argc, char** argv){
	int **a;
	int **b;
	int **c;
	int i,j,k;
	int len;
	double start, stop;

	if( argc==2 ){
		len = atoi(argv[1]);
	}else{
		len = 10;
	}

	matinit(&a, &b, &c, len);

	//int tmp = a[0][0];
	start = wtime();
	if( mat_mul(a,b,c,len) != 0 ){
		fprintf(stderr, "Failed to Matrix multiplication\n");
		exit(1);
	}
	stop = wtime();
	//print_matrix(a,"A",len);
	//print_matrix(b,"B",len);
	print_matrix(c,"C",len);
	double timesav = stop - start;
	//printf("Processing time: %f\n", stop - start);

	start = wtime();
	if( mat_mul_th(a,b,c,len) != 0 ){
		fprintf(stderr, "Failed to Matrix multiplication\n");
		exit(1);
	}
	stop = wtime();
	/*while( a[0][0]!=-1 ){
		//printf("nope");
	}
	a[0][0] = tmp;*/
	//print_matrix(a,"A",len);
	//print_matrix(b,"B",len);
	print_matrix(c,"C",len);
	printf("processing time(thread x): %f\n", timesav);
	printf("Processing time(thread o): %f\n", stop - start);

	return 0;
}

void matinit(int ***p_a, int ***p_b, int ***p_c, int len){
	int **a;
	int **b;
	int **c;
	int i, j;

	a = (int**)malloc(len*sizeof(int*));
	b = (int**)malloc(len*sizeof(int*));
	c = (int**)malloc(len*sizeof(int*));
	
	for(i=0; i<len; i++){
		a[i] = (int*)malloc(len*sizeof(int));
		b[i] = (int*)malloc(len*sizeof(int));
		c[i] = (int*)malloc(len*sizeof(int));
	}

	srandom((unsigned int)time(NULL));
	for(i=0; i<len; i++){
		for(j=0; j<len; j++){
			a[i][j] = random() % 10;
			b[i][j] = random() % 10;
			c[i][j] = 0;
		}
	}

	*p_a = a;
	*p_b = b;
	*p_c = c;
}

double wtime(){
	static int sec = -1;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	if( sec<0 ) sec = tv.tv_sec;

	return (tv.tv_sec - sec)+1.0e-6*tv.tv_usec;
}

void print_matrix(int** matrix, char* name, int len){
	printf("== %s matrix =================\n", name);
	for(int i=0; i<len; i++){
		for(int j=0; j<len; j++){
			printf("%d ",matrix[i][j]);
		}
		printf("\n");
	}
}
