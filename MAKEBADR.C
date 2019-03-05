/*
 * Programma "MAKEBADR.C"
 * Crea le tabelle Badr[] per i diversi modi grafici nelle sotto-dir
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned int Badr[400] ;

void main(int argc, char ** argv)
{
	FILE * f ;
	char fname[80] ;
	register int y ;
	int size ;

	printf("MakeBadr: crea una tabella di indirizzi per la grafica\n") ;

	if (argc<=1) {
		fprintf(stderr,"USO: MakeBadr <scheda grafica> ...\n") ;
		fprintf(stderr,"schede supportate: ATT, CGA\n") ;
		exit(1) ;
	}

	for ( argv++; --argc; argv++ ) {

		strcpy(fname,*argv) ;
		strcat(fname,"\\badr.byt") ;

		f = fopen (fname, "wb") ;
		if (!f) {
			fprintf(stderr,"Invalid specification: <%s>\n",*argv) ;
			exit(2) ;
		}

		if (!stricmp(*argv,"att")) {
			for (y=0; y<=399; Badr[y++]= ((y&3)<<13)+80*(y>>2)) /*NOP*/ ;
			size = 400 ;
		} else if (!stricmp(*argv,"cga")) {
			for (y=0; y<=199; Badr[y++]= ((y&1)<<13)+80*(y>>1)) /*NOP*/ ;
			size = 200 ;
		} else {
			fprintf(stderr,"Unrecognized device: <%s>\n",*argv) ;
			exit(1) ;
		}

		if ( size != fwrite( Badr, sizeof(unsigned int), size, f ) ) {
			perror("Error on writing") ;
			exit(1) ;
		}
	}
}
