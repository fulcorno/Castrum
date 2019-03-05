/*
 * programma "DIMEZZA.C"
 *
 * data una figura BitMap in ingresso, ne restituisce una in uscita con il
 * numero di linee verticali dimezzato, cercando di mantenere il pi— possibile
 * l'informazione inizialmente contenuta.
 *
 * USO: dimezza [-<opzioni>] <filein.ext> [<fileout[.ext]>] [<size>]
 * dove:
 * <filein> = nome della bitmap da dimezzare
 * <fileout> = nome del file da generare (se assente default-a a <filein>, il
 *    quale viene backup-ato)
 * <size> = larghezza dell'immagine, in byte
 * <opzioni> = sequenza di lettere, con il seguente significato:
 *    r : raw mode, sequenza di byte non formattati: necessario <size>
 *    i : image, formato di {put|get}image(), vietato <size>
 *    o : OR, adatto per figure bianco su nero
 *    & : AND, per figure nero su bianco
 *    a : Average, calcola una media su 3x3, dove l'esterno Š pensato del
 *        colore maggiormente presente nella figura
 *    q : quiet, calcola senza interazione
 *    v : verbose, spiega cosa sta facendo
 *    g : graphics, mostra i risultati a video 640x400
 *
 * default: -roq
 * combinazioni possibili: | = alternativa, [] = opzionale
 *    [r|i][o|&|a][q|v][g]
 * l'ordine delle opzioni *non* Š influente.
 */

#include <graphics.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>
#include <string.h>
#include <mem.h>
#include <stdlib.h>
#include <ctype.h>
#include <dos.h>

#include "drivers.h"

unsigned int * Badr ;

char * ini,	/* immagine in input  */
	 * oti,	/* immagine in output */
	 * ind,	/* inizio dati input  */
	 * otd;	/* inizio dati output */
/* N.B. con l'opzione -r ??i==??d, altrimenti con -i ??d==??i+4,
	dove ?? = 'in' opp. 'ot' */

char namein[80]="", nameout[80]="" ;

/* switch variables & defaults */
enum img_format	{ RAW, IMAGE }		format	= RAW ;
enum img_blit 	{ OR, AND, AVG }	blit	= OR ;
enum img_view 	{ QUIET, VERBOSE }	view	= QUIET ;
enum img_graph 	{ NOGRAPH, GRAPH } 	graph	= NOGRAPH ;

int isize = 0 ; /* lateral size of image */
int ysize = 0 ; /* vertical size of image */

enum errors { NOERROR, FEWPARMS, TOOPARMS, SYNOPT, UKNOPT, UKNSIZ,
	NOWANTSIZ, SYNSIZE, NOMEM, COMPLXIMG, LOWVER, NOGRDRIV, NOGRMEM,
	/* i prossimi sono errori di I/O */
	FIRST_IO_ERROR,
	FNOREN, FNOINP, FNOOUT, FRDINP, FWROUT, NOBADR,
} ;
char *errmsg[] = {
	"No error detected",
	"Too few parameters passed. Issue 'dimezza ?' for help",
	"Too many parameters passed. Issue 'dimezza ?' for help",
	"Syntax error in option format",
	"Unknown command line option",
	"Missing image horizontal size for 'raw' format",
	"Don't need image horizontal size for 'image' format",
	"Syntax error in specifying size",
	"Not enough memory to translate",
	"'Image' formats must have byte-sized widths with this release. Sorry.",
	"Feature unsupported in current release. Sorry.",
	"Can't enter graphics mode",
	"Non enough memory to show images",
	"**dummy**",
	"Can't rename file",
	"Can't open input file",
	"Can't open output file",
	"Can't read input file",
	"Can't write output file",
	"Can't find file with Base-Address of graphic page",
} ;

/*
 * doerr()
 * controlla se una chiamata Š andata a buon fine, altrimenti stampa
 * il messaggio ed esce
 */
void doerr( int err )
{
	if (!err) return ;
	/* get here only if err!=0 */
	if (err<FIRST_IO_ERROR) {
		fputs ( errmsg[err], stderr ) ;
		fputs ( "\n", stderr ) ;
	} else
		perror ( errmsg[err] ) ;
	exit(err) ;
}

/*
 * setvars()
 * Legge i valori dei parametri passati da linea di comando e
 * setta le opportune variabili di stato. Ritorna 0 se Š tutto OK,
 * altrimenti un codice d'errore
 */
int setvars (int argc, char ** argv)
{
	int ct = 1 ;
	char * p ;

	if (argc==1) return FEWPARMS ;

	if (!strcmp(argv[ct],"?")) {
		puts(
			"USO: dimezza [-<opzioni>] <filein.ext> [<fileout[.ext]>] [<size>]\n"
			"<filein> = nome della bitmap da dimezzare\n"
			"<fileout> = nome del file da generare (se assente default-a a <filein>, il\n"
			"   quale viene backup-ato)\n"
			"<size> = larghezza dell'immagine, in byte\n"
			"<opzioni> = sequenza di lettere, con il seguente significato:\n"
			"   r : raw mode, sequenza di byte non formattati: necessario <size>\n"
			"   i : image, formato di {put|get}image(), vietato <size>\n"
			"   o : OR, adatto per figure bianco su nero\n"
			"   & : AND, per figure nero su bianco\n"
			"   a : Average, calcola una media su 3x3, dove l'esterno Š pensato del\n"
			"       colore maggiormente presente nella figura\n"
			"   q : quiet, calcola senza interazione\n"
			"   v : verbose, spiega cosa sta facendo\n"
			"   g : graphics, mostra i risultati a video 640x400\n"
			"default: -roq\n"
			"combinazioni possibili: | = alternativa, [] = opzionale\n"
			"   [r|i][o|&|a][q|v][g]\n"
			"l'ordine delle opzioni *non* Š influente.\n"
		) ;
		exit(1) ;
	}

	if (*argv[ct]=='-') {
		/* parse options */
		if (!argv[ct][1]) return SYNOPT ;
		for (p=argv[ct]+1; *p; ++p)
			switch(tolower(*p)) {
				case 'r': format = RAW ; break ;
				case 'i': format = IMAGE ; break ;

				case 'o': blit = OR ; break ;
				case '&': blit = AND ; break ;
				case 'a': blit = AVG ; break ;

				case 'q': view = QUIET ; break ;
				case 'v': view = VERBOSE ; break ;

				case 'g': graph = GRAPH ; break ;

				default: return UKNOPT ;
			}
		ct++ ;
	}

	if (ct>=argc) return FEWPARMS ;

	strcpy(namein,argv[ct]) ;
	if (!strchr(namein,'.')) strcat(namein,".BYT") ;
	ct ++ ;

	strcpy(nameout,namein) ;

	if (format==RAW && ct>=argc) return UKNSIZ ;

	/* o c'Š <fileout> oppure <size> */
	p = argv[ct++] ;
	if ((isize=atoi(p))==0) {
		/* non c'era un numero */
		strcpy( nameout, p ) ;
		if (!strchr(nameout,'.'))
			strcat(nameout,strchr(namein,'.')) ;
	} else if (ct<argc) return TOOPARMS ;

	if ( format==IMAGE && ( ct<argc||isize ) ) return NOWANTSIZ ;
	if (!isize && format==RAW) {
		if ( ct>=argc ) return UKNSIZ ;
		isize = atoi(argv[ct]) ;
		if (!isize) return SYNSIZE ;
	}

	/* all went O.K. */
	return NOERROR ;
}

/*
 * chkbackup()
 * controlla se filein==fileout, e nel caso fa il backup di filein
 */
int chkbackup (void)
{
	char * p ;

	if (stricmp(namein,nameout)) return NOERROR ;

	/* must backup */
	if (view==VERBOSE) puts("Backing up input file") ;
	p = strchr(namein,'.') ;
	strcpy(p,".BAK") ;

	if ( rename(nameout,namein) ) return FNOREN ;
	return NOERROR ;
}

/*
 * openfiles()
 * apre i file controllando gli errori
 */
int openfiles( FILE ** in, FILE ** out )
{
	*in = fopen ( namein, "rb" ) ;
	if (!*in) return FNOINP ;
	*out = fopen ( nameout, "wb" ) ;
	if (!*out) return FNOOUT ;
	return NOERROR ;
}

/*
 * getin()
 * legge il file in input e setta i puntatori necessari, e 'ysize'
 * se format==IMAGE setta anche 'isize'
 */
int getin (FILE * f)
{
	size_t len = (size_t) filelength(fileno(f)) ;
	ini = malloc (len) ;
	if (!ini) return NOMEM ;
	if (len != fread (ini,1,len,f)) return FRDINP ;
	if (format == RAW) {
		ind = ini ;
		ysize = len / isize ;
	} else {
		ind = ini+4 ;
		isize = *(int*)ini+1 ;
		if (isize & 0x07) return COMPLXIMG ;
		isize >>=3 ;
		ysize = *(int*)(ini+2)+1 ;
	}
	return NOERROR ;
}

/*
 * getmem()
 * calcola ed alloca la memoria necessaria per l'immagine di output,
 * comunicandone la lunghezza effettiva.
 * se format==IMAGE predispone i primi 4 bytes
 */
int getmem(size_t * len)
{
	*len = isize * (ysize / 2) ;
	if (format==IMAGE) *len += 4 ;
	oti = malloc(*len) ;
	if (!oti) return NOMEM ;
	if (format==RAW) otd = oti ;
	else {
		otd = oti + 4 ;
		*(int*)oti = (isize << 3) - 1 ;
		*(int*)(oti+2) = ysize/2 - 1 ;
	}
	return NOERROR ;
}

/*
 * convert()
 * trasferisce l'immagine comprimendola in AND o in OR
 */
int convert(void)
{
	int y,pto,pti ;
	register int x ;

	if (blit==OR)
		for ( pto=pti=y=0; y<ysize/2; ++y, pto+=isize, pti+=isize*2)
			for ( x=0; x<isize; ++x)
				otd[pto+x] = ind[pti+x] | ind[pti+x+isize] ;
	else
		for ( pto=pti=y=0; y<ysize/2; ++y, pto+=isize, pti+=isize*2)
			for ( x=0; x<isize; ++x)
				otd[pto+x] = ind[pti+x] & ind[pti+x+isize] ;
	return NOERROR ;
}

/*
 * smoothout()
 * riduce calcolando la media 3x3
 */
#define smoothout() LOWVER

/*
 * flushfile()
 * salva il risultato dell'eleborazione e chiude il file
 */
int flushfile ( FILE * f, size_t len )
{
	if (len!=fwrite(oti,1,len,f)) return FWROUT ;
	if (fclose(f)) return FWROUT ;
	return NOERROR ;
}

/*
 * showthem()
 * mostra il sorgente e il destinatario su schermo 640x400
 */
int showthem(void)
{
	int gd=ATT400, gm=ATT400HI ;
	int y ;
	FILE * bdr ;

	Badr = malloc (400*sizeof(*Badr)) ;
	if (!Badr) return NOGRMEM ;
	bdr = fopen("att\\badr.byt","rb") ;
	if (!bdr) return NOBADR ;
	fread(Badr,400,sizeof(*Badr),bdr) ;
	fclose(bdr) ;

	initgraph(&gd,&gm,"") ;
	if (graphresult()!=grOk) return NOGRDRIV ;

	if (format==RAW)
		for (y=0; y<ysize; ++y)
			memcpy(MK_FP(0xB800,Badr[y]),ind+y*isize,isize) ;
	else putimage(0,0,ind,COPY_PUT) ;

	if ( isize>40 || ysize>300) getch() ;

	if (format==RAW)
		for (y=0; y<ysize/2; ++y)
			memcpy(MK_FP(0xB800,80-isize+Badr[y+400-ysize/2]),otd+y*isize,isize) ;
	else putimage(640-isize*8,400-ysize/2,otd,COPY_PUT) ;

	getch() ;
	closegraph() ;
	return NOERROR ;
}

void main ( int argc, char ** argv )
{
	FILE * fin, * fout ;
	size_t foutlen ;

	puts("DIMEZZA: Fulvio Corno per Jena-Soft 1989") ;
	doerr(setvars(argc,argv)) ;
	if (view==VERBOSE) printf("Translating %s to %s\n",namein,nameout) ;
	doerr(chkbackup()) ;
	doerr(openfiles(&fin,&fout)) ;
	if (view==VERBOSE) puts("Reading input image") ;
	doerr(getin(fin)) ;
	fclose(fin) ;
	doerr(getmem(&foutlen)) ;
	if (view==VERBOSE) {
		printf("Converting image %dx%d to %dx%d\n",isize<<3,ysize,isize<<3,ysize/2) ;
		printf("Resulting file size = %ud\n",foutlen) ;
		printf("Method used: %s\n",
			(blit==OR) ? "OR - white on black" :
			(blit==AND) ? "AND - black on white" :
				"Average - smoothing"
		) ;
	}
	if (blit != AVG) doerr(convert()) ;
	else doerr(smoothout()) ;
	doerr(flushfile(fout,foutlen)) ;
	if (graph==GRAPH) doerr(showthem()) ;
	free(ini); free(oti);
	exit(0) ;
}

