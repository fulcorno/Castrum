/* Icon - Object Editor */
/* Utility per disegnare le icone dei tasti funzione & immagini oggetti */

#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "drivers.h"

/* dimensioni di default del disegno */
static int Xdim=32 ,
	x8=4, /* Xdim/8 */
	Ydim=28 ;

static int dotdim=8 ;

static int size ;

#define rvs 0xFF
#define bigX 330
#define bigY 320

/* cursor keys extended codes */
enum {
	kul=71,	kuu=72,	kur=73,
	kll=75,	/* 5 */	krr=77,
	kdl=79,	kdd=80,	kdr=81,
	ins=82,	del=83
	} ;

static double aspect ;
static char curdir[200] ;

static void graphscreen(void)
{
	int x,y ;

	initgraphscreen(ATT400) ;
	restorecrtmode() ;
	getaspectratio(&x,&y) ;
	aspect = (double)y / (double)x ;
}

static void draw(int x1, int y1, int x2, int y2, int c)
{
	if ( c>=0 ) {
		setcolor(c) ;
		line(x1,y1,x2,y2) ;
	} else {
		setwritemode(XOR_PUT) ;
		setcolor(1) ;
		line(x1,y1,x2,y2) ;
		setwritemode(COPY_PUT) ;
	}
}

static int keyboard (void)
{
	int c ;

	gotoxy(25,20) ; putch('\x01') ;
	c=toupper(getch()) ;
	if ( !c ) switch( getch() ) {
		case kul: c='q'; break ;
		case kuu: c='w'; break ;
		case kur: c='e'; break ;
		case kll: c='a'; break ;
		case krr: c='d'; break ;
		case kdl: c='z'; break ;
		case kdd: c='x'; break ;
		case kdr: c='c'; break ;
		case ins: c='v'; break ;
		case del: c='g'; break ;
	}
	gotoxy(25,20) ; putch('\x02') ;
	return c ;
}

static int lit(register int x, register int y, char * obj)
{
	if ( (x<0)||(y<0)||(x>=Xdim)||(y>=Ydim) ) return 0 ;
	else return (obj[y*x8 +(x>>3)] & (1<<(7-(x&7))) ) ;
}

static int sgn(register int x)
{
	return ( (x) ? ( (x>0) ? +1 : -1 ) : 0 ) ;
}

static void box(int vx, int vy, int scale, int c)
{
	setfillstyle(SOLID_FILL, c ) ;
	bar(vx,vy,vx+scale-1,vy+scale-1) ;
}

static void square(register int x, register int y, int dx, int dy, int k)
{
	int Dx = sgn (dx), Dy = sgn(dy) ;

	if (dx||dy) {
		draw(x,y,x+dx-Dx,y,k);
		draw(x+dx,y,x+dx,y+dy-Dy,k);
		draw(x+dx,y+dy,x+Dx,y+dy,k);
		draw(x,y+dy,x,y+Dy,k);
	} else {
		draw(x,y,x,y,k) ;
	}
}

static void display(char * what)
{
	int x,y ;

	for (y=0; y<Ydim; ++y) {
		for (x=0; x<x8; ++x)
			*(char far*)MK_FP(0xB800,Badr[y+300]+x) = what[y*x8+x] ^ rvs ;
	}
}

static void display3(int i1, int j1, int i2, int j2, char * what)
{
	register int i,j ;
	int ii,jj ;

	if ( i1<0 ) i1=0 ;
	if ( i2>=Xdim ) i2=Xdim-1;
	if ( j1<0 ) j1=0;
	if ( j2>=Ydim ) j2=Ydim-1;
	for (i=i1, ii=8+i*dotdim; i<=i2; ++i, ii+=dotdim)
		for (j=j1, jj=8+j*dotdim; j<=j2; ++j, jj+=dotdim)
			box(ii,jj,dotdim,lit(i,j,what)) ;
}

static void display3all(char * what)
{
	int k ;
	char v ;
	int i,j ;
	register int x,y ;

	setfillstyle(SOLID_FILL,1) ;
	for (j=0; j<Ydim; ++j) {
		y=j*dotdim+8;
		x=8;
		for (i=0; i<x8; ++i) {
			v=what[j*x8+i] ;
			for (k=0; k<=7; ++k) {
				if ( v & 0x80 ) bar(x,y,x+dotdim-1,y+dotdim-1) ;
				v <<= 1 ;
				x += dotdim ;
			}
		}
	}
}

static void clear(char *obj)
{
	memset(obj,0x00,size);
}


	/* NESTED voidS */

static int x,y,lx,ly ;
static int dotd2, dotd4, dotd0 ;
static int pencolor ;
static int modif,moved,writing ;
static char *old, *temp ;
static int *x0, *y0 ;
static char *what ;

static void blit(register char *v, register char mask)
{
	switch( pencolor ) {
		case -1: *v ^=  mask ; break ;
		case  0: *v &= ~mask ; break ;
		case +1: *v |=  mask ; break ;
    }
}

static void pixel(register int px, register int py)
{
	if ( (px<Xdim)&&(px>=0)&&(py>=0)&&(py<Ydim) ) {
		blit(&what[py*x8+(px>>3)], 1 << (7-(px & 7))) ;
		modif = 1 ;
		display3(px,py,px,py,what);
	}
}

static void segm(int x1, int y1, int x2, int y2)
{
	register int x,y ;
	int DeltaX,DeltaY,XStep,YStep,count ;

	if ((x1==x2)&&(y1==y2)) {
		pixel(x1,y2);
		return ;
	}

	x=x1; y=y1; XStep=1; YStep=1;
	if ( x1>x2 ) XStep=-1; if ( y1>y2 ) YStep=-1;
	DeltaX=abs(x2-x1); DeltaY=abs(y2-y1);
	if ( DeltaX==0 ) count=-1 ; else count=0;
	do {
		pixel(x,y);
		if (count<0) {
			y += YStep; count += DeltaX;
		}else{
			x += XStep; count -= DeltaY;
		}
	} while (!((x==x2)&&(y==y2))) ;
}

static void circl(void)
{
	double r2 ;
	register int sn,cs ;

	r2 = ((x+*x0-lx)/aspect)*((x+*x0-lx)/aspect)+(y+*y0-ly)*(y+*y0-ly) ;
	if ( r2<0.5 ) pixel(lx,ly) ;
	else {
		cs=floor(sqrt(r2)); sn=0;
		do {
			pixel(lx+sn,ly+cs);
			if ( sn!=0 ) {
				pixel(lx-sn,ly+cs);
				pixel(lx-sn,ly-cs);
			}
			pixel(lx+sn,ly-cs);
			++sn ;
			cs = floor(sqrt(fabs(r2-(sn*sn/(aspect*aspect)) )));
		} while (sn<=cs) ;
		cs=0; sn=floor(sqrt(r2)*aspect);
		do {
			pixel(lx+sn,ly+cs);
			pixel(lx-sn,ly+cs);
			if ( cs!=0 ) {
				pixel(lx+sn,ly-cs);
				pixel(lx-sn,ly-cs);
			}
			++cs;
			sn=floor(aspect*sqrt(fabs(r2-cs*cs)));
		} while (cs<sn) ;
	}
}

static void pix(void)
{
	register int xx = *x0+x, yy = *y0+y ;

	if ( writing && (yy<Ydim) ) {
		blit(&what[yy*x8+(xx>>3)],1 << (7-(xx&7)) ) ;
		display3(xx,yy,xx,yy,what);
		modif= 1 ;
	}
}

#define up4() { if ( *y0>0 ) { *y0 -= 4; moved=1; y=3; } }

#define lt4() { if ( *x0>0 ) { *x0 -= 4; moved=1; x=3; } }

#define rt4() { if ( *x0<Xdim-4 ) { *x0 += 4; moved=1; x=0; } }

#define dn4() { if ( *y0<Ydim-4 ) { *y0 += 4; moved=1; y=0; } }

#define up() { if ( y>0 ) --y; else up4(); }

#define dn() { if ( y<3 ) ++y; else dn4(); }

#define lt() { if ( x>0 ) --x; else lt4(); }

#define rt() { if ( x<3 ) ++x; else rt4(); }

static void shiftR(void)
{
	register int i,t ;
	int j ;

	for (j=(Ydim-1)*x8; j>=0; j-=x8) {
		t=what[j+x8-1]&1 ;
		for (i=x8-1; i>0; --i)
			what[j+i] = (((unsigned char)what[j+i]) >> 1) |
				((what[j+i-1]&1)<< 7) ;
		what[j]=(((unsigned char)what[j]) >> 1) | (t << 7) ;
	}
}

static void shiftL(void)
{
	register int i,t ;
	int j ;

	for (j=(Ydim-1)*x8; j>=0; j-=x8) {
		t=what[j] & 0x80 ;
		for (i=0; i<x8-1; ++i)
			what[j+i]=(what[j+i] << 1) | ((what[j+i+1]& 0x80)>> 7) ;
		what[j+x8-1]=(what[j+x8-1] << 1) | (t >> 7) ;
	}
}

static void cursor(void)
{
	square(x*8+bigX+1,y*8+bigY+1,3,3,-1) ;
	square(*x0*dotdim+7,*y0*dotdim+7,dotd4,dotd4,-1) ;
	square((x+*x0)*dotdim+8+dotd0,(y+*y0)*dotdim+8+dotd0,dotd2,dotd2,-1);
}

/* MAIN DELLA PROCEDURA "EDIT" */

static void edit(int *_x0, int *_y0, char *_what)
{
	register int i,j ;
	int ch, locked ;

	dotd4 = dotdim*4+1 ;
	dotd0 = dotdim/3 ;
	dotd2 = (dotdim>3) ? (dotdim-2*dotd0-1) : 0 ;

	x0=_x0; y0=_y0; what = _what ;

	display(what);
	directvideo=0 ;
	square(6,6,3+dotdim*Xdim,3+dotdim*Ydim,1);
	display3all(what);
	gotoxy(25,21); printf("^ busy");
	gotoxy(52,19); printf("CursoreAncora");
	gotoxy(60,21); printf("pixel");
	gotoxy(60,23); printf("block");
	gotoxy(64,1);  printf("JENA - SOFT");
	gotoxy(64,2);  printf("Object editor");
	gotoxy(64,4);  printf("->Per muoverti");
	gotoxy(67,5);  printf("Frecce");
	gotoxy(64,7);  printf("->pi— in fretta");
	gotoxy(67,8);  printf("con lo shift");
	gotoxy(64,10); printf("INS  -> punto");
	gotoxy(64,11); printf("<CR> -> modo ");
	gotoxy(64,12); printf("+/-/* = bool") ;
	gotoxy(64,13); printf("ESC -> fine");
	gotoxy(64,14); printf("L.K.Q.R.C");
	x=0; y=0;
	square(bigX-2,bigY-2,33,33,1);
	square(bigX-18,bigY-18,65,65,1);
	for (i=-2; i<=5; ++i) for (j=-2; j<=5; ++j)
		box(i*8+bigX,j*8+bigY,6,lit(i+*x0,j+*y0,what));
	lx=*x0; ly=*y0;
	locked=writing=0;
	gotoxy(23,23); printf("UP");
	pencolor=-1;
    gotoxy(31,23); printf("XOR");
	memcpy(old,what,size) ;
	do {
		modif = moved = 0 ;
		if(!kbhit()) {
        	if ( locked ) draw(lx*dotdim+8+dotd0+dotd2/2,ly*dotdim+8+dotd0+dotd2/2,
                (x+*x0)*dotdim+8+dotd0+dotd2/2,(y+*y0)*dotdim+8+dotd0+dotd2/2,-1);
            cursor() ;
            gotoxy(50,21); printf("%4d%4d",x+*x0,y+*y0) ;
            gotoxy(50,23); printf("%4d%4d",*x0>>2,*y0>>2) ;
            gotoxy(31,23);
            ch=keyboard();
            if ( locked ) draw(lx*dotdim+8+dotd0+dotd2/2,ly*dotdim+8+dotd0+dotd2/2,
                (x+*x0)*dotdim+8+dotd0+dotd2/2,(y+*y0)*dotdim+8+dotd0+dotd2/2,-1);
            cursor() ;
		}
		else ch=keyboard();
        memcpy(temp,what,size) ;
		switch( ch ) {
			case 'a': lt(); pix(); break ;
			case 'd': rt(); pix(); break ;
			case 'w': up(); pix(); break ;
			case 'x': dn(); pix(); break ;
			case 'q': up(); lt(); pix(); break ;
			case 'e': up(); rt(); pix(); break ;
			case 'z': dn(); lt(); pix(); break ;
			case 'c': dn(); rt(); pix(); break ;
	/*INS*/	case 'v': pixel(*x0+x,*y0+y) ; break ;
	/*s->*/	case '6': rt4(); break ;
	/*s<-*/	case '4': lt4(); break ;
	/*sV*/	case '2': dn4(); break ;
	/*s^*/	case '8': up4(); break ;
			case '7': up4(); lt4(); break ;
			case '9': up4(); rt4(); break ;
			case '1': dn4(); lt4(); break ;
			case '3': dn4(); rt4(); break ;
			case '+': pencolor=1; gotoxy(31,23); printf("OR "); break ;
			case '-': pencolor=0; gotoxy(31,23); printf("NOT"); break ;
			case '*': pencolor=-1; gotoxy(31,23); printf("XOR"); break ;
			case  13: writing= !writing ; gotoxy(23,23);
				if ( writing ) printf("DN"); else printf("UP"); break ;
			case '0': for (i=0; i<4; ++i) for (j=0; j<4; ++j)
				pixel(*x0+i,*y0+j) ; break ;
			case 'g': locked=0; gotoxy(65,21); printf("%8s","");
				gotoxy(65,23); printf("%8s",""); break ;
			case '.': locked=1; lx=x+*x0; ly=y+*y0; gotoxy(65,21);
				printf("%4d%4d",lx,ly);	gotoxy(65,23);
				printf("%4d%4d",lx/4,ly/4); break ;
			case 'L': if ( locked ) segm(lx,ly,*x0+x,*y0+y); break ;
			case 'K': if ( locked ) { segm(lx,ly,*x0+x,*y0+y);
				lx=*x0+x; ly=*y0+y; } break ;
			case 'Q':if ( locked ) for (i=0; i<=abs(*x0+x-lx); ++i)
				for (j=0; j<=abs(*y0+y-ly); ++j)
				pixel(lx+i*sgn(*x0+x-lx),ly+j*sgn(*y0+y-ly));
				break ;
			case 'C':if ( locked ) circl() ; break ;
			case 'R':if ( locked ) {
				segm(lx,ly,x+*x0,ly); segm(x+*x0,ly,x+*x0,y+*y0);
				segm(x+*x0,y+*y0,lx,y+*y0); segm(lx,y+*y0,lx,ly);
				} break ;
			case 8: memcpy(what,old,size) ; modif=1 ;
				setfillstyle(SOLID_FILL,0); bar(8,8,15+Xdim*8,15+Ydim*8);
				display3all(what); break ;
			case '>': shiftR() ; modif=1 ; setfillstyle(SOLID_FILL,0);
				bar(8,8,7+Xdim*dotdim,7+Ydim*dotdim); display3all(what); break;
			case '<': shiftL() ; modif=1 ; setfillstyle(SOLID_FILL,0);
				bar(8,8,7+Xdim*dotdim,7+Ydim*dotdim); display3all(what); break;
		} /* switch */
		if ( modif ) {
			display(what);
			memcpy(old,temp,size) ;
		}
		if ( moved || modif )
			for (i=-2; i<=5; ++i) for (j=-2; j<=5; ++j)
				box((i<<3)+bigX,(j<<3)+bigY,6,lit(i+*x0,j+*y0,what)) ;
	} while (ch!=27 /*ESC*/);
	directvideo=1;
}

static int getyesno(void)
{
	int ch ;
	do {
		ch = getch();
		ch = toupper(ch) ;
	} while ((ch!='S')&&(ch!='N')) ;
	putchar(ch);
	return (ch=='S') ;
}

typedef struct frec {
	char name[13] ;
	struct frec *link ;
	} frec ;
typedef frec * pfrec ;

static char * selectfile(const char * extn)
/* permette all'utente di scegliere un file gi… esistente vedendolo
	nella directory */
{
	char s0[60], s1[60], s2[60] ;
	static char ret [200] ;
	int i,ct,dr ;
	struct ffblk s ;
	pfrec names, p1, tp1,tp2 ;
	int ioresult ;

	dr=0;
	getcwd(s0,60);
	strcpy(s2,curdir) ;
	if (chdir(s2)) strcpy(s2,".") ;
	printf("\nVa bene la directory \"%s\" ? ",s2) ;
	if ( ! getyesno() ) do {
		printf("\nIn quale directory lavori [ d:path | . ] ? -> ") ;
		scanf("%s",s2) ;
	} while (chdir(s2)) ;
	strcpy(curdir,s2) ;
	if ( s2[strlen(s2)-1]!='\\' ) strcat(s2,"\\") ;
	printf("\nLista delle immagini nella directory \"%s\" :\n",s2);
	strcpy(s1,s2) ;
	strcat(s1,"*") ;
	strcat(s1,extn) ;
	ioresult = findfirst( s1, &s, 0 ) ;
	ct=0 ;
	names=NULL ;
	while (!ioresult) {
		++ct ;
		p1 = (pfrec) malloc ( sizeof(frec) ) ;
		if (!p1) {
			printf("\nMANCA MEMORIA!!!") ;
			break ;
		}
		strcpy (p1->name,s.ff_name) ;
		/* ricerca alfabetica */
		tp1=tp2=names ; /* scandisco a doppio puntatore */
		while (tp1 && strcmp(tp1->name,p1->name)<0) tp1 = (tp2=tp1)->link ;
		if (tp1==tp2) {
			p1->link = names ;
			names = p1 ;
		} else {
			p1->link = tp1 ;
			tp2->link = p1 ;
		}
		ioresult = findnext(&s) ;
	}
	if ( !ct ) {
		printf("\nNon ci sono files del tipo giusto in questa directory.") ;
		printf("\nDimmi il nome dell'immagine -> ");
		scanf("%s",s1);
	} else {
		p1=names ;
		for (i=0; i<ct; ++i) {
			printf( "%3d) %-8.*s   ",i+1,(int)(strchr(p1->name,'.')-p1->name),
				p1->name ) ;
			p1=p1->link ;
		}
		printf("\n") ;
		do printf("Scegli un numero ( 0 = nuovo nome ) -> ") ;
			while ( (scanf("%d",&dr)!=1) || (dr<0) || (dr>ct) ) ;
		if ( !dr ) {
			printf("\nDimmi il nome dell'immagine -> ");
			scanf("%s",s1);
		} else {
			p1=names ;
			for (i=1; i<dr; ++i,p1=p1->link) /*NOP*/ ;
			strcpy(s1,p1->name) ;
		}
	}
	while (names) {
		p1=names ;
		names=names->link ;
		free(p1) ;
	}
	strcpy(ret,s2) ;
	strcat(ret,s1) ;
	chdir(s0) ;
	return (ret) ;
}

/* MAIN BODY */


void main(int argc, char**argv)
{
	char * it ;
	const char * ext ;
	int x0,y0 ;
	char filename [80] ;
	int ch ;
	FILE * io ;
	int ctr,ok ;

	/* lettura parametri della linea di comando */
	ctr=1;
	if ( (argc>1) && strchr(argv[1],'*') ) {
		ctr++ ;
		sscanf(argv[1],"%d*%d",&Xdim,&Ydim) ;
		if (!Xdim) Xdim = 1 ;
		if (!Ydim) Ydim = 1 ;
		if (Xdim<0) Xdim = -Xdim ;
		if (Ydim<0) Ydim = -Ydim ;
		x8 = Xdim >> 3 ;
		if ((x8<<3)!=Xdim) {
			x8++ ;
			Xdim = x8<<3 ;
		}
	}
	dotdim = min (512/Xdim,280/Ydim) ;

	if (Xdim==32 && Ydim==28) ext = ".ICN" ;
	else if (Xdim==160 && Ydim==90) ext = ".OGG" ;
	else ext = ".BYT" ;

	/* allocazione buffers */
	size = x8*Ydim ;
	it = calloc(size,1) ;
	old = calloc(size,1) ;
	temp = calloc(size,1) ;
	if (!(it&&old&&temp)) {
		printf("MANCA MEMORIA\n") ;
		exit(100) ;
	}

	getcwd(curdir,80);
	graphscreen();
	clrscr();
	printf("* * * SANCTI GEORGII CASTRUM - Jena Soft 1989 - Gestione immagini oggetti * * *\n");
	do {
		printf("\nElaboro immagini di formato %d*%d, "
			"estensione \"%s\".\n", Xdim,Ydim,ext) ;
		if ( argc<=ctr ) {
			printf("Scegli il nome dell'immagine da elaborare\n" ) ;
			strcpy(filename,selectfile(ext));
		} else {
			strcpy(filename,argv[ctr]);
			printf("Elaboro l'immagine nø %d: \"%s\".\n",ctr,filename);
			++ctr ;
		}
		printf("\n") ;
		if ( !strchr(filename,'.') ) strcat (filename,ext) ;
		printf("Guardo se esiste \"%s\".\n",filename) ;
		io = fopen(filename,"rb");
		if ( io ) {
			printf("Trovato: lo carico in memoria\n");
			fread(it,Ydim,x8,io);
			fclose(io);
		} else {
			printf("NON trovato: creo una nuova immagine bianca ? ") ;
			if ( getyesno() ) clear(it) ;
		}
		printf("\nPremi <ÄÙ per procedere, ESC per lasciar perdere ...\n");
		do ch=getch();
		while ((ch!=13)&&(ch!=27)) ;
		if ( ch==13 ) {
			setgraphmode(ATT400HI);
			x0 = ((Xdim+1)>>3)<<2 ;
			y0 = ((Ydim+1)>>3)<<2 ;
			edit(&x0,&y0,it);
			restorecrtmode();
			clrscr();
			do {
				ok=1 ;
				printf("Devo registrare la nuova versione ? ");
				if ( getyesno() ) {
					printf("\nVa bene se uso \"%s\" ? ",filename);
					if ( !getyesno() ) {
						printf("\nDimmi il nuovo nome da usare\n");
						strcpy(filename,selectfile(ext));
						if ( !strchr(filename,'.') ) strcat(filename,ext);
					}
					io = fopen(filename,"wb") ;
					printf("\nScrivo l""immagine \"%s\" su disco\n",filename);
					if ( !io ) {
						printf("\n* * Ci sono errori: NON riesco a scrivere * *\n");
						ok=0;
					} else {
						fwrite(it,Ydim,x8,io) ;
						fclose(io);
					} /* if !io..*/
				} /* if getyesno()..*/
			} while (!ok) ;
		} /* if ch==13..*/
		printf("\nContinui con un'altra immagine ? ");
	} while (getyesno()) ;
	printf("\n");
	printf("Jena-Soft ti ringrazia per la collaborazione. Bye.\n");
	printf("\n");
}
