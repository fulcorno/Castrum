/* printer.c

	Programma per settare i parametri della stampante velocemente da
	linea di comando DOS.

	Sintassi:
		PRINTER <command1> <command2> ...

	I <command> sono del tipo:
		#nn 	nn: codice HEX carattere in Output  (2 cifre)
		&ddd	ddd: codice DEC    "      "    "    (3 cifre)
		(xxx)	xxx: mnemonico ASCII: (CR),(LF),(DC1),...
		"cccc"	cccc: stringa caratteri ASCII (uno o piu`)
		.ext.	ext: mnemonico esteso offerto dal programma, come da
				tabella allegata.

    Il separatore (OPZIONALE) e` lo spazio. Se compare un carattere diverso
	dai precedenti e dal blank viene mandato in output. In pratica il "..."
	e` necessario solo se ... contiene spazi, '#', cifre, '.', '('.
	Maiuscole e minuscole sono indifferenti (tranne sotto "..."). Per
	introdurre il carattere <"> in una stringa usare la sequenza <"">.
	Invece <" "> (con uno spazio interposto) concatena due stringhe (inutile).

	Lista degli mnemonici estesi :

		ON		OFF		Attributo
		--------------------------------
		.DS+.	.DS-.	Double Strike
		.EN+.	.EN-.	ENlarged (per piu` righe)
		.E1+.	.E1-.	Enlarged (1 sola riga)
		.UL+.	.UL-.	sottolineato (UnderLined)
		.BO+.	.BO-.	grassetto (BOld)
		.SUP.        	apici (SUPerscript)
		.SUB.			pedici (SUBscript)
				.SU-.	apici & pedici (SUper/SUb-script OFF)
		.PI.			PIca
		.CO.			COndensed
		.EL.			ELite
		.NLQ.	.DFT.	Near Letter Quality / DraFT
		.TAB.			TABulazioni orizzontali (seguito da &nnn&nnn...&000 )
		.LNL.&nnn		lunghezza pagina in interlinee (LeNgth in Lines)
		.LNI.&nnn		   "         "    " pollici    (  "     " Inches)
		.BOF.&nnn
				.BOF-.	margine inferiore (Bottom Of Form)

	Tra i mnemonici standard ASCII troviamo:
        (NUL) (BS) (HT) (LF) (VT) (FF) (CR) (CAN) (ESC)

	Autore: Fulvio Corno
			via C.I.Giulio 24
	  10090 San Giorgio Canavese (TO)
			Tel. 0124/32160

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Parte 1: definizione dei mnemonici */

const char * ascii [32] = {
	"NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
	"BS",  "HT",  "LF",  "VT",  "FF",  "CR",  "SO",  "SI",
	"DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
	"CAN", "EM",  "SUB", "ESC", "FS",  "GS",  "RS",  "US"   } ;

struct  xmnem {    /* tabella per convertire i codici estesi .xxx. in */
	char * mnem ;  /* sequenze di codici ASCII (xxx) e #xx  */
	char * code ;
	} ;

struct xmnem table [] = {		/* equivalenze */
	{"DS+",		"(ESC)G"	},
	{"DS-",     "(ESC)H"	},
	{"E1+",		"(SO)"		},
	{"E1-",		"(DC4)"		},
	{"EN+",		"(ESC)W1"	},
	{"EN-",		"(ESC)W0"	},
	{"UL+",		"(ESC)-1"	},
	{"UL-",		"(ESC)-0"	},
	{"BO+",		"(ESC)E"	},
	{"BO-",		"(ESC)F"	},
	{"SUP",		"(ESC)S0"	},
	{"SUB",		"(ESC)S1"	},
	{"SU-",		"(ESC)T"	},
	{"PI",		"(DC2)"		},
	{"CO",		"(SI)"		},
	{"EL",		"(ESC)M"	},
	{"NLQ",		"(ESC)x1"	},
	{"DFT",		"(ESC)x0"	},
	{"LNL",		"(ESC)C"	},
	{"LNI",		"(ESC)C#00"	},
	{"BOF",		"(ESC)N"	},
	{"BOF-",	"(ESC)O"	},
	{"TAB",		"(ESC)D"	}
  } ;

int sub_extended(const char * line, int lenlin, char * result)
/* passo 1: convertire i codici extended .xxx. in sequenze di ASCII (xxx) */
{
	int pos,pdst,i,j ;
	int flag ;
	char tmp[30] ;
	const int TABLEN = sizeof(table)/sizeof(struct xmnem) ;

	pos = pdst = 0 ;
	flag = 1 ;
	while (pos<lenlin) {
		if (line[pos]=='"')
			flag = !flag ;
		if ((line[pos]=='.')&& flag) {
			i=0 ;
			pos++ ;
			while ((pos<lenlin)&&(line[pos]!='.')) {
				tmp[i++]=toupper(line[pos]) ;
				pos++ ; /* N.B. toupper e` una macro con side effects! */
			}
			if (pos>=lenlin) {
				printf("Errore: manca '.' dopo %s\n",line+pos-i) ;
				exit(1) ;
			} else pos++ ;
			tmp[i] = '\0' ;
			i=0 ;
			while((i<TABLEN)&&(strcmp(tmp,table[i].mnem)))
				i++ ;
			if (i==TABLEN) {
				printf("Errore: non riconosco '.%s.'\n",tmp) ;
				exit(1) ;
			} else { /* copio */
				for (j=0; (result[pdst]=table[i].code[j]) != 0; pdst++, j++)
					;
			}
		} else result[pdst++]=line[pos++] ;
	}
	return pdst ;
}

int sub_ascii(const char * line, int lenlin, char * result)
/* passo 2: riconoscimento mnemonici Ascii */
{
    int pos=0, pdst=0, i ;
	int flag=1 ;
	char tmp[30] ;
	const int TABLEN = sizeof(ascii)/sizeof(char *) ;
    while (pos<lenlin) {
		if (line[pos]=='"')
			flag = !flag ;
		if ((line[pos]=='(')&&flag) {
			i=0 ;
			pos++ ;
			while ((pos<lenlin)&&(line[pos]!=')')) {
				tmp[i++]=toupper(line[pos]) ;
				pos++ ; /* N.B. toupper e` una macro con side effects! */
			}
			if (pos>=lenlin) {
				printf("Errore: manca ')' dopo %s\n",line+pos-i) ;
				exit(1) ;
			} else pos++ ;
			tmp[i] = '\0' ;
			i=0 ;
			while((i<TABLEN)&&(strcmp(tmp,ascii[i])))
				i++ ;
			if (i==TABLEN) {
				printf("Errore: non riconosco '(%s)'\n",tmp) ;
				exit(1) ;
			} else result[pdst++]=i ;
		} else result[pdst++]=line[pos++] ;
	}
	return pdst ;
}

int sub_numbers(const char * line, int lenlin, char * result )
/* passo 3: conversione dei caratteri numerici #xx e &ddd e trattamento
   delle virgolette <">, eliminazione spazi superflui */
{
	int pos=0, pdst=0 ;
    int flag = 1, changed = 0 ;
    while(pos<lenlin) {
		if (line[pos]=='"') {
			flag = !flag ;
			changed = 1 ;
		} else changed = 0 ;
/* flag = 1 se sono FUORI dalle virgolette
   changed = 1 se ho appena incontrato le virgolette */
		if((line[pos]=='#')&&flag&&(pos+2<lenlin)) {
			pos++ ;
			if (isxdigit(line[pos])&&isxdigit(line[pos+1])) {
				result[pdst++]=(isdigit(line[pos]) ?
					line[pos]-'0' : toupper(line[pos])-'A'+10)*16+
					(isdigit(line[pos+1]) ? line[pos+1]-'0' :
					toupper(line[pos+1])-'A'+10) ;
				pos+=2 ;
			} else {
				printf("Errore: numero '%c%c' non esadecimale\n",line[pos],line[pos+1]) ;
				exit(1) ;
			}
		} else if((line[pos]=='&')&&flag&&(pos+3<lenlin)) {
			pos++ ;
			if(isdigit(line[pos])&&isdigit(line[pos+1])&&isdigit(line[pos+2])) {
				result[pdst++] = (line[pos]-'0')*100+(line[pos+1]-'0')*10+
								line[pos+2]-'0' ;
				pos+=3 ;
			} else {
				printf("Errore: numero '%c%c%c' non decimale\n",line[pos],line[pos+1],line[pos+2]) ;
				exit(1) ;
			}
		} else if (!changed)
			if ((line[pos]==' ')&&flag) pos++ ; /* igno r spazi fuori <"> */
			else result[pdst++]=line[pos++] ;
		else /* sono su una <"> */ {
			pos++ ;
			if ((pos<lenlin)&&(line[pos]=='"')) {
				result[pdst++]=line[pos++] ;
				flag=!flag ;
			}
		}
	}
	return(pdst) ;
}

void output(char* line, int len)
{
	int i ;
	for (i=0; i<len; ++i)
		if (line[i]>=' ') putchar(line[i]) ;
		else printf("^%c",line[i]+'@') ;
	putchar('\n') ;
}

#define MAXLEN 100
typedef char linea [MAXLEN] ;

void main()
{
	linea test, test1, test2 ;
	int	ln1,ln2,i ;

	printf("PRINTER - manda codici di controllo alla stampante\n") ;
	printf("Propriet… Jena Soft 1989 - Autore Fulvio Corno\n") ;
	printf("Configurato per Olivetti DM 100 - ? per avere aiuto\n") ;
	printf("\nComandi: ");
	gets(test) ;
	if (!*test) return ;
	if (strcmp(test,"?")) {
		ln1 = sub_extended(test, strlen(test),test1) ;
		ln2 = sub_ascii(test1,ln1,test2) ;
		ln1 = sub_numbers(test2,ln2,test1) ;
		for (i=0; i<ln1; ++i)
			putc(test1[i],stdprn) ;
	} else printf("Help function not implemented yet. Sorry!\n") ;
}
