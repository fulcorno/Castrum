/*
 * module "room.c"
 *
 * per disegnare le stanze
 *
 */

/* INTERFACE */

#include <graphics.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"
#include "windows.h"
#include "gotico.h"
#include "objects.h"

#include "room.h"

/* IMPLEMENTATION */

/*
 * alloca spazio per le var. globali
 */

indice_porta PassedThrough = 0 ; /* pubblica: settata da agisci.c */

/* n§ vertici max per poligono */
#define poly_size 30

typedef struct { int x,y ; } PointType ;
typedef PointType PolyPoints [poly_size] ;

typedef struct { double _x,_y,_z ; } Point3dType ;
typedef Point3dType Poly3dPoints [poly_size] ;

typedef struct {
    int len ;
    Poly3dPoints R3 ;
    int thick, style, color ;
    FillIndex pattern ;
} polyg ;

typedef enum { sp_niente, sp_chiuso, sp_aperto } stato_porta ;

/* room-drawing constants */

/* altezza del punto di vista in spanne */
#define ptview 30.0

/* lunghezza assi (x,y) in spanne */
#define MX 65.0
#define MY 55.0

/* dimensioni tipiche stanza: 4x3x2.8 m^3 */
#define typ_x 20
#define typ_y 15
#define typ_z 13

/* le seguenti variabili sono dichiarate come globali a questo modulo
 * per motivi di efficienza:
 *  - si evita di passarle continuamente come parametri
 *  - vengono settate e lette solo dalle routines pubbliche
 */ 

/* PUNTATORI alla stanza che si sta disegnando */
static t_luogo * luogo_loc ;
static t_stanza * stanza_loc ;

/* fattori di scala per la grafica */
static double kx,ky,hx,hy ;
/* valori locali (corretti&riscalati) di dim_(xyz) */
static double loc_x,loc_y,loc_z ;

/* indici delle finestre in uso: */
static int Wroom, Wtitolo ;

/*
 * Disegna lo sfondo grigio in cui visualizzare la stanza
 */
static void near pascal roombackground(void)
{
    setwindow(Wroom) ;
    bar ( 1,1, wind(dx)-1,wind(dy)-1 ) ;
    rectangle ( 0,0, wind(dx),wind(dy) );
}

/*
 * scrive il titolo della stanza
 */
static void near pascal roomheader ( char * txt )
{
    setwindow(Wtitolo) ;
    bar ( 0,0, wind(dx),wind(dy) ) ;
    /* arrotonda gli spigolini */
    putpixel (0,0,wind(fgcolor)); putpixel (wind(dx),0,wind(fgcolor));
    putpixel (0,wind(dy),wind(fgcolor));
    putpixel (wind(dx),wind(dy),wind(fgcolor));
    settextjustify (LEFT_TEXT,TOP_TEXT) ;
    if (textwidth(txt)>wind(dx)-8)
	internalerror("Parameter txt too long","RoomHeader",strlen(txt));
    outtextxy ( wind(xc)-textwidth(txt)/2, 1, txt ) ;
}

/*
 * Restituisce in (csi,eta) le coordinate del punto (x,y,z) proiettato sul
 * piano z=0 a partire dal punto (0,0,ptview)
 */
static void near pascal proietta ( const Point3dType * p3, double * csi,
    double * eta )
{
    double tmp ;

    tmp = ptview / (ptview - p3->_z) ;
    *csi = tmp * p3->_x ;
    *eta = tmp * p3->_y ;
}

/*
 * Restituisce le coordinate spaziali P3(_x,_y,_z) del punto (xw,yw), tenendo
 * conto delle dimensioni della stanza (lx,ly,hr) e di quale muro si tratta.
 */
static void near pascal muro ( int xw, int yw, int wall, Point3dType * p3 )
/*
 *    asse yw Â 		      Parametri:
 *	      ³ 		   - numero della parete (wall)
 *    100ÚÄÄÄÄÄÄÄÄÄ¿ (muro)	   - dimensioni del pavimento (dim_x,dim_y)
 *	 ³    ³    ³		   - altezza della stanza (dim_z)
 *    ÄÄ ÀÄÄÄÄÄÄÄÄÄÙ ÄÄÄ> asse xw  - coordinate SUL MURO di un punto (xw,yw)
 *   -100     ³     100
 *
 *	ÚÄÄÄ(3)ÄÄÄ¿ ¿
 *	³	  ³ Ã dim_y
 *     (2)	 (0)Ù
 *	³	  ³
 *	ÀÄÄÄ(1)ÄÄÄÙ
 *	ÀÄÂÄÄÙ
 *	dim_x
 */
{
    p3->_z = (double)yw * loc_z / 100.0 ;
    switch(wall) {
	case 0:
	    p3->_x = loc_x ;
	    p3->_y = - loc_y * (double)xw / 100.0 ;
	    break ;
	case 1:
	    p3->_x = - loc_x * (double)xw / 100.0 ;
	    p3->_y = - loc_y ;
	    break ;
	case 2:
	    p3->_x = - loc_x ;
	    p3->_y = loc_y * (double)xw / 100.0 ;
	    break ;
	case 3:
	    p3->_x = loc_x * (double)xw / 100.0 ;
	    p3->_y = loc_y ;
	    break ;
    } /*switch*/
} /* muro */

/*
 * Come 'muro()', ma in pi— si specifica la profondit… (distanza del punto
 * dal piano (verticale) del muro
 */
static void near pascal muro2 (int xw, int yw, int zw, int wall,
    Point3dType * p3 )
/* Š simile a "muro", con la differenza che l'asse zw Š ortogonale
   al muro e diretto verso il centro della stanza. Il centro stesso
   ha coordinate (xw,yw,zw) = (0,0,1). Si pu• pensare a "muro" come:
   muro ( xw,yw , ... ) == muro2 ( xw,yw, (zw=0) , ... )   */
{
    p3->_z = (double)yw / 100.0 * loc_z ;
    switch(wall) {
	case 0:
	    p3->_x = loc_x * (1.0-(double)zw/100.0) ;
	    p3->_y = - loc_y * (double)xw / 100.0 ;
	    break ;
	case 1:
	    p3->_x = - loc_x * (double)xw / 100.0 ;
	    p3->_y = - loc_y * (1.0-(double)zw/100.0);
	    break ;
	case 2:
	    p3->_x = - loc_x * (1.0-(double)zw/100.0) ;
	    p3->_y = loc_y * (double)xw / 100.0 ;
	    break ;
	case 3:
	    p3->_x = loc_x * (double)xw / 100.0 ;
	    p3->_y = loc_y * (1.0-(double)zw/100.0) ;
	    break ;
    } /*switch*/
} /*muro2*/

/* passa da coordinate logiche [ -MX <= csi <= MX ] a coordinate
 * fisiche in pixel [ 0 <= x <= sx2-sx1 ] : ** presume che sia settata
 * la corretta ViewPort **
 */
/* MACRO double coordX ( double csi ) ; */
#define coordX(csi) ( kx*(csi)+hx )

/* passa da coordinate logiche [ -MY <= eta <= MY ] a coordinate
 * fisiche in pixel [ 0 <= y <= sy2-sy1 ] : ** presume che sia settata
 * la corretta ViewPort **
 */
/* MACRO double coordY ( double eta ) ; */
#define coordY(eta) ( ky*(eta)+hy )

/*
 * Disegna un segmento su di un muro nell'opportuna prospettiva e con i
 * parametri particolari (spessore,stile,...) settati
 */
static void near pascal draw3d( int csi1, int eta1, int csi2, int eta2,
    int wall )
{
    Point3dType P1,P2 ; /* coord. spaziali */
    double a1,b1,a2,b2 ; /* coord. logiche nella finestra */

    muro ( csi1,eta1,wall, &P1 ) ;
    muro ( csi2,eta2,wall, &P2 ) ;
    proietta ( &P1, &a1,&b1 ) ;
    proietta ( &P2, &a2,&b2 ) ;
    line ( coordX(a1),coordY(b1), coordX(a2),coordY(b2) ) ;
} /*draw3d*/

/*
 * E` l'equivalente di "draw3d", usando "muro2"
 */
static void near pascal draw3d2 (int csi1, int eta1, int zeta1,
    int csi2, int eta2, int zeta2, int wall)
{
    Point3dType P1,P2 ;
    double a1,b1,a2,b2 ;

    muro2 ( csi1,eta1,zeta1,wall, &P1 ) ;
    muro2 ( csi2,eta2,zeta2,wall, &P2 ) ;
    proietta ( &P1, &a1,&b1 ) ;
    proietta ( &P2, &a2,&b2 ) ;
    line ( coordX(a1),coordY(b1), coordX(a2),coordY(b2) ) ;
} /*draw3d2*/

/*
 * Disegna il poligono p con le caratteristiche specificate.
 */
static void near pascal polygon3d (polyg * p)
/* Convenzioni di chiamata:
 *  1) i punti sono p.len
 *  2) le loro coordinate sono in p.R3
 *  3) la campitura Š contenuta in p.pattern ( se = 0 non campisce )
 *  4) lo spessore del bordo Š p.thick
 *  5) il tratteggio del bordo Š p.style
 *  6) il colore del bordo Š p.color
 */
{
    PolyPoints p2 ;
    register int i ;
    double csi,eta ;

    for(i=0; i< p->len; ++i) {
	proietta ( &p->R3[i], &csi,&eta ) ;
	p2[i].x = coordX(csi) ;
	p2[i].y = coordY(eta) ;
    }
    setcolor(p->color) ;
    setlinestyle (p->style,0,p->thick) ;
    setfillpattern ( Fills[p->pattern] , p->color ) ;
    setfillstyle (USER_FILL, p->color ) ;
    if (p->len > 0)
	if (p->pattern==dummy)
	    drawpoly( p->len, (int*)&p2 ) ;
	else {
	    fillpoly ( p->len, (int*)&p2 ) ;
	    if (!p->color) {
		/* il TURBO non disegna il bordo */
		memcpy(&p2[p->len],&p2[0],sizeof(PointType)) ;
		/* chiude il poligono */
		drawpoly (p->len+1, (int*)&p2) ;
	    }
	}
} /* polygon3d */

/*
 * Inizializza un nuovo poligono con i parametri indicati
 */
static void near pascal init (polyg *q, int _color, int _style,
    int _thick, FillIndex _pattern)
{
    q->len = 0 ;
    q->color = _color ;
    q->style = _style ;
    q->thick = _thick ;
    q->pattern = _pattern ;
} /* init */

/*
 * Disegna il pavimento con il pattern stanza_loc.terreno
 */
static void near pascal pavimento(void)
{
    polyg p ;
    register int i ;

    init ( &p, 0, SOLID_LINE, 1, stanza_loc->terreno) ;
    p.len = 4 ;
    for (i=0; i<4; ++i)
	muro ( -100,0, i, &p.R3[i] ) ;
    polygon3d (&p);
} /*pavimento*/

/*
 * Quello che segue da qui in poi serve esclusivamente per disegnare le
 * pareti. Precedono alcune variabili di comodo:
 */

static polyg p,q,man ; /*p globale, q locale, man omino (oppure []) */
static stato_porta s_p1, s_p2 ; /* per sapere se disegnarla aperta o chiusa */
static int num_frecce, wall ;

/*
 * aggiunge un punto al poligono q, date le sue coordinate sul muro
 */
/* MACRO void add ( int csi, int eta, polyg *q1 ) ; */
#define add(csi,eta,q1) muro ((csi),(eta),(wall), &(q1)->R3[(q1)->len++])

/*
 * E` l'equivalente di add, usando muro2
 */
/* MACRO void add2 ( int csi, int eta, int zeta, polyg * q1 ) ; */
#define add2(csi,eta,zeta,q1) muro2 ((csi),(eta),(zeta),(wall), \
    &(q1)->R3[(q1)->len++])

/*
 * Disegna il poligono p
 */
static void near pascal go(void)
{
    if (p.len>0) polygon3d(&p) ;
    p.len = 0 ;
} /*go*/

/*
 * Cornice rettangolare del muro
 */
static void near pascal quad(void)
{
    add (100,0, &p ) ; add (100,100, &p ) ;
    add (-100,100, &p ); add (-100,0, &p ) ;
} /*quad*/

/*
 * Indica la possibile uscita
 */
static void near pascal freccia(int s)
{
    t_parete * pr = & luogo_loc->pareti[wall] ;

    init( &q, 0, SOLID_LINE, 1, nero ) ;
    add2 ( s-7, 0, -15, &q ) ; add2 ( s, 0, -25, &q ) ;
    add2 ( s+7, 0, -15, &q ) ; add2 ( s, 0, -35, &q ) ;
    polygon3d(&q) ;
    setlinestyle (SOLID_LINE,0,3) ;
    setcolor(0);
    draw3d2(s,0,0,s,0,-25,wall) ;
    if ( ((++num_frecce==1) ? pr->n_porta1 : pr->n_porta2)==PassedThrough) {
	/* omino */
	init(&man,0,SOLID_LINE,1,bianco);
	add(s+10,0,&man); add(s+20,0,&man); add(s+20,40,&man);
	add(s+30,40,&man); add(s+40,40,&man); add(s+20,60,&man);
	add(s+10,60,&man); add(s+10,80,&man); add(s-10,80,&man);
	add(s-10,60,&man); add(s-20,60,&man); add(s-40,40,&man);
	add(s-30,40,&man); add(s-20,40,&man); add(s-20,0,&man);
	add(s-10,0,&man); add(s,40,&man);
    }
} /*freccia*/

/*
 * Da qui in poi ci sono le varie procedure per disegnare i particolari di
 * una parete
 */

static void near pascal door(int s, int d, int h, stato_porta s_p )
{
    int r = -30 ;

    add (s-d,0,&p) ; add (s-d,h,&p) ;
    add (s+d,h,&p) ; add (s+d,0,&p) ;
    freccia(s);
    if (s_p != sp_niente) {
	init(&q,1,SOLID_LINE,1,G2) ;
	add(s-d,0,&q); add(s-d,h,&q);
	if (s_p == sp_chiuso) {
	    add(s+d,h,&q) ; add(s+d,0,&q) ;
	} else { /* porta aperta */
	    add2(s,h,r,&q); add2(s,0,r,&q);
	}
	polygon3d(&q);
    }
} /*door*/

static void near pascal portavetri(int s, int d, int h, stato_porta s_p )
{
    int r = -30 ;

    add (s-d,0,&p) ; add (s-d,h,&p) ;
    add (s+d,h,&p) ; add (s+d,0,&p) ;
    freccia(s);
    if (s_p != sp_niente) {
	init(&q,1,SOLID_LINE,1,G6) ;
	add(s-d,0,&q); add(s-d,h,&q);
	if (s_p == sp_chiuso) {
	    add(s+d,h,&q); add(s+d,0,&q);
	} else { /* porta aperta */
	    add2(s,h,r,&q); add2(s,0,r,&q);
	}
	polygon3d(&q);
    }
} /*portavetri*/

static void near pascal portone(int s, int d, int h, stato_porta s_p )
{
    add(s-d,0,&p); add(s-d,3*h/4,&p); add(s-d/2,h,&p);
    add(s+d/2,h,&p); add(s+d,3*h/4,&p); add(s+d,0,&p);
    freccia(s);
    if (s_p == sp_chiuso) {
	init(&q,1,SOLID_LINE,1,G4);
	add(s-d,0,&q); add(s-d,3*h/4,&q); add(s-d/2,h,&q);
	add(s+d/2,h,&q); add(s+d,3*h/4,&q); add(s+d,0,&q);
	polygon3d(&q) ;
    } else if (s_p == sp_aperto) {
	init(&q,1,SOLID_LINE,1,G4);
	add(s-d,0,&q); add(s-d,h,&q); add2(s-d/2,h,-d/2,&q);
	add2(s-d/2,0,-d/2,&q);
	polygon3d(&q) ;
	init(&q,1,SOLID_LINE,1,G4);
	add(s+d,0,&q); add(s+d,h,&q); add2(s+d/2,h,-d/2,&q);
	add2(s+d/2,0,-d/2,&q);
	polygon3d(&q) ;
    }
} /*portone*/

static void near pascal cancello(int s, int d, int h, stato_porta s_p )
{
    int i ;

    add(s-d,0,&p); add(s-d,3*h/4,&p); add(s-d/2,h,&p);
    add(s+d/2,h,&p); add(s+d,h/2,&p); add(s+d,0,&p);
    freccia(s);
    if (s_p==sp_chiuso) {
	setlinestyle(SOLID_LINE,0,1) ;
	setcolor(1);
	for (i=-1; i<=1; ++i)
	    draw3d(s+(d*i)/2,0,s+(d*i)/2,h,wall) ;
	for (i=1; i<=3; ++i)
	    draw3d(s-d,(h*i)/4,s+d,(h*i)/4,wall) ;
    }
} /*cancello*/

static void near pascal finestra(int s, int t, int l, int m)
{
    int d = 3 ;

    go() ;
    init ( &q, 0, SOLID_LINE, 1, nero ) ;
    add(s-l,t-m,&q); add(s-l,t+m,&q);
    add(s+l,t+m,&q); add(s+l,t-m,&q);
    polygon3d(&q);
    init ( &q, 0, SOLID_LINE, 1, G15 ) ;
    add2(s-l,t-m,d,&q); add2(s-l,t+m,d,&q);
    add2(s+l,t+m,d,&q); add2(s+l,t-m,d,&q);
    polygon3d(&q);
    setlinestyle(USERBIT_LINE,0xAAAA,1);
    draw3d2(s,t-m,d,s,t+m,d,wall); draw3d2(s-l,t,d,s+l,t,d,wall);
} /*finestra*/

static void near pascal finesmura(int s, int t, int l, int m)
{
    int d = 3 ;

    go() ;
    init ( &q, 1, SOLID_LINE, 1, nero ) ;
    add(s-l,t-m,&q); add(s-l,t+m,&q);
    add(s+l,t+m,&q); add(s+l,t-m,&q);
    polygon3d(&q);
    init ( &q, 0, SOLID_LINE, 1, mattoni ) ;
    add2(s-l,t-m,d,&q); add2(s-l,t+m,d,&q);
    add2(s+l,t+m,d,&q); add2(s+l,t-m,d,&q);
    polygon3d(&q);
} /*finesmura*/

static void near pascal grata(int s, int t, int d, int h)
{
    int i, r = 3 ;

    go() ;
    init ( &q, 0, SOLID_LINE, 1, nero ) ;
    add(s-d,t-h,&q); add(s-d,t+h,&q);
    add(s+d,t+h,&q); add(s+d,t-h,&q);
    polygon3d(&q);
    init(&q,0,SOLID_LINE,1,bianco) ;
    add2(s-d,t-h,r,&q); add2(s+d,t-h,r,&q);
    add2(s+d,t+h,r,&q); add2(s-d,t+h,r,&q);
    polygon3d(&q) ;
    setlinestyle(USERBIT_LINE, 0x7777, 1) ;
    for (i=-2; i<=2; ++i) {
	draw3d(s+i*d/3,t-h,s+i*d/3,t+h,wall) ;
	draw3d(s-d,t+i*h/3,s+d,t+i*h/3,wall) ;
    }
} /*grata*/

static void near pascal specchio(int s, int t, int l, int m)
{
    int d = 3 ;

    go() ;
    init ( &q, 0, SOLID_LINE, 1, bianco ) ;
    add(s-l,t-m,&q); add(s-l,t+m,&q);
    add(s+l,t+m,&q); add(s+l,t-m,&q);
    polygon3d(&q);
    init ( &q, 1, SOLID_LINE, 1, G2 ) ;
    add2(s-l,t-m,d,&q); add2(s-l,t+m,d,&q);
    add2(s+l,t+m,d,&q); add2(s+l,t-m,d,&q);
    polygon3d(&q);
    setlinestyle(USERBIT_LINE,0xAAAA,1) ;
    draw3d2(s-l/2,t,d,s,t+m/2,d,wall);
    draw3d2(s-l/2,t-m/2,d,s+l/2,t+m/2,d,wall);
    draw3d2(s,t-m/2,d,s+l/2,t,d,wall);
} /*specchio*/

static void near pascal quadro(int s, int t, int l, int m)
{
    int d = 3 ;

    go() ;
    init ( &q, 1, SOLID_LINE, 1, G3 ) ;
    add(s-l,t-m,&q); add(s-l,t+m,&q);
    add(s+l,t+m,&q); add(s+l,t-m,&q);
    polygon3d(&q);
    init ( &q, 0, SOLID_LINE, 1, bianco ) ;
    add2(s-l,t-m,d,&q); add2(s-l,t+m,d,&q);
    add2(s+l,t+m,d,&q); add2(s+l,t-m,d,&q);
    polygon3d(&q);
    /* disegnino idiota */
    draw3d2(s-l/2,t+m/2,d,s-l/2,t,d,wall);
    draw3d2(s-l/2,t,d,s+l/2,t+m/2,d,wall);
    draw3d2(s,t,d,s+l/2,t,d,wall);
    draw3d2(s+l/2,t,d,s+l/2,t-m/2,d,wall);
    draw3d2(s+l/2,t-m/2,d,s,t-m/2,d,wall);
    draw3d2(s,t-m/2,d,s,t,d,wall);
} /* quadro */

static void near pascal dashes(int da, int al)
{
    setcolor(0);
    setlinestyle(USERBIT_LINE,0xF8F8,3);
    draw3d(da,0,al,0,wall);
    freccia((da+al)/2);
} /*dashes*/

static void near pascal murox(int da, int al)
{
    go() ;
    add (al,0,&p); add (al,100,&p); add (da,100,&p); add (da,0,&p);
} /*muro*/

static void near pascal mattone(int da, int al)
{
    int j,k,n,w ;

    murox(da,al);
    p.pattern=bianco;
    go();
    setlinestyle(SOLID_LINE,0,1);
    setcolor(0);
    n=(al-da)/20;
    for (j=0; j<=4; ++j)
    draw3d(da,j*25,al,j*25,wall);
    for (j=0; j<=3; ++j)
	for (k=0; k<=n-1; ++k) {
	    w= ((double)(j&1)/2.0+k) * (al-da)/((double)n)+da ;
	    draw3d(w,j*25,w,(j+1)*25,wall);
	}
} /*mattone*/

static void near pascal rottami(int da, int al)
{
    int i ;

    init(&q,0,SOLID_LINE,1,stanza_loc->terreno) ;
    add2(da,0,0, &q) ;
    for (i=1; i<=9; ++i)
	add2(da+(al-da)*i/10,random(20)-10,-random(30), &q) ;
    add2(al,0,0, &q) ;
    polygon3d(&q) ;
    freccia((al+da)/2);
} /*rottami*/

static void near pascal portasciapa(int s, int d, int h)
{
    add(s-d,0,&p); add(s-d,h,&p); add(s+d,h,&p); add(s+d,0,&p);
    freccia(s);
    setlinestyle(SOLID_LINE,0,1);
    setcolor(1);
    draw3d(s-d,h,s+d,0,wall);
    draw3d(s+d,h,s-d,0,wall);
    draw3d(s-d,h/2,s+d,h/2,wall);
} /*porta-sciap…*/

static void near pascal merli(void)
{
    add(-100,0,&p); add(100,0,&p); add(100,100,&p);
    add(85,80,&p); add(71,100,&p);
    add(71,60,&p); add(42,60,&p);
    add(42,100,&p); add(28,80,&p);
    add(14,100,&p); add(14,60,&p);
    add(-14,60,&p); add(-14,100,&p);
    add(-28,80,&p); add(-42,100,&p);
    add(-42,60,&p); add(-71,60,&p);
    add(-71,100,&p); add(-85,80,&p); add(-100,100,&p);
} /*merli*/

static void near pascal armadio(int s, int d, int h)
{
    int r=5; /* rientranza */

    go();
    init ( &q, 1, SOLID_LINE, 1, nero ) ;
    add (s-d,0,&q) ; add (s-d,h,&q) ;
    add (s+d,h,&q) ; add (s+d,0,&q) ;
    polygon3d(&q) ;
    draw3d2(s-d,0,0,s-d,0,r,wall); draw3d2(s+d,0,0,s+d,0,r,wall);
    draw3d2(s-d,h,0,s-d,h,r,wall); draw3d2(s+d,h,0,s+d,h,r,wall);
    init ( &q, 1, SOLID_LINE, 1, G2 ) ;
    add2 (s-d,0,r,&q) ; add2 (s-d,h,r,&q) ;
    add2 (s+d,h,r,&q) ; add2 (s+d,0,r,&q) ;
    polygon3d(&q) ;
    draw3d2(s,h/2,r,s+d,h/2,r,wall) ;
} /*armadio*/

static void near pascal armadiomuro(int s, int t, int d, int h)
{
    int r=5; /* rientranza */

    go();
    init ( &q, 1, SOLID_LINE, 1, nero ) ;
    add (s-d,t-h,&q) ; add (s-d,t+h,&q) ;
    add (s+d,t+h,&q) ; add (s+d,t-h,&q) ;
    polygon3d(&q) ;
    draw3d2(s-d,t-h,0,s-d,t-h,r,wall); draw3d2(s+d,t-h,0,s+d,t-h,r,wall);
    draw3d2(s-d,t+h,0,s-d,t+h,r,wall); draw3d2(s+d,t+h,0,s+d,t+h,r,wall);
    init ( &q, 1, SOLID_LINE, 1, G2 ) ;
    add2 (s-d,t-h,r,&q) ; add2 (s-d,t+h,r,&q) ;
    add2 (s+d,t+h,r,&q) ; add2 (s+d,t-h,r,&q) ;
    polygon3d(&q) ;
    draw3d2(s,t+h/2,r,s+d,t+h/2,r,wall) ;
} /*armadio*/

static void near pascal caminetto(int s)
{
    int d=15, d2=20, /*semi-larghezze*/
	r=10, r2=15, /*profondit…*/
	h1=40, h2=50, h3=60; /*altezze*/

    go();
    init(&q,0,SOLID_LINE,1, bianco ) ;
    add(s-d,0,&q); add(s-d,h1,&q); add(s+d,h1,&q); add(s+d,0,&q);
    polygon3d(&q);
    q.len=0;
    add2(s-d,0,r,&q); add2(s-d,h1,r,&q); add2(s+d,h1,r,&q); add2(s+d,0,r,&q);
    polygon3d(&q); q.len=0;
    draw3d2(s-d,0,0,s-d,0,r,wall); draw3d2(s+d,0,0,s+d,0,r,wall);
    add(s-d2,h1,&q); add(s-d2,h2,&q); add(s,h3,&q);
    add(s+d2,h2,&q); add(s+d2,h1,&q); polygon3d(&q); q.len=0;
    add2(s-d2,h1,r2,&q); add2(s-d2,h2,r2,&q);
    add2(s+d2,h2,r2,&q); add2(s+d2,h1,r2,&q); polygon3d(&q); q.len=0;
    add(s,h3,&q); add(s+d2,h2,&q); add2(s+d2,h2,r2,&q);
    polygon3d(&q); q.len=0;
    add(s,h3,&q); add(s-d2,h2,&q); add2(s-d2,h2,r2,&q);
    polygon3d(&q); q.len=0;
    add(s,h3,&q); add2(s-d2,h2,r2,&q); add2(s+d2,h2,r2,&q); polygon3d(&q);
} /*caminetto*/

static void near pascal scaffale(int s, int d, int h)
{
    int r=10, i,t ;

    go();
    init( &q, 0, SOLID_LINE, 1, G14 ) ;
    add(s-d,0,&q); add(s-d,h,&q);
    add(s+d,h,&q); add(s+d,0,&q);
    polygon3d(&q);
    for (i=0; i<=4; ++i) {
	q.len=0; /* == init( ..come sopra.. ) */
	t=h*i/4;
	add(s-d,t,&q); add2(s-d,t,r,&q);
	add2(s+d,t,r,&q); add(s+d,t,&q);
	polygon3d(&q);
    }
    draw3d2(s-d,0,r,s-d,h,r,wall); draw3d2(s+d,0,r,s+d,h,r,wall);
} /*scaffale*/

static void near pascal balcone(void)
{
    int h1=10, h2=50, h3=60, /* altezze */
	r=10, /* profondit… */
	d=6; /* diametro colonne */
    int i,j,x,dx,dy ;

    go();
    init(&q,0,SOLID_LINE,1, G15 );
    /*basamento*/
    add(-100,0,&q); add(-100,h1,&q);
    add(100,h1,&q); add(100,0,&q);
    polygon3d(&q); q.len=0;
    add2(-100,h1,-2*r,&q); add(-100,h1,&q);
    add(100,h1,&q); add2(100,h1,-2*r,&q);
    polygon3d(&q); q.len=0;
    q.pattern=dummy;
    /*colonne*/
    for (i=-2; i<=2; ++i) {
	x=i*33;
	for (j=0; j<=4; ++j) {
	    dx=(double)d*cos((2.0/5.0*M_PI)*(double)j);
	    dy=(double)d*sin((2.0/5.0*M_PI)*(double)j);
	    draw3d2(x+dx,h1,dy-r, x+dx,h2,dy-r, wall) ;
	    add2(x+dx,h1,dy-r,&q);
	}
	q.R3[q.len++]=q.R3[0] ;
	polygon3d(&q);
	q.len=0;
    }
    q.pattern = G14 ;
    /*ringhiera*/
    add(-100,h2,&q); add(-100,h3,&q);
    add(100,h3,&q); add(100,h2,&q);
    polygon3d(&q); q.len=0;
    add2(-100,h3,-2*r,&q); add(-100,h3,&q);
    add(100,h3,&q); add2(100,h3,-2*r,&q);
    polygon3d(&q); q.len=0;
} /*balcone*/

static void near pascal poggiolo(int s, int d)
{
    init(&q,0,SOLID_LINE,1,lisca);
    add2(s-d,0,0,&q); add2(s+d,0,0,&q);
    add2(s+d,0,-30,&q); add2(s-d,0,-30,&q);
    polygon3d(&q);
} /*poggiolo*/

static void near pascal salita(int s, int d)
{
    int i ;

    init(&q,0,SOLID_LINE,1,bianco) ;
    for (i=0; i<=4; ++i) {
	q.len=0;
	add2(s+d,(i+1)*16,-i*16,&q);
	add2(s-d,(i+1)*16,-i*16,&q);
	add2(s-d,i*16,-i*16,&q);
	add2(s+d,i*16,-i*16,&q);
	polygon3d(&q);
	q.len=2; /* i primi 2 punti li mantengo uguali */
	add2(s-d,(i+1)*16,-(i+1)*16,&q);
	add2(s+d,(i+1)*16,-(i+1)*16,&q);
	polygon3d(&q) ;
    }
} /*salita*/

static void near pascal discesa(int s, int d)
{
    int i ;

    init(&q,0,SOLID_LINE,1,bianco) ;
    for (i=4; i>=0; --i) {
	q.len=0;
	add2(s+d,-i*12,-i*25,&q);
	add2(s-d,-i*12,-i*25,&q);
	add2(s-d,-i*12,-(i+1)*25,&q);
	add2(s+d,-i*12,-(i+1)*25,&q);
	polygon3d(&q) ;
    }
} /*discesa*/

static void near pascal anello(int s, int h) /* anello attaccato al muro */
{
    int r=20 ;
    int i, sn,cs,sn1,cs1 ;
    double a,a1 ;

    go();
    setlinestyle(SOLID_LINE,0,3) ;
    setcolor(0);
    a1=-130.0/180.0*M_PI;
    sn1=r*sin(a1);
    cs1=r*cos(a1);
    for (i=-12; i<=13; ++i) {
	a=i*(10.0/180.0*M_PI) ;
	sn=r*sin(a);
	cs=r*cos(a);
	draw3d2(s+sn1,h,r-cs1, s+sn,h,r-cs, wall) ;
	a1=a; cs1=cs; sn1=sn;
    }
} /*anello*/

static void near pascal baldacchino(int s)
{
    int d=30 , /*larghezza*/
	t=10 , /*spessore*/
	h=70 , /*altezza da terra*/
	r=40 ; /*rientranza*/
    int i,h1 ;

    go();
    for (i=0; i<=1; ++i) {
	h1=h+i*t;
	if (!i) init(&q,0,SOLID_LINE,1,G4) ;
	else init(&q,0,SOLID_LINE,1,quadri) ;
	add(s-d,h1,&q); add(s+d,h1,&q);
	add2(s+d,h1,3*r/4,&q) ;
	add2(s+d/2,h1,r,&q);
	add2(s-d/2,h1,r,&q);
	add2(s-d,h1,3*r/4,&q);
	polygon3d(&q);
    }
} /*baldacchino*/

static void near pascal buco(int s, int h, int r)
{
    int i , n=20 ;
    double a ;

    init(&q,0,SOLID_LINE,1,nero);
    go() ;
    for (i=0; i<=n; ++i) {
	a=2*M_PI/n*i ;
	add(s+r*cos(a),h+r*sin(a),&q);
    }
    polygon3d(&q);
} /* buco */

static void near pascal cesso(void)
{
    int i, y=30, y2=55, dy=8 ;

    go();
    /* tazza */
    init(&q,0,SOLID_LINE,1,bianco);
    add2(20,y,0,&q); add2(20,y,20,&q);
    add2(15,y,30,&q); add2(-15,y,30,&q);
    add2(-20,y,20,&q); add2(-20,y,0,&q);
    polygon3d(&q);
    init(&q,0,SOLID_LINE, 1, grigio);
    add2(15,y,5,&q); add2(15,y,20,&q);
    add2(10,y,25,&q); add2(-10,y,25,&q);
    add2(-15,y,20,&q); add2(-15,y,5,&q);
    polygon3d(&q);
    /* bastone */
    init(&q,0,SOLID_LINE,1,bianco);
    add(-5,y,&q); add(-5,y2,&q);
    add(5,y2,&q); add(5,y,&q);
    polygon3d(&q);
    /* vaschetta */
    for (i=0; i<=1; ++i) {
	q.len=0; /* == init(&q,0,SOLID_LINE,1,bianco) */
	add(-20,y2+i*dy,&q); add2(-20,y2+i*dy,10,&q);
	add2(20,y2+i*dy,10,&q); add(20,y2+i*dy,&q);
	polygon3d(&q);
    }
} /* cesso */

static void near pascal vasca(void)
{
    int x=40, y=30, z=30;

    go();
    init(&q,0,SOLID_LINE,1,bianco);
    /* base */
    add2(-x,0,0,&q); add2(-x,0,z,&q);
    add2(x,0,z,&q); add2(x,0,0,&q);
    polygon3d(&q);
    q.len=0;
    /* retro */
    add(-x,0,&q); add(-x,y,&q);
    add(x,y,&q); add(x,0,&q);
    polygon3d(&q);
    q.len=0;
    /* sinistra */
    add(-x,0,&q); add(-x,y,&q);
    add2(-x,y,z,&q); add2(-x,0,z,&q);
    polygon3d(&q);
    q.len=0;
    /* destra */
    add(x,0,&q); add(x,y,&q);
    add2(x,y,z,&q); add2(x,0,z,&q);
    polygon3d(&q);
    q.len=0;
    /* fronte */
    add2(-x,0,z,&q); add2(-x,y,z,&q);
    add2(x,y,z,&q); add2(x,0,z,&q);
    polygon3d(&q);
} /* vasca */

static void near pascal castlamunt(void)
{
    int x=30, y=50, z=30, x1=6, y1=65, y2=80, xx=15;

    go();
    init(&q,0,SOLID_LINE,1,bianco);
    /* fronte */
    add2(x,0,z,&q); add2(x,y,z,&q);
    add2(-x,y,z,&q); add2(-x,0,z,&q);
    polygon3d(&q);
    /* finestra */
    init(&q,0,SOLID_LINE,1,grigio);
    add2(x / 2, y / 4,z,&q); add2(x / 2,3*y / 4,z,&q);
    add2(-x / 2, 3*y / 4, z,&q); add2(-x / 2,y / 4,z,&q);
    polygon3d(&q);
    /* top */
    init(&q,0,SOLID_LINE,1,G6);
    add(-x,y,&q); add(x,y,&q);
    add2(x,y,z,&q); add2(-x,y,z,&q);
    polygon3d(&q);
    /* cannone */
    init(&q,0,SOLID_LINE,3,G12);
    add(-x1,y,&q); add(x1,y,&q);
    add(x1,100,&q); add(-x1,100,&q);
    polygon3d(&q);
    /* radiatore */
    init(&q,0,SOLID_LINE,1,righe);
    add(-xx,y1,&q); add(-xx,y2,&q);
    add(xx,y2,&q); add(xx,y1,&q);
    polygon3d(&q);
} /* castlamunt */

static void near pascal stuva(void)
{
    int x=30, y=50, z=30, x1=6;

    go();
    init(&q,0,SOLID_LINE,1,G14);
    /* fronte */
    add2(x,0,z,&q); add2(x,y,z,&q);
    add2(-x,y,z,&q); add2(-x,0,z,&q);
    polygon3d(&q);
    /* top */
    init(&q,0,SOLID_LINE,1,G6);
    add(-x,y,&q); add(x,y,&q);
    add2(x,y,z,&q); add2(-x,y,z,&q);
    polygon3d(&q);
    /* cannone */
    init(&q,0,SOLID_LINE,3,G12);
    add(-x1,y,&q); add(x1,y,&q);
    add(x1,100,&q); add(-x1,100,&q);
    polygon3d(&q);
} /* stuva */

static void near pascal altare(void)
{
    int x=70, y=55, z=20, dx=10, dy=8, xt=20, y1=63, y2=80;

    go();
    /* fronte */
    init(&q,0,SOLID_LINE,1,bianco);
    add2(x,0,z,&q); add2(x,y,z,&q);
    add2(-x,y,z,&q); add2(-x,0,z,&q);
    polygon3d(&q);
    /* decorazione fronte */
    init(&q,0,SOLID_LINE,1,quadri);
    add2(x-dx,dy,z,&q); add2(x-dx,y-dy,z,&q);
    add2(-x+dx,y-dy,z,&q); add2(-x+dx,dy,z,&q);
    polygon3d(&q);
    /* top */
    init(&q,0,SOLID_LINE,1,G15);
    add(-x,y,&q); add(x,y,&q);
    add2(x,y,z,&q); add2(-x,y,z,&q);
    polygon3d(&q);
    /* tabernacolo */
    init(&q,0,SOLID_LINE,3,quadri);
    add(-xt,y1,&q); add(xt,y1,&q);
    add(xt,y2,&q); add(-xt,y2,&q);
    polygon3d(&q);
} /* altare */

static void near pascal transenna(int s)
{
    int x=8, y=50, i;

    go();
    /* base */
    init(&q,1,SOLID_LINE,3,G8);
    add(s-x,0,&q); add(s+x,0,&q);
    add2(s+x,0,80,&q); add2(s-x,0,80,&q);
    polygon3d(&q);
    /* sbarre */
    for (i=0; i<=4; ++i)
	draw3d2(s,0,i*20, s,y,i*20, wall) ;
    /* top */
    init(&q,1,SOLID_LINE,3,G8);
    add(s-x,y,&q); add(s+x,y,&q);
    add2(s+x,y,80,&q); add2(s-x,y,80,&q);
    polygon3d(&q);
} /* transenna */

static void near pascal ringhiera(int da, int al)
{
    int h=50, h1=65, n,i;

    /* estremi */
    setlinestyle(SOLID_LINE,0,3);
    setcolor(0);
    draw3d(da,0, da,h1, wall);
    draw3d(al,0, al,h1, wall);
    /* corrimano */
    draw3d(da,h, al,h, wall);
    /* raggi */
    setlinestyle(SOLID_LINE,0,1);
    n=(al-da)/10 ;
    for (i=1; i<=n-1; ++i)
	draw3d(da+i*(al-da)/n,0, da+i*(al-da)/n,h, wall ) ;
} /* ringhiera */

static void near pascal lavabo(void)
{
    int h=50, x=30, z=30, z0=20;

    go();
    /* sostegno */
    init(&q,0,SOLID_LINE,1,bianco);
    add(-x,0,&q); add(x,0,&q); add(x,h,&q); add(-x,h,&q);
    polygon3d(&q);
    /* vasca */
    q.len=0 ;
    add(-x,h,&q); add2(-x,h,z0,&q); add2(-x/2,h,z,&q);
    add2(x/2,h,z,&q); add2(x,h,z0,&q); add(x,h,&q);
    polygon3d(&q);
    draw3d2(-x/2,h,0, -x/2,h,z0, wall);
    draw3d2(x/2,h,0, x/2,h,z0, wall);
    draw3d2(-x/2,h,z0, x/2,h,z0, wall);
    /* rubinetti */
    setcolor(1);
    setlinestyle(SOLID_LINE,0,3) ;
    draw3d(-x/2,h+10, -x/2,h+20, wall);
    draw3d(x/2,h+10, x/2,h+20, wall);
} /* lavabo */

/*
 * Sono finiti i disegni dei particolari. Ora c'Š la procedura che li richiama
 * nel giusto ordina con i giusti parametri
 */

/*
 * Stabilisce come deve essere disegnata la porta #np in base al suo tipo ed
 * alle sue condizioni istantanee
 */
static stato_porta near pascal getdoorstatus(indice_porta np)
{
    t_porta * pp = &porta[np] ;

    if(pp->tipo==p_passaggio) return sp_niente ;
    else if (pp->tipo==p_ostruito) return sp_chiuso ;
    else /*p_porta*/
	return (pp->stato==p_aperto) ? sp_aperto : sp_chiuso ;
} /*GetDoorStatus*/

/*
 * Disegna la parete nella direzione "wall" attingendo agli array globali per
 * conoscerne forma, dimensioni, colori e tipi porte
 */
static void near pascal parete(int _wall)
{
    t_parete * pr = & luogo_loc->pareti[_wall] ;

    wall = _wall ; /* passa il parametro alle routines che richiamera` */

    /* prepara il muro standard */
    p.color = 1 ;
    p.thick = 3 ;
    p.style = SOLID_LINE ;
    p.len = 0 ;
    p.pattern = pr->tipo ;
    if (pr->n_porta1) {
	s_p1=getdoorstatus(pr->n_porta1) ;
	if (pr->n_porta2) s_p2=getdoorstatus(pr->n_porta2) ;
    }
    man.len=0 ; /* non disegnare l'uomo per default */
    num_frecce=0 ;
    switch (luogo_loc->pareti[wall].forma) {
	case 1:dashes(-1,1); break;
	case 2:quad(); break;
	case 3:quad(); door(-50,22,80,s_p1); break;
	case 4:quad(); door(0,20,80,s_p1); break;
	case 5:quad(); door(50,20,80,s_p1); break;
	case 6:quad(); door(-50,20,80,s_p1); door(50,20,80,s_p2); break;
	case 7:quad(); portone(0,40,90,s_p1); break;
	case 8:quad(); finestra(-50,50,20,30); break;
	case 9:quad(); finestra(0,50,20,30); break;
	case 10:quad(); finestra(50,50,20,30); break;
	case 11:quad(); finestra(-40,50,30,30); finestra(40,50,30,30); break;
	case 12:quad(); finestra(-60,50,20,30); finestra(0,50,20,30);
	    finestra(60,50,20,30); break;
	case 13:quad(); portone(0,30,80,s_p1); finestra(60,50,20,30); break;
	case 14:quad(); door(0,20,80,s_p1); finestra(-60,50,20,30);
	    finestra(60,50,20,30); break;
	case 15:quad(); door(20,20,80,s_p1); finestra(-50,50,20,30); break;
	case 16:quad(); specchio(-80,40,10,20); finestra(-40,50,20,30);
	    specchio(0,40,10,20); finestra(40,50,20,30); specchio(80,40,10,20);
	    break;
	case 17:quad(); door(0,20,80,s_p1); specchio(-60,40,20,20);
	    specchio(60,40,20,20); break;
	case 18:quad(); specchio(-50,40,20,30); specchio(50,40,20,30); break;
	case 19:rottami(-100,100); break;
	case 20:quad(); door(60,20,80,s_p1); finestra(-50,50,20,30); break;
	case 21:quad(); door(-60,20,80,s_p1); finestra(50,50,20,30); break;
	case 22:dashes(-100,-30); murox(-30,30); dashes(30,100); break;
	case 23:quad(); door(-60,20,70,s_p1); finestra(10,50,20,30);
	    finestra(70,50,20,30); break;
	case 24:quad(); door(0,20,70,s_p1); door(60,20,70,s_p2); break;
	case 25:murox(-100,-30); dashes(-30,30); murox(30,100); break;
	case 26:murox(-100,0); dashes(0,100); break;
	case 27:quad(); portasciapa(0,20,70); break;
	case 28:quad(); door(0,20,70,s_p1); finestra(60,50,20,30); break;
	case 29:murox(-100,30); door(-60,20,70,s_p1); dashes(30,100); break;
	case 30:mattone(-100,100); break;
	case 31:quad(); portone(0,60,90,s_p1); break;
	case 32:quad(); cancello(0,30,80,s_p1); break;
	case 33:merli(); break;
	case 34:quad(); armadio(0,20,60); break;
	case 35:quad(); scaffale(0,30,50); break;
	case 36:quad(); caminetto(0); break;
	case 37:quad(); door(50,20,70,s_p1); caminetto(-50); break;
	case 38:quad(); door(-50,20,70,s_p1); caminetto(60); break;
	case 39:quad(); door(0,20,70,s_p1); armadio(60,20,60); break;
	case 40:quad(); door(0,20,70,s_p1); caminetto(-60); armadio(60,20,60);
	    break;
	case 41:quad(); door(0,20,70,s_p1); caminetto(60); break;
	case 42:quad(); caminetto(50); break;
	case 43:quad(); caminetto(-50); break;
	case 44:balcone(); break;
	case 45:quad(); grata(0,50,20,20); break;
	case 46:quad(); portavetri(0,40,80,s_p1); break;
	case 47:quad(); poggiolo(0,30); door(0,30,70,s_p1);
	    finestra(-60,50,20,30); finestra(60,50,20,30); break;
	case 48:quad(); grata(-60,50,20,30); grata(60,50,20,30); break;
	case 49:quad(); finestra(-60,50,25,40); finestra(60,50,25,40); break;
	case 50:quad(); salita(0,40); door(0,40,70,s_p1); break;
	case 51:quad(); discesa(0,40); door(0,40,70,s_p1); break;
	case 52:quad(); salita(-50,40); discesa(50,40); door(-50,40,70,s_p1);
	    door(50,40,70,s_p2); break;
	case 53:quad(); salita(-50,40); door(-50,40,70,s_p1);
	    door(50,30,70,s_p2); break;
	case 54:quad(); door(-50,30,70,s_p1); grata(20,50,20,30);
	    grata(70,50,20,30); break;
	case 55:quad(); anello(-50,50); anello(50,50); break;
	case 56:quad(); door(-50,30,70,s_p1); baldacchino(30); break;
	case 57:quad(); poggiolo(-50,30); poggiolo(50,30);
	    door(-50,30,70,s_p1); door(50,30,70,s_p2); break;
	case 58:quad(); poggiolo(0,30); door(0,30,70,s_p1); break;
	case 59:quad(); buco(0,70,15); break;
	case 60:quad(); cesso(); break;
	case 61:quad(); vasca(); break;
	case 62:quad(); castlamunt(); break;
	case 63:quad(); armadiomuro(0,50,20,30); break;
	case 64:quad(); stuva(); break;
	case 65:quad(); door(10,36,70,s_p1); caminetto(-60); break;
	case 66:quad(); door(50,40,70,s_p1); baldacchino(-40); break;
	case 67:quad(); discesa(40,40); door(40,40,70,s_p1); break;
	case 68:quad(); altare(); break;
	case 69:quad(); transenna(-30); break;
	case 70:quad(); door(-55,30,60,s_p1); transenna(30);
	    finestra(70,50,20,20); break;
	case 71:dashes(-100,0); murox(0,100); break;
	case 72:discesa(-70,30); dashes(-100,-40); murox(-40,40);
	    dashes(40,100); break;
	case 73:dashes(-100,-40); murox(-40,40); salita(70,30); dashes(40,100);
	    break;
	case 74:quad(); discesa(-50,30); salita(50,30); door(-50,30,70,s_p1);
	    door(50,30,70,s_p2); break;
	case 75:quad(); armadio(-50,30,60); caminetto(50); break;
	case 76:quad(); door(25,35,70,s_p1); armadio(-45,25,60); break;
	case 77:quad(); portasciapa(25,40,70); caminetto(-45); break;
	case 78:quad(); door(0,36,70,s_p1); quadro(-70,50,20,20);
	    quadro(70,50,20,20); break;
	case 79:quad(); salita(-50,30); door(-50,30,70,s_p1);
	    finesmura(50,50,30,30); break;
	case 80:quad(); salita(-50,30); door(-50,30,70,s_p1); break;
	case 81:discesa(-70,30); dashes(-100,-40); dashes(40,100);
	    ringhiera(-40,40); break;
	case 82:dashes(-100,-40); salita(70,30); dashes(40,100);
	    ringhiera(-40,40); break;
	case 83:discesa(-70,30); dashes(-100,-40); salita(70,30);
	    dashes(40,100); ringhiera(-40,40); break;
	case 84:dashes(-100,-30); murox(-30,100); finestra(0,50,20,30);
	    finestra(60,50,20,30); break;
	case 85:quad(); finestra(0,15,35,15); break;
	case 86:quad(); grata(-10,50,30,30); scaffale(60,25,70); break;
	case 87:discesa(70,30); dashes(40,100); ringhiera(-100,40); break;
	case 88:quad(); grata(-60,50,20,30); grata(0,50,20,30);
	    grata(60,50,20,30); break;
	case 89:quad(); lavabo(); break;
	case 90:quad(); salita(50,40); door(50,40,70,s_p1); break;
	case 98:salita(0,100); freccia(0); break;
	default:internalerror("Parameter 'tipo' out of range","Parete",
	    luogo_loc->pareti[wall].forma) ;
    } /* switch tipo ... */
    polygon3d(&p) ;
    polygon3d(&man) ;
} /*parete*/

/*
 * Disegna la stanza in cui ti trovi, ne mostra la forma, e ne scrive la
 * descrizione
 */
void drawroom(void) /* pubblica */
{
    /* cancella la finestra */
    roombackground() ;
    /* aggiorna i puntatori per rapidit… di accesso */
    luogo_loc = & luogo[stato.dovesei] ;
    stanza_loc = & stanza[luogo_loc->n_stanza] ;
    /* NOME STANZA */
    roomheader ( stanza_loc->nome ) ;
    /* DISEGNO SOFFITTO */
    if (belongs(g_soffitto,statoAUX.grafica))
	outobject(-stanza_loc->tipo-1) ;
    else outobject(0) ;
    /* VISTA DALL'ALTO */
    /* riscala */
    loc_x=sqrt(luogo_loc->dim_x*typ_x);
    loc_y=sqrt(luogo_loc->dim_y*typ_y);
    loc_z=sqrt(stanza_loc->dim_z*typ_z);
    setwindow (Wroom) ;
    pavimento() ;
    parete(0) ; parete(1); parete(2); parete(3) ;
    /* DESCRIZIONE */
    /* il 60 che segue Š PROVVISORIO !!!!! */
    if ( belongs(p_stanze,statoAUX.prolix) &&
	(packtempo(&stato.tempo)-stanza_loc->memoria > 60 ) &&
	(stanza_loc->descrizione) )
	pprintf("%s\n",frase[stanza_loc->descrizione]) ;
    stanza[luogo_loc->n_stanza].memoria = packtempo(&stato.tempo) ;
} /*drawroom*/

/*
 * Disegna la Rosa dei Venti
 */
void drawarrows(void) /* pubblica */
{
    roombackground() ;
    roomheader("direzioni dei punti cardinali") ;
    setwindow(Wroom) ;
    setlinestyle (SOLID_LINE,0,3);
    line(coordX(-MX*0.7),coordY(0),coordX(MX*0.7),coordY(0)) ;
    line(coordX(0),coordY(-MY*0.65),coordX(0),coordY(MY*0.65)) ;
    circle(coordX(0),coordY(0),30);
    circle(coordX(0),coordY(0),41);
    line(coordX(MX*0.7),coordY(0),coordX(MX/2.0),coordY(MY/2.0));
    line(coordX(MX*0.7),coordY(0),coordX(MX/2.0),coordY(-MY/2.0));
    setcolor(wind(bkcolor)) ;
    settextjustify(LEFT_TEXT,CENTER_TEXT);
    outtextxy(coordX(MX*0.75),coordY(0),"N") ;
    settextjustify(RIGHT_TEXT,CENTER_TEXT);
    outtextxy(coordX(-MX*0.75),coordY(0),"S") ;
    settextjustify(CENTER_TEXT,CENTER_TEXT);
    outtextxy(coordX(0),coordY(MY*0.75),"W") ;
    settextjustify(CENTER_TEXT,CENTER_TEXT);
    outtextxy(coordX(0),coordY(-MY*0.75),"E") ;
} /* drawarrows */

/*
 * Inizializza le variabili usate dal resto del modulo. Da chiamare per prima
 */
void initroom (void) /* pubblica */
{
    /*inizializing code*/
    Wroom = gimmewindow ( "ROOM    ", "ROOM    " ) ;
    Wtitolo = gimmewindow( "ROOM    ", "TITOLO  " ) ;

    setwindow(Wroom) ; /* per averne accessibili le dimensioni */
    kx = ((double)wind(dx))/(2.0*MX) ;
    hx = MX*(double)kx ;
    ky = (-(double)wind(dy))/(2.0*MY) ;
    hy = MY*(double)ky+(double)wind(dy) ;

    PassedThrough=0 ;

    roombackground() ;
    roomheader("Visione della stanza");
} /* initroom */
