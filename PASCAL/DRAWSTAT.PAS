program DrawStatus ;

uses drivers, globals, graph ;

{ Barrette, cotillons e oggetti della finestra di Status }

const
     sx0 = 420 ;   sy0 = 0 ;   { TL finestra }
     dx0 = 219 ;   dy0 = 299 ; { dim finestra }

     sx1 = 8 ;    sy1 = 30 ;   { indicatori }
     dx1 = 203 ;  dy1 = 94 ;

     sx2 = 8 ;    sy2 = 128 ;  { figurine }
     dx2 = 203 ;  dy2 = 50 ;
     dst = 40 ;   dsy = 32 ;
                  d2y = 8 ;

     sx3 = 8 ;    sy3 = 182 ;  { inventari }
     dx3 = 203 ;  dy3 = 114 ;

     titl:array[0..3] of string[15] = ( 'POSIZIONE', 'VELOCITA`',
                                        'STAMPANTE', 'FISICO' ) ;

procedure bord (x,y, lx,ly, r :integer) ;
{ rettangolo con angoli smussati }
begin
     line(x+r,y,x+lx-r,y) ;
     line(x+r,y+ly,x+lx-r,y+ly) ;
     line(x,y+r,x,y+ly-r) ;
     line(x+lx,y+r,x+lx,y+ly-r) ;
     ellipse(x+r,y+r,90,180,r,r) ;
     ellipse(x+r,y+ly-r,180,270,r,r) ;
     ellipse(x+lx-r,y+ly-r,270,0,r,r) ;
     ellipse(x+lx-r,y+r,0,90,r,r) ;
end ; { bord }

procedure Arrow(x,y,ln:integer);
begin
     line(x,y-4,x,y);
     line(x,y,x+ln,y);
     line(x+ln,y,x+ln-4,y-4);
     line(x+ln,y,x+ln-4,y+4);
end ;

procedure borders ;
{ disegna il background dello status. }
var i,j,w,z:integer ;
    q:t_barrette ;
const s='Infurmasiun' ;
begin
     SetViewPort(sx0,sy0,sx0+dx0,sy0+dy0,ClipOn) ;
     SetFillStyle(SolidFill,1) ;
     SetColor(1) ;
     SetLineStyle(SolidLn,0,NormWidth) ;
     bord(0,0,dx0,dy0,16) ;
     floodfill(dx0 div 2, dy0 div 2, 1) ;
     SetColor(0) ;
     { indicatori }
     bord(sx1-1,sy1,dx1+2,dy1,8) ;
     bord(sx1+4,sy1+4,dx1-8,dy1-8,4) ;
     { figurine }
     bord(sx2-1,sy2,dx2+2,dy2,8) ;
     SetTextJustify(CenterText,CenterText);
     SetUserCharSize(5,6,3,4);
     SelectFont(2,0);
     SetfillPattern(fills[nero],1);
     SetFillstyle(userfill,1) ;
     setcolor(0);
     bord(sx2+2,sy2+6+dsy,dx2-5,d2y,3);
     for i:=0 to 3 do begin
         w:=sx2+7+i*(dst+10) ;
         bord(w,sy2+4,dst-1,dsy,4) ;
         OutTextXY(w+dst div 2-1, sy2+6+dsy+d2y div 2-1,
                  titl[i] ) ;
     end ;
     setcolor(0);
     { inventari }
     bord(sx3-1,sy3,dx3+2,dy3,8) ;
     bord(sx3+4,sy3+4,40,36,4);
     bord(sx3+4,sy3+dy3 div 2 +2,40,36,4);
     bord(sx3+4+45,sy3+4,dx3-8-45,dy3 div 2 -6,4) ;
     bord(sx3+4+45,sy3+ dy3 div 2+2, dx3-8-45, dy3 div 2 -6, 4) ;
     { campiture }
     SetFillPattern(fills[grigio],1) ;
     SetFillStyle(UserFill,1) ;
     floodfill(16,16,0) ;
     setcolor(1) ;
     bord(0,0,dx0,dy0,16) ;
     SetFillPattern(fills[g12],1);
     floodfill(sx1+1,sy1+dy1 div 2,0) ;
     floodfill(sx2+1,sy2+dy2 div 2,0) ;
     floodfill(sx2+dx2 div 2,sy2+dy2-1,0);
     floodfill(sx3+1,sy3+dy3 div 2,0) ;
     { frecce }
     setcolor(0);
     Setlinestyle(solidln,0,3);
     arrow(sx3+4+4,sy3+4+36+5,39);
     arrow(sx3+4+4,sy3+dy3 div 2+2+36+5,39);
     { titolo }
     SelectFont(GothicFont,4) ;
     SetTextJustify(CenterText,TopText) ;
     setcolor(1) ; { fondo bianco }
     for i:=-1 to 1 do for j := -1 to 1 do OutTextXY(dx0 div 2+i,-3+j,s) ;
     setcolor(0) ; { campo nero }
     OutTextXY(dx0 div 2,-3,s) ;
     SetViewPort(sx0+sx1+8,sy0+sy1+8,sx0+sx1+dx1-8,sy0+sy1+dy1-6,ClipOn) ;
     SetColor(0) ;
     SetLineStyle(SolidLn,0,NormWidth) ;
     SetUserCharSize(2,3,2,5);
     SetTextJustify(RightText,CenterText) ;
     SelectFont(8,0) ;
     z:=1 ;
     for q:=punti to antipatia do begin
         OutTextXY(86,z,n_barrette[q]) ;
         bord(89,z-1,54,8,2) ;
         inc(z,10) ;
     end ;
end ;   { borders }

var f:file ;
    sz:word ;
    pt:pointer ;

begin
     InitGraphScreen ;
     borders ;
     SetViewPort(sx0,sy0,sx0+dx0,sy0+dy0,clipon);
     sz:=ImageSize(0,0, dx0,dy0) ;
     GetMem(pt, sz);
     GetImage(0,0, dx0,dy0, pt^) ;
     SetViewPort(0,0,GetMaxX,GetMaxY,ClipOn);
     PutImage(0,0, pt^, normalPUT);
     assign(f,'status.byt') ;
     rewrite(f,1);
     BlockWrite(f,pt^,sz);
     close(f);
end.