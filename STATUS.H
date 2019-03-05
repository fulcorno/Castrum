/* header "status.h" */
/* disegna e gestisce la finestra dello STATUS */
/* procedure correlate alla visualizzazione dell'orologio */
/* e alla visione di sole e luna nel cielo */

void outwatch(void) ;
/* disegna le lancette dell'orologio */

void outsoleluna(void) ;
/* disegna gli astri alla posizione specificata dal tempo reale simulato */

void outstatus(void) ;

void initstatus(void) ;

/*
 * Revisualizza gli inventari (uno, l'altro o entrambi a seconda di 'tipo')
 */
void outinventari(int tipo) ;
