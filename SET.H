/*
	Header file "SET.H"
	Gestione del tipo di dato SET da linguaggio C
	Copyright 1989 Corno Fulvio per Jena-Soft
*/

#include <mem.h>

#define setof(y,N)	    unsigned char (y)[((N)+7)>>3]
#define belongs(x,y)	    ((y)[(x)>>3]&(1<<((x)&7)))
#define addset(x,y)	    ((y)[(x)>>3]|=(1<<((x)&7)))
#define subset(x,y)	    ((y)[(x)>>3]&=~(1<<((x)&7)))
#define nullset(y)	    (setmem(y,sizeof(y),0))
#define copyset(dst,src)    (memmove(dst,src,sizeof(src)))
#define cmpset(st1,st2)     (!memcmp(st1,st2,sizeof(st2)))

/* uso delle MACRO :

    PASCAL			    " C "
    ---------------------	    -----------------------
    var y:set of [0..N] ;	setof(y,N) ;
    ( x in y )			belongs(x,y)
    y := y + [x] ;		addset(x,y) ;
    y := y - [x] ;		subset(x,y) ;
    y := [] ;			nullset(y) ;
    Ydst := Ysrc ;		copyset(Ydst,Ysrc) ; N.B. stessa lunghezza!
    ( Y1 = Y2 ) 		cmpset(Y1,Y2)	      "      "       "
*/
