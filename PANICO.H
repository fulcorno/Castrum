/* Sancti Georgii Castrum - Jena Soft
 * File "PANICO.H"
 * Contiene le dichiarazioni delle procedure legate all'azionamento dei panici
 * e all'interazione con l'utente in quelle situazioni.
 */

/*
 * ctrl_sposta(indice_panico)
 * controlla se � il caso di far scattare un panico di spostamento
 * ritorna 1 se � scattato il panico, 0 se no.
 */
bool ctrl_sposta ( indice_panico_sposta idx ) ;

/*
 * ctrl_azione(n^panico, n^verbo)
 * controlla se � scattato un panico d'azione. Ritorna 1 se s�, else 0
 */
bool ctrl_azione (indice_panico_azione idx, indice_verbo verb) ;

/*
 * ctrl_posiz(n^panico)
 * controlla se � scattato il panico di posizione. 1 se s�, 0 se no.
 */
bool ctrl_posiz(indice_panico_posiz idx) ;

/*
 * ctrl_mille(n^panico)
 * controlla se � scattato un panico di millenario. 1 se s�, 0 se no.
 */
bool ctrl_mille (indice_panico_mille idx) ;
