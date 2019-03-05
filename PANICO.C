/* Sancti Georgii Castrum - Jena Soft
 * File "PANICO.C"
 * Contiene le procedure legate all'azionamento dei panici e all'interazione
 * con l'utente in quelle situazioni.
 */

/* USES */

#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <graphics.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"
#include "windows.h"
#include "gotico.h"

/* INTERFACE */

#include "panico.h"

/* IMPLEMENTATION */

typedef t_benemale (near pascal panic_fun)(int n) ;

static panic_fun scatta_niente
    /****************************************
    , scatta_quiz, scatta_indo, scatta_folle
    *****************************************/
    ;

static panic_fun *scatta[] =
    { scatta_niente,
    /****************************************
    scatta_quiz, scatta_indo, scatta_folle
    ****************************************/
    } ;

/*
 * MACRO int randrange(from,to)
 * ritorna un intero nel range specificato: [from,to]
 * valuta 'from' due volte
 */
#define randrange(f,t) ((f)+random((t)-(f)+1))

/*
 * fromto(low,high,val)
 * Ritorna 1 se val Š compreso (estremi inclusi) tra low e high, con
 * wrap-around nel caso in cui low>high.
 */
bool fromto(int low, int high, int val)
{
    return  (low<=high) ?
	( (low<=val)&&(val<=high) ) : ( (high<=val)||(val<=low) ) ;
}

/*
 * ctrl_sposta(indice_panico)
 * controlla se Š il caso di far scattare un panico di spostamento
 * ritorna 1 se Š scattato il panico
 */
bool ctrl_sposta ( indice_panico_sposta idx ) /* pubblica */
{
    int i ;
    struct dati *pdat ;
    t_obj *pobj ;
    t_benemale esito ;
    struct varia_sposta *q ;

    t_panico_sposta * p = & panico_sposta[idx] ;
    int delta = randrange(p->cost_min,p->cost_max) +
	stato.individuo[suo_peso] * randrange(p->peso_min,p->peso_max) +
	stato.individuo[sua_altezza] * randrange(p->alto_min,p->alto_max) +
	stato.velocita * randrange(p->velo_min,p->velo_max) ;
    
    /* Aggiorna le variabili e vede se Š ora di scattare */

    if ( (int)(p->ctr -= delta) >0 ) return(0) ;

    /* DEVE SCATTARE */

    /* Ora chiama la User Interface */
    esito = scatta[p->quesito.tipo](p->quesito.numero) ;
    if(esito!=Bene && esito!=Male)
	internalerror("Wrong BeneMale status","ctrl_sposta",esito) ;
    q = & p->varia[esito] ;

    stato.barrette[energia] *= 1.0+ (q->Den_ /100.0) ;
    stato.barrette[fatica] += q->Dfat ;
    stato.barrette[morale] += q->Dmor ;
    stato.barrette[patologia] += q->Dpat ;
    for (i=0; myobjs[i]; ++i) {
	pobj = &obj[myobjs[i]] ;
	pdat = &(dato[tipotodato(&pobj->tipo)].dati) ;
	/* se non e` abbastanza robusto si rompe */
	if (pdat->robust <= q->robust_soglia)
	    subset(d_funzio,pobj->stato) ;
	/* se non rientra nei range peso&ingombro, lo perde */
	if (! (fromto(q->peso_min, q->peso_max, pdat->peso) &&
	    fromto(q->ingo_min, q->ingo_max, pdat->ingomb ) ) )
	    pobj->dove = stato.dovesei ;
    }
    stato.dovesei = q->luogo ;
    return(1) ;
} /* end of ctrl_sposta */

/*
 * ctrl_azione(n^panico, n^verbo)
 * controlla se Š scattato un panico d'azione. Ritorna 1 se s, else 0
 */
bool ctrl_azione (indice_panico_azione idx, indice_verbo verb) /* pubblica */
{
    t_panico_azione * p = & panico_azione[idx] ;
    t_benemale esito ;
    struct varia_azione *q ;

    switch (p->status) {
	case pa_esaurito:
	    return(0) ;
	case pa_attivo:
	    if (belongs(verb,p->agisci_con)) {
		/* p->status = pa_attivo ; (lo Š gi…) */
	    } else if (belongs(verb,p->esaurisci_con)) {
		p->status = pa_esaurito ;
	    } else if (belongs(verb,p->esaurisci_senza)) {
		p->status = pa_esaurito ;
		return(0) ;
	    } else if (belongs(verb,p->disattiva_con)) {
		p->status = pa_potenziale ;
	    } else if (belongs(verb,p->disattiva_senza)) {
		p->status = pa_potenziale ;
		return(0) ;
	    } else return(0) ;
	case pa_potenziale:
	    if (belongs(verb,p->attiva_senza)) {
		p->status = pa_attivo ;
		return(0) ;
	    } else return(0) ;
	default: internalerror("Illegal panic status","ctrl_azione",p->status);
    }

    esito = scatta[p->quesito.tipo](p->quesito.numero) ;
    if(esito!=Bene && esito!=Male)
	internalerror("Wrong BeneMale status","ctrl_azione",esito) ;
    q = & p->varia[esito] ;
    
    stato.barrette[energia] *= 1.0 + q->Den_/100.0 ;
    stato.barrette[morale]  += q->Dmor ;
    stato.barrette[fatica]  += q->Dfat ;
    stato.barrette[patologia] += q->Dpat ;
    
    return(1) ;
} /* end of ctrl_azione() */

/*
 * ctrl_posiz(n^panico)
 * controlla se Š scattato il panico di posizione. 1 se s, 0 se no.
 */
bool ctrl_posiz(indice_panico_posiz idx) /* pubblica */
{
    t_panico_posiz * p = & panico_posiz[idx] ;
    t_benemale esito ;
    struct varia_posiz *q ;
    struct dati *pdat ;
    t_obj *pobj ;
    int i ;

    if(--(p->ctr) > 0 ) return (0) ;

    esito = scatta[p->quesito.tipo](p->quesito.numero) ;
    if(esito!=Bene && esito!=Male)
	internalerror("Wrong BeneMale status","ctrl_posiz",esito) ;
    q = & p->varia[esito] ;
    
    stato.barrette[energia] *= 1.0+ (q->Den_/100.0) ;
    stato.barrette[fatica] += q->Dfat ;
    stato.barrette[morale] += q->Dmor ;
    stato.barrette[patologia] += q->Dpat ;
    stato.barrette[sonno] += q->Dsonno ;
    stato.barrette[sete] += q->Dsete ;
    stato.barrette[antipatia] += q->Dimpop ;
    for (i=0; myobjs[i]; i++)
    {
	pobj = & obj[myobjs[i]] ;
	pdat = & dato[tipotodato(&pobj->tipo)].dati ;
	/* perde gli oggetti specificati */
	if(pobj->tipo.tipo1 == q->persi.tipo1 &&
	    pobj->tipo.tipo2 == q->persi.tipo2)
	    pobj->dove = stato.dovesei ;
	/* se non rientra nei range peso&ingombro, lo perde */
	if (! (fromto(q->peso_min, q->peso_max, pdat->peso) &&
	    fromto(q->ingo_min, q->ingo_max, pdat->ingomb ) ) )
	    pobj->dove = stato.dovesei ;
    }
    stato.dovesei = q->luogo ;
    return(1) ;
} /* end of ctrl_posiz() */

/*
 * ctrl_mille(n^panico)
 * controlla se Š scattato un panico di millenario. 1 se s, 0 se no.
 */
bool ctrl_mille (indice_panico_mille idx) /* pubblica */
{
    t_panico_mille * p = & panico_mille[idx] ;
    struct diseq_mille *dis ;
    int i, out_of_range[7] ;
    t_benemale esito ;

    for (i=0, dis=p->disequaz; i<=6; ++i,++dis) {
	out_of_range[i] = 0 ;
	switch(dis->verso) {
	    case 0: break ;
	    case 1:
		if(stato.barrette[i+1] <= dis->soglia)
		    return(0) ;
		else {
		    out_of_range[i]++ ; /* =1 */
		    break ;
		}
	    case -1:
		if(stato.barrette[i+1] >= dis->soglia)
		    return(0) ;
		else {
		    out_of_range[i]++ ; /* =1 */
		    break ;
		}
	    default: internalerror("Wrong inequality value","ctrl_mille",
		dis->verso) ;
		break ;
	} /* switch */
    } /* for */

    esito = scatta[p->quesito.tipo](p->quesito.numero) ;
    if(esito!=Bene && esito!=Male)
	internalerror("Wrong BeneMale status","ctrl_mille",esito) ;
    if(esito==Bene) /* altrimenti non giovi del beneficio */
	for(i=0,dis=p->disequaz; i<=6; i++, dis++)
	    if(out_of_range[i])
		stato.barrette[i+1] = randrange(dis->val_min,dis->val_max) ;
    return(1) ;
} /* end of ctrl_mille() */

/*
 * scatta_niente()
 * falso panico quando ci si salva sempre
 */
static panic_fun scatta_niente
{
    if (n<=0) internalerror("Illegal panic number","scatta_niente",n) ;
    return Bene ;
}

/***********************************************
mancano: scatta_quiz, scatta_indo, scatta_folle
************************************************/
