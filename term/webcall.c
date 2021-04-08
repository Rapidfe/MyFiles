#include <stdio.h>

#define MAX_STR_SIZE 100
//<meta http-equiv=\"refresh\" content=\"1\">
void webcall(int nClient, char ** name){

	char str[MAX_STR_SIZE] = {0};
	FILE *in;
	in = fopen("chatcache.txt","r");
	
	FILE *out;
	out = fopen("/var/www/html/website.html","w");



	fprintf(out, "<!DOCTYPE html>");

	fprintf(out, "<head><meta charset = \"UTF-8\"><meta http-equiv=\"refresh\" content=\"3\"><title>Haligali</title></head>");
	fprintf(out,"<html><body>");
	
	fprintf(out,"<img src=\"image/title.jpg\">");
	
	
	fprintf(out,"<nav><h2> Play Haligali with us!</h2><h2> %d users are waiting for you!<h2></nav>",nClient);
	fprintf(out,"<nav><h1> players' name:  %s", name[0]);
	for(int i=1; i<nClient; i++)
		fprintf(out,", %s", name[i]); 
	fprintf(out,"</h1></nav>");

	fprintf(out,"<div style=\"overflow:auto; width:500px ; height:300px;\">");

	//chatting cache print
	while(1){
		fgets(str,MAX_STR_SIZE-1,in);
		if(feof(in))
			break;

		fprintf(out,"%s",str);
		fprintf(out,"<br/>");
	}

	fprintf(out,"</div>\n");	
	
	
	fprintf(out,"</body></html>");
	
	fclose(out);

}

void gamecall(int* onn, int nClient, char ** name ){
	
	FILE *out;
	out = fopen("/var/www/html/website.html","w");



	fprintf(out, "<!DOCTYPE html>");

	fprintf(out, "<head><meta charset = \"UTF-8\"><meta http-equiv=\"refresh\" content=\"3\"><title>Haligali</title></head>");
	fprintf(out,"<html><body>");
	
	fprintf(out,"<img src=\"image/title.jpg\">");
	
	
	fprintf(out,"<nav><h2> <br>%d players are playing!</h2></nav>", nClient);
	fprintf(out, "<nav><h1> players' name: %s", name[0]);
	for(int i=1; i<nClient; i++)
		fprintf(out,", %s", name[i]);
	fprintf(out,"</h1></nav>");

	



	for(int i = 0 ; i < nClient; i++){
		if(onn[i]!=0){
			fprintf(out,"<div style = \"border: 2px solid black; float: left;width: 70; height: 500px;\">"); 
			fprintf(out,"<img src=\"image/%d.jpg\" alt=\"client %d\" width=\"300px\" height=\"400px\">", onn[i], i);
			fprintf(out,"<br/>");
			fprintf(out,"</div>");
		
		}else{
			fprintf(out,"<div style = \"border: 2px solid black; float: left;width: 70; height: 500px;\">"); 
			fprintf(out,"<img src=\"image/dontknow.png\" alt=\"client %d\" width=\"300px\" height=\"400px\">",i);
		
			fprintf(out,"</div>");
		}
	}		
	
	fprintf(out,"</body></html>");
	
	fclose(out);




}

void chatcachewrite( char*msg, int len){
	
	FILE* chatcache = fopen("chatcache.txt","a");
	for(int i =0 ; i<len ; i++)
		fprintf(chatcache,"%c",msg[i]);
	fclose(chatcache);

}


void chatcacheinit(){

	int nResult = remove("chatcache.txt");

	if(nResult == 0 ) {
		printf("cache file deleted\n");
	}
	else if(nResult == -1){
		printf("[ERROR] cache file is not deleted!!\n");
	}
}

