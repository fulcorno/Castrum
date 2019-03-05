/*
 * programma CompileDAT: CDAT.C
 * compila un file *.DAT nel corrispondente *.DCA (data compiled array)
 * ### Exclusive property of Jena Soft - 1988 ###
 * Usato per la creazione del database Sancti Georgii Castrum
 *
 * Sintassi: CDAT <nome> ...
 *  dove <nome> Š il nome di un Global Array del programma
 */

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"

#define reclen 20 /* # max righe di un record in .dat */
#define linlen 150 /* # max caratteri per riga */
char separatori[] = " .,*=" ;

#define match(s,t) ( ((!strcmp(s,"*"))||(!strcmp(s,t))) && openfiles(&inp,&out,t) )

/* variabili globali */
FILE * inp, * out ;
char rec[reclen][linlen] ;
int totrows, debug=0, times=1 ;

static void near error(char *msg)
{
	printf("*** ERRORE *** : %s.\n", msg) ;
	exit(1) ;
}

static int near openfiles(FILE **f, FILE **g, char * fnam )
{
	char nomef[linlen], nomeg[linlen] ;
	int h_f, h_g ; /*handles*/
	struct ftime t_f, t_g ;

	strcpy(nomef,fnam) ;
	strcat(nomef,".dat") ;
	h_f = open (nomef, O_RDONLY) ;
	if (h_f==-1) perror(nomef) ;
	getftime(h_f, &t_f) ;
	close(h_f) ;

	strcpy(nomeg,fnam) ;
	strcat(nomeg,".dca") ;
	h_g = open (nomeg, O_RDONLY) ;
	if (h_g==-1) t_g = t_f ;
	else {
		getftime(h_g, &t_g) ;
		close(h_g) ;
	}

	/* MACHINE DEPENDENT: assume che ftime sia di 32 bit con l'anno
		nei bit alti */
	if ( !times || (*(unsigned long *)&t_f >= *(unsigned long *)&t_g) )  {

		printf("Opening input file: %s\n",nomef) ;
		*f = fopen( nomef, "rt" ) ;
		if (!*f) error("File not found") ;

		printf("Opening output file: %s\n",nomeg) ;
		*g = fopen ( nomeg, "wb" ) ;
		if (!*f) error("File not found") ;

		totrows=0;
		return 1 ;
	} else {
		printf("%s is up to date.\n",fnam) ;
		return 0 ;
	}
} /* openfiles() */

static void near closefiles(void)
{
	printf("\rDone without errors. %ld bytes written\n",
		filelength(fileno(out)));
	fclose(inp);
	fclose(out);
}

static int near readxx(FILE *f, char * s)
{
	char * p, *q, s1[linlen+1] ;
	*s1 = '\0' ;
	fgets(s1,linlen,f) ;
	printf("Line %4d\r",++totrows) ;

	/* elimina spazi in testa */
	p = s1 ;
	while(isspace(*p))
		p++ ;

	/* elimina spazi in coda */
	q = p + strlen(p) - 1 ;
	while(isspace(*q))
		*(q--) = '\0' ;

	/* e' un commento ? */
	if (*p == '*')
		*p = '\0' ;
	strcpy(s,p) ;

	return strlen(s) ;
}

static int near getrecord(void)
/* legge un record nelle stringhe di REC, ognuna terminata da '$' */
{
	int i ;
	char s[linlen] ;

	for (i=0; i<reclen; *rec[i++]='\0')
		;
	i=0;

	/* ignora le linee vuote e/o inizianti per '*' */
	while (!readxx(inp,s)&&!feof(inp))
		;

	/* qui s contiene una linea NON nulla, oppure Š End Of File */

	/* se non Š EOF copio fino alla prossima nulla */
	if (!feof(inp)) do {
		strcpy(rec[i],s) ;
		strcat(rec[i],"$") ;
		if(debug) printf("%2d] #>%s<#\n",i,s) ;
		++i ;
	} while (!feof(inp)&&readxx(inp,s)&&(i<reclen)) ;
	return i ;
} /* getrecord() */

/**********************
***** S T A N Z A *****
*******************************************
** <n^stanza> <nome>                     **
** <dim_z> <tipo> <terreno> [<disegno>]  **
******************************************/
static void near loadstanza(void)
{
	indice_stanza i ;
	int t ;

	stanza = calloc(max_stanza,sizeof(t_stanza)) ;
	tot_stanze=-1;
	while (getrecord()) {
		if (strcmp(rec[0],"0")) {
			sscanf(rec[0],"%d%n", &i, &t) ;
			sscanf(rec[0]+t," %[^$]",stanza[i].nome ) ;
			sscanf(rec[1],"%d %d %d %d", &stanza[i].dim_z, &stanza[i].tipo,
				&stanza[i].terreno, &stanza[i].disegno /*opt*/ ) ;
			if( i>tot_stanze ) tot_stanze=i ;
		} else break ;
		if (debug) {
			printf("\tStanza %3d: %s\n",i,stanza[i].nome) ;
			printf("\taltezza %2d,tipo %2d, terreno %2d, disegno %2d\n",
				stanza[i].dim_z,stanza[i].tipo, stanza[i].terreno,
				stanza[i].disegno ) ;
		}
	}
	fwrite(stanza,sizeof(t_stanza),tot_stanze+1,out) ;
	free(stanza) ;
	closefiles() ;
} /* loadstanza() */

/********************
***** L U O G O *****
******************************************************************************
** <n^luogo> <n^stanza>                                                     **
** <dim_x> <dim_y>                                                          **
** <0.forma> <0.tipo> [ <0.porta1> <0.uscita1> [ <0.porta2> <0.uscita2> ] ] **
** <1.forma> <1.tipo> [ <1.porta1> <1.uscita1> [ <1.porta2> <1.uscita2> ] ] **
** <2.forma> <2.tipo> [ <2.porta1> <2.uscita1> [ <2.porta2> <2.uscita2> ] ] **
** <3.forma> <3.tipo> [ <3.porta1> <3.uscita1> [ <3.porta2> <3.uscita2> ] ] **
*****************************************************************************/
static void near loadluogo(void)
{
	indice_luogo i ;
	int t ;
	byte j ;

	luogo = calloc(max_luogo,sizeof(t_luogo)) ;
	tot_luoghi=-1;
	while (getrecord()) {
		if (strcmp(rec[0],"0")) {
			sscanf(rec[0],"%d %n", &i, &t);
			sscanf(rec[0]+t,"%d",&luogo[i].n_stanza ) ;
			sscanf(rec[1],"%d %d", &luogo[i].dim_x, &luogo[i].dim_y ) ;
			if(debug) printf("\tLuogo %3d->%3d (%2dx%2d)\n",i,luogo[i].n_stanza,
            	luogo[i].dim_x,	luogo[i].dim_y) ;
			for (j=0; j<=3; ++j) {
				sscanf(rec[2+j],"%d %d %d %d %d %d",
					&luogo[i].pareti[j].forma, &luogo[i].pareti[j].tipo,
					&luogo[i].pareti[j].n_porta1,&luogo[i].pareti[j].n_uscita1,
					&luogo[i].pareti[j].n_porta2,&luogo[i].pareti[j].n_uscita2
					) ;
				if(debug) printf("\tparete %d: forma %2d, tipo %2d, uscite %3d (%3d), %3d (%3d)\n",
					j,luogo[i].pareti[j].forma,luogo[i].pareti[j].tipo,
					luogo[i].pareti[j].n_porta1, luogo[i].pareti[j].n_uscita1,
					luogo[i].pareti[j].n_porta2, luogo[i].pareti[j].n_uscita2);
			}
			if( i>tot_luoghi ) tot_luoghi=i ;
		} else break ;
	}
	fwrite(luogo,sizeof(t_luogo),tot_luoghi+1,out) ;
	free(luogo) ;
	closefiles() ;
} /* loadluogo() */

/********************
***** P O R T A *****
*************************
** <n^porta>           **
** <tipo> [ <stato> ]  **
************************/
static void near loadporta(void)
{
	indice_porta i ;

	porta = calloc(max_porta,sizeof(t_porta)) ;
	tot_porte=-1;
	while (getrecord()) {
		if (strcmp(rec[0],"0")) {
			sscanf(rec[0],"%d", &i) ;
			sscanf(rec[1],"%d %d", &porta[i].tipo, &porta[i].stato) ;
			if( i>tot_porte ) tot_porte=i ;
		} else break ;
		if(debug) printf("\tPorta %3d: tipo %d, stato %d\n",
			i,porta[i].tipo,porta[i].stato) ;
	}
	fwrite(porta,sizeof(t_porta),tot_porte+1,out) ;
	free(porta) ;
	closefiles() ;
} /* loadporta() */

/******************
***** M E N U *****
*********************************************************
** <n^menu> <titolo>                                   **
** <tasto> [#<disegno>] <azione> <nextmenu> [<frase>]  **
********************************************************/
static void near loadmenu(void)
{
	indice_menu i ;
	int n,m,t,t1 ;
	char s[3] ;

	menu = calloc(max_menu,sizeof(t_menu)) ;
	tot_menus=-1;
	while (getrecord()) {
		if (strcmp(rec[0],"0")) {
			sscanf(rec[0],"%d%n",&i, &t);
			sscanf(rec[0]+t," %[^$]",menu[i].titolo) ;
			m=1 ;
			if(debug) printf("\tMenu %d: %s\n",i,menu[i].titolo) ;
			while(*rec[m]) {
				sscanf(rec[m],"%d %n", &n, &t) ;
				sscanf(rec[m]+t,"%1s%n", s, &t1) ;
				if (*s=='#') {
					t+=t1 ;
					sscanf(rec[m]+t,"%d%n",&menu[i].tasto[n-1].disegno,&t1);
					t+=t1 ;
				}
				sscanf(rec[m]+t,"%d %d%n", &menu[i].tasto[n-1].azione,
					&menu[i].tasto[n-1].nextmenu, &t1 ) ;
				sscanf(rec[m]+t+t1," %[^$]",menu[i].tasto[n-1].frase) ;
				m++;
				if(debug) printf("\tTasto %d: icona %d, azione %d, next %d; %s\n",
                	n,	menu[i].tasto[n-1].disegno, menu[i].tasto[n-1].azione,
					menu[i].tasto[n-1].nextmenu, menu[i].tasto[n-1].frase ) ;
			}
			if( i>tot_menus ) tot_menus=i ;
		} else break ;
	}
	fwrite(menu,sizeof(t_menu),tot_menus+1,out) ;
	free(menu) ;
	closefiles() ;
} /* loadmenu() */

/******************
***** D A T O *****
******************************************
** <tipo1> <tipo2> [<agget>] <nomeGLB>  **
** {<idx_dat>.<val_dat>}+               **
** #|{<verbo>+}                         **
*****************************************/
static void near loaddato(void)
{
	int t ;
	char * p ;
	t_parametri j ;
	int val ;

	dato = calloc(max_dato,sizeof(t_dato)) ;
	tot_dati=-1;
	while (getrecord()) {
		if (strcmp(rec[0],"0")) {
			++ tot_dati ;
			sscanf(rec[0],"%d %d %d%n", &dato[tot_dati].tipo.tipo1,
				&dato[tot_dati].tipo.tipo2, &dato[tot_dati].agget, &t) ;
			sscanf(rec[0]+t," %[^$]",dato[tot_dati].NomeGlb ) ;
			if(debug) printf("\tDato %3d: (%2d,%2d) agget %d; %s\n",
				tot_dati,dato[tot_dati].tipo.tipo1,	dato[tot_dati].tipo.tipo2,
				dato[tot_dati].agget, dato[tot_dati].NomeGlb) ;
			p = rec[1] ;
			while(p&&*p) {
				sscanf(p,"%d%n",&j,&t) ;
				if( sscanf(p+t,".%d", &val) != 1 )
					printf("Missing %d in object %s\n",j,dato[tot_dati].NomeGlb) ;
				else writedati(&dato[tot_dati].dati,j,val) ;
				if (debug) printf("\t%d=%d",j,readdati(&dato[tot_dati].dati,j));
				p = strchr(p+t,' ') ;
			}
			if (debug) printf("\n\tverbi: (") ;
			nullset(dato[tot_dati].verbo_P) ;
			nullset(dato[tot_dati].verbo_L) ;
			p = rec[2] ;
			if(*p!='#') while (p&&*p) {
				if(sscanf(p,"%d%n",&j,&t)==1)
					addset(j,dato[tot_dati].verbo_P) ;
				if (debug) printf("%d,",j) ;
				p = strchr(p+t,' ') ;
			}
			if (debug) printf("); (") ;
			p = rec[3] ;
			if(*p!='#') while (p&&*p) {
				if(sscanf(p,"%d%n",&j)==1)
					addset(j,dato[tot_dati].verbo_L) ;
			if (debug) printf("%d,",j) ;
				p = strchr(p+t, ' ') ;
			}
			if (debug) printf(")\n") ;
		} else break ;
	}
	fwrite(dato,sizeof(t_dato),tot_dati+1,out) ;
	free(dato) ;
	closefiles() ;
} /* loaddato() */

/********************
***** V E R B O *****
************************************
** <n^verbo> <tipo> <prep> <nome> **
** <spiegazione>                  **
***********************************/
static void near loadverbo(void)
{
	indice_verbo i ;
	int t,t1 ;

	verbo = calloc(max_verbo,sizeof(t_verbo)) ;
	tot_verbi=-1;
	while (getrecord()) {
		if (strcmp(rec[0],"0")) {
			sscanf(rec[0],"%d%n",&i,&t);
			sscanf(rec[0]+t,"%d %d%n",&verbo[i].tipo,&verbo[i].prep,&t1) ;
			sscanf(rec[0]+t+t1," %[^$]",verbo[i].nome) ;
			sscanf(rec[1]," %[^$]",verbo[i].spiega) ;
			if( i>tot_verbi ) tot_verbi=i ;
			if (debug) printf("\tVerbo %3d: tipo %d, prep %d; %s (%s...)\n",
				i,verbo[i].tipo,verbo[i].prep,verbo[i].nome, verbo[i].spiega) ;
		} else break ;
	}
	fwrite(verbo,sizeof(t_verbo),tot_verbi+1,out) ;
	free(verbo) ;
	closefiles() ;
} /* loadverbo() */

/****************
***** O B J *****
**************************************************
** <n^obj> <tipo1> <tipo2> [<agget> <nomeLOC>]  **
** [<statoiniz>+]                               **
** <luogo> [<nasconde>]                         **
*************************************************/
static void near loadobj(void)
{
	indice_obj i ;
	int j,t,t1 ;
	char * p ;

	obj = calloc(max_obj,sizeof(t_obj)) ;
	tot_objs=-1;
	while (getrecord()) {
		if (strcmp(rec[0],"0")) {
			sscanf(rec[0],"%d%n", &i, &t) ;
			if(sscanf(rec[0]+t,"%d %d %d%n", &obj[i].tipo.tipo1,
				&obj[i].tipo.tipo2, &obj[i].agget, &t1)==3)
				sscanf(rec[0]+t+t1," %[^$]",obj[i].NomeLoc ) ;
			if (debug) {
				printf("\tObj %3d: tipo (%2d,%2d), agget %d; %s\n",
				i,obj[i].tipo.tipo1, obj[i].tipo.tipo2, obj[i].agget,
				obj[i].NomeLoc ) ;
				printf("\tstato = ") ;
			}
			nullset(obj[i].stato) ;
    		p = rec[1] ;
			while (p&&*p) {
				if(sscanf(p,"%d%n",&j,&t)==1)
					addset(j,obj[i].stato) ;
				if (debug) printf("%d,",j) ;
				p = strchr(p+t,' ') ;
			}
			sscanf(rec[2],"%d %d",&obj[i].dove, &obj[i].nasconde) ;
			if (debug) printf("\n\tposto in %3d, nasconde %3d\n",
				obj[i].dove,obj[i].nasconde) ;
			if( i>tot_objs ) tot_objs=i ;
		} else break ;
	}
	fwrite(obj,sizeof(t_obj),tot_objs+1,out) ;
	free(obj) ;
	closefiles() ;
} /* loadobj() */

/* blocco ricorrente
	indice_xxx i ;
	xxx = calloc(max_xxx,sizeof(t_xxx)) ;
	tot_xxxs=-1;
	while (getrecord()) {
		if (strcmp(rec[0],"0")) {
			if( i>tot_xxxs ) tot_xxxs=i ;
		} else break ;
	}
	fwrite(xxx,sizeof(t_xxx),tot_xxxs+1,out) ;
	free(xxx) ;
	closefiles() ;
*/

void main(int argc, char ** argv)
{
	char name[linlen] ;
	int ctr ;

	printf("Jena Soft  --  .DAT => .DCA Compiler  --  S.G.CASTRUM\n");
	for (ctr=1; ctr<argc; ++ctr) {
		strcpy(name,argv[ctr]) ;
		strupr(name) ;
		if (*name == '-') {
			switch (name[1]) {
				case 'D': debug=1 ; break ; /* debug mode */
				case 'Q': debug=0 ; break ;	/* quiet mode (def) */
				case 'A': times=0 ; break ; /* compile anyway */
				case 'T': times=1 ; break ; /* look for times (def) */
			}
			continue ;
		}
		printf("\nItem to process --> %s\n",name);
		if(match(name,"STANZA") ) loadstanza() ;
		if(match(name,"LUOGO")  ) loadluogo() ;
		if(match(name,"PORTA")  ) loadporta() ;
		if(match(name,"MENU")   ) loadmenu() ;
		if(match(name,"DATO")   ) loaddato() ;
		if(match(name,"VERBO")  ) loadverbo() ;
		if(match(name,"OBJ")    ) loadobj() ;
	}
}
