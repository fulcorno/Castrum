/* modulo "scroll.c" */

/* procs per scrivere nella pergamena con caratteri gotici */

/* INTERFACE */

#include <graphics.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <mem.h>
#include <stdarg.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"

#include "scroll.h"

/* implementation */

#define Lpos	4
#define BIM		"\x01B" "L"

typedef byte chrrow [2] ;	/* 1 riga scansione = 16 bit */
typedef chrrow character [20] ;	/* 1 carattere = 20 righe scansione */
typedef character bigfnt [256] ;	/* font = 256 caratteri */
typedef char widths [256] ;	/* larghezza effettiva di ogni carattere */
typedef byte pergamena [100][80] ;	/* bitmap */

static bigfnt 		* font ;	/* puntatori alle strutture linkate insieme */
static pergamena 	* scrollimg ;
static widths 		* Wchars ;
static int	sx1,sy1,	/* TL pergamena */
			sx2,sy2,	/* BR pergamena */
			sx3,sy3,	/* TL gotici (in CARATTERI) */
			sx4,sy4 ;	/* BR gotici (   "   "    ) */

extern bigfnt bigfont ;
extern pergamena pergIMG ;
extern widths FntDim ;

/* Text Device Driver per la pergamena */

static byte buffer [22][80] ; /* alto 20+1 pixel+ 1 riga bianca */
static byte posB, posP ;
static char *s ;

static void send(void)/* scrolla in su di una riga di 20+1 pixel mostrando il contenuto di "buffer" */
{
	int i,j,i1,i2,grf ;
	byte b,v,k1,k2,b1 ;

	if ((statoAUX.stampante==s_solotxt)||(statoAUX.stampante==s_graftxt))
		fprintf(stdprn,"\n") ;
	if (Badr[0]==Badr[1]) {
	grf=2;
	for(i=0; i<=10; ++i)
		for(j=sx3; j<sx4; ++j)
			buffer[i*2][j] &= buffer[i*2+1][j] ;
	} else grf=1 ;
	for (i=0; i<=20/grf; ++i) {
		for(j=sy3; i<sy4; ++j)
			memmove(MK_FP(0xB800,Badr[j]+sx3),
				MK_FP(0xB800,Badr[j+1]+sx3), sx4-sx3+1) ;
		memmove(MK_FP(0xB800,Badr[sy4]+sx3), & buffer[i*grf][sx3], sx4-sx3+1) ;
	} /*for i*/
	if ((statoAUX.stampante==s_sologoth)||(statoAUX.stampante==s_grafgoth)) {
		for (b=0; b<=2; ++b) {
			b1=b*8 ;
			fprintf(stdprn,"\x01B" "3" "\x018") ;
			fprintf(stdprn,"%s%c%c",BIM,(posB-sx3+1)<<3,(posB-sx3+1)>>5) ;
			for(i1=sx3; i1<=posB; ++i1) {
				k1=0x80;
				for(i2=0; i2<=7; ++i2) {
					v=0 ;
					k2=0x80 ;
					for(j=b1; j<=b1+7; ++j) {
						if ( (j<=20)&& !(buffer[j][i1] & k1) ) v |= k2 ;
						k2 >>= 1 ;
					} ;
					fprintf(stdprn,"%c",v) ;
					k1 >>= 1 ;
				} /* i2 */
			} /* i1 */
			fprintf(stdprn,"\n") ;
		} /* b */
		fprintf(stdprn,"%c%c",27,'2') ;
	}
	posP=Lpos ;
	posB=sx3 ;
	setmem(buffer,sizeof(buffer), 0xff);
} ; /*send*/

static void outch(char c)
{
	typedef byte bufrow [80] ;
	byte w,j ;
	character * shape ;
	chrrow * t ;
	bufrow * p ;

	w = (*Wchars)[c] ;	/* sua larghezza */
	shape = &(*font)[c] ;	/* sua immagine */
	/* aggiungi il carattere alla posP:posB */
	for(j=0; j<=19; ++j) {
		t=&(*shape)[j] ;
		p=&buffer[j] ;
		/* sovrappongo l'immagine */
		(*p)[posB] &= ~((*t)[0]>>posP) ;
		(*p)[posB+1] &= ~ (((*t)[0]<<(8-posP))|((*t)[1]>>posP)) ;
		(*p)[posB+2] &= ~ ((*t)[1]<<(8-posP)) ;
	} /*for j*/
	posP += w ;          /* sposta il cursore */
	posB += posP>>3 ;
	posP &= 7 ;
} ;

static void scrollopen(void)
{
	/* inizializza il buffer */
	setmem(buffer, sizeof(buffer), 0xff) ;
	/* inizializza il cursore */
	posB = sx3;
	posP = Lpos;   /* 1^ pixel = sx3*8 + lpos */
	strcpy (s, "") ;
} ;

static void scrollout (char c)
{
	int j,ls,ct ;

	if ((statoAUX.stampante==s_solotxt)||(statoAUX.stampante==s_graftxt))
		fprintf(stdprn,"%c",c) ;
	if ((c==' ')||(c=='\n')) {
		ls=strlen(s) ;
		ct=0 ;
		for (j=0; j<ls; ++j)
			ct += (*Wchars)[s[j]] ;
		if (((posB<<3)+posP+ct) > ((sx4<<3)-Lpos)) send() ;
		for (j=1; j<ls; ++j) outch(s[j]) ;
		outch(' ') ;
		strcpy(s,"");
	}
	if (c=='\n') {
		send() ;
		posB=sx3;
		posP=Lpos;
	}
	if ((c!=10)&&(c!=13)&&(c!=32)) {
		int l = strlen(s) ;
        s[l] = c;
		s[l+1] = '\0' ;
	}
} ;

void initscroll(void) /* pubblica */
/* mostra l'immagine di copyright */
{ /*init code*/
	int x,y ;
	/* Definisce le dimensioni delle finestre */
	sx1 = 0 ;	sy1 = 301 ;		/* TL pergamena */
	sx2 = 639;	sy2 = 399 ; 	/* BR     "     */
	sx3 = 2 ;	sy3 = sy1+6 ;	/* TL scritte */
	sx4 = 77 ;	sy4 = sy2-9 ;	/* BR    "    */
	/* attiva la finestra */
	scrollimg = & pergIMG ;
	/* attiva il font gotico */
	font = & bigfont ;
	Wchars = & FntDim ;

	for (y=sy1; y<=sy2; ++y)
		memmove(MK_FP(0xB800,Badr[y]), (*scrollimg)[y-sy1],80) ;
	if (Badr[0]==Badr[1])
		for (y=sy1; y<=sy2; ++y)
			if (!(y%2))
				for (x=0; x<=79; ++x)
					* (char *) MK_FP(0xB800,Badr[y]+x) &= (*scrollimg)[y][x] ;
	scrollopen() ;
} ;


int pprintf(char * format, ... )  /* pubblica */
{
	va_list ap ;
	int res ;
	static char s [256], *c ;

	ap = va_start(ap, format) ;
	res = vsprintf(s,format,ap) ;
	for (c=s; *c; scrollout(*(c++)) )
		;
	va_end (ap) ;
	return res ;
}
