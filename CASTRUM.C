/* Sancti Georgii Castrum */
/* Avventura nel castello di San Giorgio Canavese - Jena Soft 1988 */

#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>

#include "set.h"
#include "drivers.h"
#include "globals.h"
#include "musica.h"
#include "funckeys.h"
#include "agisci.h"
#include "status.h"
#include "room.h"

void main(int argc, char **argv)
{
    int az ;
    int o1,o2 ;
    int again=1 ;

    t_barrette i ;
    FILE * mus ;
    void * pt ;

    loadvariables() ;
    initeverything(argc,argv) ;
    mus = fopen("ballo.mca","rb") ;
    pt = malloc((int)filelength(fileno(mus)));
    fread(pt,1,(int)filelength(fileno(mus)),mus);
    fclose(mus) ;
    /*play(pt) ;*/
    for (i=punti; i<=antipatia; ++i)
	stato.barrette[i] = (double)rand()*100.0/32767.0 ;
    stato.dovesei = 1 ;
    buildobjtables(3) ;
    outstatus() ;
    outsoleluna() ;
    outwatch() ;
    azione(-11,0,0) ;
    while(again) {
	updatetempo(10) ;
	outsoleluna() ;
	outwatch() ;
	menuselect(&az,&o1,&o2) ;
	if (az) azione(az,o1,o2);
	for (i=punti; i<=antipatia; ++i) {
	    stato.barrette[i]+=rand()/32767.0-0.4 ;
	    if (stato.barrette[i]>100.0) stato.barrette[i]=10+random(10) ;
	    else if (stato.barrette[i]<0.0) stato.barrette[i]=random(100) ;
	}
	outstatus() ;
	if (az==-128) again=0 ;
    }
    closegraph() ;
} /* prog "castrum.exe" */
