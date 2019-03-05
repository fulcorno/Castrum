/*
 * Header file: "DRIVERS.H"
 * Costanti, tabelle e procedure che per accedere pi— efficientemente alla
 * grafica, ai file e alle stringhe.
 */

extern unsigned int *Badr ;
/* indirizzi delle righe dello schermo grafico
 */

extern char _8to7[256] ;
/* tabella di conversione per eliminare le lettere accentate e i caratteri
 * >=0x80
 */

typedef enum {
    nero, G1, G2, G3, G4, G5, G6, G7, G8, G9, G10, G11, G12, G13, G14, G15,
    bianco, grigio, rombi, quadri, mattoni, lisca, quadrombi, esagoni, cerchi,
    frecce, lago, rombi2, erba, diagonal, quadri2, zigzag, pianella, parquet,
    righe, palle, ottagon, quadri3, foglie, fiori, bracci, dummy
} FillIndex ;

extern char Fills[][8] ;

void initgraphscreen(int mode) ;
/* attiva il modo grafico: mode == ATT400 opp. CGA
 */

void *loadfile(const char *name) ;
/* rende residente in memoria il file specificato. Restuituisce l'indirizzo a
 * cui Š caricato. Potr… essere liberato in seguito con una free()
 */

void selectfont(int n, int size);
/* fa una SetTextStyle, eventualmente caricando prima il font
 */

/*
 * Crea il file name completo (allocato in un buffer STATICO) per un file
 * nella directory grafica, appendendo automaticamente ".BYT"
 */
char * grpath (const char * name) ;
