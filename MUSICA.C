/*
 * modulo "musica.c"
 *
 * installa un INTERRUPT DRIVER per suonare in background qualsiasi musica
 * precedentemente "compilata" con CMUS.EXE
 */

/* Proprieta` Jena-Soft 1989 - autore Fulvio Corno */

/* INTERFACE */

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

#include "musica.h"

/* IMPLEMENTATION */

#define IntNo 0x1C
/* numero dell'interrupt da dirottare */

typedef struct {
    int freq ;
    int dura ;
} nota ;

typedef nota brano[] ;

/* static const nota
    n_fine={-1, 0} ,
    n_capo={ 0, 0} ,
    n_lega={-3, 0} ,
    n_stac={-4, 0} ,
    n_lest={-2, 0} ; */

typedef enum {legato, legastacca, staccato } stat ;

static void interrupt (*saveint1C)() ; /* vecchio indirizzo dell'int 0x1C */
static unsigned int
    countdown ;     /* quanto bisogna ancora aspettare per la prossima nota */
static nota * cur_note ; /* nota che deve essere suonata. Se Š =0, non suona */
static nota * musicaddr ;
static int installed, pausing ;
static stat status ;

static void interrupt background(void)
{
    if (cur_note) {
	if (!countdown) {
	    if (status!=legato) nosound() ;
	    if (pausing) {
		if ((countdown=(++cur_note)->dura)==NULL)
		    /* convenzione per COMANDO SPECIALE */
		    switch(cur_note->freq) {
			case 0:nosound() ;
			    musicaddr=cur_note=NULL ;
			    break ;
			case -1:cur_note=NULL ; break ;
			case -3:status=legato ; break ;
			case -2:status=legastacca ; break ;
			case -4:status=staccato ; break ;
			default:cur_note=musicaddr+cur_note->freq-1 ; break ;
		    } /*switch*/
		else {
		    if (status==staccato) pausing=0 ;
		    if (!cur_note->freq) nosound();
		    else sound(cur_note->freq) ;
		}  /* if dura */
	    } else {
		pausing=1 ;
		countdown=0 ;
	    } /* if pausing */
	} /* if countdown */
	else --countdown ;
    } /* if musicaddr */
} /* background */

static void near pascal install(void)
{
    setvect(IntNo, background) ;
    installed = 1 ;
} /*install*/

static void near pascal deinstall(void)
{
    setvect(IntNo, saveint1C) ;
    installed = 0 ;
} /*deinstall*/

void musicon (void) /* PUBBLICA */
{
    countdown = 0 ;
    if (musicaddr) install() ;
}

void musicoff (void) /* PUBBLICA */
{
    deinstall() ;
    nosound() ;
}

void play ( void * music ) /* PUBBLICA */
{
    musicoff() ;
    cur_note = musicaddr = (nota*)music ;
    musicon() ;
}

long currentnote(void) /* PUBBLICA */
{
    if (!musicaddr) return (0) ;
    else if (installed) return ((cur_note-musicaddr)+(cur_note==musicaddr)) ;
    return (-(cur_note-musicaddr)-(cur_note==musicaddr)) ;
}

void gotonote(int n) /* PUBBLICA */
{
    musicoff() ;
    cur_note=musicaddr + n-1 ;
    countdown=0;
    musicon() ;
}

void initmusica(void) /* pubblica */
{   /* init code */
    saveint1C = getvect(IntNo);
    if (atexit(musicoff)) {
	fprintf(stderr, "Fatal error: can't install MUSICA.OBJ\n") ;
	exit(1) ;
    }
    /* assicuriamoci si scollegare l'interrupt prima di finire il programma,
       per evitare un sicuro CRASH ! */
    cur_note = musicaddr = NULL ; /* all'inizio non ci sono brani da suonare */
    installed= 0 ;
    status=legastacca ;
    pausing=1 ;
}
