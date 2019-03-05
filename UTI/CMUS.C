/***********************************************************************
     CMUS.EXE - Music Compiler for 'MUSICA.TPU'
 ***********************************************************************
 Copyright Jena Soft - Author: Fulvio Corno 1988

Compila un file di testo .MUS in un file di tipo .MCA (Music Compiled Array),
adatto ad essere eseguito direttamente dalla Play di MUSICA.TPU .

Convenzioni per il file .MUS :
                         <--\
              R>>            +--+      (anche minuscole)
       -D>>- -----       <--/   |     D, R, M, F, S, L, Z : note Do..Si
  Z>                      <--\  |   MODIFICABILI CON :
 ----  ----- -----       -L>- | +---Suffisso '>>': 2 ottave sopra
                        S>    +-----Suffisso '>' : 1 ottava sopra
----------------------F>----- | +---Suffisso '<' : 1 ottava sotto
                    M>        | |   Suffisso '<<': 2 ottave sotto
------------------R>--------- | |   Prefisso '*' : bequadro (=annulla #,·)
                D>        <--/  |   Prefisso '#' : 1/2 tono sopra
--------------Z--------------   |   Prefisso '$' : 1/2 tono sotto (sinonimo ·)
            L  <<=== 440 Hz     |
----------S------------------   |   =N:    N =   1   2   4   8   16
        F                       |     =>durata  4/4 2/4 1/4 1/8 1/16
------M----------------------   |
    R                           |   Prefisso numerico: estensione durata
 -D-  ---- ---- ---- ----       |    Es: =4 3#D>
       Z<                 <--\  |         |  \-- questo Do# sopra dura 3/4
           -L<- ---- ----    +--+         \-- ogni nota in unitÖ di 1/4
                 S<          |
                     -F<- <--/      Suffisso '.': estensione durata del 50%
                                     Es: =4 2#D>. dÖ lo stesso risultato
                                          (2*(1/4))+50% = 3/4
Altri comandi :

%n : sceglie il tempo : <n> = numero di note da 1/4 in 1 minuto
@[$|·|#][D..Z]:imposta un diesis o bemolli in chiave (Es: @$ZM --> Si· e Mi·)
              ( vale fino alla successiva impostazione )
@* : annulla ogni precedente @[$|·|#]
&f : sceglie la frequenza 'f' del La fondamentale ( *L ) (in HERTZ)
&* &> &>> &< &<< : Esegue un &f rispettivamente alla frequenza naturale di
                   *L *L> *L>> *L< *L<< ( 440, 880, 1760, 220, 110 )
! : tutto ciï che segue '!' ä ignorato tranne per un caso:
!% : se c'ä un commento '!%' il brano verrÖ ripartirÖ da capo una volta finito

? : una pausa si indica con ? e viene trattata come ogni nota
  ( ä quindi valido 2? ed anche ?.. )

^ : seleziona il modo STACCATO: ogni nota ä seguita da una pausa di 1/18 sec.
- : seleziona il modo LEGATO: non ci sono pause tra le note
-^ : seleziona il legato-puntato: c'ä appena uno stacco tra le 2 note.
' : STACCA le due note che separa

'' : Le linee vuote sono ignorate

Defaults : ogni brano comincia automaticamente con:
&* @* %120 =4 ^
I defaults si possono alterare con gli switches opzionali come segue :
Switches == /&f /@[$|#][D..Z] /%n /=n /[^|-]
N.B. non si possono usare &> e simili perchä i simboli '<' e '>' vengono
     interpretati dal DOS come I/O REDIRECTION .

Sintassi :  CMUS [Switches] <source>[.MUS] [<destination>[.MCA]]
 (Se manca <destination>, si assume uguale a "<source>.MCA")

Oppure : CMUS ? (mostra queste note esplicative a video)

L'autore ä a vostra disposizione :  '' +----------------+
       Fulvio Corno                    |   Jena  Soft   |
       via C.I.Giulio 24               |    Canaväis    |
 10090 S.Giorgio Canavese (TO)         | Software house |
       Tel. (0124) 32160               +----------------+  ,,

*************************************************************************/

/* Programma CMUS.EXE */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "set.h"

typedef struct {
	int dura, freq ;
	} nota ;

typedef enum
{ do_, re_, mi_, fa_, so_, la_, si_ } note ;

typedef setof(chiave,7) ;

char *nomi = "DRMFSLZ" ;
const double clock = 18.20648 /*Hz*/ ;
const int offset[7] =/* semitoni di distanza da *L */
	{ -9, -7, -5, -4, -2, 0, 2 } ;
const char * errors[] = {
	"E00: Compilazione conclusa senza errori",
	"E01: Manca il parametro <source>",
	"E02: Valore illegale per =n",
	"E03: Valore illegale per %N",
	"E04: Valore illegale per &f",
	"E05: Nome nota illegale (non DRMFSLZ)",
	"E06: Formato della chiave illegale",
	"E07: Direttiva non riconosciuta",
	"E08: Troppi nomi di file",
	"E09: <source> e <destination> sono identici",
	"E10: Non riesco a leggere <source>",
	"E11: Non riesco a scrivere su <destination>",
	"E12: Suffisso illegale per una nota "
} ;

FILE * inp ; /* file <source>.MUS */
FILE * out ; /* file <destination>.MCA */
char source[100],dest[100] ;
int tempo ;	/* %n: frequenza di default (in (1/4)/min ) */
int durata;	/* =N: durata di ogni nota */
double fondamentale ;
chiave diesis,bemolli ;
int line ;	/* linea a cui si ä arrivati a scorrere il file */
char frase[100],parola[100] ;
double ln2a1su12 ;
nota last ;
int count ;

const nota stacca = {0,1} ;

static void outerror(int n)
{
	puts(errors[n]) ;
	if (!line) puts("nella linea di comando") ;
	else printf("alla linea %d",line) ;
	exit(n) ;
}

void selectdefaults(void)
{
	nullset(diesis) ;
	nullset(bemolli) ;
	fondamentale = 440.0 ;
	durata= 4 ;
	tempo=120 ;
	strcpy(source,"") ;
	strcpy(dest,"");
}

note ch2note(int c)
{
	c=toupper(c);
	if (!strchr(nomi,c)) outerror(5) ;
	return (strchr(nomi,c)-nomi) ;
}

void controllo(char * s)
{
	int v,e ;
	char s1[200] ;

	strcpy(s1,s+1) ;
	e = sscanf(s1,"%d",&v) ;
	switch(s[1]) {
	case '=':
		if (e==1) durata=v ;
		else outerror(2);
		break ;
	case '%':
		if (e==1) tempo=v;
		else outerror(3);
		break ;
	case '&':
		if (e==1) fondamentale=v ;
		else if (!strcmp(s1,"*")) fondamentale=440.0 ;
		else if (!strcmp(s1,"<")) fondamentale=220.0 ;
		else if (!strcmp(s1,">")) fondamentale=880.0 ;
		else if (!strcmp(s1,">>")) fondamentale=1760.0 ;
		else if (!strcmp(s1,"<<")) fondamentale=110.0 ;
		else outerror(4) ;
		break ;
	case '@':
		nullset(diesis) ;
		nullset(bemolli) ;
		if ((s1[0]=='$')||(s1[0]=='·'))
			for (e=1; s1[e]; ++e)
				addset(ch2note(s1[e]),bemolli) ;
		else if (s1[0]=='#')
			for (e=1; s1[e]; ++e)
				addset(ch2note(s1[e]),bemolli) ;
		else if (strcmp(s1,"*")) outerror(6) ;
		break ;
	default: outerror(7) ;
	} /*switch*/
}

void getparameters(int argc, char** argv)
{
	int i ;
	char *s ;
	if ((argc==2)&&(!strcmp(argv[1],"?"))) {
		/*DisplayHelp() ;*/
		exit(255); /* l'errore ä il + grave: l'utente non sa usare
		    il comando */
	}
	if (argc<2) outerror(1) ;
	selectdefaults() ;
	for (i=1; i<argc; ++i) {
		s=argv[i] ;
		if (s[0]=='/') controllo(s+1) ;
		else if (!*source) strcpy(source,s) ;
		else if (!*dest) strcpy(dest,s) ;
		else outerror(8);
	}
	if ((!*source)&&(!*dest)) outerror(1) ;
	if (!*dest) strcpy(dest,source) ;
	if (!strchr(source,'.')) strcat(source,".MUS") ;
	if (!strchr(dest,'.')) strcat(dest,".MCA") ;
	if (!strcmp(dest,source)) outerror(9) ;
}

void outnota(char *s)
{
	nota what ;
	double dura_glb, dura_loc, dura_dot, freq_loc ;
	char s1[80] ;
	int i,altera,c ;
	note questa ;

	dura_glb=(4*60)/(tempo*durata) ; /* durata della nota in secondi */
	i=1;
	strcpy(s1,"") ;
	while (strchr("0123456789",s[i])) {
		s1[strlen(s1)+1] = '\0' ;
		s1[strlen(s1)] = s[i] ;
		++i ;
	}
	if (!*s1) sprintf(s1,"%d",&dura_loc) ;
	else dura_loc=1 ;
	s += strlen(s1)+1 ;
	altera=0 ;
	if (!strcmp(s,"'")) what=stacca ;
	else if (!strcmp(s,"-")) what=n_lega ;
	else if (!strcmp(s,"^")) what=n_stac ;
	else if (!strcmp(s,"-^")) what=n_lest ;
	else {
		if (s[0]=='?') {
			freq_loc=0.0;
			strcpy(s1,s+1) ;
		} else {
			if ((s[0]=='$')||(s[0]=='·')) altera=-1 ;
			if (s[1]=='#') altera=1 ;
			if (strchr("#$·*",s[0])) c=s[1] ; else c=s[0] ;
			questa=ch2note(c) ;
			if ((s[0]!='*')&&(altera==0)) {
			}
			freq_loc=fondamentale*exp((altera+offset[questa])*ln2a1su12) ;
			strcpy(s1,strchr(s,c)+1) ;
		}
		dura_dot=1 ;
		for (i=0; s1[i]; ++i)
			switch(s1[i]) {
			case '<':
				freq_loc /= 2.0 ;
				break ;
			case '>':
				freq_loc *= 2.0 ;
				break ;
			case '.':
				dura_dot *= 2.0 ;
			default: outerror(12);
			}
			dura_loc=dura_loc*(2-1/dura_dot) ;
			what.freq=floor(freq_loc) ;
			what.dura=floor(clock*dura_glb*dura_loc) ;
			if (!what.dura) what.dura=1 ;
		}
		fwrite(&what,1,sizeof(what),out) ;
		++count ;
}

void main(int argc, char ** argv)
{
	line=0;
	count=0;
	ln2a1su12=log(2)/12 ;
	stacca.dura=1;
	stacca.freq=0;
	getparameters(argc,argv) ;
	printf("Compilo %s in %s\n",source,dest) ;
	/* apri i files */
	inp = fopen(source,"rt") ;
	if(!inp) outerror(10) ;
	out = fopen(dest,"wb") ;
	if(!out) outerror(11) ;
	last.dura=0 ;
	last.freq=0 ;
	while (!feof(inp)) {
		fgets(frase,100,inp) ;
		++line ;
		while (*frase) {
				if (!strcmp(frase,"!%")) last.freq=1 ;
				strcpy(frase,"") ; /* SIAMO PAZZI ? */
				strcpy(parola,"") ;
				sscanf(frase,"%s",parola) ;
				if (*parola)
		}
		printf("\rLinea %4d --> %5d note",line,count) ;
	}
	printf("\n") ;
	fwrite(&last,1,sizeof(last),out);
	fclose(inp);
	fclose(out);
	outerror(0);
}
