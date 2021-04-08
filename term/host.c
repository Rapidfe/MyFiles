#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "game.h"
#include "webcall.c"
#include "sensor.c"


#define MAX_CLIENT 100
#define BUF_SIZE 200
#define MAX_IP 30
#define ID_SIZE 10

int client_cnt=0;
int client_n=0;
int client_socks[MAX_CLIENT];
pthread_mutex_t mutx;
char ipstr[20];

void* handle_clnt(void*);
void send_msg(char*, int);
void error_handling(char*);
char* severState(int);
void menu(int);

int isGame = 0;
int turn;
char* ids[MAX_CLIENT];
int wait_t=0;
int wait_s=0;

typedef struct{
	int sock;
	int num;
	int index;
}give_thread;

void* waiting(){
	while(1)
		if( wait_t ){
			fruit(wait_t%10, wait_t/10);
			wait_t = 0;
		}
}
void* switch_wait(){
	while(1)
		if( wait_s ){
			usleep(2000000); // 2sec
			wait_s = 0;
		}
}

int main(int argc, char *argv[]){
	
	if( argc!=2 ){
		printf("Please input [IP address]\n");
		exit(0);
	}
	sprintf(ipstr, "%s", argv[1]);
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	pthread_t a_thread;
	pthread_t wait_th;
	pthread_t swit_th;
	give_thread th;

	//server socket ready 

	menu(9734);

	fruit(5,0);

	unlink("server_sockt");
	pthread_mutex_init(&mutx, NULL);
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(9734);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

	listen(server_sockfd, 5);

	chatcacheinit();

	printf("serversocket is ready\n");
	
	FILE* chatcache = fopen("chatcache.txt", "w");
	fprintf(chatcache, "server: %s\n\n", ipstr);
	fclose(chatcache);
	webcall(client_cnt, ids);
	
	pthread_create(&wait_th, NULL, waiting, NULL);
	pthread_create(&swit_th, NULL, switch_wait, NULL);
	pthread_detach(wait_th);
	pthread_detach(swit_th);
	while(1){
		client_len = sizeof(client_address);
		client_sockfd=accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);


		if( isGame ){
			write(client_sockfd, "x", 1);
			close(client_sockfd);
			continue;
		}
		write(client_sockfd, "y", 1);
		pthread_mutex_lock(&mutx);
		client_socks[client_cnt]=client_sockfd;
		th.index = client_cnt++;
		th.num = ++client_n;
		pthread_mutex_unlock(&mutx);
		th.sock = client_sockfd;
		pthread_create(&a_thread, NULL, handle_clnt, (void*) &th);
		pthread_detach(a_thread);
		printf("Connected chatter #%d IP : %s\n", client_cnt, inet_ntoa(client_address.sin_addr));
	}

	close(server_sockfd);
	return 0;
}


void *handle_clnt(void *arg){

	give_thread th = *(give_thread*) arg;
	int myNum = th.num;
	int client_sockfd = th.sock;
	char myid[ID_SIZE];
	int str_len=0, i, j;
	char msg[BUF_SIZE];

	// send id
	sprintf(myid, "%dplayer", myNum);
	ids[th.index] = myid;
	write(client_sockfd, myid, strlen(myid)*sizeof(char));
	read(client_sockfd, msg, 1);

	// joining messages
	write(client_sockfd, ">> join the chat !! \n", 21);
	sprintf(msg, ">> You are %s\n", myid);
	write(client_sockfd, msg, strlen(msg)*sizeof(char));
	sprintf(msg, "** %s is joined! **\n", myid);
	pthread_mutex_lock(&mutx);
	for(i=0; i<client_cnt; i++)
		write(client_socks[i], msg, strlen(msg)*sizeof(char));
	pthread_mutex_unlock(&mutx);
	if( client_socks[0]==client_sockfd ){
		write(client_sockfd, "** You are a MASTER **\n", 23);
	}	

	chatcachewrite(0, 0);
	webcall(client_cnt, ids);
	// repeat
	while( (str_len=read(client_sockfd , msg, BUF_SIZE))!=0 ){
		// during game
		pthread_mutex_lock(&mutx);
		if( isGame ){
			during_game(client_sockfd, msg);
			pthread_mutex_unlock(&mutx);
			continue;
		}
		pthread_mutex_unlock(&mutx);

		// start game
		if( msg[0]==':' ){
			if( msg[1]=='s' )
				make_start(client_sockfd);
			else if( msg[1]=='c' ){
				pthread_mutex_lock(&mutx);
				for(i=0; i<client_cnt; i++){
					if( client_socks[i]==client_sockfd ){
						printf("%s -> ", ids[i]);
						msg[str_len] = '\0';
						sprintf(myid, "%s", msg+2);
						break;
					}
				}
				pthread_mutex_unlock(&mutx);
				printf("%s\n", ids[i]);
			}
		}else
			send_msg(msg, str_len);
	}

	char masterM[35];
	//remove disconnected client
	pthread_mutex_lock(&mutx);
	for(i=0; i<client_cnt; i++){
		if( client_socks[i]==client_sockfd ){
			j = i;
			break;
		}
	}
	for(; i<client_cnt; i++){ 
		client_socks[i]=client_socks[i+1];
		ids[i] = ids[i+1];
	}
	client_cnt--;
	if( client_cnt>0 ){ 
		if( isGame ){
			sprintf(msg, "** %s leave the chat **\n:e", myid);
			sprintf(masterM, "** %s leave the chat **\n:E", myid);
		}else{
			sprintf(msg, "** %s leave the chat **\n", myid);
			sprintf(masterM, "** You are a MASTER **\n");
		}
		for(i=0; i<client_cnt; i++){
			if( isGame){
				if( j==0 && i==0 )
					write(client_socks[i], masterM, strlen(masterM)*sizeof(char));
				else
					write(client_socks[i], msg, strlen(msg)*sizeof(char));
			}else{
				write(client_socks[i], msg, strlen(msg)*sizeof(char));
				if( j==0 && i==0 )
					write(client_socks[i], masterM, strlen(masterM)*sizeof(char));
			}
		}
	}
	if( isGame){
		isGame = 0;
		printf("<HALLIGALLI end>\n");
	}
	pthread_mutex_unlock(&mutx);
	close(client_sockfd);

	return NULL;

}


void send_msg(char*msg, int len){

	int i;
	chatcachewrite(msg, len);
	webcall(client_cnt, ids);

	pthread_mutex_lock(&mutx);
	for(i=0; i<client_cnt ; i++){
		write(client_socks[i], msg, len);
	}
	pthread_mutex_unlock(&mutx);

}

void error_handling(char *msg){
	
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);

}

char* serverState(int count){

	char* stateMsg = malloc (sizeof(char) *20);

	strcpy(stateMsg, "None");

	if(count<5)
		strcpy(stateMsg, "Good");
	else
		strcpy(stateMsg, "Bad");

	return stateMsg;
}

void menu(int port){

	system("clear");
	printf("* embedded programming chat server *\n");
	printf("server IP : %s\n", ipstr);
	printf("server port : %d\n", port);
	printf("server state: %s\n", serverState(client_cnt));
	printf("max connection : %d\n", MAX_CLIENT);

}


