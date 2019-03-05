/* Disegna la pergamena con tecnica frattale */

#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <dos.h>
#include <mem.h>
#include <assert.h>

#include "drivers.h"

double h ;

double scale ;
int color,dx,dy ;

void generaX(int x1, int x2, double y1, double y2)
{
	double y ;
	int x ;

	if ((x2-x1)<=1) putpixel(floor((x1+x2)/2.0+0.5),
		floor((y1+y2)/2.0+0.5),color) ;
	else if (!kbhit()) {
		y=scale*((rand()-16384.0)/32767.0)*exp(2.0*h*log((x2-x1)/(double)dx))+
			(y1+y2)/2.0 ;
		x=(x1+x2) / 2 ;
		generaX(x1,x,y1,y) ;
		generaX(x,x2,y,y2) ;
	}
} ; /*generaX*/

void lineaX(int xx1, int yy1, int xx2, int yy2, int col, double scle)
{
	scale = scle/2. ;
	color = col ;
	dx = xx2-xx1 ;
	generaX(xx1,xx2,yy1,yy2) ;
} ;
void generaY(int y1, int y2, double x1, double x2)
{
	double x ;
	int y ;

	if ((y2-y1)<=1) putpixel(floor((x1+x2)/2.0+0.5),
		floor((y1+y2)/2.0+0.5),color) ;
	else if (!kbhit()) {
		x=scale*((rand()-16384.0)/32767.0)*exp(2.0*h*log((y2-y1)/(double)dy))+
			(x1+x2)/2.0 ;
		y=(y1+y2)/2 ;
		generaY(y1,y,x1,x) ;
		generaY(y,y2,x,x2) ;
	}
} ; /*generaX*/

void lineaY(int xx1, int yy1, int xx2, int yy2, int col, double scle)
{
	scale = scle ;
	color = col ;
	dy = yy2-yy1 ;
	generaY(yy1,yy2,xx1,xx2) ;
} ;

typedef char riga[80] ;

void main(void)
{
	FILE * fl ;
	char * pt ;
	size_t size ;

	srand(2) ;
	initgraphscreen(CGA) ;
	h=0.2 ;
	lineaX(8,305/2,632,305/2,1,5) ;/* limite sup. */
	ellipse(631,310/2,0,90,8,6/2) ;/* angolo TR */
	lineaY(638,310/2,625,392/2,1,3) ; /* limite dx */
	line(625,392/2,634,392/2) ;
	ellipse(634,395/2,270,90,5,3/2) ; /* bordo dx rotolo */
	lineaX(8,398/2,634,398/2,1,2) ;/* limite inf. */
	ellipse(8,392/2,180,270,7,6/2) ;/* bordo BL */
	lineaY(15,310/2,1,392/2,1,3) ;/* limite sx */
	ellipse(8,310/2,90,270,7,6/2) ;/* bordo TL */
	line(8,316/2,15,316/2) ;
	floodfill(350,320/2,1) ;
	setcolor(0) ;
	ellipse(7,310/2,300,45,7,7/2) ; /* rotolo sup. */
	ellipse(6,311/2,270,90,5,5/2) ;
	ellipse(6,310/2,90,270,3,4/2) ;
	ellipse(6,311/2,270,90,2,3/2) ;
	ellipse(6,310/2,90,270,1,2/2) ;
	lineaX(8,392/2,626,392/2,0,2) ;/* rotolo inf. */
	ellipse(8,395/2,270,90,4,3/2) ;
	ellipse(8,394/2,90,270,4,2/2) ;
	ellipse(7,395/2,270,90,3,1/2) ;

	/* scritte interne */
	setviewport(16,306/2,639-8,389/2,1) ;
	setusercharsize(18,10,5,8) ;
	selectfont(4,0) ;
	settextjustify(CENTER_TEXT,CENTER_TEXT) ;
	outtextxy(307,7,"Sancti Georgii Castrum") ;
	setusercharsize(2,3,2,5) ;
	settextjustify(LEFT_TEXT,CENTER_TEXT) ;
	selectfont(7,0);
	moveto(10,45/2);
	outtext("Gioco di avventura e fantasia di ");
	selectfont(1,0) ;
	setusercharsize(1,1,2,5) ;
	outtext("Jena-Soft") ;
	selectfont(3,0);
	setusercharsize(2,3,2,5) ;
	outtext(" 1988");
	setusercharsize(7,10,3,7) ;
	selectfont(10,0) ;
	settextjustify(CENTER_TEXT,BOTTOM_TEXT) ;
	outtextxy(307,39,"Fulvio Corno  Stefano Roletti  Paolo Galetto") ;

	setviewport(0,0,getmaxx(),getmaxy(),1) ;
	fl=fopen("cga\\pergam.byt","wb") ;
	size = imagesize(0,150,639,199) ;
	pt = malloc(size) ;
	assert(pt) ;
	getimage(0,150,639,199,pt) ;
	putimage(0,0,pt,COPY_PUT) ;
	fwrite(pt+4,size-4,1,fl) ;
	fclose(fl) ;
	getch();
	restorecrtmode() ;
} ;
