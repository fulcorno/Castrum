/* Programma per ridurre lo spazio occupato dai files ".OGG",
   immagini degli oggetti in Sancti Georgii Castrum, e concatenare le
   immagini in un unico file.
   Scritto da Fulvio Corno per Jena-Soft.
*/

/********************
  ALGORITMO:
     1   chiedi il nome del file .PKI (PacKed_Image) di destinazione ;
     2   chiedi il nome del file .OGG da comprimere/aggiungere ;
     3   comprimi il file :
        3.1 leggi un carattere ;
        3.2 leggi tutti i seguenti caratteri uguali a questo ;
        3.3 sia N il numero di caratteri letti e CH il loro valore ;
        3.4 se (N=1)
           3.4.1  ALLORA memorizza [CH] ;
           3.4.2  ALTRIMENTI memorizza [ESC/N/CH] ;
        3.5 torna a 3.1 finchä il file non finisce ;
     4   aggiungi il file compresso al .PKI previo conferma. I primi 2
        bytes conterranno la lunghezza del file compresso, i successivi
        2 la lunghezza del file originario (come controllo), i restanti
        conterranno il file compresso stesso ;
     5   torna a 2 se l'utente vuole ;
     6 END

   N.B. ESC = ascii 0xFF
      se CH=ESC e N < 0xFF allora memorizza: [ESC/ESC]
      se N >= 0xFF allora spezza la sequenza in pió parti lunghe
         al massimo 0xFE bytes. (Es: 300 "X" --> ESC/254/X/ESC/46/X )

****************/

/* #include's */
#include <io.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

/* #define's */
#define STRSIZE   160           /* max string length */
#define ESC       0xFF          /* compressed escape character */

/* prototypes */
long comprimi ( FILE * infile, char * outbuffer, long oldsize ) ;

void main(void)
{
   /* variabili di main() */
   FILE * in, * out ;            /* in/out files */
   char * buffer ;               /* conterrÖ l'immagine compressa */
   long imgsize, newsize ;       /* dimensioni prima e dopo compressione */
   int ch ;
   char inN[STRSIZE], outN[STRSIZE] ; /* in/out file names */
   /* intestazione */
   printf("Sancti Georgii Castrum - Jena Soft - COMPRESS.EXE : compattatore di immagini\n") ;
   /* lettura file output */
   do {  
      printf("Nome file di OUTPUT [.PKI] : ") ;
      gets(outN) ;
      if (!strchr(outN,'.'))
         strcat(outN,".pki") ;
   } while ( !(out=fopen(outN,"ab")) ) ;
   /* loop principale: scansione files in input */
   do
      {
      printf("Nome file di INPUT [.OGG] : ") ;
      gets(inN) ;
      if (!strchr(inN,'.'))
         strcat(inN,".ogg") ;
      if (in=fopen(inN,"rb"))
         {
         imgsize = filelength(fileno(in)) ;
         buffer = (char *) malloc(imgsize) ;
         if (!buffer)
            {
            fprintf(stderr,"Ho poca memoria: non posso lavorare\n") ;
            abort() ;
            }
         newsize = comprimi(in,buffer,imgsize) ;
         printf("Ho compresso il file %s da %ld a %ld caratteri: %ld%%.\n",
            inN,imgsize,newsize,100-100*newsize/imgsize) ;
         printf("Posso salvarlo su %s [S/N] ? _",outN) ;
         do
            {
            putch('\b') ;
            ch=getche() ;
            ch=toupper(ch) ;  /* toupper(getche()) is WRONG because
                                toupper() is a MACRO whose argument
                                gets evaluated TWICE !!! */
            }   
         while ((ch!='S')&&(ch!='N')) ;
         printf("\n") ;
         if (ch=='S')
            {
            fwrite(&newsize,sizeof newsize,1,out) ;
            fwrite(&imgsize,sizeof imgsize,1,out) ;
            fwrite(buffer,newsize,1,out) ;
            }
         fclose(in) ;
         free(buffer) ;
         }
      else printf("Non esiste il file \"%s\".\n",inN ) ;
      printf("Premi un tasto - ESC per finire\n") ;
   } while (getch()!=0x1B) ;
   fclose(out) ;
   printf("COMPRESS terminato. Che la Jena sia con voi!\n") ; 
}

/* function COMPRIMI: programma vero e proprio */
long comprimi ( FILE * infile, char * outbuffer, long oldsize ) 
{
   int curch=-1, newch ; /* il 1^ char ä sconosciuto */
   long count=0 ;
   char * buf = outbuffer ;

   while (!feof(infile))
      {
      if ((newch=fgetc(infile))!=curch)
         {
         /* il carattere ä diverso: devo scriverlo */
         if (count==1)
            {  /* carattere singolo */
            if (curch==ESC)
               { /* singolo ESC */
               *(buf++)=ESC ;
               *(buf++)=ESC ;
               }
            else  /* singolo != ESC */
               {
               *(buf++) = curch ;
               }
            }
         if ((count>1)&&(count<ESC))
            { /* pió caratteri (meno di 0xFF) */
            *(buf++) = ESC ;
            *(buf++) = count ;
            *(buf++) = curch ;
            }
         if (count>=ESC)
            {
            /* pió di 0xFF caratteri */
            while (count>=ESC)
               {
               *(buf++) = ESC ;
               *(buf++) = ESC - 1 ;
               *(buf++) = curch ;
               count -= ESC - 1 ;
               }
            *(buf++) = ESC ;
            *(buf++) = count ;
            *(buf++) = curch ;
            }
         /* reset counters */
         curch = newch ;
         count = 1 ;
         }
      else
         { /* carattere = al precedente */
         count ++ ;
         }
   }
   return((unsigned)(buf-outbuffer)) ;
}
