/* globals.h
 *
 * contiene tutte (e sole) le variabili completamente GLOBALI, cioŠ
 * accessibili ad ogni modulo e modificabili secondo la necessit…, che
 * concernono l'avventura in sŠ e non i suoi aspetti grafici e/o
 * implementativi
 */

/*
 * Parte 0: costanti e types comunemente usati
 */

typedef signed char sbyte  ;
typedef unsigned char byte ;
typedef byte bool ;

/*
 * Parte 1: si definiscono gli arrays da tenere nell'heap
 */

/*
 * sez. 1a: dimensioni arrays
 */

/* formato .DCA: */
#define max_porta	1000
#define max_luogo	600
#define max_stanza	250
#define max_menu	30
#define max_dato	250
#define max_obj		400
#define max_verbo	50
#define max_panico_sposta	100
#define max_panico_azione	100
#define max_panico_posiz	100
#define max_panico_mille	100
#define max_ques_niente		100
#define max_ques_quiz		100
#define max_ques_indo		100
#define max_ques_folle		100
/* non compilate: */
#define max_icona	60
#define max_frase	1000
#define max_msg		200

/*
 * sez. 1b: menmonici per gli indici
 */
	/* se indice_xxx = 0 indica una situazione particolare:
	 * la rispettiva variabile NON ha valore significativo 
	 */
typedef int indice_porta	;
typedef int indice_luogo	;
typedef int indice_stanza	;
typedef int indice_menu		;
typedef int indice_dato		;
typedef int indice_obj		;
typedef int indice_verbo	;
typedef int indice_icona	;
typedef int indice_frase	;
typedef int indice_msg		;
typedef int indice_panico_sposta	;
typedef int indice_panico_azione	;
typedef int indice_panico_posiz		;
typedef int indice_panico_mille		;
typedef int indice_ques_niente		;
typedef int indice_ques_quiz		;
typedef int indice_ques_indo		;
typedef int indice_ques_folle		;

/*
 * sez. 1c: tipi enumerativi per la costituzione delle stanze e degli oggetti
 */

typedef enum {  s_piano,   s_botte,   s_crociera,     /* possibili tipi */
		s_butala,  s_cielo,   s_chiocciola,   /* di stanza	*/
		s_sciapa,  s_stroc,   s_querc,
		s_foresta, s_caverna, s_cassettoni   } t_soffitto ;

typedef enum { p_passaggio, p_porta, p_ostruito } t_porte ;
/* possibili tipi di porta */

typedef enum { p_aperto, p_chiuso, p_bloccato } t_passaggio ;
/* diversi stati di una porta */

typedef enum { d_robust, d_nutrit, d_rusnen, d_affila, d_traspa, d_resist,
    d_marces, d_capaci, d_riflet, d_umidez, d_schegg, d_incend, d_compat,
    d_peso  , d_ingomb, d_elasti, d_equili } t_parametri ;
/* attributi invarianti degli oggetti */

typedef enum { 	d_aperto, d_visibi, d_sposta, d_acceso, d_pieno , d_funzio }
    t_statobj ;
/* switch on/off per ogni oggetto */

typedef enum { Bene, Male } t_benemale ;
/* tipo di risposta ad un panico */

typedef enum { p_panico_sposta, p_panico_azione, p_panico_posiz,
    p_panico_mille } t_tipopanico ;
/* possibili panici */

typedef enum { q_ques_niente, q_ques_quiz, q_ques_indo, q_ques_folle }
    t_tipoques ;
/* possibili quesiti */

/* sez. 1d: struttura interna delle diverse entit… */

/* identificatore univoco del tipo di oggetto */
typedef struct {
    int tipo1,tipo2 ;
} t_tipobj ;

/* caratteristiche dei vari tipi di oggetto */
struct dati {
    unsigned robust:4, nutrit:4, rusnen:4, affila:4, traspa:4, resist:4,
	marces:4, capaci:4, riflet:4, umidez:4, schegg:4, incend:4, compat:4,
	peso:4, ingomb:4, elasti:4, equili:4 ;
} ;

typedef struct {
    t_tipobj tipo ;	    /* quale tipo di obj descrive */
    int agget ; 	    /* aggettivi da usare */
    setof(verbo_L,max_verbo) ; /*azioni possibili*/
    setof(verbo_P,max_verbo) ;
    struct dati dati ; /*caratteristiche*/
    char NomeGlb [25];	   /* nome di default */		   /******/
} t_dato ;

/* oggetto vero e proprio */
typedef struct {
    t_tipobj tipo ; /* che caratteristiche ha ? */
    int agget ;     /* aggettivi */
    indice_frase descrizione ;
    indice_obj nasconde ;  /*oggetto legato/nascosto/bloccato*/
    indice_panico_azione n_panico ;
    int dove ; /* luogo in cui Š posto :
		   > 0 corrisponde a indice_luogo
		   = 0 oggetto non disponibile
		   = -1 posseduto	      */
    setof(stato,d_funzio+1) ; /* simula 'array of bool' */
    char NomeLoc [25] ;     /* se Š <> default */		    /*******/
} t_obj ;

/* porte */
typedef struct {
    t_porte tipo ;	    /* costante */
    t_passaggio stato ;     /* valore istantaneo */
    indice_panico_sposta n_panico ;	    /* possibile pericolo celato */
    indice_obj n_chiave ;	    /* se Š chiusa a chiave */
    indice_frase descrizione ;
} t_porta ;

/* parete */
typedef struct {
    FillIndex tipo ;	    /* tappezzeria */
    int forma ; 	    /* tipo di parete */
    indice_porta n_porta1,  n_porta2  ;  /* # porta */
    indice_luogo n_uscita1, n_uscita2 ;  /*luogo collegato*/
} t_parete ;

/* luogo */
typedef struct {   /*  unit… base del castello  */
    indice_stanza n_stanza ; /* a cui appartiene */
    int dim_x, dim_y ;	    /* in spanne */
    indice_panico_posiz n_panico ;
    t_parete pareti[4] ;
} t_luogo ;

/* stanza */
typedef struct {
    t_soffitto tipo ;  /* per il disegno 3d esterno */
    FillIndex terreno ; /* campitura pavimento */
    int disegno ;      /* ornamento pavimento */
    int dim_z ; /* in spanne */
    unsigned memoria ; /* tempo 'packed' dell'ultima volta
	che ci sei stato */
    indice_frase descrizione ;
    char nome [40] ;
} t_stanza ;

/* quesito 1: il quiz */
typedef struct {
    int tempo ;
    char *domanda, *r1,*r2,*r3,*r4,*c2,*c3,*c4 ;
} t_ques_quiz ;

/* quesito 2: l'indovinello */
typedef struct {
    char *domanda, *r1,*r2,*r3,*r4,*c1 ;
} t_ques_indo ;

/* quesito 3: il folletto */
typedef struct {
    int musica ; /* PROVVISORIO: ci vuole un 'indice_musica' */ /****/
    char *nome, *descriz, *frase ;
} t_ques_folle ;

/* super-indice quesiti */
typedef struct {
    t_tipoques tipo ;
    int numero ; /* di tipo indice_ques_???? */
} t_quesito ;

/* panico di spostamento */
typedef struct {
    unsigned Vpi ; /* val. prossimo iniziale */
    unsigned ctr ;
    unsigned /* range variazione 'ëctr' */
	peso_min, peso_max, /* in miria^-1 codificate */
	alto_min, alto_max, /* in spanne^-1 */
	velo_min, velo_max, /* in vouleur^-1 [0-3] */
	cost_min, cost_max; /* stesse unit… di 'ctr' */
    struct varia_sposta {
	indice_luogo luogo ;
	int robust_soglia ;
	int Den_, Dfat, Dmor, Dpat ; /* variaz.stato */
	unsigned  /* oggetti tenuti */
	peso_min:4, peso_max:4,
	ingo_min:4, ingo_max:4;
	char commento[20] ;			/****/
    } varia [2] ; /* bene & male */
    t_quesito quesito ;
} t_panico_sposta ;

/* stato della macchina a stati finiti per i panici di azione */
typedef enum {
    pa_esaurito, pa_potenziale, pa_attivo
} t_stat_panico_azione ;

/* panico di azione */
typedef struct {
    setof (esaurisci_senza, max_verbo) ;
    setof (esaurisci_con,   max_verbo) ;
    setof (attiva_senza,    max_verbo) ;
    setof (disattiva_senza, max_verbo) ;
    setof (disattiva_con,   max_verbo) ;
    setof (agisci_con,	    max_verbo) ;
    t_stat_panico_azione status ;
    struct varia_azione {
	int Den_, Dmor, Dfat, Dpat ;
	char commento [20] ;
    } varia [2] ;
    t_quesito quesito ;
} t_panico_azione ;

/* panico di posizione */
typedef struct {
    int Vpi, ctr ;
    struct varia_posiz {
	indice_luogo luogo ;
	int robust_soglia ;
	int Den_, Dfat, Dmor, Dpat,
	Dsonno, Dsete, Dimpop ; /* variaz.stato */
	unsigned  /* oggetti tenuti */
	    peso_min:4, peso_max:4,
	    ingo_min:4, ingo_max:4;
	t_tipobj persi ; /* oggetti che si perdono */
	char commento[20] ;
    } varia [2] ; /* bene & male */
    t_quesito quesito ;
} t_panico_posiz ;

/* panico di millenario */
typedef struct {
    struct diseq_mille {
	sbyte verso ; /* <:-1, >:1, niente:0 */
	byte soglia, val_min, val_max ;
    } disequaz [7] ; /* energia, sonno, sete, fatica,
		morale, patologia, antipatia */
    char commento [20] ;
    t_quesito quesito ;
} t_panico_mille ;

/* tasto funzione */
typedef struct {
    indice_icona disegno ;
    int nextmenu ;  /* <-- se >0 altro menu
			      =0 esegui azione
			      <0 menu speciale */
    int azione ;    /* <-- se >0 esegue azione
			      =0 non agisce
			      <0 funzione speciale */
    char frase [25] ;
} t_tastofunz ;

/* menu` */
typedef struct {
    t_tastofunz tasto [ 10 ] ;
    char titolo [40] ;
} t_menu ;

/* verbo */
typedef struct {
    int tipo ;	 /* con/senza 2^ parametro e di che tipo */
    int prep ;	 /* eventuale preposizione 2^ parametro */
    char nome [25] ;  /* per scrivere nel tasto funz.(imperativo) */
    char spiega [25] ; /* per scrivere nella pergamena (2a sing.) */
} t_verbo ;

/* frase */
typedef char * t_frase ;

/* msg */
typedef indice_frase t_msg ;

/*
 * sez. 1e: dimensioni data base
 */

extern int    /* tot_xxx == reale numero di xxx presenti */
    tot_stanze, tot_luoghi, tot_porte, tot_panici_sposta, tot_panici_azione,
    tot_panici_posiz, tot_panici_mille, tot_quess_quiz, tot_quess_indo,
    tot_quess_folle, tot_menus, tot_dati, tot_objs, tot_verbi, tot_icone,
    tot_frasi, tot_msgs ;

/*
 * sez. 1f: puntatori agli array globali (posti nell'heap per non superare
 * i 64K di dati nel Data Segment)
 */

extern t_stanza	* stanza	;
extern t_luogo	* luogo		;
extern t_porta	* porta		;
extern t_menu	* menu		;
extern t_dato	* dato		;
extern t_obj	* obj		;
extern t_verbo	* verbo		;

extern byte	* icona 	;
extern t_frase	* frase		;
extern t_msg	* msg		;

extern t_panico_sposta * panico_sposta ;
extern t_panico_azione * panico_azione ;
extern t_panico_posiz  * panico_posiz  ;
extern t_panico_mille  * panico_mille  ;

extern t_ques_quiz  * ques_quiz  ;
extern t_ques_indo  * ques_indo  ;
extern t_ques_folle * ques_folle ;

/*
 * Parte 2 : definizione entit… componenti la "variabile di stato"
 */

/*
 * sez. 2a: tabella di conversione Miria <-> 0..15 ed altre costanti
 * fondamentali per lo Stato
 */

extern const float c_miria [] ;

#define ListObjSize 60

/*
 * sez. 2b: tipi base con cui costituire lo status
 */

typedef struct {   /* 'tempo reale simulato' */
    byte giorno,ore,min,sec ;
} t_orologio ;

typedef enum { punti, energia, sonno, sete, fatica, morale, patologia,
    antipatia } t_barrette ;
typedef enum { suo_peso, sua_altezza, sua_eta } t_individuo;
typedef enum { tot_peso, tot_ingombro } t_trasporta ;
typedef enum { in_piedi, abbassato, accosciato, a4gambe } t_posizione ;
typedef enum { punta_piedi, cammina, svelto, corri } t_velocita ;

/*
 * sez. 2c: arrays atti a contenere le informazioni
 */

typedef float ta_barrette [ 9 ] ;  /* saranno sempre numeri tra 0
				e 100, ma li rappresento come REAL per
				avere maggior risoluzione numerica */
typedef float ta_individuo [ 3 ] ;
typedef float ta_trasporta [ 2 ] ;

typedef struct {
    ta_individuo individuo ;	    /* dati giocatore */
    ta_trasporta trasporta ;	    /* glob. objs. trasportati */
    ta_barrette  barrette  ;	    /* indici 0..100 */
    t_posizione  posizione ;	    /* pos.fisica */
    t_velocita	 velocita  ;	    /* vel. effettiva */
    t_orologio	 tempo	   ;	    /* tempo reale simulato */
    indice_luogo dovesei   ;	    /* dove ti trovi ora */
} t_status ;

typedef indice_obj t_list_objs[ListObjSize] ;
/* elenco di oggetti (max ListObjSize) */

/*
 * sez. 2d: variabili vere e proprie
 */

extern t_status	stato ;
extern t_list_objs		myobjs, l_objs ;

/*
 * sez. 2e: tabelle di costanti numeriche di correzione
 */

extern const float c_posizione [] ;
/* moltiplicativi per l'altezza in funzione della posizione fisica */

extern const float c_velocita [] ;
/* moltiplicativi per trovare la reale velocit… */

/*
 * sez. 2f: stringhe contenenti il nome dei componenti gli ENUM TYPE
 */

typedef const char * labels [] ;

extern labels n_barrette ;
extern labels n_posizione ;
extern labels n_velocita ;
extern labels n_soffitto ;
extern labels n_artDET ;
extern labels n_artIND ;
extern labels prepos ;

/*
 * Parte 3: definizione delle "preferences" (controlli)
 */

/*
 * sez. 3a: enum types
 */

typedef enum /* stampante */ {
    s_niente, s_solotxt, s_sologoth, s_graftxt, s_grafgoth, s_solograf
} t_stampante ;

typedef enum { g_soffitto, g_oggetto, g_fkeys } t_grafica ;
/* rendimento grafico */

typedef enum { p_azioni, p_stanze, p_oggetti } t_prolix ;
/* uso pergamena */

/*
 * sez. 3b: collezione di switches
 */

typedef struct {
    t_stampante stampante ;
    setof(grafica,3) ;
    setof(prolix,3) ;
} switches ;

/*
 * sez. 3c: variabile di stato ausiliaria
 */

extern switches statoAUX ;

/*
 * Parte 4: Definizioni (headers) delle procedure 'pubbliche'
 */

/*
 * sez. 4a: procedure di servizio
 */

void internal_error(char *code, char *proc, int val, char *file, int linea) ;
/* comunica all'utente un'inconsistenza interna del programma */

/* MACRO void internalerror(char * code, char * proc, int val) ; */
#define internalerror(code,proc,val) \
    internal_error(code,proc,val,__FILE__,__LINE__)

int initglobals(int argc, char ** argv) ;
/* da chiamare per prima: inizializza le variabili vitali */

void loadvariables(void) ;
/* riempie gli ARRAY con i giusti dati prevevandoli dai file */

/*
 * sez. 4b: compiti di conversione
 */

unsigned packtempo(t_orologio *tempo) ;
/* trasforma un record di tipo 'orologio' in un numero di minuti, trascurando
  i secondi */

byte miriato015(double miria) ;
/* codifica un valore in miria nel range 0..15 secondo la tabella c_miria */

indice_dato tipotodato(t_tipobj *t) ;
/* restituisce il # di dato che corrisponde al tipo_obj specificato */

/*
 * sez. 4c: aggiornamento variabili di stato
 */

void updatetempo(int secs) ;
/* avanza l'orologio di "secs" secondi */

/*
 * sez. 4d: Lettura delle informazioni implicite (esplicitazione)
 */

byte numeroporte(indice_luogo n, int wall) ;
/* Restituisce il numero di porte esistenti nella parete specificata del luogo
  specificato. Restituisce 0, 1 o 2 */

char * nomeobj(indice_obj n, int art) ;  /* restituisce il nome
  (come stringa) dell'obj di cui si specifica il # d'ordine. Pu• essere il
  nome globale (di default) o, se esiste, quello locale. Viene preposto
  l'articolo (0=nessuno; 1=determinativo; 2=indeterminativo) */

char * messaggio(indice_msg n) ; /* Emette un messaggio 'custom' sulla
  pergamena */

byte readdati(struct dati * d, register t_parametri i) ; /* Legge in modo
  indicizzato la struttura 'dati', di bit fields */

void writedati(struct dati * d, register t_parametri i, int /*byte*/ value) ;
/* Scrive in modo indicizzato la struttura 'dati', di bit fields */
