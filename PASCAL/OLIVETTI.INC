{
Turbo Pascal 3D-Power
Questo Š il file "Olivetti.inc"
Da includere PER PRIMO

ROUTINES 'Screen', 'Plot' e 'Draw' PER OLIVETTI M24
Permette di abilitare lo schermo 640x400.

}



TYPE Reg=RECORD CASE integer OF
             1:(ax,bx,cx,dx,bp,si,di,ds,es,flgs: integer);
             2:(al,ah,bl,bh,cl,ch,dl,dh: byte);
           END;
VAR Scrx,Scry:integer;
CONST MaxScreen=3;

PROCEDURE Screen(mode:integer);
  VAR regs:Reg;
  BEGIN
    WITH Regs DO BEGIN
         CASE ABS(mode) OF
              0:begin
                     ax:=$0003;
                     Scrx:=-1;Scry:=-1
                end;
              1:begin
                     ax:=$0004;
                     Scrx:=319;Scry:=199
                end;
              2:begin
                     ax:=$0006;
                     Scrx:=639;Scry:=199
                end;
              3..MAXINT:begin
                     ax:=$0040;
                     Scrx:=639;Scry:=399;
                end;
         END;{case}
         Intr($10,regs);
         TextColor(15);
    END;
  END;

PROCEDURE Plot(x,y,c:integer);
{disegna un punto in 640x400 indirizzando la memoria video
Per 'c' si ha: 0-cancella, 1-disegna, (-1)-inverte}
VAR regs:Reg;
BEGIN
     IF (x>=0) and (x<=Scrx) and (y>=0) and (y<=Scry) THEN WITH REGS DO BEGIN
        ah:=$0C;
        al:=c;
        dx:=y; cx:=x;
        intr($10,regs)
     END;
END;

PROCEDURE Draw(x1,y1,x2,y2,c:integer);
{segmento in 640x400. L'algoritmo Š di TurboGraphixToolBox}
    VAR x,y,DeltaX,DeltaY,XStep,YStep,count:integer;
BEGIN
      x:=x1; y:=y1; XStep:=1; YStep:=1;
      IF x1>x2 THEN XStep:=-1; IF y1>y2 THEN YStep:=-1;
      DeltaX:=abs(x2-x1); DeltaY:=abs(y2-y1);
      IF DeltaX=0 THEN count:=-1 ELSE count:=0;
      WHILE not ((x=x2) and (y=y2)) DO BEGIN
        Plot(x,y,c);
        IF count<0
           THEN   BEGIN  y:=y+YStep; count:=count+DeltaX;  END
           ELSE   BEGIN  x:=x+XStep; count:=count-DeltaY;  END;
      END;
END;
