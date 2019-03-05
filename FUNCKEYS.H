/* header "FuncKeys.h" */
/* Gestisce la finestra dei tasti funzione */

void initfunckeys(void) ;

void outkeytitle(const char * s) ;
/* disegna titolo tasti funzione */

void delfkeys(void) ;
/* disegna la mascherina vuota */

void outmenu(int n) ;
/* Disegna l'intero menu` numero 'n' (di tipo "speciale" se n<0) */

int readkeyboard(void) ;
/* restituisce un codice indicante il tasto che ä stato premuto (** NON aspetta
 * il tasto **)
 */

void rejectkey(int key) ;
/* Mette il tasto "key" in lista d'attesa, cioä sarÖ il primo in uscita da
 * ReadKeyboard
 */

void flushkeyboard(void) ;
/* svuota il buffer di tastiera */

/* INTERPRETAZIONE dell"uscita di ReadKeyboard (R.K.) :
   R.K.   | significato
----------|---------------------------------
   = 0    | nessun tasto premuto
   > 0    | carattere normale 1..255
   < 0    | codice esteso tasti funzione & C.   */

/* costanti con cui confrontare "R.K." per conoscere il tasto.
 * k_xx : codice (esteso<0 o normale>0) del tasto xx
 */

enum codici_tastiera { /* uso enum al posto di tante #define */
    k_f1 = -59 ,    k_f2 = -60 ,
    k_f3 = -61 ,    k_f4 = -62 ,
    k_f5 = -63 ,    k_f6 = -64 , /* tasti funzione */
    k_f7 = -65 ,    k_f8 = -66 ,
    k_f9 = -67 ,    k_f0 = -68 ,

    k_home = -71 ,  k_up= -72 ,     k_pgup = -73 ,
    k_lt= -75 , 		    k_rt= -77 ,     /* tastierino numerico */
    k_end= -79 ,    k_dn= -80 ,     k_pgdn = -81 ,
    k_ins= -82 ,    k_del = -83 ,

    k_tab=9 ,	    k_esc= 27 ,     k_back =8 ,
    k_ret= 13 ,     k_bar= 32 ,     k_star = 42 ,   /* varå */
    k_plus = 43 ,   k_mins = 45
} ;

void menuselect ( int * verbo, int * obj1, int * obj2 ) ;
/* permette di compiere un passo nel " menu tree " verso un figlio, o di
 * abortire verso il main. Restituisce il # dell"azione da compiere e i
 * # degli oggetti su cui compierle. In generale obj# ä un indice_obj, ma
 * (a seconda di "verbo") puï anche essere indice_porta, indice_stanza,
 * indice_luogo, ...
 */

void gotomenu (indice_menu n) ;
/* Forza un menó dall'esterno, indipendentemente dallo stato attuale del
 * MENU TREE
 */

