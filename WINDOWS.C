/*
 * modulo "windows.c"
 *
 * Gestione delle finestre dell'avventura. Permette l'indipendenza dell'output
 * grafico dal tipo di scheda grafica montata, individuando run-time quali
 * files devono essere letti ed usati per le visualizzazioni.
 */

/* INTERFACE */

#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"

#include "windows.h"

/* IMPLEMENTATION */

#define MaxWindow 30

struct Wnames {
    char own[8];
    char id[8];
} ;

static struct finestra windows [ MaxWindow ] ;
static struct Wnames Wnames [ MaxWindow ] ;
static int tot_finestre = 0 ;

struct finestra * cur_win = & windows[0] ;

/*
 * Assegna la campitura ed il colore di default della finestra corrente
 */
void setpencolor(void) /* pubblica */
{
    setfillstyle(USER_FILL,wind(bkcolor)) ;
    setfillpattern(Fills[wind(bkfill)],wind(bkcolor)) ;
    setcolor(wind(fgcolor)) ;
} /* end of setpencolor() */

/*
 * Ricerca le coordinate "vere" della finestra wind
 */
static void near pascal ltcorner(register int win, int * x, int * y )
{
    *x += windows[win].x0 ;
    *y += windows[win].y0 ;
    if (windows[win].father)
	ltcorner(windows[win].father, x, y) ;
} /* end of ltcorner() */

/*
 * Rende corrente la finestra specificata. Se ritorna != 0 c'Š stato errore
 */
int setwindow (int win) /* pubblica */
{
    int x=0,y=0 ;

    if ((win>=0)&&(win<tot_finestre)) {
	cur_win = & windows [win] ;
	ltcorner(win, &x, &y) ;
	setviewport(x,y, x+wind(dx), y+wind(dy), 1) ;
	setpencolor() ;
	selectfont(wind(font),0) ;
	setusercharsize(wind(multx),wind(divx),wind(multy),wind(divy)) ;
	setlinestyle(SOLID_LINE,0,1) ;
	return 0 ;
    } else {
	cur_win = & windows [0] ;
	return -1 ;
    }
} /* end of setwindow() */

/*
 * Crea una nuova finestra con i parametri specificati e restituisce il suo
 * 'identificativo', ossia il numero con il quale ci si dovr… riferire ad essa
 * nelle successive manipolazioni. Se ritorna <0 c'Š stato errore.
 */
int createwindow ( int x0, int y0, int dx, int dy, int father, int xc, int yc,
    int font, int multx, int divx, int multy, int divy, FillIndex bkfill,
    int bkcolor, int fgcolor ) /* pubblica */
{
#   define assign(x) cw->x=x
    struct finestra * cw = & windows [tot_finestre] ;

    if (tot_finestre<MaxWindow) {
	tot_finestre++ ;
	assign(x0); assign(y0); assign(dx); assign(dy); assign(father);
	assign(xc); assign(yc); assign(font); assign(multx); assign(divx);
	assign(multy); assign(divy); assign(bkfill); assign(bkcolor);
	assign(fgcolor) ;
	return (tot_finestre - 1) ;
    } else return -1 ;
#   undef assign
} /* end of createwindow() */

/*
 * Inizializza le variabili (in particolare Windows[0]) in base al modo grafico
 * correntemente settato
 */
void initwindows ( void ) /* pubblica */
{
    tot_finestre = 1 ;
    /* N.B. window[0].xxx == window->xxx */
    windows->x0 = windows->y0 = 0 ;
    windows->dx = getmaxx() ;
    windows->dy = getmaxy() ;
    windows->father = 0 ;
    windows->xc = windows->yc = 0 ;
    windows->font = DEFAULT_FONT ;
    windows->multx = windows->divx= windows->multy = windows->divy = 1 ;
    windows->bkfill = nero ;
    windows->bkcolor = 1 ;
    windows->fgcolor = 0 ;
} /* end of initwindows() */

/*
 * Ritorna il numero totale di finestre inizializzate
 */
int getmaxwindow(void) /* pubblica */
{
    return tot_finestre ;
} /* end of maxwindow() */

/*
 * Salva il blocco di 'n' finestre sul file
 */
void savewindows(int n, FILE * f) /* pubblica */
{
    fwrite(windows,sizeof(struct finestra),n,f) ;
} /* end of savewindows() */

/*
 * Legge il blocco di 'n' finestre dal file
 */
void readwindows(int n, FILE * f) /* pubblica */
{
    fread(windows,sizeof(struct finestra),n,f) ;
    tot_finestre = n ;
} /* end of readwindows() */

/*
 * Restituisce l'indice della finestra nominata
 */
int gimmewindow ( char owner[8], char ident[8] ) /* pubblica */
{
    int i ;

    for (i=1; i<tot_finestre; ++i)
    if ((!strncmp(owner,Wnames[i].own,8))&&(!strncmp(ident,Wnames[i].id,8)))
	return i ;
    return 0 ;
} /* end of gimmewindow() */

/*
 * Carica un set di finestre come specificato dal file
 */
void loadenviron( const char * fname ) /* pubblica */
{
    FILE * f ;
    char filename [40] ;
    int i ;

    strcpy( filename, fname ) ;
    strcat( filename, ".WIN" ) ;
    f = fopen ( filename, "rb" ) ;

    if (!f) {
	fprintf(stderr,"Missing file %s\n",filename) ;
	exit(1) ;
    }

    tot_finestre = getw( f ) ;
    fread( Wnames, sizeof(struct Wnames), tot_finestre, f ) ;
    fread( windows, sizeof(struct finestra), tot_finestre, f) ;

    fclose(f) ;

    /* inizializzazioni necessarie PENA CRASH! */
    for (i=0; i<tot_finestre; ++i)
	windows[i].covers = NULL ;
} /* end of loadenviron() */

/*
 * savebkground(n^finestra)
 * salva l'immagine contenuta nella zona dove andr• ad aprire una nuova
 * finestra, in modo che possa poi essere ripristinata alla chiusura
 * di quest'ultima.
 */
void savebkground(int win) /* pubblica */
{
    size_t ln ;
    struct finestra * w = & windows[win] ;
    int x0,y0 ;

    if (win >= tot_finestre) internalerror ("Window number out of range",
	"savebkground",win) ;
    if (windows[win].covers) internalerror ("Window superimposed twice",
	"savebkground",win) ;

    x0 = y0 = 0 ;
    ltcorner(win,&x0,&y0) ;
    ln = imagesize(x0,y0,x0+w->dx,y0+w->dy) ;
    if ( (w->covers = malloc(ln)) == NULL )
	internalerror ("No memory for background storing","savebkground",0) ;
    getimage(x0,y0,x0+w->dx,y0+w->dy,w->covers) ;
} /* end of savebkground() */

/*
 * loadbkground(n^finestra)
 * rimette il video com'era precedentemente all'apertura della finestra,
 * facendola scomparire a tutti gli effetti.
 */
void loadbkground(int win) /* pubblica */
{
    int x0=0, y0=0 ;
    struct finestra * w = & windows[win] ;

    if (win >= tot_finestre) internalerror ("Window number out of range",
	"loadbkground",win) ;
    if (w->covers) {
	ltcorner(win,&x0,&y0) ;
	putimage(x0,y0,w->covers,COPY_PUT) ;
	free(w->covers) ;
	w->covers = NULL ;
    }
} /* end of loadbkground() */
