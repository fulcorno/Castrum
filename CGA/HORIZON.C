/* programma HORIZON.C
 * disegna il bordo dell'orizzonte artificiale e salva la finestra
 */

#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <stdlib.h>

int x,y,i;
double x2,y2,as2;
unsigned size ;
void *pt;
FILE *out ;
double aspectglb ;
int cx=64 ;
int cy=27 ;

void main(void)
{
	x=CGA; y=CGAHI;
	initgraph(&x,&y,"");
	getaspectratio(&x,&y) ;
	aspectglb = (double)x/(double)y ;
	as2=aspectglb*aspectglb;
	setviewport(0,0,getmaxx(),getmaxy(),0) ;
	for (x=-60; x<=60; ++x)
		for (y=-3; y<=-1; ++y)
			if ((x+y)&1) putpixel(x+cx,-y+cy,1);
	for (x=61; x<=62; ++x)
		for (y=-3; y<=-1; ++y)
			if ((x&1)||(y&1)) { putpixel(x+cx,-y+cy,1); putpixel(-x+cx,-y+cy,1) ; }
	for (x=-62; x<=62; ++x)
		for (y=0; y<=27; ++y) {
			x2 = x*x;
			y2 = y*y;
			if (x2+y2/as2 <=20) {
				putpixel(x+cx,-y+cy,1);
				putpixel(x+cx,y+cy,1) ;
			} else if (x2+y2/as2 <=3481 ) {
				if (!((x+y)&3) && !((x-y)&3)) putpixel(x+cx,-y+cy,1) ;
			} else if (x2+y2/as2 <=3721) {
				if ((x+y)&1) putpixel(x+cx,-y+cy,1) ;
			} else if (x2+y2/as2 <=3969) {
				if ((x&1)||(y&1)) putpixel(x+cx,-y+cy,1) ;
			}
		}
	line(-60+cx,3+cy,60+cx,3+cy);
	getch() ;
	size=imagesize(0,0,127,31) ;
	pt= malloc((int)size) ;
	getimage(0,0,127,31,pt) ;
	putimage(320,100,pt,COPY_PUT) ;
	out = fopen ("cga\\horizon.byt", "wb");
	fwrite(pt,1,size,out) ;
	fclose(out) ;
	closegraph();
}