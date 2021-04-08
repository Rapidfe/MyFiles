#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/ioctl.h>


#define FILEPATH "/dev/fb0"
#define NUM_WORDS 64
#define FILESIZE (NUM_WORDS * sizeof(uint16_t))

#define RGB565_RED 0xF800
#define RGB565_GREEN 0x07E0
#define RGB565_BLUE 0x001F
#define RGB565_PURPLE 0xF00F
#define RGB565_YELLOW 0xFFE0

// 딸기 - RED  바나나-yellow  라임- GREEN   자두-purple 

void delay(int);
void strawberry(int, uint16_t *);
void banana(int, uint16_t *);
void lime(int, uint16_t *);
void jadoo(int, uint16_t *);
void showFruit(int, int, uint16_t *);
void glow(int [][8], int , uint16_t *);

void fruit(int wfruit, int nfruit){

	int i,j;
	int fbfd;
	uint16_t *map;
	uint16_t *p;
	struct fb_fix_screeninfo fix_info;
	char *s1 = malloc(sizeof(char)*10);

	//open the led frame buffer device 
	fbfd = open(FILEPATH, O_RDWR);
	if(fbfd ==-1){
		perror("error (call to 'open')");
		exit (EXIT_FAILURE);
	}

	//read fixed screen info
	if(ioctl(fbfd, FBIOGET_FSCREENINFO,&fix_info)==-1){
		perror("error (call to 'ioctl')");
		close(fbfd);
		exit (EXIT_FAILURE);
	}

	//check the correct device
	if (strcmp(fix_info.id, "RPi-Sense FB") !=0){
		printf("%s\n", "Error: RPi-Sense FB not found");
		close(fbfd);
		exit(EXIT_FAILURE);
	}

	map = mmap (NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (map == MAP_FAILED){
		close(fbfd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}

	//pointer : memory area start address
	p= map;
	
	//clear the led matrix
	memset(map, 0, FILESIZE);



	//printf("led matrix clear complete\n");
//	printf("do you want to start a game?\n");
//	scanf("%s\n",s1);

//	while(strcmp(s1,"NO")!=0){
		//strawberry(5,p);
		//banana(3,p);
		//lime(2,p);
		//jadoo(1,p);
		
		switch(wfruit){
			case 0 :
				strawberry(nfruit,p);
				break;
			case 1 :
				banana(nfruit,p);
				break;
			case 2:
				lime(nfruit,p);
				break;
			case 3:
				jadoo(nfruit,p);
				break;
			case 5:
				break;
			default:
				printf("fruit error!!\n");
		}

		/*for(i=0; i<3; i++){
			delay(250);
			memset(map, 0xFF, FILESIZE);
			delay(250);
			memset(map, 0, FILESIZE);
		}
	
		delay(250);*/
//		printf("do you want to start a game?\n");
//		scanf("%s\n",s1);
//	}






	//clear the led matrix
	memset(map, 0, FILESIZE);
	
	//un-map and close 
	if(munmap(map, FILESIZE) == -1){
		perror("Error un-mmapping the file ");
	}


	close (fbfd);
	free(s1);

//	return 0;
}

void delay(int t)
{

	usleep(t*1000);

}

void strawberry(int nfruit, uint16_t *p){
	
	int led[8][8]={0};

	led[0][3]=2;
	led[0][4]=2;
	led[0][5]=2;
	led[1][3]=2;
	led[1][4]=2;
	led[2][0]=1;
	led[2][1]=1;
	led[2][2]=1;
	led[2][3]=1;
	led[2][4]=1;
	led[2][5]=1;
	led[2][6]=1;
	led[2][7]=1;
	led[3][0]=1;
	led[3][1]=1;
	led[3][2]=1;
	led[3][3]=1;
	led[3][4]=1;
	led[3][5]=1;
	led[3][6]=1;
	led[3][7]=1;
	led[4][1]=1;
	led[4][2]=1;
	led[4][3]=1;
	led[4][4]=1;
	led[4][5]=1;
	led[4][6]=1;
	led[5][2]=1;
	led[5][3]=1;
	led[5][4]=1;
	led[5][5]=1;
	led[6][3]=1;
	led[6][4]=1;
	led[7][3]=1;
	led[7][4]=1;

	showFruit(nfruit,RGB565_RED,p);
	glow(led,RGB565_RED,p);
	showFruit(nfruit,RGB565_RED,p);
	glow(led,RGB565_RED,p);
	showFruit(nfruit,RGB565_RED,p);
	glow(led,RGB565_RED,p);
	showFruit(nfruit,RGB565_RED,p);
	glow(led,RGB565_RED,p);


}

void lime(int nfruit,uint16_t *p){
	
	
	int led[8][8]={0};

	/*
	 * .......
	 * ....00..
	 * ..00000.
	 * .0000000
	 * 00000000
	 * .000000.
	 * ..0000..
	 * ...0....
	 *
	 * */


	led[1][4]=1;
	led[1][5]=1;
	led[2][2]=1;
	led[2][3]=1;
	led[2][4]=1;
	led[2][5]=1;
	led[2][6]=1;

	led[3][1]=1;
	led[3][2]=1;
	led[3][3]=1;
	led[3][4]=1;
	led[3][5]=1;
	led[3][6]=1;
	led[3][7]=1;

	led[4][0]=1;
	led[4][1]=1;
	led[4][2]=1;
	led[4][3]=1;
	led[4][4]=1;
	led[4][5]=1;
	led[4][6]=1;
	led[4][7]=1;

	led[5][1]=1;
	led[5][2]=1;
	led[5][3]=1;
	led[5][4]=1;
	led[5][5]=1;
	led[5][6]=1;

	led[6][2]=1;
	led[6][3]=1;
	led[6][4]=1;
	led[6][5]=1;

	led[7][3]=1;
	
	showFruit(nfruit,RGB565_GREEN,p);
	glow(led,RGB565_GREEN,p);
	showFruit(nfruit,RGB565_GREEN,p);
	glow(led,RGB565_GREEN,p);
	showFruit(nfruit,RGB565_GREEN,p);
	glow(led,RGB565_GREEN,p);
	showFruit(nfruit,RGB565_GREEN,p);
	glow(led,RGB565_GREEN,p);
	

	char msg[40];
}

void banana(int nfruit,uint16_t *p){
	
/*
 *  ....0000
 *  .....00.
 *  ....000.
 *  ...000..
 *  ..0000..
 *  .0000...
	char msg[40];
 *  000.....
 *  ........
 * */

	int led[8][8]={0};

	led[0][4]=1;
	led[0][5]=1;
	led[0][6]=1;
	led[0][7]=1;

	led[1][5]=1;
	led[1][6]=1;
	
	led[2][4]=1;
	led[2][5]=1;
	led[2][6]=1;
	
	led[3][3]=1;
	led[3][4]=1;
	led[3][5]=1;
	
	led[4][2]=1;
	led[4][3]=1;
	led[4][4]=1;
	led[4][5]=1;
	
	led[5][1]=1;
	led[5][2]=1;
	led[5][3]=1;
	led[5][4]=1;
	
	led[6][0]=1;
	led[6][1]=1;
	led[6][2]=1;


	showFruit(nfruit,RGB565_YELLOW,p);
	glow(led,RGB565_YELLOW,p);
	showFruit(nfruit,RGB565_YELLOW,p);
	glow(led,RGB565_YELLOW,p);
	showFruit(nfruit,RGB565_YELLOW,p);
	glow(led,RGB565_YELLOW,p);
	showFruit(nfruit,RGB565_YELLOW,p);
	glow(led,RGB565_YELLOW,p);

}

void jadoo(int nfruit,uint16_t *p){
	
	int led[8][8]={0};

/*	.....0..
 *	....0...
 *	....0...
 *	.000000.
 *	00000000
 *	00000000
 *	.000000.
 *	..0000..
 *
 * */


	led[0][5]=1;
	led[1][4]=1;
	led[2][4]=1;

	led[3][1]=1;
	led[3][2]=1;
	led[3][3]=1;
	led[3][4]=1;
	led[3][5]=1;
	led[3][6]=1;

	led[4][0]=1;
	led[4][1]=1;
	led[4][2]=1;
	led[4][3]=1;
	led[4][4]=1;
	led[4][5]=1;
	led[4][6]=1;
	led[4][7]=1;
	led[5][0]=1;
	led[5][1]=1;
	led[5][2]=1;
	led[5][3]=1;
	led[5][4]=1;
	led[5][5]=1;
	led[5][6]=1;
	led[5][7]=1;

	led[6][1]=1;
	led[6][2]=1;
	led[6][3]=1;
	led[6][4]=1;
	led[6][5]=1;
	led[6][6]=1;
	
	led[7][2]=1;
	led[7][3]=1;
	led[7][4]=1;
	led[7][5]=1;


	showFruit(nfruit,RGB565_PURPLE,p);
	glow(led,RGB565_PURPLE,p);
	showFruit(nfruit,RGB565_PURPLE,p);
	glow(led,RGB565_PURPLE,p);
	showFruit(nfruit,RGB565_PURPLE,p);
	glow(led,RGB565_PURPLE,p);
	showFruit(nfruit,RGB565_PURPLE,p);
	glow(led,RGB565_PURPLE,p);

}


void showFruit(int nfruit, int color, uint16_t *p){
	int led[8][8]={0};

	switch (nfruit){
		case 1:
			led[3][3]=1;
			led[3][4]=1;
			led[4][3]=1;
			led[4][4]=1;
			break;
		case 2:
			led[3][1]=1;
			led[3][2]=1;
			led[3][5]=1;
			led[3][6]=1;
			led[4][1]=1;
			led[4][2]=1;
			led[4][5]=1;
			led[4][6]=1;
			break;
		case 3:
			led[1][3]=1;
			led[1][4]=1;
			led[2][3]=1;
			led[2][4]=1;
			led[5][1]=1;
			led[5][2]=1;
			led[5][5]=1;
			led[5][6]=1;
			led[6][1]=1;
			led[6][2]=1;
			led[6][5]=1;
			led[6][6]=1;
			break;
		case 4:
			led[1][1]=1;
			led[1][2]=1;
			led[1][5]=1;
			led[1][6]=1;
			led[2][1]=1;
			led[2][2]=1;
			led[2][5]=1;
			led[2][6]=1;
			led[5][1]=1;
			led[5][2]=1;
			led[5][5]=1;
			led[5][6]=1;
			led[6][1]=1;
			led[6][2]=1;
			led[6][5]=1;
			led[6][6]=1;
			break;
		case 5 :
			led[1][1]=1;
			led[1][2]=1;
			led[1][5]=1;
			led[1][6]=1;
			led[2][1]=1;
			led[2][2]=1;
			led[2][5]=1;
			led[2][6]=1;
			led[3][3]=1;
			led[3][4]=1;
			led[4][3]=1;
			led[4][4]=1;
			led[5][1]=1;
			led[5][2]=1;
			led[5][5]=1;
			led[5][6]=1;
			led[6][1]=1;
			led[6][2]=1;
			led[6][5]=1;
			led[6][6]=1;
			break;
		default:
			printf("invalid fruit number\n");


	}
	glow (led,color,p);


}

void glow(int led[][8],int color, uint16_t *p){
	for(int i=0 ; i<8 ; i++){
		for(int j=0 ; j<8 ; j++){
			if(led[i][j]==1){
				*(p+8*i+j)=color;
			}else if( color==RGB565_RED ){
				if(led[i][j]==2)
					*(p+8*i+j)=RGB565_GREEN;
			}
		}
	}

	delay(400);
	
	memset(p,0,FILESIZE);
	//delay(250);

}


