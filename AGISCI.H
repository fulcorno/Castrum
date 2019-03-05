/* header "agisci.h" */
/* contiene le procedure per :
- muoversi di stanza in stanza
- prendere/posare oggetti
- emettere descrizioni
- modificare la Variabile di Stato per simulare lo stato fisico
- usare i vari oggetti in tutti i modi possibili
*/

void buildobjtables(int tipo) ;
/* riempie l_objs[] e myobjs[] con i giusti valori.
 * 'tipo' dice quale delle due riempire (o entrambe)
 */

void azione(int az, int o1, int o2) ;
/* compie l'azione specificata */

void initeverything(int argc, char ** argv) ;
