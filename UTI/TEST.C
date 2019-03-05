/*
 * test.c
 * prova di utilizzo del mouse
 */

#include <stdio.h>
#include <dos.h>
#include "..\mouse\mouse.h"

int main(void)
{
    int present,buttons ;
    union REGS r ;
    if(!MOUSEinit(&present,&buttons)) {
	printf("Missing mouse driver\n") ;
	return 1 ;
    }
    r.x.ax = 32 ;
    r.x.bx = 0 ;
    /*int86(0x33,&r,&r) ;*/
    MOUSEsetxlim(0,639) ;
    MOUSEsetylim(0,200) ;
    MOUSEsetpos(320,100) ;
    MOUSEsettcur(0, 0xFFFF, 0xFF00) ;
    MOUSEshow() ;
}
