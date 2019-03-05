/* SOFFITTI.C */

#include <graphics.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <conio.h>

#include "drivers.h"

typedef double matrix[3][3] ;
typedef double vector[3] ;

void mult(matrix a, matrix b, matrix c)
/* c__=a__*b__ */
{
	int i,j,k ;
	double ct ;

	for (i=0; i<=2; ++i)
		for (j=0; j<=2; ++j) {
			ct=0.0;
			for (k=0; k<=2; ++k)
				ct += a[i][k]*b[k][j] ;
			c[i][j]=ct;
		}
} /*mult*/

void mulv(matrix a, vector b, vector c)
/* c_=a__*b_ */
{
	int i,k ;
	double ct ;

	for (i=0; i<=2; ++i) {
		ct=0.0;
		for (k=0; k<=2; ++k)
			ct += a[i][k]*b[k] ;
		c[i]=ct ;
	}
} /* mulv */

double modulo(vector v)
{
	return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]) ;
}

double beta, r00, sxs, sxy, kpro, sx0,sy0 ;
vector v ;
matrix r,p ;

void initvars(double dimx, double dimy, double scale)
{
	double r0 ;
	matrix rz,ry ;

	sx0=dimx;
	sy0=dimy;
	sxs=scale ;
	sxy=0.8 ;
	r00=modulo(v);
	kpro=sxs/(tan(beta)) ;
	r0=hypot(v[0],v[1]) ;

	/* matrice Ry */
	ry[0][0]=r0/r00;	ry[0][1]=0.0;	ry[0][2]=v[2]/r00;
	ry[1][0]=0.0 ;		ry[1][1]=1.0;	ry[1][2]=0.0 ;
	ry[2][0]=-v[2]/r00;	ry[2][1]=0.0;	ry[2][2]=r0/r00;

	/* matrice Rz */
	rz[0][0]=v[0]/r0;	rz[0][1]=v[1]/r0;	rz[0][2]=0.0;
	rz[1][0]=-v[1]/r0;	rz[1][1]=v[0]/r0;	rz[1][2]=0.0;
	rz[2][0]=0.0;		rz[2][1]=0.0;		rz[2][2]=1.0;

	/* matrice R */
	mult(ry,rz, r ) ;
} /*initvars*/

void proietta(vector t, double *x, double *y)
{
	vector p ;
	double d ;

	mulv(r,t, p) ;/* p=r*t */
	d=kpro/(r00-p[0]) ;
	*x=d*p[1] ;
	*y=d*p[2] ;
}

void point(double x, double y, double z, int c )
{
	double xx,yy ;
	vector t ;

	t[0]=x ;
	t[1]=y ;
	t[2]=z ;
	proietta(t, &xx,&yy) ;
	putpixel(floor(xx+sx0),floor(sy0-yy*sxy),c) ;
}

void linea(double x1, double y1, double z1,  double x2, double y2, double z2)
{
	vector t1,t2 ;
	double xx1,xx2,yy1,yy2 ;

	t1[0]=x1; t1[1]=y1; t1[2]=z1 ;
	t2[0]=x2; t2[1]=y2; t2[2]=z2 ;
	proietta(t1,&xx1,&yy1);
	proietta(t2,&xx2,&yy2);
	line(floor(xx1+sx0),floor(sy0-yy1*sxy),floor(xx2+sx0),floor(sy0-yy2*sxy)) ;
}

void assi(double a)
{
	linea(0,0,0, a,0,0);
	linea(0,0,0, 0,a,0);
	linea(0,0,0, 0,0,a);
}

void box(double x0, double y0, double z0, double x, double y, double z)
/* parallelepipedo di centro P0 e 1/2 lati P */
{
	setlinestyle(DASHED_LINE,0,0);
	linea(x0-x,y0-y,z0-z, x0-x,y0-y,z0+z);
	linea(x0-x,y0-y,z0-z, x0-x,y0+y,z0-z);
	linea(x0-x,y0-y,z0-z, x0+x,y0-y,z0-z);
	setlinestyle(SOLID_LINE,0,0);
	linea(x0-x,y0-y,z0+z, x0-x,y0+y,z0+z);
	linea(x0-x,y0-y,z0+z, x0+x,y0-y,z0+z);
	linea(x0-x,y0+y,z0-z, x0+x,y0+y,z0-z);
	linea(x0-x,y0+y,z0-z, x0-x,y0+y,z0+z);
	linea(x0+x,y0-y,z0-z, x0+x,y0+y,z0-z);
	linea(x0+x,y0-y,z0-z, x0+x,y0-y,z0+z);
	linea(x0+x,y0+y,z0-z, x0+x,y0+y,z0+z);
	linea(x0-x,y0+y,z0+z, x0+x,y0+y,z0+z);
	linea(x0+x,y0-y,z0+z, x0+x,y0+y,z0+z);
}

void circlex(double xc, double yc, double zc, double r, double from, double tu)
{
	int i , n=100 ;
	double an ;

	for (i=0; i<=n; ++i) {
		an=from+i*(tu-from)/n ;
		point(xc,yc+r*cos(an),zc+r*sin(an), 0);
	}
}

void circley(double xc, double yc, double zc, double r, double from, double tu)
{
	int i,n ;
	double an ;

	n=floor(200/(2*M_PI)*(tu-from)) ;
	for (i=0; i<=n; ++i) {
		an=from+i*(tu-from)/n ;
		point(xc+r*cos(an),yc,zc+r*sin(an), 0);
	}
}

void circlez(double xc, double yc, double zc, double r, double from, double tu)
{
	int i, n=40 ;
	double an ;

	for (i=0; i<=n; ++i) {
		an=from+i*(tu-from)/n ;
		point(xc+r*cos(an),yc+r*sin(an),zc, 0);
	}
}

void cilindry ( double xc, double y1c,double y2c, double zc,
	double r, double from,double tu)
{
	int i, n=30 ;
	double an,x,z ;

	for (i=0; i<=n; ++i) {
		an=from+i*(tu-from)/n ;
		x=xc+r*cos(an);
		z=zc+r*sin(an);
		linea(x,y1c,z, x,y2c,z);
	}
}

void raggiy(double xc,double yc,double zc, double r, double from,double tu)
{
	int i, n=30 ;
	double an,x,z ;

	for (i=0; i<=n; ++i) {
		an=from+i*(tu-from)/n ;
		x=xc+r*cos(an);
		z=zc+r*sin(an);
		linea(xc,yc,zc, x,yc,z);
	}
}

void cilindrx(double x1c,double x2c,double yc,double zc,
	double r, double from,double tu)
{
	int i, n=30 ;
	double an,y,z ;

	for (i=0; i<=n; ++i) {
		an=from+i*(tu-from)/n ;
		y=yc+r*cos(an);
		z=zc+r*sin(an);
		linea(x1c,y,z, x2c,y,z);
	}
}

void cilindrz(double xc,double yc,double z1c,double z2c,
	double r, double from,double tu)
{
	int i, n=10 ;
	double an,x,y ;

	for (i=0; i<=n; ++i) {
		an=from+i*(tu-from)/n ;
		x=xc+r*cos(an);
		y=yc+r*sin(an);
		linea(x,y,z1c, x,y,z2c);
	}
}

void raggix(double xc,double yc,double zc, double r, double from,double tu )
{
	int i, n=30 ;
	double an,y,z ;

	for (i=0; i<=n; ++i) {
		an=from+i*(tu-from)/n ;
		y=yc+r*cos(an);
		z=zc+r*sin(an);
		linea(xc,yc,zc, xc,y,z);
	}
}

void fill(double x,double y,double z, int style)
{
	double xx,yy ;
	vector t ;

	t[0]=x; t[1]=y; t[2]=z;
	proietta(t, &xx,&yy);
	setfillpattern(Fills[style],0);
	setfillstyle(USER_FILL,0);
	floodfill(floor(sx0+xx),floor(sy0-yy*sxy),0);
}

void soffitto(int n)
{
	int i,j,k ;
	double a,b,x,y,z,x1,y1 ;
	char *s ;

	switch(n) {
		case 0:/* piatto */
			box(0,0,0, 3,4,2);
			s="soffitto piatto" ;
			fill(0,0,2, grigio);
			fill(3,0,0, G12);
			fill(0,4,0, G8);
			break ;
		case 1:/* botte */
			box(0,0,-1, 3,4,2);
			fill(3,0,-1, G12);
			fill(0,4,-1, G8);
			cilindry(0, 4,-4, 1, 3, 0,M_PI);
			raggiy(0,4,1, 3, 0,M_PI);
			circley(0,4,1, 3, 0,M_PI);
			circley(0,-4,1, 3, 0,M_PI);
			s="soffitto a botte";
			break ;
		case 11:/* cassettoni */
			box(0,0,0, 3,4,2);
			fill(3,0,0, G12);
			fill(0,4,0, G8);
			for (i=0; i<=3; ++i) linea(-3+6*(i+1)/5,4,2, -3+6*(i+1)/5,-4,2);
			for (i=0; i<=2; ++i) linea(3,-4+8*(i+1)/4,2, -3,-4+8*(i+1)/4,2);
			s="stanza a cassettoni";
			break;
		case 3:/* butala */
			box(0,0,-1.2, 3,4,0.2);
			cilindrx(3,-3,0,-1, 4, 0,M_PI);
			raggix(3,0,-1, 4, 0,M_PI);
			circlex(-3,0,-1, 4, 0,M_PI);
			circlex(3,0,-1, 4, 0,M_PI);
			s="stanza a butala";
			break;
		case 5:/* chiocciola */
			a=0; b=-3; x=3; y=0; z=b;
			linea(0,0,b, x,y,b);
			for (i=0; i<=60; ++i) {
				a+=M_PI/8;
				b+=0.1 ;
				x1=3*cos(a); y1=3*sin(a);
				linea(x,y,z, x1,y1,b);
				linea(0,0,b, x1,y1,b);
				z=b; x=x1; y=y1;
			}
			s="scala a chiocciola";
			break ;
		case 2:/* crociera */
			box(0,0,-1, 3,4,2);
			fill(3,0,-1, G12);
			fill(0,4,-1, G8);
			for (i=0; i<=20; ++i) {
				z=1+3.0*i/20.0;
				x=sqrt(9-(z-1)*(z-1));
				y=4*sqrt(1-(z-1)*(z-1)/9.0);
				linea(x,y,z, x,-y,z);linea(x,y,z, x,y,z-0.3);
				if (i==19) linea(x,-y,z, -x,-y,z);
				linea(x,-y,z, x,-y,z-0.3);
				if (i==19) linea(-x,-y,z, -x,y,z);
				linea(-x,y,z, x,y,z);linea(-x,y,z, -x,y,z-0.3);
			}
			s="soffitto a crociera";
			break;
		case 4:/* cielo */
			for (i=0; i<=15; ++i) {
				x=80+65*cos(i);
				y=45+30*sin(i/3);
				a=rand()/32767.0*4+3;
				b=a*2+rand()/32767.0*3;
				x1=rand()/32767.0;
				for (j=0; j<=4; ++j) {
					z=j*0.4*M_PI+x1;
					line(floor(x+a*cos(z)),floor(y+a*sin(z)),
						floor(x+b*cos(z+M_PI/5)),floor(y+b*sin(z+M_PI/5)));
					line(floor(x+a*cos(z)),floor(y+a*sin(z)),
						floor(x+b*cos(z-M_PI/5)),floor(y+b*sin(z-M_PI/5)));
				}
				setfillpattern(Fills[random(17)],0);
				setfillstyle(USER_FILL,0);
				floodfill(floor(x),floor(y),0);
			}
			s="a cielo aperto";
			break;
		case 7:/* stroc */
			/* base */
			setlinestyle(SOLID_LINE,0,0);
			linea(-3,4,-2, 3,4,-2);
			linea(3,4,-2, 3,-4,-2);
			setlinestyle(DASHED_LINE,0,0);
			linea(-3,-4,-2, -3,4,-2);
			linea(3,-4,-2, -3,-4,-2);
			/* montanti */
			linea(-3,-4,-2, -3,-4,3);
			setlinestyle(SOLID_LINE,0,0);
			linea(-3,4,-2, -3,4,3.5);
			linea(3,4,-2, 3,4,2);
			linea(3,-4,-2, 3,-4,1);
			/* soffitto */
			linea(-3,-4,3, -3,4,3.5);
			linea(-3,4,3.5, 3,4,2);
			linea(3,4,2, 3,-4,1);
			linea(3,-4,1, -3,-4,3);
			fill(3,0,-1, G12);
			fill(0,4,-1, G8);
			fill(0,0,2, grigio);
			s="soffitto stroc";
			break;
		case 8:/* querc */
			linea(3,4,1, 0,3,4);
			linea(-3,4,1, 0,3,4);
			linea(3,-4,1, 0,-3,4);
			linea(0,3,4, 0,-3,4);
			linea(3,4,1, -3,4,1);
			linea(3,4,1, 3,-4,1);
			fill(0,3,2, G6);
			fill(3,0,2, grigio);
			box(0,0,-1, 3,4,2);
			linea(-3,-4,1, 0,-3,4);
			fill(3,0,-1, G12);
			fill(0,4,-1, G8);
			s="soffitto a querc";
			break;
		case 6:/* sciap… */
			v[0]=5; v[1]=15; v[2]=10;
			initvars(79,40,6);
			box(0,0,0, 3,4,2);
			fill(3,0,0, G8);
			fill(0,4,0, G12);
			linea(-3,4,2, 0,4,1);
			for (i=-2; i<=1; ++i) {
				linea(0,i*2,1, -1,i*2+1,1);
				linea(-1,i*2+1,1, 0,i*2+2,1);
			}
			linea(0,-4,1, -3,-4,2);
			fill(-2,0,2, nero);
			linea(3,4,2, 1,4,1);
			for (i=-2; i<=1; ++i) {
				linea(1,i*2,1, 0,i*2+1,1);
				linea(0,i*2+1,1, 1,i*2+2,1);
			}
			linea(1,-4,1, 3,-4,2);
			fill(2.5,0,2, nero);
			s="soffitto sciap…";
			break;
		case 10:/* caverna */
			cilindrx(4,-3, 0, -1, 4, 0,M_PI);
			circlex(4,0,-1, 4, 0,M_PI);
			for (i=0; i<=30; ++i) {
				a=i*M_PI/30 ;
				y=4*cos(a);
				z=4*sin(a);
				circley(-3,y,-1, z, M_PI/2,M_PI);
			}
			linea(4,-4,-1, 4,4,-1);
			s="caverna";
			break ;
		case 9:/* foresta */
			v[0]=10; v[1]=25; v[2]=10;
			initvars(76,46,10);
			for (i=-1; i<=1; ++i)
				for (j=-1; j<=1; ++j) {
					x=i*4.2-1;
					y=j*6-3.5;
					cilindrz(x,y,-2,1, 0.3, 0,2*M_PI);
					circlez(x,y,-2, 0.3, 0,2*M_PI);
					for (k=-10; k<=10; ++k) {
						z=1.0+k/5.0;
						a=0.7*(3.0-z)/2.0;
						circlez(x,y,z, a, 0,2*M_PI);
					}
				}
			s="foresta";
			break;
		default: s="? ? ?" ;
	}
	settextjustify(CENTER_TEXT,TOP_TEXT);
	outtextxy(79,91,s);
}

/* MAIN BODY */

typedef char row[20] ;
typedef row ogg[90] ;
typedef ogg soff[12] ;

soff sof ;

void main(void)
{
	int i,j ;
    int xi,yi ;
	FILE * out ;

	initgraphscreen(0) ;
	beta=M_PI*0.01 ;
	cleardevice();
	setcolor(0);
	setfillstyle(SOLID_FILL,1);
	bar(0,0,639,399);
	for (i=0; i<=11; ++i) {
		xi=(i%4)*160 ;
		yi=(i/4)*120 ;
		setviewport(xi,yi, xi+159,yi+99, 1);
		v[0]=15; v[1]=25; v[2]=10;
		initvars(79, 44, 10);
		soffitto(i);
		for(j=0; j<=89; ++j) memmove(sof[i][j],MK_FP(0xB800,Badr[yi+j]+(xi/8)),
			sizeof(row) ) ;
	}
	out = fopen("soffitti.byt","wb") ;
	fwrite(sof,sizeof(ogg),12,out);
	fclose(out);
	getch() ;
	restorecrtmode() ;
}
