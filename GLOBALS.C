/* globals.c
 *
 * contiene tutte (e sole) le variabili completamente GLOBALI, cioŠ
 * accessibili ad ogni modulo e modificabili secondo la necessit…, che
 * concernono l'avventura in sŠ e non i suoi aspetti grafici e/o
 * implementativi
 */

#include <graphics.h>
#include <values.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <alloc.h>
#include <ctype.h>
#include <conio.h>

#include "drivers.h"
#include "set.h"

#include "globals.h"

/* IMPLEMENTATION */

int    /* tot_xxx == reale numero di xxx presenti */
    tot_stanze, tot_luoghi, tot_porte, tot_menus, tot_dati, tot_objs, tot_verbi,
    tot_icone, tot_frasi, tot_msgs, tot_panici_sposta, tot_panici_azione,
    tot_panici_posiz, tot_panici_mille, tot_quess_quiz, tot_quess_indo,
    tot_quess_folle ;

t_stanza* stanza	;
t_luogo	* luogo		;
t_porta	* porta		;
t_menu	* menu		;
t_dato	* dato		;
t_obj	* obj		;
t_verbo	* verbo		;

byte	* icona		;
t_frase	* frase		;
t_msg	* msg		;

t_panico_sposta * panico_sposta ;
t_panico_azione * panico_azione ;
t_panico_posiz  * panico_posiz  ;
t_panico_mille  * panico_mille  ;

t_ques_quiz  * ques_quiz  ;
t_ques_indo  * ques_indo  ;
t_ques_folle * ques_folle ;

const float c_miria [] = {
    0, 0.003, 0.01, 0.05, 0.1, 0.2, 0.5, 0.7, 1.5, 3.0,
    6.0, 10.0, 20.0, 30.0, 50.0, MAXFLOAT
} ;

t_status	stato ;
t_list_objs	myobjs, l_objs ;

const float c_posizione [] = { 1.0, 0.75, 0.5, 0.3 } ;
const float c_velocita []  = { 0.5, 1.0, 1.5, 2.0 } ;

labels n_barrette = {
    "Punti", "Energia", "Sonno", "Sete", "Fatica", "Morale",
    "Patologia", "Antipatia"
} ;
labels n_posizione =
	{ "in piedi", "abbassato", "accosciato", "a quattro gambe" } ;
labels n_velocita =
	{ "muoverti in punta di piedi", "camminare normalmente",
	"camminare velocemente", "correre" } ;  /* hai deciso di ... */
labels n_soffitto =
	{ "soffitto piatto", "soffitto a botte", "soffitto a crociera",
	"soffitto a butala", "luogo a cielo aperto", "scala a chiocciola",
	"soffitto \" sciapa` \"", "soffitto \" stroc \"",
	"soffitto a \" querc \"", "foresta", "caverna",
	"soffitto a cassettoni" } ;
labels n_artDET =
	{ "", "il ", "la ", "l'", "l'", "lo ", "le ", "i ", "la ", "gli " } ;
labels n_artIND =
	{ "", "un ", "una ", "un ", "un'", "uno ", "delle ", "dei ", "della ",
	"degli " } ;
labels prepos = { "", "con", "su", "contro" } ;

switches statoAUX ;

/*
 * codice vero e proprio
 */

/*
 * Restituisce il numero di secondi dalla mezzanotte del giorno 0
 */
unsigned packtempo(t_orologio * ora)  /*pubblica*/
{
    return ora->min + ora->ore * 60 + ora->giorno * 1440 ;
} /* packtempo */

/*
 * Avanza l'orologio di 'secs' secondi
 */
void updatetempo(int secs) /*pubblica*/
{
    int s ;
    t_orologio *p = &stato.tempo ;

    s = p->sec + secs ;
    p->min += s / 60 ;
    p->ore += p->min / 60 ;
    p->giorno += p->ore / 24 ;
    p->sec = s % 60 ;
    p->min %= 60 ;
    p->ore %= 24 ;
} /* updatetempo */

/*
 * Converte un valore in miria nella sua codifica 0..15
 */
byte miriato015(double miria) /* pubblica */
{
    register int i = 0 ;

    while (miria > c_miria[i++])
	/*NOP*/ ;
    return i ;
} /* miriato015 */

/*
 * Interpreta i parametri della linea di comando
 */
static int near pascal command_line_parameters(int argc, char ** argv)
{
    register int i /*,e*/ ;
    /* float v ; */

    /* defaults */
    stato.individuo[suo_peso] = 7. ;
    stato.individuo[sua_altezza] = 7.5 ;
    stato.individuo[sua_eta] = 30. ;
    for (i=1; i<argc; ++i) {
	/*  if ((i>=1)&&(i<=3)) {
	 *	e = sscanf(argv[i],"%f",&v) ;
	 *	if (e==1) switch (i) {
	 *	    case 1:stato.individuo[suo_peso]=v ;
	 *		break ;
	 *	    case 2:stato.individuo[sua_altezza]=v ;
	 *		break ;
	 *	    case 3:stato.individuo[sua_eta]=v ;
	 *		break ;
	 *	}
	 *  }
	 */
	if (!stricmp(argv[i],"cga")) return CGA ;
	if (!stricmp(argv[i],"att")) return ATT400 ;
    } /* for */
    return ATT400 ;
} /* command_line_parameters */

/*
 * Legge un file di dati del tipo ".DCA"
 */
static void near pascal GetIn(char * name, void ** pt,
    int *tot, size_t len, int maxsize)
{
    FILE * f ;
    size_t fl ;
    char tmp[80]="\0" ;
    strcpy(tmp,name) ;
    strcat(tmp,".DCA") ;
    f=fopen(tmp,"rb") ;
    if (!f) {
	printf("Can't open file %s. Aborting\n",tmp) ;
	exit(-1) ;
    }
    fl=(size_t)filelength(fileno(f)) ;
    *pt = malloc(fl+1) ;
    *tot = fread(*pt,len,maxsize,f) ;
    fclose(f);
    printf("Loaded '%-8s' : %5d bytes -> %4d items.\n",name,fl,*tot) ;
}

/* MACRO void getin ( varname what, varname plural, int max ) ;
   chiama GetIn() con i giusti parametri */
#define getin(what,plural,max) \
	GetIn(#what,&(void*)what,&tot_##plural,sizeof(t_##what), max )

/*
 * Legge in memoria il data base di cui ha bisogno
 */
void loadvariables(void)  /* pubblica */
{
    FILE *fr ;
    char s[512], *p ;
    int i,c,t ;

/* 1) Lettura di tutti i files ".DCA" */

    getin ( stanza, stanze, max_stanza ) ;
    getin ( luogo,  luoghi, max_luogo  ) ;
    getin ( porta,  porte,  max_porta  ) ;
    getin ( menu,   menus,  max_menu   ) ;
    getin ( dato,   dati,   max_dato   ) ;
    getin ( verbo,  verbi,  max_verbo  ) ;
    getin ( obj,    objs,   max_obj    ) ;

/* 2) Lettura del file FRASI.TXT */

    fr = fopen("frasi.txt","rt") ;
    printf("Loading messages\n") ;
    if (!fr) internalerror("Can't open text file","LoadVariables",0) ;
    msg = (t_msg *) calloc(1,sizeof(t_msg)*max_msg) ;
    frase = (t_frase *) calloc(1,sizeof(t_frase)*max_frase) ;
    if((!msg)||(!frase))
	internalerror("Can't allocate msg pointers","LoadVariables",0) ;
    tot_frasi=0;
    tot_msgs=0;
    while (!feof(fr)) {
	fgets(s,511,fr) ;
	p = s+ strlen(s) - 1 ;
	while((p>=s)&&isspace(*p))
	*(p--)='\0' ;
	if ((!*s)||(*s=='*')) continue ;
	/* get here only if not comment line */
	sscanf(s,"%c%d %n",&c,&i,&t) ;
	if((frase[++tot_frasi] = (t_frase) strdup(s+t))==NULL)
	    internalerror("Can't allocate string","LoadVariables",tot_frasi) ;
	switch (toupper(c)) {
	    case 'S':stanza[i].descrizione=tot_frasi ;
		break ;
	    case 'P':porta[i].descrizione=tot_frasi ;
		break ;
	    case 'O':obj[i].descrizione=tot_frasi ;
		break ;
	    case '.':if (i>tot_msgs) tot_msgs=i ;
		msg[i]=tot_frasi ;
		break ;
	    default: printf("TXT Error: %c%d %s\n",c,i,s) ;
		break ;
	} /*switch*/
    }  /*while*/
    fclose(fr) ;

    printf("Avaiable memory: %lu bytes\n",coreleft()) ;
    /*(void) getch() ;*/
}  /* LoadVariables */

/*
 * Emette un messaggio che comunica all'utente un grave errore di
 * programmazione, d… alcune informazioni per poterlo localizzare e riprodurre,
 * e invita l'utente a comunicarlo agli autori
 */
void internal_error (char * code, char * proc, int val, char * file, int linea)
/* pubblica */
{
    printf("\n*** FATAL ERROR OCCURRED or INTERNAL INCONSISTENCY DETECTED ***"
	"\n - note conditions & press a key (ESC to resume at YOUR risk)" ) ;
    if (getch()!=27) {
	restorecrtmode() ;
	printf("*** INTERNAL ERROR *** please report to Jena-Soft\n") ;
	printf("Error condition:\n%s in function %s()\n"
	    "Position: %s line %d\nValue: %d",code,proc,file,linea,val) ;
	exit(-1) ;
    }
}  /* internal_error */

/*
 * Dice quante porte ci sono nella parete specificata
 */
byte numeroporte(indice_luogo n, int wall)  /* pubblica */
{
    t_parete *p = & luogo[n].pareti[wall] ;

    if (!p->n_uscita1) return 0 ;
    else if (!p->n_uscita2) return 1 ;
    else return 2 ;
}

/*
 * Dato un tipo di dato, dice in quale posizione all'interno di 'dato[]'
 * esso Š descritto
 */
indice_dato tipotodato(t_tipobj * t) /* pubblica */
{
    t_tipobj * p ;
    indice_dato i = 1 ;

    do {
	p = & dato[i].tipo ;
	if ((p->tipo1==t->tipo1)&&(p->tipo2==t->tipo2))
	return i ;
    } while(++i<tot_dati) ;
    return 0 ;
}  /* tipotodato */

/*
 * Restituisce il nome di un oggetto, corredato dal giusto articolo
 */
char * nomeobj(indice_obj n, int art) /* pubblica */
{
    indice_dato i ;
    t_dato * d ;
    static char result [50] ;
    t_obj * p = & obj[n] ;

    if (*(p->NomeLoc))
	switch (art) {
	    case 0:return strcpy(result,p->NomeLoc) ;
	    case 1:strcpy(result,n_artDET[p->agget]) ;
		return strcat(result,p->NomeLoc) ;
	    case 2:strcpy(result,n_artIND[p->agget]) ;
		return strcat(result,p->NomeLoc) ;
	}
    else {
	i=tipotodato(& p->tipo) ;
	if (!i) {
	    sprintf(result,"(%d,%d)=???",p->tipo.tipo1,p->tipo.tipo2) ;
	    return result ;
	} else {
	    d = dato+i ;
	    switch(art) {
		case 0:return strcpy(result,d->NomeGlb) ;
		case 1:strcpy(result,n_artDET[d->agget]) ;
		    return strcat(result,d->NomeGlb) ;
		case 2:strcpy(result,n_artIND[d->agget]) ;
		    return strcat(result,d->NomeGlb) ;
	    } /*switch */
	} /* if !i */
    } /* if NomeLoc */
    internalerror("Invalid article type","nomeobj",art) ;
    return NULL ;
} /* NomeObj */

/*
 * Emette un messaggio 'custom' sulla pergamena
 */
char * messaggio(indice_msg n) /* pubblica */
{
    char * i ;
    const char * t ;
    static char result[256] ;
    char tmp [256] ;

    strcpy(result,frase[msg[n]]) ;
    do {
	i = strchr(result,'%') ;
	if (i) {
	    switch (toupper(*(i+1))) {
		case 'S': t = stanza[luogo[stato.dovesei].n_stanza].nome ;
		    break ;
		default: t = "(Unknown Symbol)" ;
		    break ;
	    } /*switch*/

	    /* attenzione:
	     *	+-------------+ 	    +-----+---------+
	     *	|     %x    \0| result	    | t[] | (i+2)[] |  tmp
	     *	+-----||------+ 	    +-----+----|----+
	     *	      ||[^^^^^]-->---->---->---->------/
	     *	      |\-< i+1
	     *	      \--< i
	     */
	    strcpy(tmp,t) ;
	    strcat(tmp,i+2) ;
	    strcpy(i,tmp) ;
	} /* if */
    } while(i) ;
    return result ;
} /* messaggio */

/*
 * Legge in modo indicizzato la struttura 'dati', di bit fields
 */
byte readdati(struct dati * d, register t_parametri i) /* pubblica */
{
#define ret(x) case d_##x:return d->x
    switch (i) {
	ret(robust); ret(nutrit); ret(rusnen); ret(affila); ret(traspa);
	ret(resist); ret(marces); ret(capaci); ret(riflet); ret(umidez);
	ret(schegg); ret(incend); ret(compat); ret( peso ); ret(ingomb);
	ret(elasti); ret(equili);
	default:internalerror("Wrong parameter index","readdati",i) ;
	    return -1 ;
    }
#undef ret
}

/*
 * Scrive in modo indicizzato la struttura 'dati', di bit fields
 */
void writedati(struct dati * d, register t_parametri i, int /*byte*/ value)
/* pubblica */
{
#define ass(x) case d_##x:d->x = value; break ;
    switch(i) {
	ass(robust); ass(nutrit); ass(rusnen); ass(affila); ass(traspa);
	ass(resist); ass(marces); ass(capaci); ass(riflet); ass(umidez);
	ass(schegg); ass(incend); ass(compat); ass( peso ); ass(ingomb);
	ass(elasti); ass(equili);
	default:internalerror("Wrong parameter index","writedati",i) ;
    }
#undef ass
}

/*
 * Inizializza le variabili globali
 */
int initglobals(int argc, char **argv)
{
    int grmode ;

    /* l'avventura inizia alle 06:00 */
    stato.tempo.giorno	    = 0 ;
    stato.tempo.ore	    = 6 ;
    stato.tempo.min	    = 0 ;
    stato.tempo.sec	    = 0 ;

    /* stato fisico iniziale */
    stato.posizione = in_piedi ;
    stato.velocita  = cammina ;
    memset(stato.barrette,0,sizeof(stato.barrette)) ;
    grmode = command_line_parameters(argc,argv) ;

    /* stanza di partenza */
    stato.dovesei = 1 ;

    /* tutte le possibilit… attivate... */
    nullset(statoAUX.grafica) ;
    addset( g_fkeys,	    statoAUX.grafica ) ;
    addset( g_soffitto,     statoAUX.grafica ) ;
    addset( g_oggetto,	    statoAUX.grafica ) ;

    nullset(statoAUX.prolix) ;
    addset( p_azioni,	    statoAUX.prolix ) ;
    addset( p_stanze,	    statoAUX.prolix ) ;
    addset( p_oggetti,	    statoAUX.prolix ) ;

    statoAUX.stampante	    = s_niente ;   /* ...tranne la stampante */

    return grmode ;
} /* initglobals */
