/*
 * mouse.c
 * C interface for Logitech Mouse
 */

#include <dos.h>

#include "mouse.h"

#define MOUSE_INT 0x33

int  MOUSEinit(int *status, int *buttons)   /* Mouse Initialization - Funct.0 */
{
    union REGS r ;
    *status = *buttons = 0 ;
    r.x.ax = 0 ;
    int86(MOUSE_INT,&r,&r) ;
    *buttons = r.x.bx ;
    return *status = r.x.ax ;
}

void MOUSEshow(void)			    /* Show Cursor - Function 1 */
{
    union REGS r ;
    r.x.ax = 1 ;
    int86(MOUSE_INT,&r,&r) ;
}

void MOUSEhide(void)			    /* Hide Cursor - Function 2 */
{
    union REGS r ;
    r.x.ax = 2 ;
    int86(MOUSE_INT,&r,&r) ;
}

void MOUSEgetposbut(int *buttons, int *horiz, int *vert)
			/*Get Mouse Position & Button Status - Function 3 */
{
    union REGS r ;
    r.x.ax = 3 ;
    int86(MOUSE_INT,&r,&r) ;
    *buttons = r.x.bx ;
    *horiz = r.x.cx ;
    *vert = r.x.dx ;
}

void MOUSEsetpos(int horiz, int vert)
			/* Set Mouse Cursor Position - Function 4 */
{
    union REGS r ;
    r.x.ax = 4 ;
    r.x.bx = horiz ;
    r.x.cx = vert ;
    int86(MOUSE_INT,&r,&r) ;
}

int  MOUSEgetbutpres(int whichbutton, int *butstat, int *presscount,
	int *horiz, int *vert)
			/* Get Button Press Information - Function 5 */
{
    union REGS r ;
    r.x.ax = 5 ;
    r.x.bx = whichbutton ;
    int86(MOUSE_INT,&r,&r) ;
    *butstat = r.x.ax ;
    *horiz = r.x.cx ;
    *vert = r.x.dx ;
    return *presscount = r.x.bx ;
}

int  MOUSEgetbutrel(int whichbutton, int *butstat, int *presscount,
	int *horiz, int *vert)
			/* Get Button Release Information - Function 6 */
{
    union REGS r ;
    r.x.ax = 6 ;
    r.x.bx = whichbutton ;
    int86(MOUSE_INT,&r,&r) ;
    *butstat = r.x.ax ;
    *horiz = r.x.cx ;
    *vert = r.x.dx ;
    return *presscount = r.x.bx ;
}

void MOUSEsetxlim(int xmin, int xmax)
			/* Set Minimum & Maximum X Position - Function 7 */
{
    union REGS r ;
    r.x.ax = 7 ;
    r.x.cx = xmin ;
    r.x.dx = xmax ;
    int86(MOUSE_INT,&r,&r) ;
}

void MOUSEsetylim(int ymin, int ymax)
			/* Set Minimum & Maximum Y Position - Function 8 */
{
    union REGS r ;
    r.x.ax = 8 ;
    r.x.cx = ymin ;
    r.x.dx = ymax ;
    int86(MOUSE_INT,&r,&r) ;
}

void MOUSEsetgcur(struct MOUSEgcursor * gcursor)
			/* Define Graphics Cursor Block - Function 9 */
{
    union REGS r ;
    struct SREGS s ;
    void far * p ;

    r.x.ax = 9 ;

    p = (void far *) gcursor ;
    segread(&s) ;
    s.es = FP_SEG(p) ;
    r.x.dx = FP_OFF(p) ;

    r.x.bx = gcursor->hotx ;
    r.x.cx = gcursor->hoty ;

    int86x(MOUSE_INT, &r,&r,&s) ;
}

void MOUSEsettcur(int type, unsigned screen /*start*/, unsigned curs /*stop*/)
			/* Define Text Cursor - Function 10 */
{
    union REGS r ;
    r.x.ax = 10 ;
    r.x.bx = type ;
    r.x.cx = screen ;
    r.x.dx = curs ;
    int86(MOUSE_INT,&r,&r) ;
}

void MOUSEreadcnt(int *horiz, int *vert)
			/* Read Mouse Motion Counters - Function 11 */
{
    union REGS r ;
    r.x.ax = 11 ;
    int86(MOUSE_INT,&r,&r) ;
    *horiz = r.x.cx ;
    *vert = r.x.dx ;
}

void MOUSEsethandler(int mask, void interrupt (far *handler)() )
			/* Define Event Handler - Function 12 */
{
    union REGS r ;
    struct SREGS s ;

    r.x.ax = 12 ;
    segread(&s) ;
    r.x.cx = mask ;

    s.es = FP_SEG(handler) ;
    r.x.dx = FP_OFF(handler) ;

    int86x(MOUSE_INT,&r,&r,&s) ;
}

void MOUSElightpenon(void)
			/* Light Pen Emulation Mode On - Function 13 */
{
    union REGS r ;
    r.x.ax = 13 ;
    int86(MOUSE_INT,&r,&r) ;
}

void MOUSElightpenoff(int horiz, int vert)
			/* Light Pen Emulation Mode Off - Function 14 */
{
    union REGS r ;
    r.x.ax = 14 ;
    r.x.cx = horiz ;
    r.x.dx = vert ;
    int86(MOUSE_INT,&r,&r) ;
}

void MOUSEsetpixratio(int horiz, int vert)
			/* Set Mouse Motion/Pixel Ratio - Function 15 */
{
    union REGS r ;
    r.x.ax = 15 ;
    r.x.cx = horiz ;
    r.x.dx = vert ;
    int86(MOUSE_INT,&r,&r) ;
}

void MOUSEareahide(int left, int top, int rigth, int bottom)
			/* Conditional Hide Cursor - Function 16 */
{
    union REGS r ;
    r.x.ax = 16 ;
    r.x.cx = left ;
    r.x.dx = top ;
    r.x.si = rigth ;
    r.x.di = bottom ;
    int86(MOUSE_INT,&r,&r) ;
}

void MOUSEsetthreshold(int threshold)
			/* Set Speed Threshold - Function 19 */
{
    union REGS r ;
    r.x.ax = 19 ;
    r.x.dx = threshold ;
    int86(MOUSE_INT,&r,&r) ;
}
