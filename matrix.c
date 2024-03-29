#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int mat_mul(int** src1, int** src2, int** dst, int len){
	int i, j, k;

	for(i=0; i<len; i++){
		for(j=0; j<len; j++){
			dst[i][j] = 0;
			for(k=0; k<len; k++){
				dst[i][j] += src1[i][k]*src2[k][j];
			}
		}
	}
	return 0;
}

typedef struct{
	int i;
	int** src1;
	int** src2;
	int** dst;
	int len;
}matmul_arg_t;

//int mat_mul_th_kernel(int i, int** src1, int**src2, int** dst, int len){
void *mat_mul_th_kernel(void *arg){
	int j, k, result;
	matmul_arg_t *parg = (matmul_arg_t*)arg;
	int i = parg->i;
	int **src1 = parg->src1;
	int **src2 = parg->src2;
	int **dst = parg->dst;
	int len = parg->len;
	
	//printf("thread %d is started\n", i);
	for(j=0; j<len; j++){
		result = 0;
		for(k=0; k<len; k++){
			result += src1[i][k]*src2[k][j];
		}
		dst[i][j] = result;
	}
}

int mat_mul_th(int** src1, int** src2, int** dst, int len){
	int				i, j, k, res;
	matmul_arg_t 	*arg;
	pthread_t 		*a_thread;
	void 			*thread_result;
	
	a_thread = (pthread_t*)malloc(sizeof(pthread_t)*len);

	for(i=0; i<len; i++){
		arg = (matmul_arg_t*)malloc(sizeof(matmul_arg_t));
		arg->i = i;
		arg->src1 = src1;
		arg->src2 = src2;
		arg->dst = dst;
		arg->len = len;
		//mat_mul_th_kernel((void*)&arg);
		//printf("thread %d is called\n", i);
		res = pthread_create(a_thread+i, NULL, mat_mul_th_kernel, (void*)arg);
		if( res!=0 ){
			perror("Thread creation failed");
			exit(EXIT_FAILURE);
		}
	}

	for(i=0; i>len; i++){
		res = pthread_join(a_thread[i], &thread_result);
		if( res!=0 ){
			perror("Thread join failed");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
