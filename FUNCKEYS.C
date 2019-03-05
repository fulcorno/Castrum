/* modulo "FuncKeys.c"
 * Gestisce la finestra dei tasti funzione
 */

/* INTERFACE */

#include <graphics.h>
#include <string.h>
#include <ctype.h>
#include <mem.h>
#include <conio.h>
#include <stdio.h>
#include <dos.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"
#include "windows.h"

#include "funckeys.h"

/* IMPLEMENTATION */

static struct icona {
    int dx,dy ;
    byte dati [150] ;
} icon_buf ;

static int cur_verbo ; /* azione scelta nei menus */
static indice_obj cur_obj1, cur_obj2 ; /* parametri scelti da menu */
static indice_obj * cur_obj_ptr ; /* "alias" per cur_obj# (v. commenti oltre)*/
static int cur_menu = 0 ; /* # menu corrente */
static t_menu menu_loc ; /* copia locale del menu in output */

static int num_objs[10] ; /* associazione #obj <- funcKey */

static int lastkey ; /* memorizza il tasto per ReadKeyboard/RejectKey */

static byte *mascherina ;
/* immagine (bitmap) dei tasti funzione */

static int icnX, icnY, icnS ; /* dimensioni (in pixel) dell'icona */

static int Wtasti, Wtitolo, Wtasto[10] ;

enum mnemonici_tasti_funzione
    { F1, F2, F3, F4, F5, F6, F7, F8, F9, F10 } ;
/* N.B. F(i) == (i-1) . Ricordatelo !! */

/*
 * scrive il titolo dei tasti funzione, su due righe (separatore = '@')
 */
void outkeytitle(const char * sp)  /* pubblica */
{
    register int i ;
    char s1[40],s2[40] ;
    char * cp, s[80] ;

    for(i=0; sp[i]; ++i)
	s[i]=_8to7[sp[i]] ; /* Niente accenti & C. */
    s[i] = '\0' ; /* termina con NULL */
    /* Disegna lo sfondo del rettangolo per il titolo */
    setwindow(Wtitolo) ;
    bar(0,0,wind(dx),wind(dy)) ; /* fondo bianco */
    setlinestyle(USERBIT_LINE,0x5555,THICK_WIDTH) ;
    rectangle(0,0,wind(dx),wind(dy)) ; /* bordo "grigio" */
    setlinestyle(SOLID_LINE,0,1) ;
    settextjustify(CENTER_TEXT,TOP_TEXT) ;
    /* riconosci la 1^a e la 2^a riga, controlla la lunghezza e scrivi */
    cp=strchr(s,'@') ;
    if(cp) {
	strcpy(s2,cp+1) ;
	*cp = 0 ;
	strcpy(s1,s) ;
    } else {
	strcpy(s1,s) ;
	s2[0] = '\0' ;
    }
    outtextxy(wind(dx)/2, 0, s1 ) ;
    outtextxy(wind(dx)/2, textheight(s1) , s2 ) ;
} /* outkeytitle */

/*
 * Visualizza la mascherina dei tasti funzione vuota
 */
void delfkeys(void) /* pubblica */
{
    outkeytitle("") ;
    setwindow(Wtasti) ;
    putimage(0,0,mascherina,COPY_PUT) ;
} /* delfkeys */

/*
 * Scrive una frase in un tasto funzione
 */
static void near pascal outfkey(int n, const char * s)
{
    int y ;
    register int ct, ln ;
    char s1 [30], st[3] ;

    setwindow(Wtasto[n]) ;
    settextjustify(CENTER_TEXT,TOP_TEXT) ;
    bar(0,0,icnX-1,icnY-1);
    ln = strlen(s) ;
    ct = 0 ;
    y = -1 ;
    /* scrive una parola per riga (righe separate da "@") */
    while (((textheight("A")+y)<icnY)&&(ct<=ln)) {
	s1[0]='\0' ;
	while ((ct<ln)&&(s[ct]!='@')) {
	    st[0] = toupper(_8to7[s[ct]]) ;
	    st[1] = '\0' ;
	    strcat(s1,st) ;
	    ++ct ;
	}
	++ct ;
	outtextxy(wind(xc),y,s1) ;
	y += textheight(s1) ;
    }
} /* OutFKey */

/*
 * Disegna un'icona in un tasto funzione
 */
static void near pascal outicon(int n, indice_icona c)
{
    icon_buf.dx=icnX-1;
    icon_buf.dy=icnY-1;
    memcpy(&icon_buf.dati, icona+c*icnS, icnS) ;
    setwindow(Wtasto[n]) ;
    bar(0,0,icnX-1,icnY-1);
    putimage(0,0,&icon_buf,NOT_PUT);
} /* OutIcon */

/*
 * Fa "lampeggiare" (anzi, tremolare) il tasto funzione premuto (n)
 */
static void near pascal flashkey(int n)
{
    register int i ;

    setwindow(Wtasto[n]) ;
    getimage(0,0,icnX-1,icnY-1,&icon_buf) ;
    for(i=0; i<10; ++i) {
	putimage(-1,-1, &icon_buf, COPY_PUT) ;
	putimage( 0,-1, &icon_buf, COPY_PUT) ;
	putimage( 1,-1, &icon_buf, COPY_PUT) ;
	putimage( 1, 0, &icon_buf, COPY_PUT) ;
	putimage( 1, 1, &icon_buf, COPY_PUT) ;
	putimage( 0, 1, &icon_buf, COPY_PUT) ;
	putimage(-1, 1, &icon_buf, COPY_PUT) ;
	putimage(-1, 0, &icon_buf, COPY_PUT) ;
    }
} /* FlashKey */

/*
 * Crea un tasto funzione in 'menu_loc' con gli attributi dati
 */
static void near pascal makekey(int n, int az, int nm, indice_icona ic,
    char * des )
{
    t_tastofunz * p = & menu_loc.tasto[n] ;
    p->azione	= az ;
    p->nextmenu = nm ;
    p->disegno	= ic ;
    strcpy(p->frase, des) ;
} /* makekey */

/*
 * Lista di oggetti posseduti e/o nel luogo
 */
static void near pascal lista(int tipo, int nm, int reset)

/* crea una lista di tasti funzione, contenente:
 *  (tipo==1): gli oggetti nel luogo
 *  (tipo==2):	"     "    posseduti
 *  (tipo==3):	"     "    nel luogo e/o posseduti
 *  In pi— se (reset==1) parte dell'inizio, altrimenti continua da dove si
 *  era fermata l'ultima chiamata. Nel caso in cui ci siano pi— di 9 voci
 *  da visualizzare, visualizza solo le prime 9 ed associa al tasto F10 la
 *  possibilit… di vedere le successive (menu speciale -8).
 *  Pu• venire chiamata quindi dai men— -1,-2,-3 (con reset==1), oppure dal
 *  -8, questa volta trattandosi di un'<autochiamata>.
 *  Elegante, vero ? Sembra quasi di lavorare in concorrenza: mancano solo i
 *  semafori e poi siamo al completo.
 */

{
    int ctr=F1, /* #fkey to fill */
	j,k ;
    int loop=1 ;
    char s[25],s1[25] ;
    static indice_obj * loc_list ;
    static int next, loc_nm, loc_tipo ;

    if(reset) {
	next = 0 ;
	loc_list = (tipo==2) ? myobjs : l_objs ;
	loc_nm = nm ;
	loc_tipo = tipo ;
    }

    while(loop) {
	if ( (next<ListObjSize)&&(loc_list[next])) {
	    /* se sono qui ci sono ancora oggetti nella lista corrente */
	    if (ctr<=F9) {
		/* qui c'Š posto per inserirlo (NON uso F10) */
		if (belongs(d_visibi,obj[loc_list[next]].stato)) {
		    /* qui lo DEVO inserire */
		    strcpy(s,nomeobj(loc_list[next],0)) ;
		    /* spezza su pi— linee */
		    for (k=j=0; s[j]; ++j) {
			if(!((j+1)%6)) s1[k++] = '@' ;
			s1[k++] = s[j] ;
		    }
		    s1[k]='\0' ;

		    num_objs[ctr]=loc_list[next] ;
		    makekey(ctr, 0,loc_nm, 0,s1) ;
		    ++ ctr ;
		} /* else non lo inserisco */
		/* qui non uso pi— l'oggetto appena memorizzato */
		++ next ;
	    } else {
		/* non ho pi— posto per inserirlo */
		if(belongs(d_visibi,obj[loc_list[next]].stato)) {
		    /* se proprio lo devo inserire */
		    makekey(F10, 0,-8, 0,"--->@segue@--->") ;
		    loop = 0 ;
		} else ++next ; /* se no lo scarto */
	    }
	} else {
	    /* qui ho raggiunto la fine di una lista */
	    if ((loc_tipo==3)&&(loc_list==l_objs)) {
		/* qui devo continuare con l'altra lista */
		loc_list = myobjs ;
		next = 0 ;
	    } else loop = 0 ; /* se no esci */
	}
    } /* while loop */
} /* lista */

/*
 * Lista dei verbi possibili con l'oggetto 'og', posseduto o nel luogo a
 * seconda del valore di 'where' (risp. 2 e 1)
 */
static void near pascal listaverbi(indice_obj og, int where, int *ctr )
{
    indice_verbo i ;
    t_dato * j ;
    setof(v,max_verbo) ;

    j = & dato [ tipotodato(&obj[og].tipo) ] ;
    if (where==1) copyset(v,j->verbo_L) ;
    else copyset(v,j->verbo_P) ;
    for(i=1; i<=tot_verbi; ++i)
	if (belongs(i,v)) {
	    ++ *ctr ;
	    if (*ctr<=10)
		makekey(*ctr-1, i, - verbo[i].tipo, 0, verbo[i].nome) ;
	    else internalerror("Too many items in list","listaverbi",*ctr) ;
	}
} /* ListaVerbi */

/*
 * Visualizza un menu` preso dall'array menu[] se n>0, o un menu` "speciale"
 * quando n<0
 */
void outmenu(int n) /* pubblica */
{
    int i,x,y,z ;
    t_tastofunz * p ;

    delfkeys() ;
    memset(&num_objs,0,sizeof(num_objs)) ;
    memset(&menu_loc,0,sizeof(menu_loc)) ;
    if (n>0) memcpy(&menu_loc,&menu[n],sizeof(t_menu)) ;
    else if (n<0) switch (n) {
	case -1:
	case -2:
	case -3:
	    if (cur_verbo<=0) strcpy(menu_loc.titolo,"Scegli un@OGGETTO") ;
	    else {
		strcpy(menu_loc.titolo,prepos[verbo[cur_verbo].prep]) ;
		strcat(menu_loc.titolo,"@cosa?" ) ;
	    }
	    lista( -n, ( (cur_verbo) ? 0 : (n-3) ), 1 ) ;
	    /* fa la lista degli oggetti del tipo giusto, conservando l'azione
	     * o preparandosi per chiedere il verbo, ripartendo da capo
	     */
	    break ;
	case -4:
	case -5:
	case -6:
	    strcpy(menu_loc.titolo,"Che cosa@ci fai?") ;
	    x=0 ;
	    if ((n==-4)||(n==-6)) listaverbi(cur_obj1,1,&x);
	    if ((n==-5)||(n==-6)) listaverbi(cur_obj1,2,&x);
	    break ;
	case -7: /* lista delle porte nella stanza */
	    if (cur_verbo==-7)
		strcpy(menu_loc.titolo,"Quale PORTA@vuoi GUARDARE ?") ;
	    else if ((cur_verbo==-5)||(cur_verbo==43))
		strcpy(menu_loc.titolo,"Quale PORTA@vuoi APRIRE ?") ;
	    else if ((cur_verbo==-6)||(cur_verbo==44))
		strcpy(menu_loc.titolo,"Quale PORTA@vuoi CHIUDERE ?") ;
	    for (x=0; x<=3; ++x) {
		y=numeroporte( stato.dovesei ,x ) ;
		if (y==1) makekey(x*2, 0,0, 45+x,"") ;
		else if (y==2) {
		    makekey(x*2, 0,0, x*2+49,"") ;
		    makekey(x*2+1, 0,0, x*2+50,"") ;
		}
	    }
	    break ;

	case -8: /* questo Š un punto "di sponda" attraverso il quale la
		procedura lista() richiama s‚ stessa. Io devo solo porre reset=0
		in modo che continui ad usare i parametri gi… memorizzati. */
	    lista(0,0,0) ;
	    break ;
	case -10:
	case -11:
	case -12:
	case -13: /* 2 porte nella parete (-n-10) */
	    strcpy(menu_loc.titolo,"Da quale PORTA@vuoi USCIRE ?") ;
	    x=-n-10 ;  /* x == #parete */
	    switch (x) {
		case 0: y=F2; z=F4; break ;
		case 1: y=F4; z=F3; break ;
		case 2: y=F3; z=F1; break ;
		case 3: y=F1; z=F2; break ;
		default: internalerror("Invalid wall number","outmenu",x) ;
	    }
	    makekey(y, -10,0, 49+x*2,"") ;
	    makekey(z, -10,0, 50+x*2,"") ;
	    num_objs[y]=-x*2-1 ;
	    num_objs[z]=-x*2-2 ;
	    break ;

	default:
	    internalerror("Parameter 'n' out of range","OutMenu",n) ;
    } /* switch */
    outkeytitle(menu_loc.titolo) ;
    for(i=0; i<=9;++i) {
	p = & menu_loc.tasto[i] ;
	if ((p->disegno)&&(!*p->frase))
	    outicon(i,p->disegno) ;
	else if ((!p->disegno)&&(*p->frase))
	    outfkey(i,p->frase) ;
	else if ((p->disegno)||(*p->frase))
	    if belongs(g_fkeys,statoAUX.grafica)
		outicon(i,p->disegno) ;
	    else outfkey(i,p->frase) ;
    }
} /* OutMenu */

/*
 * Legge il tasto premuto (se ce n'e` uno), gestendo un carattere di "pushback"
 * e gli extended codes
 */
int readkeyboard(void) /* pubblica */
{
    int c ;

    if (lastkey) {
	c = lastkey ;
	lastkey = 0 ;
	return (c) ;
    } else if (kbhit()) {
	c=getch() ;
	if (!c) return ( - getch() ) ;
	else return (c) ;
    } else return (0) ;
} /* ReadKeyboard */

/*
 * Effettua il pushback del carattere specificato
 */
void rejectkey(int key) /*pubblica*/
{
    lastkey=key ;
} /* RejectKey */

/*
 * Cancella il buffer di tastiera ed il pushback
 */
void flushkeyboard(void) /* pubblica */
{
    while(readkeyboard())
	/*NOP*/ ;
} /* FlushKeyboard */

/*
 * Riporta alla radice dell'albero dei menu`
 */
static void near pascal resettree(void)
{
    cur_menu=1 ;
    cur_verbo=0 ;
    cur_obj1=0 ;
    cur_obj2=0 ;
    cur_obj_ptr = &cur_obj1 ; /* si inizia sempre a scegliere il 1^ oggetto */
} /* ResetTree */

/*
 * Finge la selezione lungo la catena dei menu` dell'azione specificata
 */
static void near pascal simula(int s_verbo, int s_obj1, int s_obj2 )
{
    cur_menu=0 ;
    cur_verbo=s_verbo ;
    cur_obj1=s_obj1 ;
    cur_obj2=s_obj2 ;
} /* simula */

/*
 * Forza il menu` specificato, irrispettoso della posizione precedente lungo
 * l'albero
 */
void gotomenu (indice_menu n) /* pubblica */
{
    resettree() ;
    cur_menu=n ;
    outmenu(n) ;
}   

/*
 * Gestisce l'interazione con l'utente lungo l'albero dei menu`, gestendo anche
 * le "hot keys"
 */
void menuselect ( int *verbo, int *obj1, int *obj2 )  /* pubblica */
{
    int rk,key,lst_menu ;
    t_tastofunz * p ;

    if (!cur_menu) {
	resettree() ;
	outmenu(cur_menu) ;
    }

    lst_menu=cur_menu ;
    rk=readkeyboard() ;
    key=k_f1-rk ;
    if ((key>=0)&&(key<=9)) {
	p = & menu_loc.tasto[key] ;
	if ( p->azione || p->nextmenu || p->disegno || *p->frase ) {
	    flashkey(key);
	    cur_menu = p->nextmenu ;
	    if (p->azione) cur_verbo=p->azione ;
	    *cur_obj_ptr = num_objs [key] ; /* oggetto scelto */
	    if (*cur_obj_ptr) cur_obj_ptr= & cur_obj2 ;
/* se ho effettivamente scelto un oggetto, mi preparo per scegliere
    il secondo, aggiornando l"alias (se era gi… il secondo, nulla
    varia). Il metodo Š abbastanza "sporco", ma funziona. */
	    else *cur_obj_ptr = key ;
	}
/* se non c'Š un argomento pronto, trasmetto in uscita il numero di tasto
    premuto nell'ultimo menu selezionato */
    } else switch(rk) {
	case k_esc:
	    resettree() ;  /* ESC => annulla azione */
	    break ;

	case k_rt:
	    simula(-10,F7,0) ; /* --> */
	    break ;
	case k_dn:
	    simula(-10,F8,0) ; /* \|/ */
	    break ;
	case k_lt:
	    simula(-10,F9,0) ; /* <-- */
	    break ;
	case k_up:
	    simula(-10,F10,0); /* /|\ */
	    break ;

	case k_tab:	/* -->| TAB => switcha tra frasi e icone */
	    lst_menu=0 ;
	    if (belongs(g_fkeys,statoAUX.grafica))
		subset(g_fkeys,statoAUX.grafica) ;
	    else addset(g_fkeys,statoAUX.grafica) ;
	    break ;

	case k_pgup:	/* PgUp => alza la POSIZIONE */
	    if (stato.posizione>in_piedi)
		--stato.posizione ;
	    break ;
	case k_pgdn:	/* PgDn => abbassa la POSIZIONE */
	    if (stato.posizione<a4gambe)
		++stato.posizione ;
	    break ;

	case k_plus:	/* '+, => aumenta la VELOCITA` */
	    if (stato.velocita<corri)
		++stato.velocita ;
	    break ;
	case k_mins:	/* '-, => diminuisce VELOCITA`*/
	    if (stato.velocita>punta_piedi)
		--stato.velocita ;
	    break ;

	case k_star:	/* '*, => spegne la stampante */
	    simula(-15,F1,0) ;
	    break ;

	case k_ret:	/* ferma il gioco: DA ELIMINARE */
	    simula(-128,0,0) ;
	    break ;
    }
    if (!cur_menu) {   /* non c'Š un menu successivo ==> si deve
			    compiere una qualche azione */
	*verbo=cur_verbo ;
	*obj1=cur_obj1 ; /* aggiornati indirettamente tramite il loro */
	*obj2=cur_obj2 ; /* comune "alias" cur_obj_ptr */
	resettree() ;
	lst_menu=1 ;
	cur_menu=0 ;
    } else *verbo=0 ; /* indica che NON ci sono azioni da compiere al
	momento per le routines esterne: si Š solo passati ad un altro menu,
	eventualmente con qualche azione "pendente", che per• non Š ancora
	attuabile e quindi viene "custodita" localmente */
    if (lst_menu!=cur_menu) outmenu(cur_menu) ;
} /* MenuSelect */

/*
 * Da chiamare per prima: inizializza le variabili necessarie al corretto
 * funzionamento di tutte le procedure contenute nel modulo
 */
void initfunckeys(void)
{
    register int i ;
    char buf[10] ;

    Wtasti = gimmewindow( "FUNCKEYS", "TASTI   " ) ;
    Wtitolo = gimmewindow( "FUNCKEYS", "TITOLO  " ) ;
    mascherina = loadfile (grpath("tasti.byt")) ;
    icona = loadfile(grpath("icons.byt")) ;
    for (i=F1; i<=F10; ++i) {
	sprintf(buf,"TASTO%d  ",i) ;
	Wtasto[i] = gimmewindow ( "FUNCKEYS", buf ) ;
    }
    setwindow(Wtasti) ;
    icnX = wind(xc) ;
    icnY = wind(yc) ;
    icnS = (icnX*icnY)>>3 ;
    resettree() ;
    rejectkey(0) ;
} /* module FuncKeys */
