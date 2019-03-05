/* header "musica.h" */
/* installa un INTERRUPT DRIVER per suonare in background qualsiasi musica
precedentemente "compilata" con CMUS.EXE */
/* Proprieta` Jena-Soft 1989 - autore Fulvio Corno */
void musicon(void) ;   /* installa il driver */
void musicoff(void) ;	/* de-installa il driver */
void play(void * music) ;	/* inizia a suonare la musica specificata */
long currentnote(void) ; /* restituisce il # della nota che si sta per suonare.
    Convenzione:
	# = 0 nessun brano in esecuzione
	# < 0 brano sospeso alla nota abs(#)
	# > 0 brano in esecuzione alla nota # */
void gotonote(int n) ; /* salta alla nota specificata */
void initmusica(void) ; /* inizializza l'ambiente: DEVE ESSERE CHIAMATA PER
 PRIMA, se si vuole evitare un clamoroso CRASH ! */
