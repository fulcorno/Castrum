/* programma ENVIRON.C
 * Stabilisce la posizione corretta delle finestre in base alla scheda grafica
 * che si vuiole usare
 */


#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "set.h"
#include "drivers.h"
#include "windows.h"

/*
 * identificatore univoco della finestra
 */
typedef struct {
	char module[8] ;
	char windname[8] ;
} Wident ;

static Wident Wnames[30] ;

static void writefile ( const char * name )
{
	FILE * f ;
	int totW ;
	char fname[20] ;

	strcpy(fname,name) ;
	strcat(fname,".WIN") ;
	f = fopen(fname,"wb") ;
	if (!f) {
		fprintf(stderr,"Can't open file %s.\n",fname) ;
		exit(1) ;
	}

	totW = getmaxwindow() ;
	putw(totW,f) ;
	fwrite(Wnames,sizeof(Wident),totW,f) ;
	savewindows(totW,f) ;

	fclose(f) ;
}

static void assign ( char owner[8], char ident[8], int pos )
{
	strncpy( Wnames[pos].module, owner, 8 ) ;
	strncpy( Wnames[pos].windname, ident, 8 ) ;
	setwindow(pos) ;
	setcolor(1) ;
	rectangle (0,0,wind(dx),wind(dy)) ;
}


static void fill_ATT (void)
{
	int i ;
	char buf[10] ;
	int Wtasti, Wogg, Woggs, Wstatus, Wgotico ;
	int wht = 28 ;
	int icnX=32, icnY=28 ;
	int gm,gd ;

	gm = ATT400 ; gd = ATT400HI ;
	initgraph(&gm,&gd,"") ;

	initwindows() ;

	/* ROOM */
	assign ( "ROOM    ", "ROOM    ",
		createwindow ( 0,0, 316,187, 0, 158,43, 10, 1,1,1,1, grigio, 0,1 )
	) ;

	assign ( "ROOM    ", "TITOLO  ",
		createwindow( 1,189, 314,9, 0, 156,4, DEFAULT_FONT, 1,1,1,1, bianco, 1,0 )
	) ;

	/* FUNCKEYS */
	assign ( "FUNCKEYS", "TASTI   ",
		Wtasti = createwindow( 320,23, 95,197, 0, icnX,icnY, 0, 1,1,1,1, dummy, 1,0 )
	) ;

	assign ( "FUNCKEYS", "TITOLO  ",
		createwindow( 320,0, 95,21, 0, 0,0, SMALL_FONT, 1,1,1,1, bianco, 1,0 )
	) ;

	for (i=0; i<10; ++i) {
		sprintf(buf,"TASTO%d  ",i) ;
		assign ( "FUNCKEYS", buf,
			createwindow ( (i%2) ? 55 : 9, 9+(i>>1)*(10+wht), 31,wht-1,Wtasti, 15,0, SMALL_FONT, 7,8,1,1, bianco, 1,0 )
		) ;
	}

	/* OBJECTS */

	assign ( "OBJECTS ", "OGG     ",
		Wogg = createwindow( 0,200, 159,89, 0, 0,0, 0, 1,1,1,1, bianco, 1,BLACK )
	) ;

	assign ( "OBJECTS ", "TITOLO  ",
		createwindow( 0,91, 159,8, Wogg, 79,3, SMALL_FONT, 1,1,1,1, bianco,1,BLACK )
	) ;

	/* STATUS */

	assign ( "STATUS  ", "STATUS  ",
		Wstatus = createwindow( 420,0, 219,299, 0, 0,0, 0, 1,1,1,1, dummy, 1,0 )
	) ;

	assign ( "STATUS  ", "INDIC   ",
		createwindow( 8,30, 203,94, Wstatus, 9,10, 6, 2,5,1,5, dummy, 1,0 )
	) ;

	assign ( "STATUS  ", "ICONS   ",
		createwindow( 8,133, 203,47, Wstatus, icnX,icnY, 0, 1,1,1,1, nero, 1,0)
	) ;

	assign ( "STATUS  ", "OGGS    ",
		Woggs = createwindow( 8,182, 203,114, Wstatus, 0,0, 0, 1,1,1,1, dummy, 1,0)
	) ;

	assign ( "STATUS  ", "MYOGGS  ",
		createwindow( 51,5, 146,49, Woggs, 0,0, 2, 1,1,1,1, bianco, 1,0)
	) ;

	assign ( "STATUS  ", "LOGGS   ",
		createwindow( 51,60, 146,49, Woggs, 0,0, 2, 1,1,1,1, bianco, 1,0)
	) ;

	assign ( "STATUS  ", "ORA     ",
		createwindow( 168,200, 119,99, 0, 60,50, 0, 1,1,1,1, dummy, 1,0)
	) ;

	assign ( "STATUS  ", "HORZ    ",
		createwindow( 288,230, 127,52, 0, 64,53, 0, 1,1,1,1, dummy, 0,1)
	) ;

	/* GOTICO */
	assign ( "GOTICO  ", "PERGAM  ",
		Wgotico = createwindow( 0,301, 639,98, 0, 16,6, 0, 1,1,1,1, dummy, 0,1)
	) ;

	assign ( "GOTICO  ", "TESTO   ",
		createwindow( 16,6, 607, 83, Wgotico, 2,20, 0, 1,1,1,1, dummy, 0,1)
	) ;

	getchar() ;
	closegraph() ;
}

static void fill_CGA (void)
{
	int i ;
	char buf[10] ;
	int Wtasti, Wogg, Woggs, Wstatus, Wgotico ;
	int wht = 14 ;
	int icnX=32, icnY=14 ;
	int gm,gd ;

	gm = CGA ; gd = CGAHI ;
	initgraph(&gm,&gd,"") ;

	initwindows() ;

	/* ROOM */
	assign ( "ROOM    ", "ROOM    ",
		createwindow ( 0,0, 316,88, 0, 158,43, 10, 1,1,1,2, grigio, 0,1 )
	) ;

	assign ( "ROOM    ", "TITOLO  ",
		createwindow( 1,90, 314,8, 0, 156,4, DEFAULT_FONT, 1,1,1,1, bianco, 1,0 )
	) ;

	/* FUNCKEYS */
	assign ( "FUNCKEYS", "TASTI   ",
		Wtasti = createwindow( 320,18, 95,99, 0, icnX,icnY, 0, 1,1,1,1, dummy, 1,0 )
	) ;

	assign ( "FUNCKEYS", "TITOLO  ",
		createwindow( 320,0, 95,17, 0, 0,0, SMALL_FONT, 1,1,2,3, bianco, 1,0 )
	) ;

	for (i=0; i<10; ++i) {
		sprintf(buf,"TASTO%d  ",i) ;
		assign ( "FUNCKEYS", buf,
			createwindow ( (i%2) ? 55 : 9, 4+(i>>1)*(5+wht), 31,wht-1,Wtasti, 15,0, SMALL_FONT, 7,8,1,2, bianco, 1,0 )
		) ;
	}

	/* OBJECTS */

	assign ( "OBJECTS ", "OGG     ",
		Wogg = createwindow( 0,99, 159,44, 0, 0,0, 0, 1,1,1,1, bianco, 1,BLACK )
	) ;

	assign ( "OBJECTS ", "TITOLO  ",
		createwindow( 0,45, 159,6, Wogg, 79,3, SMALL_FONT, 1,1,3,4, bianco,1,BLACK )
	) ;

	/* STATUS */

	assign ( "STATUS  ", "STATUS  ",
		Wstatus = createwindow( 420,0, 219,149, 0, 0,0, 0, 1,1,1,1, dummy, 1,0 )
	) ;

	assign ( "STATUS  ", "INDIC   ",
		createwindow( 8,15, 203,47, Wstatus, 5,5, 6, 2,5,1,10, dummy, 1,0 )
	) ;

	assign ( "STATUS  ", "ICONS   ",
		createwindow( 8,67, 203,22, Wstatus, icnX,icnY, 0, 1,1,1,1, nero, 1,0)
	) ;

	assign ( "STATUS  ", "OGGS    ",
		Woggs = createwindow( 8,92, 203,57, Wstatus, 0,0, 0, 1,1,1,1, dummy, 1,0)
	) ;

	assign ( "STATUS  ", "MYOGGS  ",
		createwindow( 51,2, 146,23, Woggs, 0,0, 2, 1,1,1,2, bianco, 1,0)
	) ;

	assign ( "STATUS  ", "LOGGS   ",
		createwindow( 51,29, 146,23, Woggs, 0,0, 2, 1,1,1,2, bianco, 1,0)
	) ;

	assign ( "STATUS  ", "ORA     ",
		createwindow( 168,100, 119,49, 0, 60,25, 0, 1,1,1,1, dummy, 1,0)
	) ;

	assign ( "STATUS  ", "HORZ    ",
		createwindow( 288,120, 127,26, 0, 64,27, 0, 1,1,1,1, dummy, 0,1)
	) ;

	/* GOTICO */
	assign ( "GOTICO  ", "PERGAM  ",
		Wgotico = createwindow( 0,150, 639,49, 0, 16,3, 0, 1,1,1,1, dummy, 0,1)
	) ;

	assign ( "GOTICO  ", "TESTO   ",
		createwindow( 16,3, 607, 41, Wgotico, 2,10, 0, 1,1,1,1, dummy, 0,1)
	) ;

	getchar() ;
	closegraph() ;
}

void main ( int argc, char ** argv )
{
	int ct ;

	printf("Jena Soft 1989 - S.G.Castrum auxiliary program\n") ;
	printf("ENVIRON: crea i files \".WIN\" contenenti le informazioni sulle finestre\n") ;

	if ( argc == 1 ) {
		fprintf(stderr,"USO: environ <nome scheda grafica> [...]\n") ;
		exit(1) ;
	}

	for (ct=1; ct<argc; ++ct) {
		if (!stricmp(argv[ct],"att"))
			fill_ATT() ;
		else if (!stricmp(argv[ct],"cga"))
			fill_CGA() ;
		else continue ;
		writefile (argv[ct]) ;
	}
}