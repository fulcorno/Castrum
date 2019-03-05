/*
 * module "OBJECTS.C"
 *
 * gestisce la "finestra dell'oggetto"
 *
 */

/* INTERFACE */

#include <graphics.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"
#include "windows.h"

#include "objects.h"

/* IMPLEMENTATION */

typedef byte objm [90][20] ;

static int Wogg, Wtit ;

static byte *pt ;
static byte *p_soffitti ;

static void near pascal header(char * s)
{
    int i ;

    setwindow(Wtit) ;
    bar(1,0,wind(dx)-1,wind(dy)) ;
    setcolor(wind(fgcolor)) ;
    putpixel(1,0,wind(fgcolor)) ;
    putpixel(wind(dx)-1,0,wind(fgcolor)) ;
    putpixel(1,wind(dy),wind(fgcolor)) ;
    putpixel(wind(dx)-1,wind(dy),wind(fgcolor)) ;
    settextjustify( CENTER_TEXT, CENTER_TEXT ) ;
    for (i=0; s[i]; ++i)
	s[i]=toupper(_8to7[s[i]]) ;
    if (textwidth(s)>wind(dx)-4) internalerror("Parameter 's' too long",
	"header",strlen(s)) ;
    outtextxy(wind(xc), wind(yc), s) ;
}

void display(byte *p, const char * s)
{
    int i ;

    for (i=0; i<=wind(dy); ++i)
	memmove(MK_FP(0xB800,Badr[wind(y0)+i]+(wind(x0)>>3)),&p[i*20], 20) ;
    header((char*)s) ;
}

/* ****** PROVVISORIO: RISCRIVERE ***** */

void outobject(int n) /* pubblica */
{
    setwindow(Wogg) ;
    if ((n>=-12)&&(n<=-1)) display( &p_soffitti[(-n-1)*20*(1+wind(dy))],
	n_soffitto[-n-1] ) ;
    else display(pt,"Marchio \"JENA-SOFT\"") ;
}

/* ****** PROVVISORIO: ELIMINARE ******** */

static void near pascal read_it(void)
{
    register int i,j ;

    setwindow(Wogg) ;
    pt = loadfile(grpath("marchio.ogg")) ;
    for (i=0; i<=19; ++i)
	for (j=0; j<=wind(dy); ++j)
	    pt[j*20+i] ^= 0xFF ;
    p_soffitti = loadfile(grpath("soffitti.byt")) ;
}

void initobjects(void) /* pubblica */
{
    Wogg = gimmewindow( "OBJECTS ", "OGG     " ) ;
    Wtit = gimmewindow( "OBJECTS ", "TITOLO  " ) ;
    read_it() ;
}
