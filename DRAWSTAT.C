/* Programma DrawStatus */

#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"

/* Barrette, cotillons e oggetti della finestra di Status */

#define sx0 420
#define sy0 0	/* TL finestra */
#define dx0 219
#define dy0 299	/* dim finestra */

#define sx1 8
#define sy1 30	/* indicatori */
#define dx1 203
#define dy1 94

#define sx2 8
#define sy2 128	/* figurine */
#define dx2 203
#define dy2 50
#define dst 40
#define dsy 32
#define d2y 8

#define sx3 8
#define sy3 182	/* inventari */
#define dx3 203
#define dy3 114

char * titl[] = { "POSIZIONE", "VELOCITA`", "STAMPANTE", "FISICO" } ;

void bord (int x, int y, int lx, int ly, int r)
/* rettangolo con angoli smussati */
{
	line(x+r,y,x+lx-r,y) ;
	line(x+r,y+ly,x+lx-r,y+ly) ;
	line(x,y+r,x,y+ly-r) ;
	line(x+lx,y+r,x+lx,y+ly-r) ;
	ellipse(x+r,y+r,90,180,r,r) ;
	ellipse(x+r,y+ly-r,180,270,r,r) ;
	ellipse(x+lx-r,y+ly-r,270,0,r,r) ;
	ellipse(x+lx-r,y+r,0,90,r,r) ;
} ; /* bord */

void arrow(int x, int y, int ln)
{
	line(x,y-4,x,y);
	line(x,y,x+ln,y);
	line(x+ln,y,x+ln-4,y-4);
	line(x+ln,y,x+ln-4,y+4);
} ;

void borders(void)
/* disegna il background dello status. */
{
	int i,j,w,z ;
	t_barrette q ;
	char s[] = "Infurmasiun" ;

	setviewport(sx0,sy0,sx0+dx0,sy0+dy0,1) ;
	setfillstyle(SOLID_FILL,1) ;
	setcolor(1) ;
	setlinestyle(SOLID_LINE,0,1) ;
	bord(0,0,dx0,dy0,16) ;
	floodfill(dx0/2, dy0/2, 1) ;
	setcolor(0) ;
	/* indicatori */
	bord(sx1-1,sy1,dx1+2,dy1,8) ;
	bord(sx1+4,sy1+4,dx1-8,dy1-8,4) ;
	/* figurine */
	bord(sx2-1,sy2,dx2+2,dy2,8) ;
	settextjustify(CENTER_TEXT,CENTER_TEXT);
	setusercharsize(5,6,3,4);
	selectfont(2,0);
	setfillpattern(Fills[nero],1);
	setfillstyle(USER_FILL,1) ;
	setcolor(0);
	bord(sx2+2,sy2+6+dsy,dx2-5,d2y,3);
	for (i=0; i<=3; ++i) {
		w=sx2+7+i*(dst+10) ;
		bord(w,sy2+4,dst-1,dsy,4) ;
		outtextxy(w+dst/2-1, sy2+6+dsy+d2y/2-1, titl[i] ) ;
	}
	setcolor(0);
	/* inventari */
	bord(sx3-1,sy3,dx3+2,dy3,8) ;
	bord(sx3+4,sy3+4,40,36,4);
	bord(sx3+4,sy3+dy3/2+2,40,36,4);
	bord(sx3+4+45,sy3+4,dx3-8-45,dy3/2-6,4) ;
	bord(sx3+4+45,sy3+dy3/2+2, dx3-8-45, dy3/2-6, 4) ;
	/* campiture */
	setfillpattern(Fills[grigio],1) ;
	setfillstyle(USER_FILL,1) ;
	floodfill(16,16,0) ;
	setcolor(1) ;
	bord(0,0,dx0,dy0,16) ;
	setfillpattern(Fills[G12],1);
	floodfill(sx1+1,sy1+dy1/2,0) ;
	floodfill(sx2+1,sy2+dy2/2,0) ;
	floodfill(sx2+dx2/2,sy2+dy2-1,0);
	floodfill(sx3+1,sy3+dy3/2,0) ;
	/* frecce */
	setcolor(0);
	setlinestyle(SOLID_LINE,0,3);
	arrow(sx3+4+4,sy3+4+36+5,39);
	arrow(sx3+4+4,sy3+dy3/2+2+36+5,39);
	/* titolo */
	selectfont(GOTHIC_FONT,4) ;
	settextjustify(CENTER_TEXT,TOP_TEXT) ;
	setcolor(1) ; /* fondo bianco */
	for (i=-1; i<=1; ++i)
		for (j=-1; j<=1; ++j)
			outtextxy(dx0/2+i,-3+j,s) ;
	setcolor(0) ; /* campo nero */
	outtextxy(dx0/2,-3,s) ;
	setviewport(sx0+sx1+8,sy0+sy1+8,sx0+sx1+dx1-8,sy0+sy1+dy1-6,1) ;
	setcolor(0) ;
	setlinestyle(SOLID_LINE,0,1) ;
	setusercharsize(2,3,2,5);
	settextjustify(RIGHT_TEXT,CENTER_TEXT) ;
	selectfont(8,0) ;
	z=1 ;
	for (q=punti; q<=antipatia; ++q) {
		outtextxy(86,z,(char*)n_barrette[q]) ;
		bord(89,z-1,54,8,2) ;
		z+=10;
	}
} ; /* borders */

void main(void)
{
	FILE * f ;
	size_t sz ;
	void *pt ;

	initgraphscreen(0) ;
	borders() ;
	setviewport(sx0,sy0,sx0+dx0,sy0+dy0,1);
	sz=imagesize(0,0, dx0,dy0) ;
	pt = calloc(sz,1) ;
	if (!pt) exit(0) ;
	getimage(0,0, dx0,dy0, pt) ;
	setviewport(0,0,getmaxx(),getmaxy(),1);
	putimage(0,0, pt, COPY_PUT);
/*	f = fopen("status.byt","wb") ;
	fwrite(pt,sz,1,f);
	fclose(f); */
   getch() ;
   restorecrtmode() ;
}
