#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int ages[90] = {0};
int names[252526] = {0};
int zips[16] = {0};
int nameN = 0;
int dataN = 0;

void print_st(int i){
	char *st = (char*)malloc(sizeof(char)*14);
	int idx = 0;
	while( i!=0 ){
		int one = i%10;
		i /= 10;
		st[idx++] = one+'0';
		if( idx!=0 && idx%4==3 && i!=0 ) st[idx++] = ',';
	}
	for(int iter=idx; iter>0; iter--) printf("%c",st[iter-1]);
	free(st);
}

void print_info(){
	printf("\n--------------------------------------------------\n");
	printf("전체 데이터 갯수 ");
	print_st(dataN);
	printf("개 중 이름은 ");
	print_st(nameN);
	printf("개\n\n나이:\n");
	for(int x=1; x<9; x++){
		int begin = x*10;
		int acc = 0;
		for(int y=begin; y<begin+10; y++)
			acc += ages[y];
		printf("%d ~ %d: ",begin,begin+9);
		print_st(acc);
		printf("명\n");
	}
	printf("\nZip code:\n");
	for(int x=1; x<16; x++){
		if( x<10 ) printf("KS00%d: ",x);
		else printf("KS0%d: ",x);
		print_st(zips[x]);
		printf("명\n");
	}
	printf("---------------------------------------------------\n\n");
}

int main(){
	FILE *in;
	char cwd_buf[255];
	getcwd(cwd_buf, 255);
	in = fopen("data.txt", "r");
	if( in==NULL ){
		printf("!! failed to open \"data.txt\" from cwd(%s)\n",cwd_buf);
		exit(0);
	}
	int age, name, zip;
	char c = getc(in);
	while( !feof(in) ){
		age = c-'0';
		c = getc(in);
		age = 10*age + c-'0';
		//printf("%d\n",age);

		c = getc(in); c = getc(in);
		name = c-'A';
		c = getc(in);
		name = 100*name + c-'A';
		c = getc(in);
		name = 100*name + c-'A';
		//printf("%d\n",name);

		c = getc(in); c = getc(in);
		c = getc(in); c = getc(in);
		c = getc(in);
		zip = c-'0';
		c = getc(in);
		zip = 10*zip + c-'0';
		//printf("%d\n",zip);

		ages[age]++;
		names[name]++;
		if( names[name]==1 ) nameN++;
		zips[zip]++;

		c = getc(in);
		while( c==10 || c==13 ) c = getc(in);
		dataN++;
	}
	print_info();

	return 0;
}
