/*
 * KEYS2.c
 * Disegna (in modo parametrizzato) la mascherina dei tasti funzione
 * Versione 2: senza ombre e tasti in real-time.
 */

#include <graphics.h>
#include <conio.h>
#include <stdlib.h>

/*#########################*/

#define ROWS 5
#define COLS 2

#define XSUPPRESSED { {1,0,1}, {0,1,0}, {1,0,1} }

#define EXT_X 80
#define EXT_Y 52
#define INT_X 60
#define INT_Y 36

#define DIST_X 101
#define DIST_Y 72

#define OFS_X 72
#define OFS_Y 52

#define SHADE 32
/* MUST be EVEN */
 
/*##############################*/
 
int suppressed[COLS][ROWS]
#ifdef SUPPRESSED
= SUPPRESSED
#endif
 ;
 
typedef char filltype[8] ;
 
filltype fill[] = {
#define FILL_0 0
   { 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00 } ,
#define FILL_25 1
   { 0xAA, 0x00, 0xAA, 0x00,  0xAA, 0x00, 0xAA, 0x00 } ,
#define FILL_50 2
   { 0x55, 0xAA, 0x55, 0xAA,  0x55, 0xAA, 0x55, 0xAA } ,
#define FILL_75 3
   { 0xFF, 0xAA, 0xFF, 0xAA,  0xFF, 0xAA, 0xFF, 0xAA } ,
#define FILL_100 4
   { 0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF }
} ;
 
#define TOT_X (2*OFS_X+(COLS-1)*DIST_X)
#define TOT_Y (2*OFS_Y+(ROWS-1)*DIST_Y)

#define X1 0
#define Y1 0
#define X2 EXT_X
#define Y2 Y1
#define X3 X1
#define Y3 EXT_Y
#define X4 X2
#define Y4 Y3

#define X5 EXT_X/2-INT_X/2
#define Y5 EXT_Y/2-INT_Y/2
#define X6 EXT_X/2+INT_X/2
#define Y6 Y5
#define X7 X5
#define Y7 EXT_Y/2+INT_Y/2
#define X8 X6
#define Y8 Y7

void *tasto, *ombra ;
 
void rectround(int x1, int y1, int x2, int y2, int xradius, int yradius)
{
    line(x1+xradius,y1, x2-xradius,y1) ;
    line(x1+xradius,y2, x2-xradius,y2) ;
    line(x1,y1+yradius, x1,y2-yradius) ;
    line(x2,y1+yradius, x2,y2-yradius) ;
    ellipse(x1+xradius,y1+yradius,  90,180, xradius,yradius) ;
    ellipse(x1+xradius,y2-yradius, 180,270, xradius,yradius) ;
    ellipse(x2-xradius,y2-yradius, 270,  0, xradius,yradius) ;
    ellipse(x2-xradius,y1+yradius,   0, 90, xradius,yradius) ;
}

void sfondo(int complete)
{
    setcolor(1) ;
    setfillpattern(fill[FILL_75],1) ;
    bar(OFS_X-EXT_X/2,OFS_Y-EXT_Y/2, TOT_X-OFS_X+EXT_X/2, TOT_Y-OFS_Y+EXT_Y/2);
    if(complete) {
	rectround(0,0,TOT_X,TOT_Y, OFS_X-EXT_X/2,OFS_Y-EXT_Y/2) ;
	floodfill(OFS_X-EXT_X/2, (OFS_Y-EXT_Y/2)/2, 1) ;
    }
}

void createshapes(void)
{
    unsigned size ;
    int coords[12] = { 0,SHADE, 0,SHADE+EXT_Y, EXT_X,SHADE+EXT_Y,
	EXT_X+SHADE,EXT_Y, SHADE,EXT_Y, SHADE,0 } ;

    /* TASTO */
    setfillpattern(fill[FILL_0],1) ;
    bar(0,0,EXT_X,EXT_Y) ;
    setcolor(1) ;
    rectangle(X1,Y1, X4,Y4) ;
    rectangle(X5,Y5, X8,Y8) ;
    line(X1,Y1, X5,Y5) ;
    line(X3,Y3, X7,Y7) ;
    line(X4,Y4, X8,Y8) ;
    line(X2,Y2, X6,Y6) ;
    setfillpattern(fill[FILL_50],1) ;
    floodfill((X1+X2)/2, (Y1+Y5)/2,1) ;
    setfillpattern(fill[FILL_100],1) ;
    floodfill((X6+X2)/2, (Y2+Y4)/2,1) ;
    floodfill((X5+X6)/2, (Y5+Y7)/2,1) ;
    setcolor(0) ;
    line(X6,Y6, X8,Y8) ;
    /*line(X1,Y1, X2,Y2) ;*/
    line(X1,Y2, X3,Y3) ;
    line(X3,Y3, X4,Y4) ;
    size = imagesize(X1,Y1, X4,Y4) ;
    tasto = malloc(size) ;
    getimage(X1,Y1, X4,Y4, tasto) ;
    putimage(X1,Y1, tasto, XOR_PUT) ;

    /* OMBRA */
    setcolor(0) ;
    setfillpattern(fill[FILL_50],1) ;
    fillpoly(6,coords) ;
    size = imagesize(0,0,EXT_X+SHADE,EXT_Y+SHADE) ;
    ombra = malloc(size) ;
    getimage(0,0, EXT_X+SHADE, EXT_Y+SHADE, ombra) ;
    putimage(0,0, ombra, NOT_PUT) ;
    getimage(0,0, EXT_X+SHADE, EXT_Y+SHADE, ombra) ;
    putimage(0,0, ombra, XOR_PUT) ;
}
    int top[] = { X1,Y1, X5,Y5, X6,Y6, X2,Y2 } ;
    int white[] = { X2,Y2, X6,Y6, X5,Y5, X7,Y7, X8,Y8, X4,Y4 } ;
    int black[] = { X1,Y1, X5,Y5, X7,Y7, X8,Y8, X4,Y4, X3,Y3 } ;
    int lines[] = { X1,Y1, X5,Y5, X7,Y7, X3,Y3 } ;
    int nolines[] = { X2,Y2, X6,Y6, X8,Y8 } ;

void buttons(void)
{
    int i,j ;
    int x,y ;


    for(i=0; i<COLS; ++i) for(j=ROWS-1; j>=0; --j)
    {
	if (suppressed[i][j]) continue ;
	x = OFS_X+i*DIST_X-EXT_X/2 ;
	y = OFS_Y+j*DIST_Y-EXT_Y/2 ;

	setviewport(SHADE+x,y, getmaxx(),getmaxy(), 0) ;
	setfillpattern(fill[FILL_0],1) ;
	fillpoly(6,black) ;
	setfillpattern(fill[FILL_50],1) ;
	fillpoly(4, top) ;
	setfillpattern(fill[FILL_100],1) ;
	fillpoly(6, white) ;
	setcolor(1) ;
	drawpoly(4,lines) ;
	setcolor(0) ;
	drawpoly(3,nolines) ;
    }
}

void main(void)
{
    int gd,gm,ct ;
    gd = ATT400 ;
    gm = ATT400HI ;
    initgraph(&gd,&gm,"") ;
    setviewport(SHADE,0,getmaxx(),getmaxy(), 0 ) ;
    setfillstyle(USER_FILL,1) ;
    /*createshapes() ;*/
    sfondo(1) ;
    for(ct=0; ct<100; ct++) {
	setviewport(SHADE,0,getmaxx(),getmaxy(), 0 ) ;
	sfondo(0) ;
	buttons() ;
    }
    getch() ;
    closegraph() ;
}