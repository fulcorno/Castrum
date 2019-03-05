/* Disegna la pergamena con tecnica frattale */

#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <dos.h>
#include <mem.h>

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
	scale = scle ;
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
	int y ;
	riga tmp ;

	srand(2) ;
	initgraphscreen(0) ;
	for (y=10; y>=1; --y) loadfont(y,"") ;
	h=0.2 ;
	lineaX(8,305,632,305,1,5) ;/* limite sup. */
	ellipse(631,310,0,90,8,6) ;/* angolo TR */
	lineaY(638,310,625,392,1,3) ; /* limite dx */
	line(625,392,634,392) ;
	ellipse(634,395,270,90,5,3) ; /* bordo dx rotolo */
	lineaX(8,398,634,398,1,2) ;/* limite inf. */
	ellipse(8,392,180,270,7,6) ;/* bordo BL */
	lineaY(15,310,1,392,1,3) ;/* limite sx */
	ellipse(8,310,90,270,7,6) ;/* bordo TL */
	line(8,316,15,316) ;
	floodfill(350,320,1) ;
	setcolor(0) ;
	ellipse(7,310,300,45,7,7) ; /* rotolo sup. */
	ellipse(6,311,270,90,5,5) ;
	ellipse(6,310,90,270,3,4) ;
	ellipse(6,311,270,90,2,3) ;
	ellipse(6,310,90,270,1,2) ;
	lineaX(8,392,626,392,0,2) ;/* rotolo inf. */
	ellipse(8,395,270,90,4,3) ;
	ellipse(8,394,90,270,4,2) ;
	ellipse(7,395,270,90,3,1) ;
	/* scritte interne */
	setviewport(16,306,639-16,389,1) ;
	setusercharsize(18,10,5,4) ;
	settextstyle(4,HORIZ_DIR,USER_CHAR_SIZE) ;
	settextjustify(CENTER_TEXT,CENTER_TEXT) ;
	outtextxy(303,15,"Sancti Georgii Castrum") ;
	setusercharsize(2,3,4,5) ;
	settextjustify(LEFT_TEXT,CENTER_TEXT) ;
	settextstyle(7,HORIZ_DIR,USER_CHAR_SIZE);
	moveto(10,45);
	outtext("Gioco di avventura e fantasia di ");
	settextstyle(1,HORIZ_DIR,4) ;
	outtext("Jena-Soft") ;
	settextstyle(3,HORIZ_DIR,USER_CHAR_SIZE);
	outtext(" 1988");
	setusercharsize(7,10,3,4) ;
	settextstyle(10,HORIZ_DIR,USER_CHAR_SIZE) ;
	settextjustify(CENTER_TEXT,BOTTOM_TEXT) ;
	outtextxy(303,78,"Fulvio CornoStefano RolettiPaolo Galetto") ;

	fl=fopen("pergam.byt","wb") ;
	for (y=301; y<=399; ++y) {
		memmove(tmp,MK_FP(0xB800,Badr[y]),80) ;
		fwrite(tmp,80,1,fl);
		memmove(MK_FP(0xB800,Badr[y-300]),tmp,80) ;
	}
	fclose(fl) ;
	getch();
	restorecrtmode() ;
} ;
