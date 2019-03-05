/* program MAKEORA.C
 * disegna (e salva) l'immagine dell'orologio di S.G.Castrum
 */

#include <graphics.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <conio.h>

int x,y,i;
double t,aspectglb,AS2,phi;
int ax,ay;
unsigned size ;
void * pt ;
FILE *fout ;

void drawpoint(int x,int y)
{
	 putpixel(x,y,1) ;
}

void drawline(double x1,double y1,double x2,double y2)
{
	 line(floor(x1),floor(y1),floor(x2),floor(y2)) ;
}

void main(void)
{
	 x=CGA; y=CGAHI; initgraph(&x,&y,"");
	 getaspectratio(&ax,&ay) ;
	 aspectglb = (double)ax/(double)ay ;
	 AS2 = aspectglb * aspectglb ;
	 /*quadrante*/
	 for (x=0; x<=119; ++x)
		 for (y=0; y<=49; ++y) {
			 t=(x-60)*(x-60)+(y-25)*(y-25)/AS2;
			 if (t <=1225) drawpoint(x,y) ;
				else if (t<=2500) { if ((x+y)&1) drawpoint(x,y) ; }
				else if ((t<=2809) && ((x&1)||(y&1))) drawpoint(x,y);
		 }
	 /*tacche orarie*/
	 setfillstyle(SOLID_FILL,0);
	 bar(59,25-floor(45*aspectglb),61,25-floor(35*aspectglb));
	 bar(95,24,105,25);
	 bar(15,24,25,25);
	 bar(59,25+floor(35*aspectglb),60,25+floor(45*aspectglb));
     setcolor(0);
	 for (i=0; i<=11; ++i)
		 if (i%3) {
			phi= (double)i/6*M_PI;
			drawline(60+35*cos(phi),25-35*aspectglb*sin(phi),60+44*cos(phi),25-44*aspectglb*sin(phi));
		 }
	 /*corona "rolex"*/
	 setlinestyle(USERBIT_LINE,0xaaaa,1);
	 moveto(50,20);
	 lineto(70,20);
	 lineto(73,15);
	 lineto(65,18);
	 lineto(60,16);
	 lineto(55,18);
	 lineto(47,15);
	 lineto(50,20);
	 /*crocetta centrale*/
	 setlinestyle(SOLID_LINE,0,1);
	 line(59,25,61,25);
	 line(60,24,60,26);
	 /*scritta "swiss"*/
	 settextstyle(SMALL_FONT,HORIZ_DIR,0);
	 setusercharsize(4,4,2,3) ;
	 settextjustify(CENTER_TEXT,CENTER_TEXT);
	 outtextxy(60,35,"SWISS");
	 setcolor(1);
	 fout = fopen("cga\\watch.byt","wb") ;
	 size =imagesize(0,0,119,49) ;
	 pt = malloc((int)size) ;
	 getimage(0,0,119,49,pt) ;
	 putimage(320,100,pt,COPY_PUT);
	 fwrite(pt,size,1,fout);
	 fclose(fout);
	 getch() ;
	 closegraph() ;
}