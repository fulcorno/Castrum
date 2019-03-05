/*
 * header "windows.h"
 *
 * Gestione delle finestre dell'avventura. Permette l'indipendenza dell'output
 * grafico dal tipo di scheda grafica montata, individuando run-time quali
 * files devono essere letti ed usati per le visualizzazioni.
 */

typedef struct finestra {
    int x0, y0, dx, dy ; /* dimensioni e posizione */
    int father ;    /* finestra a cui sono riferite x0 e y0 */
    int xc, yc ;    /* "centro" virtuale (usato dall'applicativo) */
    int font, multx,divx, multy,divy ;	    /* carattere di scrittura */
    FillIndex bkfill ; /* campitura sfondo */
    int bkcolor ;   /* colore sfondo (=1 se b/w) */
    int fgcolor ;   /* colore di primo piano (contrastante con lo sfondo) */
    void * covers ; /* immagine che questa finestra copre */
} ;

extern struct finestra * cur_win ; /* _*non*_ usare direttamente ! */

/* MACRO wind ( tag ) ; */
#define wind(x) (cur_win->x)
/* si accede ai campi della struttura come wind(xxx) */

/*
 * setpencolor()
 * Assegna la campitura ed il colore di default della finestra corrente
 */
void setpencolor(void) ;

/*
 * setwindow(n^finestra)
 * Rende corrente la finestra specificata
 */
int setwindow (int wind) ;

/*
 * createwindow(x0,y0,dx,dy,father,xc,yc,font,multx,divx,multy,divy,bkfill,
 *	bkcolor,fgcolor)
 * Crea una nuova finestra con i parametri specificati e restituisce il suo
 * 'identificativo', ossia il numero con il quale ci si dovr… riferire ad essa
 * nelle successive manipolazioni. Se ritorna <0 c'Š stato errore.
 */
int createwindow ( int x0, int y0, int dx, int dy, int father, int xc, int yc,
    int font, int multx, int divx, int multy, int divy, FillIndex bkfill,
    int bkcolor, int fgcolor ) ;

/*
 * initwindows()
 * Inizializza le variabili (in particolare Windows[0]) in base al modo grafico
 * correntemente settato
 */
void initwindows ( void ) ;

/*
 * getmaxwindow()
 * Ritorna il numero totale di finestre inizializzate
 */
int getmaxwindow(void) ;

/*
 * savewindows(n^finestre,nomefile)
 * Salva il blocco di 'n' finestre sul file
 */
void savewindows(int n, FILE * f) ;

/*
 * readwindows(n^finestre,nomefile)
 * Legge il blocco di 'n' finestre dal file
 */
void readwindows(int n, FILE * f) ;

/*
 * gimmewindow(owner,ident)
 * Restituisce l'indice della finestra nominata
 */
int gimmewindow ( char owner[8], char ident[8] ) ;

/*
 * loadenviron(nomefile)
 * Carica un set di finestre come specificato dal file
 */
void loadenviron( const char * fname ) ;

/*
 * savebkground(n^finestra)
 * salva l'immagine contenuta nella zona dove andr• ad aprire una nuova
 * finestra, in modo che possa poi essere ripristinata alla chiusura
 * di quest'ultima.
 */
void savebkground(int win) ;

/*
 * loadbkground(n^finestra)
 * rimette il video com'era precedentemente all'apertura della finestra,
 * facendola scomparire a tutti gli effetti.
 */
void loadbkground(int win) ;
