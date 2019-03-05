/*
 * viewmsp.c
 * "visore" di files .MSP (MicroSoft Paint)
 */

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <dos.h>

char name[80] ;
int Badr[400] ;

void main(void)
{
  FILE *in ;
  int gm,gd, x,y ;
  int ch ;
  int head[16] ;

  for (y=0; y<400; Badr[y++]= ((y&3)<<13)+80*(y>>2)) /*NOP*/ ;

  printf("File: ") ;
  gets(name) ;
  in = fopen(name,"rb") ;
  if(!in) {
    printf("Can't open %s\n",name) ;
    exit(1) ;
  }
  read(fileno(in),head,sizeof(head)) ;

  gd = ATT400 ; gm = ATT400HI ;
  initgraph(&gd,&gm,"") ;

  for(y=0; y<400 && y<head[3]; ++y)
    for(x=0; x<(head[2]>>3); ++x)
    {
      ch = getc(in) ;
      if(ch==EOF)
      {
	y = 400 ;
	x = 80 ;
	continue ;
      }
      *(char*)MK_FP(0xB800,Badr[y]+x) = ch ;
    }
  getchar() ;
  restorecrtmode() ;
}
