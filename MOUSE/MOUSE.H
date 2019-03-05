/*
 * mouse.h
 * definitions & prototypes for Logitech Mouse
 */

struct MOUSEgcursor {
    unsigned screen[16] ;
    unsigned cursor[16] ;
    int hotx, hoty ;
} ;

/* BASE functions - Microsoft compatible */

int  MOUSEinit(int *status, int *buttons) ;
			/* Mouse Initialization - Function 0 */
void MOUSEshow(void) ;	/* Show Cursor - Function 1 */
void MOUSEhide(void) ;	/* Hide Cursor - Function 2 */
void MOUSEgetposbut(int *buttons, int *horiz, int *vert) ;
			/*Get Mouse Position & Button Status - Function 3 */
void MOUSEsetpos(int horiz, int vert) ;
			/* Set Mouse Cursor Position - Function 4 */
int  MOUSEgetbutpres(int whichbutton, int *butstat, int *presscount,
	int *horiz, int *vert) ;
			/* Get Button Press Information - Function 5 */
int  MOUSEgetbutrel(int whichbutton, int *butstat, int *presscount,
	int *horiz, int *vert) ;
			/* Get Button Release Information - Function 6 */
void MOUSEsetxlim(int xmin, int xmax) ;
			/* Set Minimum & Maximum X Position - Function 7 */
void MOUSEsetylim(int ymin, int ymax) ;
			/* Set Minimum & Maximum Y Position - Function 8 */
void MOUSEsetgcur(struct MOUSEgcursor * gcursor) ;
			/* Define Graphics Cursor Block - Function 9 */
void MOUSEsettcur(int type, unsigned screen /*start*/, unsigned curs /*stop*/) ;
			/* Define Text Cursor - Function 10 */
void MOUSEreadcnt(int *horiz, int *vert) ;
			/* Read Mouse Motion Counters - Function 11 */
void MOUSEsethandler(int mask, void interrupt (far *handler)() ) ;
			/* Define Event Handler - Function 12 */
void MOUSElightpenon(void) ;
			/* Light Pen Emulation Mode On - Function 13 */
void MOUSElightpenoff(int horiz, int vert) ;
			/* Light Pen Emulation Mode Off - Function 14 */
void MOUSEsetpixratio(int horiz, int vert) ;
			/* Set Mouse Motion/Pixel Ratio - Function 15 */
void MOUSEareahide(int left, int top, int rigth, int bottom) ;
			/* Conditional Hide Cursor - Function 16 */
void MOUSEsetthreshold(int threshold) ;
			/* Set Speed Threshold - Function 19 */

/* EXTENDED functions - Logitech specific */


