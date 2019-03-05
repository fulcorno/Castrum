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

static void near bord (int x, int y, int lx, int ly, int r)
/* rettangolo con angoli smussati */
{
	line(x+r,y,x+lx-r,y) ;
	line(x+r,y+ly,x+lx-r,y+ly) ;
	line(x,y+r/2,x,y+ly-r/2) ;
	line(x+lx,y+r/2,x+lx,y+ly-r/2) ;
	ellipse(x+r,y+r/2,90,180,r,r/2) ;
	ellipse(x+r,y+ly-r/2,180,270,r,r/2) ;
	ellipse(x+lx-r,y+ly-r/2,270,0,r,r/2) ;
	ellipse(x+lx-r,y+r/2,0,90,r,r/2) ;
} ; /* bord */

static void near arrow(int x, int y, int ln)
{
	line(x,y-2,x,y);
	line(x,y,x+ln,y);
	line(x+ln,y,x+ln-4,y-2);
	line(x+ln,y,x+ln-4,y+2);
} ;

static void near borders(void)
/* disegna il background dello status. */
{
	int i,j,w,z ;
	t_barrette q ;
	char s[] = "Infurmasiun" ;

	setwindow(Wstatus) ;
	noclip() ;
	setfillstyle(SOLID_FILL,1) ;
	setcolor(1) ;
	setlinestyle(SOLID_LINE,0,1) ;
	bord(0,0,wind(dx),wind(dy),16) ;
	floodfill(wind(dx)/2, wind(dy)/2, 1) ;
	setcolor(0) ;
	/* indicatori */
	setwindow(Windic) ;
	noclip() ;
	bord(-1,0,wind(dx)+2,wind(dy),8) ;
	bord(4,2,wind(dx)-9,wind(dy)-4,4) ;

	setwindow(Wicons) ;
	noclip() ;
	/* figurine */
	bord(-1,0,wind(dx)+2,wind(dy),8) ;
	settextjustify(CENTER_TEXT,CENTER_TEXT);
	setusercharsize(5,6,3,5);
	selectfont(2,0);
	for (i=0; i<=3; ++i) {
		w=7+i*(dst+10) ;
		bord(w,1,dst-1,dsy,4) ;
	}
	setfillpattern(Fills[G12],1);
	setfillstyle(USER_FILL,1) ;
	floodfill(1,wind(dy)/2,0) ;
	setfillpattern(Fills[bianco],1);
	for (i=0; i<=3; ++i) {
		w=7+i*(dst+10) ;
		bar(w,dsy+2,w+dst-1,wind(dy)-2) ;
		outtextxy(w+dst/2-1, 2+dsy+d2y/2, titl[i] ) ;
	}
	setcolor(0);

	/* inventari */
	setwindow(Woggs) ;
	noclip() ;
	bord(-1,0,wind(dx)+2,wind(dy),8) ;
	bord(4,2,40,18,4);
	bord(4,wind(dy)/2+1,40,18,4);
	bord(4+45,2,wind(dx)-8-45,wind(dy)/2-3,4) ;
	bord(4+45,wind(dy)/2+1, wind(dx)-8-45, wind(dy)/2-3, 4) ;

	setwindow(Wstatus) ;
	noclip() ;
	/* campiture */
	setfillpattern(Fills[grigio],1) ;
	setfillstyle(USER_FILL,1) ;
	floodfill(16,8,0) ;
	setcolor(1) ;
	bord(0,0,wind(dx),wind(dy),16) ;

	setwindow(Windic) ;
	noclip() ;
	setfillpattern(Fills[G12],1);
	noclip() ;
	floodfill(1,wind(dy)/2,0) ;
	setwindow(Wstatus) ;
	noclip() ;
	setfillpattern(Fills[G12],1) ;
	floodfill(sx3+1,sy3+dy3/2,0) ;
	/* frecce */
	setcolor(0);
	arrow(sx3+4+4,sy3+2+18+3,39);
	arrow(sx3+4+4,sy3+dy3/2+1+18+3,39);
	/* titolo */
	selectfont(GOTHIC_FONT,0) ;
	settextjustify(CENTER_TEXT,TOP_TEXT) ;
	setusercharsize(1,1,1,2) ;
	setcolor(1) ; /* fondo bianco */
	for (i=-1; i<=1; ++i)
		for (j=-1; j<=1; ++j)
			outtextxy(wind(dx)/2+i,-4+j,s) ;
	setcolor(0) ; /* campo nero */
	outtextxy(wind(dx)/2,-4,s) ;

	/* barrette */
	setwindow(Windic) ;
	setviewport(420+wind(x0)+8,wind(y0)+4,420+wind(x0)+wind(dx)-8,wind(y0)+wind(dy)-3,1) ;
	setcolor(0) ;
	setlinestyle(SOLID_LINE,0,1) ;
	setusercharsize(3,2,2,3);
	settextjustify(RIGHT_TEXT,CENTER_TEXT) ;
	selectfont(2,0) ;
	z=1 ;
	for (q=punti; q<=antipatia; ++q) {
		outtextxy(86,z+1,(char*)n_barrette[q]) ;
		bord(89,z-1,54,5,2) ;
		z+=5;
	}


} /* borders */

void addicns(void)
{
	int i ;

	/* icone inventari */
	setwindow(Woggs) ;
	a.x=31;
	a.y=13;
	memcpy(a.z,icona+12*4*14,4*14) ;
	putimage(8,8,&a,NOT_PUT);
	memcpy(a.z,icona+13*4*14,4*14) ;
	putimage(8,wind(dy)/2+6,&a,NOT_PUT);

	/* simboli "%" */

	setwindow(Windic) ;
	settextjustify(LEFT_TEXT,TOP_TEXT) ;
	setusercharsize(1,1,1,2);
	selectfont(2,0) ;
	for(i=punti; i<=antipatia; ++i)
	outtextxy(wind(dx)-16-2,wind(yc)*i+wind(xc),"%") ;
}

void main(void)
{
	FILE * f ;
	size_t sz ;
	void *pt ;

	initgraphscreen(CGA) ;
	initwindows() ;
	loadenviron("CGA") ;
	Wstatus = gimmewindow("STATUS  ","STATUS  ") ;
	Windic  = gimmewindow("STATUS  ","INDIC   ") ;
	Wicons  = gimmewindow("STATUS  ","ICONS   ") ;
	Woggs   = gimmewindow("STATUS  ","OGGS    ") ;
	Wmyoggs = gimmewindow("STATUS  ","MYOGGS  ") ;
	Wloggs  = gimmewindow("STATUS  ","LOGGS   ") ;
	icona = loadfile ( grpath ( "icons.byt" ) ) ;

	setwindow(Woggs) ;
	wind(y0) -= 3 ;
	setwindow(Wicons) ;
	wind(y0) -= 2 ;
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
	f = fopen("cga\\status.byt","wb") ;
	fwrite(pt,sz,1,f);
	fclose(f);
	getch() ;
	restorecrtmode() ;
}
