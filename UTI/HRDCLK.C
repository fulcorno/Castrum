/*
 * programma per leggere/settare la data colloquiando direttamente con
 * l'orologio hardware dell'M24.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

static int yers[] = { 0, 366, 366+365, 366+2*365, 366+3*365, 2*366+3*365,
	2*366+4*365, 2*366+5*365 } ;
static int days[] = { 31,28,31,30,31,30,31,31,30,31,30,31 } ;

void main(void)
{
	union REGS r1,r2,r3 ;
	unsigned date,yr,mt,dy ;
	int i ;

	r1.h.ah = -1 ; /* write Hardware clock */
	r1.x.bx = 10 ;
	r1.h.ch = 18 ;
	r1.h.cl = 34 ;
	int86(0x1A, &r1, &r2) ;
	if(r2.h.ah != 0) {
		printf("Error in setting HW clock\n") ;
		exit(1) ;
	}

	r1.h.ah = -2 ; /* read Hardware clock */
	int86(0x1A,&r1,&r2) ;
	date = r2.x.bx ;

	r2.h.ah = 0x2D ; /* set time */
	/* intdos(&r2,&r3) ; */
	/* if(r3.h.al) printf("Error in setting time\n") ;*/
	yr = 7 ;
	while (date<=yers[yr]) yr-- ;
	date -= yers[yr] ;
	if (!(yr%4)) days[1]++ ;
	yr += 1984 ;
	i = 0 ;
	while (date>=days[i]) {
		date -= days[i++] ;
	}
	mt = i+1 ;
	dy = date + 1 ;
	r3.h.ah = 0x2B ;
	r3.x.cx = yr ;
	r3.h.dh = mt ;
	r3.h.dl = dy ;
	/* intdos(&r3,&r1) ;*/
	/* if(r1.h.al) printf("Error in setting date\n") ;*/
	printf( "Ore %02d:%02d:%02d.%02d del %2d/%2d/%4d\n", r2.h.ch, r2.h.cl,
		r2.h.dh, r2.h.dl, dy,mt,yr ) ;
}
