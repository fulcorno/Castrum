/* gestione della pagina video STYX */

#include <dos.h>
#include <stdlib.h>

#define regN 6

static const int rgs[regN] = { 0x1F, 0x06, 0x19, 0x10, 0x02, 0x07  } ;
static const int rg[regN]  = { 0x7F, 0x00, 0x64, 0x41, 0x02, 0x01  } ;

static void send(int reg, int value)
/* manda un byte in un registro del CRT 6845 */
{
	outp(0x3D0, reg) ;
	outp(0x3D1, value) ;
}

void cls(int color)
/* prepara lo schermo styx , cancellandolo */
{
	register unsigned int i,cl ;
	cl = ( color | (color <<4) ) << 8 ;
	for (i=0; i<8192; i++) poke (0xB800, i<<1, 0xDD | cl) ;
}

void entergraph(void)
/* setta il CRT 6845 con gli opportuni valori. */
{
	register int i ;
	for (i=0; i<regN; i++) send(i+4,rg[i]);
	outp(0x3D8, 0x09) ; /* no blink */
	cls(0);
}

void exitgraph(void)
/* ristabilisce i modo video a colori 80x25 */
{
	register int i ;
	for (i=0; i<regN; i++) send(i+4,rgs[i]);
	cls(0);
	outp (0x3D8, 0x29) ; /* restore blink */
}

void plot(int x, int y, int color)
/* disegna un punto sul video STYX */
{
	register unsigned int ad ;
	register unsigned char val, mask, col ;
	ad = 1+((80*y+(x>>1))<<1) ;
	mask = (x & 1) ? 0x0F : 0xF0 ;
	col = (x & 1) ? (color<<4) : color ;
	val = peekb(0xB800,ad) ;
	val = (val & mask) | col ;
	pokeb (0xB800,ad, val ) ;
}

void draw(int x1, int y1, int x2, int y2, int c )
/* segmento. L'algoritmo Š di TurboGraphixToolBox */
{
	register int x,y, DX, DY, XS, YS, ct ;

	x=x1; y=y1;
	XS = (x1>x2) ? -1 : 1 ;
	YS = (y1>y2) ? -1 : 1 ;
	DX = abs (x2-x1);
	DY = abs (y2-y1);
	ct = (DX) ? 0 : -1 ;
	while (!((x==x2) && (y==y2))) {
		plot(x,y,c);
		if (ct<0) {
			y+=YS; ct+=DX;
		} else {
			x+=XS; ct-=DY;
		}
	}
}
