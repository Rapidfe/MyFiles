#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <wiringPi.h>

#define BUF_SIZE 100
#define NORMAL_SIZE 20
#define SWITCH 4
#define LED 1


void* send_msg(void* arg);
void* recv_msg(void* arg);
void* switch_check(void* arg);
void error_handling(char* msg);

void changeName(int);
void menuOptions();


char name[NORMAL_SIZE];
char rawname[NORMAL_SIZE];
int isName = 0;
char msg_form[NORMAL_SIZE];
char serv_time[NORMAL_SIZE];
char msg [BUF_SIZE];
char serv_port [NORMAL_SIZE];
char client_ip[NORMAL_SIZE];
int currentSwitch = 0;
char ipstr[20];

int mode = 0;
pthread_mutex_t mutx;

int main(int argc, char *argv[]){
	if( argc!=2 ){
		printf("Please input [Host IP address]\n");
		exit(0);
	}
	sprintf(ipstr, "%s", argv[1]);
	int sockfd;
	int len;
	struct sockaddr_in server_addr;
	pthread_t snd_thread;
	pthread_t rcv_thread;
	pthread_t switch_thread;
	void* thread_return;
	int result;
	char tmp[5];

	if( wiringPiSetup()==1 ) printf("wiringPiSetup error!\n");
	pinMode(SWITCH, INPUT);
	pinMode(LED, OUTPUT);

	sockfd = socket (AF_INET, SOCK_STREAM, 0);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ipstr);
	server_addr.sin_port = htons (9734);

	len = sizeof(server_addr);

	result = connect(sockfd, (struct sockaddr *) &server_addr , len);

	if(result == -1)
		error_handling("connnect error\n");
	
	menuOptions();
	read(sockfd, tmp, 1);
	if( tmp[0]!='x' ){
		pthread_create(&snd_thread, NULL, send_msg, (void*)&sockfd);
		pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sockfd);
		pthread_create(&switch_thread, NULL, switch_check, (void*)&sockfd);

		pthread_join(snd_thread, &thread_return);
		pthread_join(rcv_thread, &thread_return);
		pthread_join(switch_thread, &thread_return);
	}else
		printf("!BUSY\n");
	close(sockfd);
	return 0;

}


void* send_msg(void* arg){
	
	int sockfd=*((int*)arg);
	char name_msg[NORMAL_SIZE+BUF_SIZE];
	char myInfo[BUF_SIZE];
	char* who = NULL;
	char temp[BUF_SIZE];

	while(1){

		fgets(msg, BUF_SIZE, stdin);
		
		pthread_mutex_lock(&mutx);
		if( mode==1 ){
			pthread_mutex_unlock(&mutx);
			if( !strcmp(msg, ":d\n") ){
				write(sockfd, ":d", 2);
			}else if( !strcmp(msg, "a\n") ){
				write(sockfd, "a", 1);
			}else
				printf("<WRONG COMMAND>\n");
			continue;
		}
		pthread_mutex_unlock(&mutx);
		if(!strcmp(msg, "!menu\n")){
			menuOptions();
			continue;
		}else if(!strcmp(msg, ":q\n")){
			close(sockfd);
			exit(0);
		}else if(!strcmp(msg, ":s\n")){
			write(sockfd, ":s", 2);
			continue;
		}else if(!strcmp(msg, ":c\n")){
			changeName(sockfd);
			continue;
		}

		sprintf(name_msg, "%s: %s", name, msg);
		write(sockfd, name_msg, strlen(name_msg));


	}

	return NULL;

}



void * recv_msg(void* arg){

	int sockfd =*((int*)arg);
	char name_msg[500];
	int str_len, i, j;
	while(!isName){
		str_len = read(sockfd, name_msg, 10);
		if(str_len == -1)
			return (void*) -1;
		if(str_len>0){
			name_msg[str_len]='\0';
			for(i=0; i<str_len; i++) rawname[i] = name_msg[i];
			sprintf(name, "[%s]", name_msg);
			isName = 1;
			write(sockfd, "x", 1);
		}
	}
	
	while(1)
	{
		str_len = read(sockfd, name_msg, 499);
		if(str_len == -1)
			return (void*) -1;
		
		pthread_mutex_lock(&mutx);
		if( name_msg[0]==':' && name_msg[1]=='s' ){
			mode = 1;
			system("clear");
			printf("//---- HOLLIGALLI Game! ----//\n");
			for(i=0; i<3; i++){
				for(j=0; j<(name_msg[2]-'0'); j++)
					printf("   [ ]  ");
				printf("\n");
			}
			pthread_mutex_unlock(&mutx);
			if( name_msg[3]=='s' )
				printf("\n\nNext turn: YOU\n");
			else if( name_msg[3]=='S' ){
				printf("\n\nNext turn: ");
				name_msg[str_len]='\0';
				fputs(name_msg+4, stdout);
			}
			continue;
		}
		if( mode==1 ){
			if( name_msg[0]==':' ){
				if( name_msg[1]=='h' ){
					pthread_mutex_unlock(&mutx);
					system("clear");
					printf("//---- HOLLIGALLI Game! ----//\n");
					for(i=0; i<3; i++){
						for(j=0; j<(name_msg[2]-'0'); j++)
							printf("   [ ]  ");
						printf("\n");
					}
					j = 5;
					if( name_msg[3]=='h' )
						printf("\n\nNext turn: YOU\n");
					else if( name_msg[3]=='H' ){
						printf("\n\nNext turn: ");
						for(; name_msg[j]!=':'; j++)
							printf("%c",name_msg[j]);
						printf("\n");
					}
					if( name_msg[4]=='h' )
						printf("YOU HIT!\n");
					else if( name_msg[4]=='H' ){
						name_msg[str_len]='\0';
						fputs(name_msg+j+1, stdout);
					}
					continue;
				}
			}else if( name_msg[str_len-2]==':' ){
				if( name_msg[str_len-4]=='*' )
					printf("//------   Game End   ------//\n");
				mode = 0;	
				pthread_mutex_unlock(&mutx);
				name_msg[str_len-2]='\0';
				fputs(name_msg, stdout);
				if( name_msg[str_len-1]=='E' )
					printf("** You are a MASTER **\n");
				if( name_msg[str_len-4]!='*' )
					printf("//------   Game End   ------//\n");
				continue;
			}
			if( name_msg[0]==' ' ){
				name_msg[str_len]='\0';
				system("clear");
				printf("//---- HOLLIGALLI Game! ----//\n");
				fputs(name_msg, stdout);
				pthread_mutex_unlock(&mutx);
				continue;
			}
		}
		pthread_mutex_unlock(&mutx);
		
		name_msg[str_len]='\0';
		//printf("%d\n", mode);
		fputs(name_msg, stdout);
		
	}

	return NULL;

}

void* switch_check(void* arg){
	int sockfd = *((int*)arg);

	while( 1 ){
		if( digitalRead(SWITCH)==HIGH ){
			digitalWrite(LED, HIGH);
			if( mode==1 ){
				write(sockfd, "a", 1);
			}
			delay(1000);
		}else{
			digitalWrite(LED,LOW);
		}
	}
}
void menuOptions(){

	int select;

	printf("\n\t┌─────────────── 매뉴얼 ───────────────┐\n");
	printf("\t│        !menu  --  매뉴얼 보기        │\n");
	printf("\t│           :c  --  닉네임 설정        │\n");
	printf("\t│           :q  --  나가기             │\n");
	printf("\t│                                      │\n");
	printf("\t│       ..... 게임 커맨드 .....        │\n");
	printf("\t│           :s  --  할리갈리 시작하기  │\n");
	printf("\t│           :d  --  카드 뒤집기        │\n");
	printf("\t│ a 또는 스위치 --  종 치기            │\n");
	printf("\t└──────────────────────────────────────┘\n");
	
}


void changeName(int sockfd){

	int i;
	char nameTemp[100];
	printf("# Enter a new name (MAXIMUM 10 Bytes)\n");
	scanf("%s", nameTemp);
	if( strlen(nameTemp)>10 ){
		printf("# Too long! Change FAILED\n");
		return;
	}
	getchar();
	for(i=0; i<10; i++)
		rawname[i] = nameTemp[i];
	sprintf(name, "[%s]", nameTemp);
	sprintf(nameTemp, ":c%s", rawname);
	write(sockfd, nameTemp, strlen(nameTemp)*sizeof(char));
	printf("# done!\n");

}

void error_handling(char* msg){

	fputs(msg, stderr);
	exit(1);

}