/* modulo "status.c"
 * disegna e gestisce la finestra dello STATUS
 * procedure correlate alla visualizzazione dell'orologio
 * e alla visione di sole e luna nel cielo
 */

/* INTERFACE */

#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"
#include "windows.h"

#include "status.h"

/* IMPLEMENTATION */

static int Wora, Whorz, Wstatus, Windic, Woggs, Wmyogg, Wlogg, Wicons ;

static int icnX,icnY, icnS ; /* dimensioni in pixel delle icone */

/*
 * 1) gestione orologio
 */

/* lunghezze lancette */
#define rh 20.0
#define rm 32.0

/* rapporto X/Y del video */
static double asp ;

static unsigned oldtime ;

static byte * watchIMG ;

/*
 * Disegna il quadrante dell'orologio vuoto
 */
static void near pascal initwatch(void)
{
    watchIMG = loadfile (grpath("watch.byt")) ;
    setwindow(Wora) ;
    putimage(0,0,watchIMG,COPY_PUT) ;
} /* initwatch */

/*
 * Disegna l'orologio con le lancette che puntano l'ora esatta
 */
void outwatch(void) /* pubblica */
{
    float ang,ang2 ;
    int ix,iy,ix2,iy2 ;
    unsigned newtime ;

    newtime = packtempo(&stato.tempo) ;
    if (newtime>oldtime) {
	setwindow(Wora) ;
	setlinestyle(SOLID_LINE,0,(asp>0.5) ? 3 : 1) ;
	/* calcola i segmenti */
	ang=M_PI_2-((double)stato.tempo.ore+(double)stato.tempo.min/60.0)
	    /6.0*M_PI ;
	ang2=M_PI_2-((double)stato.tempo.min/30.0)*M_PI ;
	ix=rh*cos(ang) ;
	iy=rh*asp*sin(ang) ;
	ix2=rm*cos(ang2) ;
	iy2=rm*asp*sin(ang2) ;
	putimage(0,0,watchIMG,COPY_PUT) ;
	line(wind(xc),wind(yc),wind(xc)+ix,wind(yc)-iy) ;
	line(wind(xc),wind(yc),wind(xc)+ix2,wind(yc)-iy2) ;
	oldtime=newtime ;
    }
} /* OutWatch */

/*
 * 2) gestione orizzonte
 */

typedef struct {
	unsigned Dx,Dy ;
	/* seguirebbero i bytes di bitmap, che non manipolo ==> non definisco */
    } astro ;

static unsigned oldsolu ;

/* immagini bitmap degli oggetti */
static byte * horizon ;
static astro * sole, * luna ;

/*
 * Disegna l'orizzonte vuoto
 */
static void near pascal showhorizon(void)
{
    setwindow(Whorz) ;
    putimage(0,0,horizon,COPY_PUT) ;
} /* ShowHorizon */

/*
 * Disegna gli astri all'interno della cornice
 */
void outsoleluna(void) /* pubblica */
{
    const int ix=icnX>>1, iy=icnY>>1 ; /* offset centro-spigolo dell'astro */
    int dX,dY, temp ;
    float ang ;
    unsigned newsolu=packtempo(&stato.tempo) ;

    if (newsolu>oldsolu+5) {
	showhorizon() ; /* cancella */
	ang=-M_PI_2-((double)stato.tempo.ore+(double)stato.tempo.min/60.0)
	    /12.0*M_PI ;
	dX=35.0*cos(ang) ;
	dY=(35.0*asp)*sin(ang) ;
	if (dY+iy>0) {
	    if (dY+iy<(temp=sole->Dy)) sole->Dy=dY+iy ;
	    /* riduce artificialmente l'altezza dell'immagine
		"ingannando" la PutImage */
	    putimage(wind(xc)-ix+dX,wind(yc)-iy-dY,sole,OR_PUT) ;
	    sole->Dy = temp ; /* rimettiamo le cose subito a posto !! */
	}
	if (-dY+iy>0) {
	    if (-dY+iy<(temp=luna->Dy)) luna->Dy=-dY+iy ; /* v.sopra */
	    putimage(wind(xc)-ix-dX,wind(yc)-iy+dY,luna,OR_PUT);
	    luna->Dy = temp ;
	}
	oldsolu=newsolu ;
    }
} /* OutSoleLuna */

/*
 * Disegna i 2 quadranti vuoti
 */
static void near pascal initquadranti(void)
{
    sole = loadfile (grpath("sole.byt")) ;
    luna = loadfile (grpath("luna.byt")) ;
    horizon = loadfile (grpath("horizon.byt")) ;
    showhorizon() ;
    initwatch() ;
} /* InitQuadranti */

/*
 * 3) Barrette, cotillons e oggetti della finestra di Status
 */

/* inventari */
#define delX 52
#define delY 5
#define dimX 144
#define dimY 49
#define ofsY 55

/* formato intermedio di visualizzazione delle icone */
typedef struct {
    unsigned x,y ;
    byte z[150] ; /*stiamo comodi*/
} icimg ;

typedef struct {
    unsigned x,y ;
    byte z[30000] ;
} allimg ;

static char OLDbarrette [antipatia+1] ;

/*
 * Disegna gli indicatori %
 */
static void near pascal barrette(void)
{
    int x ;
    register int x2,y ;
    char s[5] ;
    t_barrette i ;

    setwindow(Windic) ;
    settextjustify(RIGHT_TEXT,TOP_TEXT) ;
    y=wind(xc) ;
    for(i=punti; i<=antipatia; ++i) {
	x=stato.barrette[i] ;
	if (x!=OLDbarrette[i]) {
	    OLDbarrette[i]=x ;
	    sprintf(s,"%3d",x) ;
	    x2=x/2 ;
	    setfillpattern(Fills[grigio],1) ;
	    setfillstyle(USER_FILL,1) ;
	    bar(99,y+1,99+x2,y+wind(yc)/2) ;
	    setfillstyle(SOLID_FILL,1) ;
	    bar(99+x2+1,y+1,99+50,y+wind(yc)/2) ;

	    bar(99+50+3,y,183,y-1+textheight("0")) ;
	    outtextxy(183,y-1,s) ;
	} /* if */
	y+=wind(yc) ;
    } /* for */
}/* barrette */

/*
 * Scrive un elenco di nomi di oggetto nelle finestre-inventario
 */
static void near pascal elenco(t_list_objs d)
{
#define MAXCHARS 200
    register int i,j ;
    char s [25] ;
    char st [MAXCHARS], *c, *ip ;

    *st = '\0' ;
    for(i=0; i<ListObjSize; ++i) {
	if (!d[i]) break ; /* esci dal loop al primo elemento 0 */
	if (d[i]&&(belongs(d_visibi,obj[d[i]].stato))) {
	    strcpy(s,nomeobj(d[i],0)) ;
	    for(j=0; s[j]; ++j) s[j]=_8to7[s[j]] ;
	    s[0]=toupper(s[0]) ;
	    if (strlen(st)+strlen(s)+1 >= MAXCHARS-1) break ;
	    strcat(st,s) ;
	    strcat(st," ") ;
	}
    }
    moveto(0,-1) ;
    setcolor(0) ;
    /* N.B. st[] finisce CERTAMENTE per spazio (o Š vuota) */
    c = st ;
    while (*c) {
	ip=strchr(c,' ') ;
	*s = '\0' ;
	strncat(s,c,(size_t)(ip-c)) ;
	c=ip+1 ;
	if(textwidth(s)+getx() > dimX) moveto(0,gety()+textheight("A") ) ;
	outtext(strcat(s," ")) ;
    }
#undef MAXCHARS
} /* elenco */

/*
 * Aggiorna barrette ed icone della finestra di stato
 */
void outstatus(void) /* pubblica */
{
    icimg a ;
    barrette() ;
    setwindow(Wicons) ;
    a.x=icnX-1;
    a.y=icnY-1;
    memcpy(a.z,icona+(24+stato.posizione)*icnS,icnS) ;
    putimage(11,0,&a,NOT_PUT);
    memcpy(a.z,icona+(20+stato.velocita)*icnS,icnS) ;
    putimage(61,0,&a,NOT_PUT);
    memcpy(a.z,icona+(38+statoAUX.stampante)*icnS,icnS) ;
    putimage(111,0,&a,NOT_PUT);
} /* OutStatus */

/*
 * Revisualizza gli inventari (uno, l'altro o entrambi a seconda di 'tipo')
 */
void outinventari(int tipo) /* pubblica */
{
    settextjustify(LEFT_TEXT,TOP_TEXT);
    if ((tipo==2)||(tipo==3)) {
	setwindow(Wmyogg) ;
	bar(0,0,wind(dx),wind(dy));
	elenco(myobjs) ;
    }
    if ((tipo==1)||(tipo==3)) {
	setwindow(Wlogg) ;
	bar(0,0,wind(dx),wind(dy));
	elenco(l_objs);
    }
} /* OutInventari */

/*
 * Inizializza le variabili usate dal modulo
 */
void initstatus(void) /* pubblica */
{
    void * p ;
    int x,y ;

    getaspectratio ( &x, &y ) ;
    asp = (double)x / (double)y ;

    /* init code */
    setmem(OLDbarrette,sizeof(OLDbarrette),0xFF);
    oldtime = 0 ;
    oldsolu = 0 ;

    Wstatus = gimmewindow( "STATUS  ", "STATUS  " ) ;
    Windic  = gimmewindow( "STATUS  ", "INDIC   " ) ;
    Wicons  = gimmewindow( "STATUS  ", "ICONS   " ) ;
    Woggs   = gimmewindow( "STATUS  ", "OGGS    " ) ;
    Wmyogg  = gimmewindow( "STATUS  ", "MYOGGS  " ) ;
    Wlogg   = gimmewindow( "STATUS  ", "LOGGS   " ) ;

    Wora    = gimmewindow( "STATUS  ", "ORA     " ) ;
    Whorz   = gimmewindow( "STATUS  ", "HORZ    " ) ;

    setwindow(Wstatus);
    p = loadfile(grpath("status.byt")) ;
    putimage(0,0,p,COPY_PUT);
    free(p) ;

    setwindow(Wicons) ;
    icnX = wind(xc) ;
    icnY = wind(yc) ;
    icnS = (icnX*icnY)>>3 ;

    initquadranti() ;
} /* InitStatus */
