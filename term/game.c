#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "game.h"
//#include "gamecall.c"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_RESET   "\x1b[0m"

int nums[] = {1,1,1,1,1,2,2,2,3,3,3,4,4,5};
char* fruits[] = {" 딸 기 ", " 바나나", " 라 임 ", " 자 두 "};
char* fruit_nums[][2] = {{"       ","   ●   "},
						 {"   ●   ","   ●   "},
						 {"   ●   ","  ● ●  "},
						 {"  ● ●  ","  ● ●  "},
						 {"  ● ●  "," ● ● ● "}};
char* print_color[] = {COLOR_RED, COLOR_YELLOW, COLOR_GREEN, COLOR_MAGENTA};
int chk_return[] = {1,1,1,1};

void during_game(int client_sockfd, char* msg){
	int nowidx;
	char turnmsg[25];
	for(nowidx=0; nowidx<client_cnt; nowidx++)
		if( client_socks[nowidx]==client_sockfd )
			break;
	if( eachEnd[nowidx]==0 ){
		write(client_sockfd, "<DEAD>\n", 7);
	}else if( msg[0]=='a' ){
		if( isYes ){
			wait_s = 1;
			get_oncards(client_sockfd, nowidx);
		}else if( !wait_s ){
			give_mycards(nowidx);
		}
	}else if( client_socks[turn]==client_sockfd ){
		if( msg[0]==':' && msg[1]=='d' ){
			//draw
			if( wait_t ){
				write(client_sockfd, "<PLEASE WAIT FOR LEDs>\n", 23);
			}else{
				card_draw();
				if( isGame ){
					do{
						turn = (turn+1)%client_cnt;
					}while( eachEnd[turn]==0 );
					sprintf(turnmsg, "\nNext turn: %s\n", ids[turn]);
					for(int i=0; i<client_cnt; i++){
						if( i==turn )
							write(client_socks[i], "\nNext turn: YOU\n", 16);
						else
							write(client_socks[i], turnmsg, strlen(turnmsg)*sizeof(char));
					}
				}
			}
		}
	}else{
		write(client_sockfd, "<NOT YOUR TURN>\n", 16);
	}
}

void make_start(int client_sockfd){	
	pthread_mutex_lock(&mutx);
	if( client_socks[0] != client_sockfd ){
		pthread_mutex_unlock(&mutx);
		write(client_sockfd, "<Only MASTER can make start>\n", 29);
		return;
	}
	if( client_cnt<2 || client_cnt>4 ){
		pthread_mutex_unlock(&mutx);
		write(client_sockfd, "<HOLLIGALLI needs 2~4 players>\n", 31);
		return;
	}
	holli_st();
	pthread_mutex_unlock(&mutx);
}
		

void give_mycards(int ii){
	int i, idx=0, cnt=0;
	char msg[20];
	sprintf(msg, "%s missed!\n", ids[ii]);
	for(i=0; i<client_cnt; i++)
		write(client_socks[i], msg, strlen(msg)*sizeof(char));
	if( eachEnd[ii]<live ){
		for(i=0; i<eachEnd[ii]; i++, idx++){
			if( idx==ii || eachEnd[idx]==0 ){
				i--;
				continue;
			}
			eachCards[idx][eachEnd[idx]++] = eachCards[ii][i];
		}
		eachEnd[ii] = 0;
		live--;
		sprintf(msg, "%s is dead!\n", ids[ii]);
		for(i=0; i<client_cnt; i++){
			if( i!=ii ) write(client_socks[i], msg, strlen(msg)*sizeof(char));
		}
		sprintf(msg, "You are dead\n");
		write(client_socks[ii], msg, strlen(msg)*sizeof(char));

		printf("each: ");
		for(i=0; i<client_cnt; i++){
			printf("%d ", eachEnd[i]);
		}
		printf("\n");

		chk_end();
		if( ii==turn ){
			if( isGame ){
				do{
					turn = (turn+1)%client_cnt;
				}while( eachEnd[turn]==0 );
			}
		}
	}else{
		for(i=0; i<live-1; i++, idx++){
			if( idx==ii || eachEnd[idx]==0 ){
				i--;
				continue;
			}
			eachCards[idx][eachEnd[idx]++] = eachCards[ii][i];
		}
		int tmp = live-1;
		for(i=0; i<eachEnd[ii]-tmp; i++){
			eachCards[ii][i] = eachCards[ii][i+tmp];
		}
		eachEnd[ii] -= tmp;
		printf("each: ");
		for(i=0; i<client_cnt; i++){
			printf("%d ", eachEnd[i]);
		}
		printf("\n");
	}
}

void get_oncards(int sock, int idx){
	int i, j;
	char msg[30];
	for(i=0; i<onCi; i++){
		eachCards[idx][eachEnd[idx]++] = onCards[i];
	}
	onCi = 0;
	onn[0]=0, onn[1]=0, onn[2]=0, onn[3]=0;
	sprintf(msg, ":h%d", client_cnt);
	for(i=0; i<client_cnt; i++){
		j = 5;
		if( i==turn ) msg[3] = 'h';
		else{
			msg[3] = 'H';
			for(; j<5+strlen(ids[turn]); j++)
				msg[j] = ids[turn][j-5];
		}
		msg[j++] = ':';
		if( i==idx ){
			msg[4] = 'h';
			msg[j] = '\0';
		}
		else{
			msg[4] = 'H';
			sprintf(msg+j, "%s HIT!\n", ids[idx]);
		}
		write(client_socks[i], msg, strlen(msg)*sizeof(char));
	}
	printf("\n");
	onCnt[0]=0, onCnt[1]=0, onCnt[2]=0, onCnt[3]=0, isYes=0;
	gamecall(onn, client_cnt, ids);
}

void chk_end(){
	if( live==1 ){
		int i, j;
		char msg1[20];
		char msg2[20];
		for(i=0; i<client_cnt; i++)
			if( eachEnd[i]>0 ) break;
		sprintf(msg1, "%s WIN!\n:e", ids[i]);
		sprintf(msg2, "You WIN!\n:e");
		for(j=0; j<client_cnt; j++){
			if( j!=i ) write(client_socks[j], msg1, strlen(msg1)*sizeof(char));
			else write(client_socks[j], msg2, strlen(msg2)*sizeof(char));
		}
		isGame = 0;
		printf("<HALLIGALLI end>\n");
	}
}

void card_draw(){
	int card, i, j, k;
	char msg[25];
	char concat[500];
	char chk[5];
	card = eachCards[turn][0];
	if( onn[turn]>0 ) onCnt[onn[turn]%10] -= onn[turn]/10;
	onn[turn] = card;
	onCnt[card%10] += card/10;
	if( onCnt[0]==5 || onCnt[1]==5 || onCnt[2]==5 || onCnt[3]==5 ) isYes = 1;
	//printf("go!\n");}
	//
	else isYes = 0;
	onCards[onCi++] = card;
	gamecall(onn, client_cnt, ids);
	wait_t = card;
	printf("card: %d\n",card);

	for(k=0; k<2; k++){
		for(i=0; i<client_cnt; i++){
			if( onn[i]>0 )
				sprintf(msg, " %s%s", print_color[onn[i]%10], fruit_nums[onn[i]/10-1][k]);
			else
				sprintf(msg, " %s  [ ]  ", COLOR_RESET);
			if( i==client_cnt-1 ) sprintf(msg, "%s%s\n", msg, COLOR_RESET);
			if( k!=0 || i!=0 ) sprintf(concat, "%s%s", concat, msg);
			else sprintf(concat, "%s", msg);
		}
	}
	for(i=0; i<client_cnt; i++){
		if( onn[i]>0 )
			sprintf(msg, " %s%s", print_color[onn[i]%10], fruits[onn[i]%10]);
		else
			sprintf(msg, " %s  [ ]  ", COLOR_RESET);
		if( i==client_cnt-1 ) sprintf(msg, "%s%s\n", msg, COLOR_RESET);
		sprintf(concat, "%s%s", concat, msg);
	}
	for(i=0; i<client_cnt; i++){
		if( turn==i ){
			sprintf(concat, "%s    ↑   ", concat);
		}else{
			sprintf(concat, "%s        ", concat);
		}
		if( i==client_cnt-1 )
			sprintf(concat, "%s\n", concat);
	}
	
	for(j=0; j<client_cnt; j++)
		write(client_socks[j], concat, strlen(concat)*sizeof(char));

	for(i=0; i<eachEnd[turn]; i++)
		eachCards[turn][i] = eachCards[turn][i+1];
	eachEnd[turn]--;

	printf("each: ");
	for(i=0; i<client_cnt; i++){
		printf("%d ", eachEnd[i]);
	}
	printf("\n");

	// if dead
	if( eachEnd[turn]==0 ){
		live--;
		sprintf(msg, "%s is dead!\n", ids[turn]);
		for(i=0; i<client_cnt; i++)
			if( i!=turn ) write(client_socks[i], msg, strlen(msg)*sizeof(char));
		sprintf(msg, "You are dead\n");
		write(client_socks[turn], msg, strlen(msg)*sizeof(char));
		// if game end
		chk_end();
	}
}	

void holli_st(){
	live = client_cnt;
	onCi = 0;
	isGame = 1;
	turn = 0;
	onCnt[0]=0, onCnt[1]=0, onCnt[2]=0, onCnt[3]=0, isYes=0;
	eachEnd[0] = 56/client_cnt;
	eachEnd[1]=eachEnd[0], eachEnd[2]=eachEnd[0], eachEnd[3]=eachEnd[0];
	onn[0]=0, onn[1]=0, onn[2]=0, onn[3]=0;
	printf("<HOLLIGALLI start>\n");
	set(all);
	shuffle(all);
	
	// send start signal
	char msg[20];
	sprintf(msg, ":s%ds", client_cnt);
	write(client_socks[0], msg, 4);
	sprintf(msg, ":s%dS%s\n", client_cnt, ids[0]);
	gamecall(onn, client_cnt, ids);
	for(int i=1; i<client_cnt; i++)
		write(client_socks[i], msg, strlen(msg)*sizeof(char));
}


void set(int *arr){
	for(int i=0; i<4; i++){
		for(int j=0; j<14; j++){
			arr[14*i+j] = nums[j]*10+i;
		}
	}
}

void shuffle(int *arr){
	srand(time(NULL));
	int temp, i, j;
	int rn;
	int num = 56;
	for(i=0; i<num-1; i++){
		rn = rand()%(num-i)+i;
		temp = arr[i];
		arr[i] = arr[rn];
		arr[rn] = temp;
	}
	int idx=0;
	for(i=0; i<client_cnt; i++){
		for(j=0; j<eachEnd[0]; j++){
			eachCards[i][j] = all[idx++];
		}
	}
}
	
