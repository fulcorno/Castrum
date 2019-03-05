/* modulo "agisci.c"
 * contiene le procedure per :
 * - muoversi di stanza in stanza
 * - prendere/posare oggetti
 * - emettere descrizioni
 * - modificare la Variabile di Stato per simulare lo stato fisico
 * - usare i vari oggetti in tutti i modi possibili
 */

#include <stdio.h>
#include <graphics.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"
#include "windows.h"
#include "gotico.h"
#include "funckeys.h"
#include "room.h"
#include "status.h"
#include "objects.h"
#include "musica.h"

#include "agisci.h"

/* IMPLEMENTATION */


enum mnemonici_tasti_funzione
    { F1, F2, F3, F4, F5, F6, F7, F8, F9, F10 } ;

/*
 * Inizializza tutti moduli del programma con i giusti argomenti
 */
void initeverything(int argc, char ** argv) /* pubblica */
{
    initgraphscreen(initglobals(argc,argv)) ;
    initwindows() ;
    loadenviron ( getdrivername() ) ;
    initgotico() ;
    initfunckeys() ;
    initstatus() ;
    initroom() ;
    initobjects() ;
    outobject(0);
    outmenu(1);
    initmusica() ;
}

/* MACRO void outmsg(indice_msg n) ; */
#define outmsg(n) pprintf("%s\n",messaggio(n))

/*
 * Riempie la lista ls[] con il numero di tutti gli oggetti posti in 'dove'
 */
static void near pascal lookforobjs(int dove, t_list_objs ls )
{
    indice_obj i ;
    int c=0 ;

    memset(ls,0,sizeof(t_list_objs)) ;
    for (i=1; i<tot_objs; ++i)
	if (obj[i].dove == dove)
	    if (c<ListObjSize)
		ls[c++]=i ;
	    else internalerror("Too many objects to fit in list",
		"lookforobjs",c) ;
} /* look for objs */

/*
 * ricostruisce le talelle l_objs[] e/o myobjs[]
 */
void buildobjtables (int tipo) /* pubblica */
{
    int c=0 ;
    indice_dato i ;

    if ((tipo==1)||(tipo==3)) lookforobjs(stato.dovesei,l_objs) ;
    if ((tipo==2)||(tipo==3)) {
	lookforobjs(-1,myobjs);
	stato.trasporta[tot_peso]=0.0 ;
	stato.trasporta[tot_ingombro]=0.0 ;
	while (myobjs[c]) {
	    i = tipotodato(&obj[myobjs[c]].tipo) ;
	    stato.trasporta[tot_peso]+=c_miria[readdati(&dato[i].dati,d_peso)];
	    stato.trasporta[tot_ingombro] += readdati(&dato[i].dati,d_ingomb) ;
	    ++c ;
	} /* while */
    }
    outinventari(tipo) ;
} /* build obj tables */

/*
 * "Esce" da una porta
 */
static void near pascal esci(int parete, int door)
/* parete :: 0..3 ; door :: 1..2 */
{
    indice_porta np ;
    indice_luogo us ;
    t_parete * pr = &luogo[stato.dovesei].pareti[parete] ;
    t_porta * pp ;

    if (door==1) {
	np = pr->n_porta1 ;
	us = pr->n_uscita1 ;
    } else if (door==2) {
	np = pr->n_porta2 ;
	us = pr->n_uscita2 ;
    } else internalerror("Parameter 'door' out of range","esci",door) ;
    pp = & porta[np] ;
    if ((pp->tipo==p_passaggio)||((pp->tipo==p_porta)&&(pp->stato==p_aperto)))
    {
/* CONTROLLA E GESTISCI IL PANICO : DA AGGIUNGERE */
	stato.dovesei = us ;
	PassedThrough = np ;
	buildobjtables(1) ;
	drawroom() ;
    } else outmsg(5) ;
} /* esci */

/*
 * Prende (se possibile) un oggetto
 */
static void near pascal prendi(indice_obj og)
{
    t_obj * po = & obj[og] ;

    if (po->dove!=stato.dovesei)
    internalerror("Parameter 'og' means nonsense","prendi",og) ;
    else if ( !belongs(d_visibi,po->stato) || !belongs(d_sposta,po->stato) )
	pprintf("%s %s.\n",messaggio(11),nomeobj(og,1)) ;
    else {
	po->dove = -1 ;
	if (po->nasconde)
	    subset(d_visibi,obj[po->nasconde].stato) ;
	buildobjtables(3) ;
	if (belongs(p_azioni,statoAUX.prolix))
	    pprintf("%s %s.\n",messaggio(12),nomeobj(og,1)) ;
    }
} /* prendi */

/*
 * Posa un oggetto posseduto
 */
static void near pascal posa(indice_obj og)
{
    t_obj * po = & obj[og] ;

    if (po->dove != -1)
	internalerror("Parameter 'og' means nonsense","posa",og) ;
    else {
	po->dove = stato.dovesei ;
	buildobjtables(3) ;
	if (belongs(p_azioni,statoAUX.prolix))
	    pprintf("%s %s.\n",messaggio(13),nomeobj(og,1)) ;
    }
} /* posa */

/*
 * Apre una porta
 */
static void near pascal apri(int n)
{
    int par = (n/2) ,
	por = (n%2)+1,
	np ;
    t_parete * pp = & luogo[stato.dovesei].pareti[par] ;
    t_porta * po ;

    np = (por==1) ? pp->n_porta1 : pp->n_porta2 ;
    po = & porta[np] ;

    if (po->tipo==p_passaggio) outmsg(6) ;
    else if (po->tipo==p_ostruito) outmsg(10) ;
    else if (po->stato==p_aperto) outmsg(6) ;
    else if (po->stato==p_bloccato) outmsg(9) ;
    else {
	po->stato = p_aperto ;
	if (belongs(p_azioni,statoAUX.prolix)) outmsg(14) ;
	PassedThrough = np ;
	drawroom() ;
    }
} /* apri */

/*
 * Chiude una porta
 */
static void near pascal chiudi(int n)
{
    int par = n/2 ,
	por = (n%2)+1 ,
	np ;
    t_parete * pp = & luogo[stato.dovesei].pareti[par] ;
    t_porta * po ;

    np = (por==1) ? pp->n_porta1 : pp->n_porta2 ;
    po = & porta[np] ;

    if (po->tipo==p_passaggio) outmsg(8) ;
    else if (po->tipo==p_ostruito) outmsg(7) ;
    else if (po->stato==p_chiuso) outmsg(7) ;
    else if (po->stato==p_bloccato) outmsg(7) ;
    else {
	po->stato=p_chiuso ;
	if (belongs(p_azioni,statoAUX.prolix)) outmsg(15) ;
	PassedThrough = np ;
	drawroom() ;
    }
} /* chiudi */

/*
 * Emette la descrizione di una porta (se esiste)
 */
static void near pascal descrporta(int n)
{
    int par = n/2 ,
	por = (n%2)+1 ,
	np ;
    t_parete * pp = & luogo[stato.dovesei].pareti[par] ;
    t_porta * po ;

    np = (por==1) ? pp->n_porta1 : pp->n_porta2 ;
    po = & porta[np] ;

    if (!po->descrizione) outmsg(2) ;
    else pprintf("%s\n",frase[po->descrizione]) ;
} /* descr porta */

/*
 * Emette la descrizione di un oggetto (se esiste)
 */
static void near pascal descrobj(indice_obj n)
{
    t_obj * po = & obj[n] ;

    if (!po->descrizione)
	pprintf("%s %s.\n",messaggio(3),nomeobj(n,1)) ;
    else pprintf("%s\n",frase[po->descrizione]) ;
} /* descr obj */

/*
 * Emette la descrizione della stanza (se esiste)
 */
static void near pascal descrstanza(void)
{
    t_stanza * ps = & stanza[luogo[stato.dovesei].n_stanza] ;

    if (!ps->descrizione) outmsg(1) ;
    else pprintf("%s\n",frase[ps->descrizione]) ;
}

/*
 * Esegue l'azione (normale o 'speciale') 'az', con parametri 'o1' e 'o2'
 */
void azione(int az, int o1, int o2) /* pubblica */
{
    if (az<0) switch(az) { /* azioni speciali <0 */
	case -1:prendi(o1); break ;
	case -2:posa(o1); break ;
	case -3:descrobj(o1); break ;
	case -4:descrstanza(); break ;
	case -5:apri(o1); break ;
	case -6:chiudi(o1); break ;
	case -7:descrporta(o1); break ;
	case -8:stato.velocita=o1 ; break ;
	case -9:stato.posizione=o1 ; break ;
	case -10:
	    if ((o1>=F7)&&(o1<=F10)) {
		int x = numeroporte(stato.dovesei,o1-F7) ;
		if (!x) outmsg(4) ;
		else if (x==1) esci(o1-F7,1) ;
		else if (x==2) gotomenu(-10-(o1-F7)) ;
		else internalerror("Invalid # of doors in wall","azione",x) ;
	    } else if ( ((-1-o1)>=0) && ((-1-o1)<=7) )
				/* -o1-1 == #porta + 2*parete */
		esci( (-1-o1)/2, ((-1-o1)%2)+1 ) ;
	    else internalerror("Invalid door specification","azione",-o1-1) ;
	    break ;
	case -11:
	    drawarrows() ;
	    while(!readkeyboard()) ;
	    drawroom() ;
	    break ;
	case -13:
	    switch(o1) {
		case F1:addset(p_azioni,statoAUX.prolix) ; break ;
		case F2:subset(p_azioni,statoAUX.prolix) ; break ;
		case F3:addset(p_stanze,statoAUX.prolix) ; break ;
		case F4:subset(p_stanze,statoAUX.prolix) ; break ;
		case F5:addset(p_oggetti,statoAUX.prolix) ; break ;
		case F6:subset(p_oggetti,statoAUX.prolix) ; break ;
		default:internalerror("Invalid 'prolix' switch","agisci",o1) ;
	    }
	    break ;
	case -14:
	    switch(o1) {
		case F1:addset(g_soffitto,statoAUX.grafica); break ;
		case F2:subset(g_soffitto,statoAUX.grafica); break ;
		case F3:addset(g_oggetto,statoAUX.grafica); break ;
		case F4:subset(g_oggetto,statoAUX.grafica); break ;
		case F5:subset(g_fkeys,statoAUX.grafica); break ;
		case F6:addset(g_fkeys,statoAUX.grafica); break ;
		default:internalerror("Invalid 'grafica' switch","agisci",o1) ;
	    }
	    break ;
	case -15:
	    if((o1>=F1)&&(o1<=F6))
		statoAUX.stampante=o1 ;
	    else internalerror("Invalid 'stampante' switch","agisci",o1) ;
	    break ;
	case -18:if (o1==F10) musicoff() ;
	    else if (o1==F9) musicon() ;
	    /* da integrare, e poi...
	     * else internalerror("Call to undefined feature","agisci",o1) ;
	     */
	    break ;
	default:pprintf("Azione speciale sconosciuta: %d (%d,%d)\n",az,o1,o2) ;
	    break ;
    } /* switch */
    else if (az>0) {
	t_verbo * pv = & verbo[az] ;
	pprintf("Tu %s %s",pv->spiega,nomeobj(o1,1)) ;
	if (pv->tipo) {
	    if (pv->prep) pprintf(" %s",prepos[pv->prep]);
	    pprintf(" %s",nomeobj(o2,2));
	}
	pprintf("\n") ;
	/* ESEGUI MATERIALMENTE L'AZIONE QUI */
    }
} /* azione */
