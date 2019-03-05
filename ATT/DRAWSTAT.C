/* Programma DrawStatus */

#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"
#include "windows.h"
#include "status.h"

/* Barrette, cotillons e oggetti della finestra di Status */

static int Wstatus,Windic,Wicons,Woggs,Wmyoggs,Wloggs ;

#define sx2 8
#define sy2 64	/* figurine */
#define dx2 203
#define dy2 25
#define dst 40
#define dsy 16
#define d2y 4

#define sx3 8
#define sy3 91	/* inventari */
#define dx3 203
#define dy3 57

char * titl[] = { "POSIZIONE", "VELOCITA", "STAMPA", "FISICO" } ;

struct {
	int x,y ;
	byte z[150] ;
} a ;

static void near noclip (void)
{
	struct viewporttype v ;
	getviewsettings(&v) ;
	setviewport(v.left,v.top,v.right,v.bottom,0) ;
}

void addicns(void)
{
	int i ;

	/* icone inventari */
	setwindow(Woggs) ;
	a.x=31;
	a.y=27;
	memcpy(a.z,icona+12*4*28,4*28) ;
	putimage(8,16,&a,NOT_PUT);
	memcpy(a.z,icona+13*4*28,4*28) ;
	putimage(8,wind(dy)/2+12,&a,NOT_PUT);

	/* simboli "%" */

	setwindow(Windic) ;
	settextjustify(LEFT_TEXT,TOP_TEXT) ;
	setusercharsize(1,1,1,1);
	selectfont(2,0) ;
	for(i=punti; i<=antipatia; ++i)
	outtextxy(wind(dx)-16-2,wind(yc)*i+wind(xc),"%") ;
}

void main(void)
{
	FILE * f ;
	size_t sz ;
	void *pt ;

	initgraphscreen(ATT400) ;
	initwindows() ;
	loadenviron("ATT") ;
	Wstatus = gimmewindow("STATUS  ","STATUS  ") ;
	Windic  = gimmewindow("STATUS  ","INDIC   ") ;
	Wicons  = gimmewindow("STATUS  ","ICONS   ") ;
	Woggs   = gimmewindow("STATUS  ","OGGS    ") ;
	Wmyoggs = gimmewindow("STATUS  ","MYOGGS  ") ;
	Wloggs  = gimmewindow("STATUS  ","LOGGS   ") ;
	icona = loadfile ( grpath ( "icons.byt" ) ) ;

	setwindow(Woggs) ;
	wind(y0) -= 6 ;
	setwindow(Wicons) ;
	wind(y0) -= 4 ;
	initstatus() ;

	addicns() ;

	setwindow(Wstatus) ;
	noclip() ;
	sz=imagesize(0,0, wind(dx),wind(dy)) ;
	pt = calloc(sz,1) ;
	if (!pt) exit(0) ;
	getimage(0,0, wind(dx),wind(dy), pt) ;
	setviewport(0,0,getmaxx(),getmaxy(),1);
	putimage(0,0, pt, COPY_PUT);
	f = fopen("att\\status.byt","wb") ;
	if (f) {
		fwrite(pt,sz,1,f);
		fclose(f);
	} else printf("xxx") ;
	getch() ;
	restorecrtmode() ;
}
