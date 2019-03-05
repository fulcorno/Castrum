{$N+} {use 8087 math}
program SOFFITTI ;

uses graph, crt, drivers ;

type num = single ;

type matrix=array[0..2,0..2] of num ;
     vector=array[0..2] of num ;

procedure mult(var a,b:matrix; var c:matrix) ;
{ c__=a__*b__ }
var i,j,k:byte ;
    ct:num ;
begin
     for i:=0 to 2 do
         for j:=0 to 2 do begin
             ct:=0;
             for k :=0 to 2 do
                 ct:=ct+a[i,k]*b[k,j] ;
             c[i,j]:=ct;
     end ;
end; {mult}

procedure mulv(var a:matrix; var b:vector; var c:vector) ;
{ c_=a__*b_ }
var i,k:byte ;
    ct:num ;
begin
     for i:=0 to 2 do begin
         ct:=0;
         for k:=0 to 2 do
             ct:=ct+a[i,k]*b[k] ;
         c[i]:=ct ;
     end ;
end ;

function modulo(var v:vector):num ;
begin
     modulo := sqrt(sqr(v[0])+sqr(v[1])+sqr(v[2])) ;
end ;

var beta, r00, sxs, sxy, kpro, sx0,sy0 : num ;
    v:vector ;
    r,p:matrix ;

const null:matrix=((0,0,0),(0,0,0),(0,0,0)) ;

procedure InitVars(dimx,dimy,scale:num) ;
var r0:num ;
    Rz,Ry:matrix ;
begin
     sx0:=dimx;
     sy0:=dimy;
     sxs:=scale ;
     sxy:=0.8 ;
     r00:=modulo(v);
     kpro:=sxs/(sin(beta)/cos(beta)) ;
     r0:=sqrt(sqr(v[0])+sqr(v[1])) ;
     Ry:=null ;
     Rz:=null ;
     { matrice Ry }
     ry[0,0]:=r0/r00;                          ry[0,2]:=v[2]/r00;
                           ry[1,1]:=1.0;
     ry[2,0]:=-v[2]/r00;                       ry[2,2]:=r0/r00;
     { matrice Rz }
     rz[0,0]:=v[0]/r0;     rz[0,1]:=v[1]/r0;
     rz[1,0]:=-v[1]/r0;    rz[1,1]:=v[0]/r0;
                                               rz[2,2]:=1.0;
     { matrice R }
     mult(ry,rz, R ) ;
end ; {InitVars}

procedure Proietta(var t:vector; var x,y:num) ;
var p:vector ;
    d:num ;
begin
     mulv(r,t, p ) ;  { p:=r*v }
     d:=kpro/(r00-p[0]) ;
     x:=d*p[1] ;
     y:=d*p[2] ;
end ;

procedure point(x,y,z:num; c:word ) ;
var xx,yy:num ;
    t:vector ;
begin
     t[0]:=x ;
     t[1]:=y ;
     t[2]:=z ;
     proietta(t, xx,yy) ;
     PutPixel(trunc(xx+sx0),round(sy0-yy*sxy),c) ;
end ;

procedure linea(x1,y1,z1, x2,y2,z2:num) ;
var t1,t2:vector ;
    xx1,xx2,yy1,yy2:num ;
begin
     t1[0]:=x1; t1[1]:=y1; t1[2]:=z1 ;
     t2[0]:=x2; t2[1]:=y2; t2[2]:=z2 ;
     proietta(t1,xx1,yy1);
     proietta(t2,xx2,yy2);
     line(trunc(xx1+sx0),trunc(sy0-yy1*sxy),trunc(xx2+sx0),trunc(sy0-yy2*sxy)) ;
end ;

procedure assi(a:num) ;
begin
     linea(0,0,0, a,0,0);
     linea(0,0,0, 0,a,0);
     linea(0,0,0, 0,0,a);
end ;

procedure box(x0,y0,z0, x,y,z:num);
{ parallelepipedo di centro P0 e 1/2 lati P }
begin
     setlinestyle(dashedln,0,0);
     linea(x0-x,y0-y,z0-z, x0-x,y0-y,z0+z);
     linea(x0-x,y0-y,z0-z, x0-x,y0+y,z0-z);
     linea(x0-x,y0-y,z0-z, x0+x,y0-y,z0-z);
     setlinestyle(solidln,0,0);
     linea(x0-x,y0-y,z0+z, x0-x,y0+y,z0+z);
     linea(x0-x,y0-y,z0+z, x0+x,y0-y,z0+z);
     linea(x0-x,y0+y,z0-z, x0+x,y0+y,z0-z);
     linea(x0-x,y0+y,z0-z, x0-x,y0+y,z0+z);
     linea(x0+x,y0-y,z0-z, x0+x,y0+y,z0-z);
     linea(x0+x,y0-y,z0-z, x0+x,y0-y,z0+z);
     linea(x0+x,y0+y,z0-z, x0+x,y0+y,z0+z);
     linea(x0-x,y0+y,z0+z, x0+x,y0+y,z0+z);
     linea(x0+x,y0-y,z0+z, x0+x,y0+y,z0+z);
end;

procedure circleX(xc,yc,zc, r, from,tu : num);
var i:integer ;
    an:num;
const n=100 ;
begin
     for i:= 0 to n do begin
         an:=from+i*(tu-from)/n ;
         point(xc,yc+r*cos(an),zc+r*sin(an), 0);
     end ;
end ;

procedure circleY(xc,yc,zc, r, from,tu : num);
var i,n:integer ;
    an:num;
begin
     n:=trunc(200/(2*pi)*(tu-from)) ;
     for i:= 0 to n do begin
         an:=from+i*(tu-from)/n ;
         point(xc+r*cos(an),yc,zc+r*sin(an), 0);
     end ;
end ;

procedure circleZ(xc,yc,zc, r, from,tu : num);
var i:integer ;
    an:num;
const n=40 ;
begin
     for i:= 0 to n do begin
         an:=from+i*(tu-from)/n ;
         point(xc+r*cos(an),yc+r*sin(an),zc, 0);
     end ;
end ;

procedure cilindrY(xc,y1c,y2c,zc, r, from,tu : num);
var i:integer ;
    an,x,z:num;
const n=30 ;
begin
     for i:= 0 to n do begin
         an:=from+i*(tu-from)/n ;
         x:=xc+r*cos(an);
         z:=zc+r*sin(an);
         linea(x,y1c,z, x,y2c,z);
     end ;
end ;

procedure raggiY(xc,yc,zc, r, from,tu : num);
var i:integer ;
    an,x,z:num;
const n=30 ;
begin
     for i:= 0 to n do begin
         an:=from+i*(tu-from)/n ;
         x:=xc+r*cos(an);
         z:=zc+r*sin(an);
         linea(xc,yc,zc, x,yc,z);
     end ;
end ;

procedure cilindrX(x1c,x2c,yc,zc, r, from,tu : num);
var i:integer ;
    an,y,z:num;
const n=30 ;
begin
     for i:= 0 to n do begin
         an:=from+i*(tu-from)/n ;
         y:=yc+r*cos(an);
         z:=zc+r*sin(an);
         linea(x1c,y,z, x2c,y,z);
     end ;
end ;

procedure cilindrZ(xc,yc,z1c,z2c, r, from,tu : num);
var i:integer ;
    an,x,y:num;
const n=10 ;
begin
     for i:= 0 to n do begin
         an:=from+i*(tu-from)/n ;
         x:=xc+r*cos(an);
         y:=yc+r*sin(an);
         linea(x,y,z1c, x,y,z2c);
     end ;
end ;

procedure raggiX(xc,yc,zc, r, from,tu : num);
var i:integer ;
    an,y,z:num;
const n=30 ;
begin
     for i:= 0 to n do begin
         an:=from+i*(tu-from)/n ;
         y:=yc+r*cos(an);
         z:=zc+r*sin(an);
         linea(xc,yc,zc, xc,y,z);
     end ;
end ;

procedure fill(x,y,z:num; style:fillindex);
var xx,yy:num;
    t:vector ;
begin
     t[0]:=x; t[1]:=y; t[2]:=z;
     proietta(t, xx,yy);
     setfillpattern(fills[style],0);
     setfillstyle(userFill,0);
     floodfill(trunc(sx0+xx),trunc(sy0-yy*sxy),0);
end;

procedure soffitto(n:byte);
var s:string ;
    i,j,k:integer;
    a,b,x,y,z,x1,y1:num;
begin
     case n of
          0:begin  { piatto }
              box(0,0,0, 3,4,2);
              s:='soffitto piatto';
              fill(0,0,2, grigio);
              fill(3,0,0, G12);
              fill(0,4,0, G8);
            end ;
          1:begin  { botte }
              box(0,0,-1, 3,4,2);
              fill(3,0,-1, G12);
              fill(0,4,-1, G8);
              cilindrY(0, 4,-4, 1, 3, 0,pi);
              raggiY(0,4,1, 3, 0,pi);
              circleY(0,4,1, 3, 0,pi);
              circleY(0,-4,1, 3, 0,pi);
              s:='soffitto a botte';
            end;
          11:begin  { cassettoni }
              box(0,0,0, 3,4,2);
              fill(3,0,0, G12);
              fill(0,4,0, G8);
              for i:=0 to 3 do linea(-3+6*(i+1)/5,4,2, -3+6*(i+1)/5,-4,2);
              for i:=0 to 2 do linea(3,-4+8*(i+1)/4,2, -3,-4+8*(i+1)/4,2);
              s:='stanza a cassettoni';
            end;
          3:begin  { butala }
              box(0,0,-1.2, 3,4,0.2);
              cilindrX(3,-3,0,-1, 4, 0,pi);
              raggiX(3,0,-1, 4, 0,pi);
              circleX(-3,0,-1, 4, 0,pi);
              circleX(3,0,-1, 4, 0,pi);
              s:='stanza a butala';
            end;
          5:begin  { chiocciola }
              a:=0;
              b:=-3;
              x:=3;
              y:=0;
              z:=b;
              linea(0,0,b, x,y,b);
              for i:=0 to 60 do begin
                  a:=a+pi/8;
                  b:=b+0.1 ;
                  x1:=3*cos(a);
                  y1:=3*sin(a);
                  linea(x,y,z, x1,y1,b);
                  linea(0,0,b, x1,y1,b);
                  z:=b; x:=x1; y:=y1;
              end ;
              s:='scala a chiocciola';
            end ;
          2:begin  { crociera }
              box(0,0,-1, 3,4,2);
              fill(3,0,-1, G12);
              fill(0,4,-1, G8);
              for i:=0 to 20 do begin
                  z:=1+3*i/20;
                  x:=sqrt(9-sqr(z-1));
                  y:=4*sqrt(1-sqr((z-1)/3));
                  linea(x,y,z, x,-y,z);     linea(x,y,z, x,y,z-0.3);
                  if i=19 then linea(x,-y,z, -x,-y,z);
                  linea(x,-y,z, x,-y,z-0.3);
                  if i=19 then linea(-x,-y,z, -x,y,z);
                  {linea(-x,-y,z, -x,-y,z-0.3);}
                  linea(-x,y,z, x,y,z);     linea(-x,y,z, -x,y,z-0.3);
              end ;
              s:='soffitto a crociera';
            end;
          4:begin  { cielo }
              for i:=0 to 15 do begin
                  x:=80+65*cos(i);
                  y:=45+30*sin(i/3);
                  a:=random*4+3;
                  b:=a*2+random*3;
                  x1:=random;
                  for j:=0 to 4 do begin
                      z:=j*0.4*pi+x1;
                      line(trunc(x+a*cos(z)),trunc(y+a*sin(z)),
                           trunc(x+b*cos(z+pi/5)),trunc(y+b*sin(z+pi/5)));
                      line(trunc(x+a*cos(z)),trunc(y+a*sin(z)),
                           trunc(x+b*cos(z-pi/5)),trunc(y+b*sin(z-pi/5)));
                  end ;
                  setfillpattern(fills[fillindex(ord(nero)+random(17))],0);
                  setfillstyle(userfill,0);
                  floodfill(trunc(x),trunc(y),0);
              end;
              s:='a cielo aperto';
          end;
          7:begin  { stroc }
              { base }
              setlinestyle(solidLN,0,0);
              linea(-3,4,-2, 3,4,-2);
              linea(3,4,-2, 3,-4,-2);
              setlinestyle(dashedln,0,0);
              linea(-3,-4,-2, -3,4,-2);
              linea(3,-4,-2, -3,-4,-2);
              { montanti }
              linea(-3,-4,-2, -3,-4,3);
              setlinestyle(solidln,0,0);
              linea(-3,4,-2, -3,4,3.5);
              linea(3,4,-2, 3,4,2);
              linea(3,-4,-2, 3,-4,1);
              { soffitto }
              linea(-3,-4,3, -3,4,3.5);
              linea(-3,4,3.5, 3,4,2);
              linea(3,4,2, 3,-4,1);
              linea(3,-4,1, -3,-4,3);
              fill(3,0,-1, G12);
              fill(0,4,-1, G8);
              fill(0,0,2, grigio);
              s:='soffitto stroc';
          end;
          8:begin  { querc }
              linea(3,4,1, 0,3,4);
              linea(-3,4,1, 0,3,4);
              linea(3,-4,1, 0,-3,4);
              linea(0,3,4, 0,-3,4);
              linea(3,4,1, -3,4,1);
              linea(3,4,1, 3,-4,1);
              fill(0,3,2, g6);
              fill(3,0,2, Grigio);
              box(0,0,-1, 3,4,2);
              linea(-3,-4,1, 0,-3,4);
              fill(3,0,-1, G12);
              fill(0,4,-1, G8);
              s:='soffitto a querc';
          end;
          6:begin  { sciap… }
              v[0]:=5;
              v[1]:=15;
              v[2]:=10;
              initvars(79,40,6);
              box(0,0,0, 3,4,2);
              fill(3,0,0, G8);
              fill(0,4,0, G12);
              linea(-3,4,2, 0,4,1);
              for i:=-2 to 1 do begin
                  linea(0,i*2,1, -1,i*2+1,1);
                  linea(-1,i*2+1,1, 0,i*2+2,1);
              end;
              linea(0,-4,1, -3,-4,2);
              fill(-2,0,2, nero);
              linea(3,4,2, 1,4,1);
              for i:=-2 to 1 do begin
                  linea(1,i*2,1, 0,i*2+1,1);
                  linea(0,i*2+1,1, 1,i*2+2,1);
              end;
              linea(1,-4,1, 3,-4,2);
              fill(2.5,0,2, nero);
              s:='soffitto sciap…';
          end;
          10:begin  { caverna }
              cilindrX(4,-3, 0, -1, 4, 0,pi);
              circleX(4,0,-1, 4, 0,pi);
              for i:= 0 to 30 do begin
                  a:=i*pi/30 ;
                  y:=4*cos(a);
                  z:=4*sin(a);
                  circleY(-3,y,-1, z, pi/2,pi);
              end ;
              linea(4,-4,-1, 4,4,-1);
              s:='caverna';
          end;
          9:begin  { foresta }
              v[0]:=10;
              v[1]:=25;
              v[2]:=10;
              initvars(76,46,10);
               for i:=-1 to 1 do
                   for j:=-1 to 1 do begin
                   x:=i*4.2-1;
                   y:=j*6-3.5;
                   cilindrZ(x,y,-2,1, 0.3, 0,2*pi);
                   circleZ(x,y,-2, 0.3, 0,2*pi);
                   for k:=-10 to 10 do begin
                       z:=1+k/5;
                       a:=0.7*(3-z)/2;
                       circleZ(x,y,z, a, 0,2*pi);
                   end;
               end ;
               s:='foresta';
          end;
          else s:='';
     end ;
     SetTextJustify(CenterText,TopText);
     OutTextXY(79,91,s);
end ;

{ MAIN BODY }

type row=array[0..19]of byte ;
     ogg=array[0..89]of row ;
     soff=array[0..11]of ogg ;

var i,j,k:integer ;
    x,y:num ;
    xi,yi:integer ;
    sof:soff;
    out:file of soff ;

begin
     InitGraphScreen(0) ;
     beta:=pi*0.01 ;
     cleardevice;
     setcolor(0);
     SetFillStyle(SolidFill,1);
     bar(0,0,639,399);
     for i:=0 to 11 do begin
         xi:=(i mod 4) * 160 ;
         yi:=(i div 4) * 120 ;
         SetViewPort(xi,yi, xi+159,yi+99, ClipOn);
         v[0]:=15;
         v[1]:=25;
         v[2]:=10;
         initvars(79,44, 10);
         soffitto(i);
         for j:=0 to 89 do move( ptr($b800,badr[yi+j]+(xi div 8))^,
                                 sof[i,j], sizeof(row) ) ;
     end ;
{     assign(out,'soffitti.byt');
     rewrite(out);
     write(out,sof);
     close(out);}
end .