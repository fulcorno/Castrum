/*
 * modulo "gotico.c"
 *
 * procs per scrivere nella pergamena con caratteri gotici
 *
 */

/* INTERFACE */

#include <graphics.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <mem.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <bios.h>

#include "set.h"
#include "drivers.h"
#include "windows.h"
#include "globals.h"

#include "gotico.h"

/* IMPLEMENTATION */

#define Lpos	4
static char BIM [] = { 0x1B, 'L', 0 } ; /* active bit image mode */
static char INL [] = { 0x1B, '3', 0x18, 0 } ; /* set interline */
static char IND [] = { 0x1B, '2', 0 } ; /* restore default interline */

typedef byte chrrow [2] ;	/* 1 riga scansione = 16 bit */

static int  sx1,sy1,	    /* TL pergamena */
	    sx2,sy2,	    /* BR pergamena */
	    sx3,sy3,	    /* TL gotici (in CARATTERI) */
	    sx4,sy4,	    /* BR gotici (   "   "    ) */
	    hgh ; /* altezza carattere */

static int Wscroll ;
static int Wtesto ;

static chrrow * bigfont ;
static byte * FntDim ;

/*
 * Text Device Driver per la pergamena
 */

static byte * buffer ; /* alto 20+1 pixel+ 1 riga bianca */
static byte posB, posP ;
static char gothbuf[100] ;

/*
 * Scrolla in su di una riga mostrando il contenuto di "buffer"
 */
static void near pascal send(void)
{
    register int i,j,i1,sz ;
    byte b,v,k1,k2 ;
    byte * bufptr ;

    if ((statoAUX.stampante==s_solotxt)||(statoAUX.stampante==s_graftxt))
	biosprint(0,'\n',0) ;

    sz = sx4-sx3+1 ;
    for (i=0,bufptr=buffer+sx3; i<=hgh; ++i,bufptr+=80) {
	/* N.B. nel for() ci vuole proprio <=, non < */
	for(j=sy3; j<sy4; ++j)
	    movedata(0xB800,Badr[j+1]+sx3,0xB800,Badr[j]+sx3, sz) ;
	memmove(MK_FP(0xB800,Badr[sy4]+sx3), bufptr, sz) ;
    }
    if ((statoAUX.stampante==s_sologoth)||(statoAUX.stampante==s_grafgoth)) {
	/* output line in graphics mode: DA FARE IN ASSEMBLER */
	for (b=0; b<=16; b+=8) { /* up to 3 lines, 8 dots each */
	    fprintf(stdprn,"%s%s%c%c",INL,BIM,(posB-sx3+1)<<3,(posB-sx3+1)>>5);
	    for(i1=sx3; i1<=posB; ++i1) {
		for(k1=0x80; k1; k1>>=1) {
		    for(j=b, k2=0x80, v=0; k2 && (j<=hgh); ++j,k2>>=1)
			if ( !(buffer[j*80+i1] & k1) )
			    v |= k2 ;
			biosprint(0,v,0) ;
		} /* k1 */
	    } /* i1 */
	    biosprint(0,'\n',0) ;
	} /* b */
	fprintf(stdprn,"%s",IND) ;
    }
    posP=Lpos ;
    posB=sx3 ;
    setmem(buffer,80*(hgh+1), 0xFF);
} /* send */

/*
 * Memorizza in buffer[] il carattere 'c', senza controllare alcun limite
 */
static void near pascal outch(char c)
{
    register int j ;
    byte * t ;
    byte * p ;
    byte * bufptr ;

    t = bigfont[(byte)c*hgh] ;	    /* sua immagine */
    /* aggiungi il carattere alla posP:posB */
    for(j=0,bufptr=buffer+posB; j<hgh; ++j,bufptr+=80) {
	p = bufptr ;
	/* sovrappongo l'immagine */
	*(p++) &= ~(*t>>posP) ;
	*(p++) &= ~((*t<<(8-posP))|(t[1]>>posP)) ;
	*p &= ~(t[1]<<(8-posP)) ;
	t += 2 ;
    } /*for j*/
    posP += FntDim[(byte)c] ;	    /* sposta il cursore */
    posB += posP>>3 ;
    posP &= 7 ;
}

/*
 * Inizializza le variabili necessarie per il funzionamento della pergamena
 */
static void near pascal goticoopen(void)
{
    /* inizializza il buffer */
    setmem(buffer, 80*(hgh+1), 0xFF) ;
    /* inizializza il cursore */
    posB = sx3;
    posP = Lpos;    /* 1^ pixel = sx3*8 + lpos */
    *gothbuf = '\0' ;
}

/*
 * Manda un carattere sulla pergamena e/o alla stampante, gestendo scrolling
 * e wrap-around
 */
static void near pascal goticoout (char c)
{
    register int ct ;
    char * w ;

    if ((statoAUX.stampante==s_solotxt)||(statoAUX.stampante==s_graftxt))
	biosprint(0,c,0) ;
    if (isspace(c)) {
	for (ct=0, w=gothbuf; *w; ct += FntDim[(byte)*w++] )
	    /*NOP*/ ;
	if (((posB<<3)+posP+ct) > ((sx4<<3)-Lpos)) send() ;
	for (w=gothbuf; *w; outch(*w++))
	    /*NOP*/ ;
	outch(c) ;
	*gothbuf = '\0' ;
    } else {
	int l = strlen(gothbuf) ;
	gothbuf[l] = c ;
	gothbuf[l+1] = '\0' ;
    }
    if (c=='\n') {
	send() ;
	posB=sx3 ;
	posP=Lpos ;
    }
} /* goticoout */

/*
 * Mostra l'immagine di CopyRight iniziale e attiva il sistema. Da chiamare
 * per prima.
 */
void initgotico(void) /* pubblica */
{
    register int y ;
    void *p ;

    FntDim = loadfile (grpath("fntdim.byt")) ;
    bigfont = loadfile (grpath("20x16.byt")) ;
    Wscroll = gimmewindow( "GOTICO  ","PERGAM  ") ;
    Wtesto  = gimmewindow( "GOTICO  ","TESTO   ") ;

    setwindow(Wscroll) ;
    /* Definisce le dimensioni delle finestre */
    sx1 = wind(x0); sy1 = wind(y0);  /* TL pergamena */
    sx2 = wind(x0)+wind(dx); sy2 = wind(y0)+wind(dy);  /* BR */
    sx3 = (sx1+wind(xc))/8 ; sy3 = sy1+wind(yc) ; /* TL scritte */
    sx4 = (sx2-wind(xc))/8 ; sy4 = sy2-3*wind(yc)/2 ; /* BR */

    p = loadfile(grpath("pergam.byt")) ;
    /* visualizza la finestra */
    for (y=sy1; y<=sy2; ++y)
	memmove(MK_FP(0xB800,Badr[y]), &((byte*)p)[(y-sy1)*80],80) ;
    free(p) ;

    setwindow(Wtesto) ;
    hgh = wind(yc) ;
    buffer = calloc( hgh+1,80 ) ;
    if (!buffer) internalerror("Malloc failed","initgotico",0) ;

    goticoopen() ;

} /* initgotico */

#define MAXBYTESOUTPUT 500

/*
 * Equivalente di 'printf()', ma con destinazione la pergamena. Non spedire
 * pi— di 'MaxBytesOutput' (v. appena sopra) caratteri per volta, pena il
 * crash
 */
int pprintf(char * format, ... )  /* pubblica */
{
    va_list ap ;
    int res ;
    char s [MAXBYTESOUTPUT], *c ;

    setwindow(Wtesto) ;
    va_start(ap, format) ;
    res = vsprintf(s,format,ap) ;
    if (res>=MAXBYTESOUTPUT)
	internalerror("Message too long for gothic driver","pprintf",res) ;
    for (c=s; *c; goticoout(*(c++)) )
	/*NOP*/ ;
    va_end (ap) ;
    return res ;
} /* pprintf */
