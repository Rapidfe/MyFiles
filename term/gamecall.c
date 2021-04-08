#include <stdio.h>

void gamecall(int* onn, int nClient ){
	
	FILE *out;
	out = fopen("/var/www/html/website.html","w");



	fprintf(out, "<!DOCTYPE html>");

	fprintf(out, "<head><meta charset = \"UTF-8\"><meta http-equiv=\"refresh\" content=\"3\"><title>Haligali</title></head>");
	fprintf(out,"<html><body>");
	
	
	
	fprintf(out,"<header><h1> This is embedded system project page </h1></header>");
	
	
	fprintf(out,"<nav><h2> Play Haligali with us!</h2><h2> %d users are waiting for you!<h2></nav>",nClient);
	fprintf(out,"</h1></nav>");
	



	for(int i = 0 ; i < nClient; i++){
		fprintf(out,"<img src=\"image/%d.jpg\" alt=\"client %d\">", onn[i], i);
	}
	
	fprintf(out,"</body></html>");
	
	fclose(out);




}

