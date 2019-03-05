/* pallone da calcio nella pagina video 'styx' */

#include "styx.h"
#include <stdio.h>
#include <math.h>
#include <conio.h>

void main(void)
{
	int colors[] = {13,6,10,3,6,3,10,3,6,13,3,10} ,
		d[] = {160,100},i,k,
		palette[] = { 0,  8,7,14,  6,2,10,  0,  4,12,13,  9,3,11,  14,15 },
		x_min,x_max,y_min,y_max ;
	unsigned short random=0 ;
	float a,b,c,l0,l1,l2,LN,ln1,n0,n1,n2,p,q,r=45,s,v[12][3] ;
	double t ;
	register int x,y ;

	/* enter graphic mode */
	entergraph() ;
	cls(0) ;

	/* pixel aspect ratio */
	a=1.25 ;
	/* screen center coords */
	b = (d[0]-1)/2 ;
	c = (d[1]-1)/2 ;
	/* unit lenght light source vector */
	l0 = -1/sqrt(3.0) ;
	l1 = l0 ;
	l2 = -l0 ;
	/* calculate vertices on the sphere */
	v[0][0] = 0 ;
	v[0][1] = 0 ;
	v[0][2] = 1 ;
	s = sqrt(5.0) ;
	for ( i=1; i<11; i++) {
		p = M_PI*i/5 ;
		v[i][0] = 2*cos(p)/s ;
		v[i][1] = 2*sin(p)/s ;
		v[i][2] = (1.0 - i%2*2 ) / s ;
	}
	v[11][0] = 0 ;
	v[11][1] = 0 ;
	v[11][2] = -1 ;

	/* Loop to Phong shade each pixel */
	y_max = c+r ;
	y_min = 2*c-y_max ;
	for (y=y_min; y<=y_max; y++) {
		s=y-c ;
		n1=s/r ;
		ln1=l1*n1 ;
		s=r*r-s*s ;
		x_max = b+a*sqrt(s) ;
		x_min = 2*b-x_max ;
		for (x=x_min; x<=x_max; x++) {
			t=(x-b)/a ;
			n0=t/r ;
			t = sqrt(s-t*t) ;
			n2 = t/r ;
			/* dot product & get pos. value */
			LN = l0*n0+ln1+l2*n2 ;
			if (LN<0) LN=0 ;
			/* cos(e.r)**27 */
			t=LN*n2 ;
			t += t-l2 ;
			t *= t*t ;
			t *= t*t ;
			t *= t*t ;
			/* nearest vertex to normal has max dot product: get color */
			for (i=0, p=0; i<11; i++ )
				if (p<(q=n0*v[i][0]+n1*v[i][1]+n2*v[i][2])) {
					p=q ;
					k=colors[i] ;
				}
			/* aggregate ambient, diffuse, intensities, dither */
			i = k-2.5+2.5*LN+t+(random=37*random+1)/65536.0 ;
			/* restrict to 3-colors range or force to b/w */
			if (i<k-2) i=0 ;
			else if (i>k) i=15 ;
			plot(x,y,palette[i]);
		}
	if (kbhit()) break ;
	}
	getch() ;
	exitgraph() ;
}
