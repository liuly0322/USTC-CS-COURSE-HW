// expr lexer
#include <stdlib.h>
#include <string.h>
#include "pl0.h"

#ifndef LEXERGEN
void getch() {
    if(cc==ll){
	if(feof(infile)){
	    exit(1);
	}
	ll=0; cc=0;
	while((!feof(infile))&&((ch=getc(infile))!='\n')){
	    ll=ll+1; line[ll]=ch;
	}
	printf("\n");
	ll=ll+1; line[ll]=' ';
    }
    cc=cc+1; ch=line[cc];
}

void getsym(){
    long i,j,k;

    while(ch==' '||ch=='\t'){
	getch();
    }
    if(isdigit(ch)){ // number
	k=0; num=0; sym=number;
	do{
	    num=num*10+(ch-'0');
	    k=k+1; getch();
	}while(isdigit(ch));
	if(k>nmax){
	    error(31);
	}
    } else if (ch=='+') {
	sym=plus;
	getch();
    } else {
	sym=nul;
	getch();
    }
}
#endif

int main(int argc, char *argv[])
{
	
	printf("please input source program file name: ");
	scanf("%s", infilename);
	if ( (infile=fopen(infilename, "r"))==NULL ) {
		printf("File %s can't be opened.\n", infilename);
		exit(1);
	}
#ifdef LEXERGEN
	extern FILE * yyin;
	yyin=infile;
#endif
	getsym();	
	while (sym) {
		switch(sym) {
		case number:
			printf("%ld", num);
			break;
		case plus:
			printf("+"); break;
		}
		getsym();
	}
	printf("File %s has already been scanned.\n", infilename);
	fclose(infile);
}
