program ObjectEditor;
{Utility per disegnare le immagini degli oggetti}

uses crt, drivers, graph ;

const Xdim=159;   x8=19;{Xdim/8}
      Ydim=89;   rvs=$ff;
{ NON superare 160x90 }


VAR Scrx,Scry:integer;

PROCEDURE GraphScreen;
  VAR gd,gm:integer;
  BEGIN
         gd:=att400 ; gm:=att400hi ;
         InitGraph(gd,gm,'') ;
         Scrx:=639; Scry:=399;
  END;

function address(y:integer):integer;
begin
         Address:=(Y and 3) shl 13 + 80*(Y shr 2);
end;

PROCEDURE Plot(x,y,c:integer);
{disegna un punto in 640x400 indirizzando la memoria video
Per 'c' si ha: 0-cancella, 1-disegna, (-1)-inverte}
VAR adr,bit:integer;
BEGIN
     case c of
          0,1:putpixel(x,y,c);
         -1:begin
                 adr:=address(y)+x shr 3;
                 bit:=$80 shr (x and 7);
                 mem[$b800:adr]:=mem[$b800:adr] xor bit;
            end ;
     end;
END;

PROCEDURE Draw(x1,y1,x2,y2,c:integer);
    VAR x,y,DeltaX,DeltaY,XStep,YStep,count:integer;
BEGIN
      if c>=0 then begin
         setcolor(c) ;
         line(x1,y1,x2,y2) ;
      end else begin
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
      end
END;

type row=array[0..x8] of byte;
     image=array[0..ydim] of row;
     inout=file of image;

function keyboard:char;
var c1,c2:char;
begin
     gotoxy(25,20); write(^A);
     CheckBreak:=true;
     c1:=readkey;
     CheckBreak:=false;
     if (c1=#0) and keypressed then begin
        c2:=readkey;
        case c2 of
                   #77:c1:='D';
                   #75:c1:='S';
                   #72:c1:='E';
                   #80:c1:='X';
                   #79:c1:='A';
                   #71:c1:=' ';
                   #73:c1:='C';
                   #81:c1:='R';
                   #82:c1:='F';
                   #83:c1:='Z';
                   else c1:=' ';
        end;
     end;
     keyboard := c1;
     gotoxy(25,20); write(^B);
end;

function lit(x,y:byte;var obj:image):boolean;
begin
     if (x<0) or (y<0) or (x>xdim) or (y>ydim) then lit:=false else
     lit:=odd(obj[y,x shr 3] shr (7-(x and 7)));
end;

function min(a,b:integer):integer;
begin
     min:=a;
     if b<a then min :=b;
end;

procedure BOX(vx,vy:integer; scale:shortint; c:boolean);
var i,j:integer;
begin
     SetFillStyle(SolidFill,ord(c)) ;
     bar(vx,vy,vx+scale-1,vy+scale-1) ;
end;

procedure square(x,y,dx,dy,k:integer);
begin
     draw(x,y,x+dx,y,k);
     draw(x+dx,y,x+dx,y+dy,k);
     draw(x+dx,y+dy,x,y+dy,k);
     draw(x,y+dy,x,y,k);
end;

procedure Display(var what:image);
var x,y:integer;
    r:row;
begin
     for y:=0 to ydim do begin
         for x:=0 to x8 do r[x]:=what[y,x] xor rvs;
         move(r,mem[$b800:address(y+300)],sizeof(r));
     end;
end;

procedure Display3(i1,j1,i2,j2:integer;var what:image);
var i,j:integer;
begin
     if i1<0 then i1:=0;
     if i2>xdim then i2:=xdim;
     if j1<0 then j1:=0;
     if j2>ydim then j2:=ydim;
     for i:=i1 to i2 do for j:=j1 to j2 do box(i*3+8,j*3+8,3,lit(i,j,what));
end;

procedure clear(var obj:image);
begin
     fillchar(obj,sizeof(obj),$00);
end;

procedure edit(x0,y0:integer; var what:image);
var i,j,h,k,x,y:integer;
    ch:char;
begin
     gotoxy(30,23); write('frecce - Home - End':50);
     x:=0; y:=0;
     square(318,298,33,33,1);
     repeat
           for i:=0 to 3 do for j:=0 to 3 do
               box(i*8+320,j*8+300,6,lit(i+x0,j+y0,what));
           square(x*8+321,y*8+301,3,3,-1);
           gotoxy(50,20); write(x+x0:4,y+y0:4);
           ch:=upcase(keyboard);
           square(x*8+321,y*8+301,3,3,-1);
           case ch of
                'S':if x>0 then x:=pred(x);
                'D':if x<3 then x:=succ(x);
                'E':if y>0 then y:=pred(y);
                'X':if y<3 then y:=succ(y);
                'A':if y0+y<=ydim then what[y0+y,(x0+x)shr 3]
                       := what[y0+y,(x0+x)shr 3] xor ($80 shr ((x0+x)and 7));
           end;
     until (ch=' ');
end;

function select(var x,y:integer;var what:image):boolean;
var ch:char;
    i,j:integer;
begin
     square(7,7,(xdim+1)*3+1,(ydim+1)*3+1,1);
     repeat
           square(x+x+x+7,y+y+y+7,13,13,-1);
           gotoxy(30,23); write('frecce  Home  End  ESC  PgUp  PgDn  Ins  Del':50);
           gotoxy(50,20); write(x:4,y:4);
           gotoxy(50,21); write(x div 4:4,y div 4:4);
           display3(x,y,x+3,min(y+3,ydim),what);
           ch:=upcase(keyboard);
           square(x+x+x+7,y+y+y+7,13,13,-1);
           case ch of
                'S':if x>=4 then x:=x-4;
                'D':if x<=xdim-7 then x:=x+4;
                'E':if y>=4 then y:=y-4;
                'X':if y<=ydim-2 then y:=y+4;
                'A':edit(x,y,what);
           end;
     until (ch=' ') or (ch=^[);
     select:=(ch=^[);
end;

var it:image;
    x0,y0:integer;
    filename:string[80];
    io:inout;

begin
  DirectVideo:=false ;
  repeat
     CheckBreak:=true;
     writeln; write('Name of file to edit ?  ');
     readln(filename); if pos('.',filename)=0 then filename:=filename+'.ogg';
     CheckBreak:=false;
     GraphScreen;
     gotoxy(25,21); write('^ busy');
     gotoxy(50,19); write('Cursor position');
     gotoxy(60,20); write('pixel');
     gotoxy(60,21); write('block');
     x0:=((xdim+1) shr 3)shl 2; y0:=((ydim+1)shr 3)shl 2;
     clear(it);
     assign(io,filename);
     {$i-} reset(io); {$i+}
     if ioresult=0 then read(io,it);
     display3(0,0,xdim,ydim,it) ;
     repeat
           display(it);
     until select(x0,y0,it);
     rewrite(io);
     write(io,it);
     close(io);
     textmode(lastmode);
  until false;
end.