program IconEditor;
{Utility per disegnare le icone dei tastifunzione&c}

uses dos, crt, graph, drivers ;

const Xdim=31;   x8=3;{Xdim/8}
      Ydim=27;    rvs=$ff;
{ NON superare 160x90 }
      bigX=330;
      bigY=320;
    { cursor keys extended codes }
      kul=#71;      kuu=#72;      kur=#73;
      kll=#75;       { 5 }        krr=#77;
      kdl=#79;      kdd=#80;      kdr=#81;
      ins=#82;                    del=#83;

var aspect:real;
    curdir:string;

PROCEDURE GraphScreen;
  VAR gd,gm:integer ;
      x,y:word ;
  BEGIN
         InitGraphScreen(0) ;
         RestoreCrtMode;
         getaspectratio(x,y);
         aspect:=y/x;
  END;

PROCEDURE Plot(x,y,c:integer);
{disegna un punto in 640x400 indirizzando la memoria video
Per 'c' si ha: 0-cancella, 1-disegna, (-1)-inverte}
VAR adr,bit:integer;
BEGIN
     case c of
          0,1:putpixel(x,y,c);
         -1:begin
                 adr:=BAdr[y]+x shr 3;
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
var c:char;
begin
     gotoxy(25,20); write(^A);
     c:=upcase(readkey);
     if (c=#0) then case readkey of
        kul: c:='q'; kuu: c:='w'; kur: c:='e';
        kll: c:='a';              krr: c:='d';
        kdl: c:='z'; kdd: c:='x'; kdr: c:='c';
        ins: c:='v';              del: c:='g';
     end {case} ;
     keyboard := c;
     gotoxy(25,20); write(^B);
end;

function lit(x,y:byte;var obj:image):boolean;
begin
     if (x<0) or (y<0) or (x>xdim) or (y>ydim) then lit:=false else
     lit:=odd(obj[y,x shr 3] shr (7-(x and 7)));
end;

function sgn(x:integer):integer;
begin
     if x<0 then sgn:=-1;
     if x=0 then sgn:=0;
     if x>0 then sgn:=1;
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
         move(r,mem[$b800:BAdr[y+300]],sizeof(r));
     end;
end;

procedure Display3(i1,j1,i2,j2:integer;var what:image);
var i,j:integer;
begin
     if i1<0 then i1:=0;
     if i2>xdim then i2:=xdim;
     if j1<0 then j1:=0;
     if j2>ydim then j2:=ydim;
     for i:=i1 to i2 do for j:=j1 to j2 do box(i*8+8,j*8+8,8,lit(i,j,what));
end;

procedure display3all(var what:image);
var i,j,k:byte;
    v:shortint;
    x,y:integer ;
begin
     for j := 0 to ydim do begin
       y:=j*8+8;
       x:=8;
       for i := 0 to x8 do begin
           v:=what[j,i] ;
           for k:=0 to 7 do begin
               if v<0 then box(x,y,8,true);
               v:= v shl 1 ;
               inc(x,8);
           end
       end
     end
end ;

procedure clear(var obj:image);
begin
     fillchar(obj,sizeof(obj),$00);
end;

procedure edit(var x0,y0:integer; var what:image);
var i,j,x,y,lx,ly:integer;
    pencolor:shortint;
    ch:char;
    modif,moved,writing,locked:boolean ;
    old,temp:image ;


{ NESTED PROCEDURES }

procedure blit(var v:byte; mask:byte);
begin
     case pencolor of
          -1: v:=v xor mask;
           0: v:=v and not mask ;
           1: v:=v or  mask ;
     end;
end;

procedure pixel(px,py:byte);
begin
     if (px<=xdim) and (px>=0) and (py>=0) and (py<=ydim) then begin
        blit(what[py,px shr 3], 1 shl (7-(px and 7))) ;
        modif:=true ;
        display3(px,py,px,py,what);
     end ;
end ;

PROCEDURE segm(x1,y1,x2,y2:byte);
    VAR x,y,DeltaX,DeltaY,XStep,YStep,count:integer;
BEGIN
     x:=x1; y:=y1; XStep:=1; YStep:=1;
     IF x1>x2 THEN XStep:=-1; IF y1>y2 THEN YStep:=-1;
     DeltaX:=abs(x2-x1); DeltaY:=abs(y2-y1);
     IF DeltaX=0 THEN count:=-1 ELSE count:=0;
     repeat
           pixel(x,y);
           IF count<0
              THEN   BEGIN  y:=y+YStep; count:=count+DeltaX;  END
              ELSE   BEGIN  x:=x+XStep; count:=count-DeltaY;  END;
     until ((x=x2) and (y=y2))
END;

procedure circl ;
var r2:real;
    sn,cs,s1,c1:integer ;
begin
  r2:=sqr((x+x0-lx)/aspect)+sqr(y+y0-ly) ;
  if r2<0.5 then pixel(lx,ly) else begin
     cs:=round(sqrt(r2)); sn:=0;
     repeat
         pixel(lx+sn,ly+cs);
         if sn<>0 then begin
            pixel(lx-sn,ly+cs);
            pixel(lx-sn,ly-cs);
         end ;
         pixel(lx+sn,ly-cs);
         inc(sn);
         cs:=round(sqrt(abs(r2-sqr(sn/aspect))));
     until sn>cs;
     cs:=0; sn:=round(sqrt(r2)*aspect);
     repeat
         pixel(lx+sn,ly+cs);
         pixel(lx-sn,ly+cs);
         if cs<>0 then begin
            pixel(lx+sn,ly-cs);
            pixel(lx-sn,ly-cs);
          end;
         inc(cs);
         sn:=round(aspect*sqrt(abs(r2-sqr(cs))));
     until cs>=sn ;
   end;
end;

procedure pix ;
begin
     if writing and (y0+y<=ydim) then begin
        blit(what[y+y0,(x+x0)shr 3],1 shl (7-((x+x0)and 7))) ;
        display3(x0+x,y0+y,x0+x,y0+y,what);
        modif:=true;
     end ;
end ;

procedure up4 ;
begin
     if y0>0 then begin dec(y0,4); moved:=true; y:=3; end ;
end ;

procedure lt4 ;
begin
     if x0>0 then begin dec(x0,4); moved:=true; x:=3; end ;
end ;

procedure rt4 ;
begin
     if x0<xdim-3 then begin inc(x0,4); moved:=true; x:=0; end ;
end ;

procedure dn4 ;
begin
     if y0<ydim-3 then begin inc(y0,4); moved:=true; y:=0; end ;
end ;

procedure up;
begin
     if y>0 then dec(y) else up4;
end;

procedure dn;
begin
     if y<3 then inc(y) else dn4;
end;

procedure lt;
begin
     if x>0 then dec(x) else lt4;
end;

procedure rt;
begin
     if x<3 then inc(x) else rt4;
end;

procedure shiftR ;
var i,j:integer ;
    m,t:byte;
begin
     for j:=0 to Ydim do begin
         t:=what[j,x8]and 1 ;
         for i:=x8 downto 1 do
             what[j,i]:=(what[j,i] shr 1)or ((what[j,i-1]and 1)shl 7) ;
         what[j,0]:=(what[j,0] shr 1) or (t shl 7 ) ;
     end ;
end ;

procedure shiftL ;
var i,j:integer ;
    m,t:byte;
begin
     for j:=0 to Ydim do begin
         t:=what[j,0]and $80 ;
         for i:=0 to x8-1 do
             what[j,i]:=(what[j,i] shl 1)or ((what[j,i+1]and $80)shr 7) ;
         what[j,x8]:=(what[j,x8] shl 1) or (t shr 7 ) ;
     end ;
end ;

{ MAIN DELLA PROCEDURA "EDIT" }

begin
     DirectVideo:=false;
     display(what);
     square(7,7,9+8*xdim,9+8*ydim,1);
     display3all(what);
     gotoxy(25,21); write('^ busy');
     gotoxy(52,19); write('Cursore        Ancora');
     gotoxy(60,21); write('pixel');
     gotoxy(60,23); write('block');
     gotoxy(64,1);  write('JENA - SOFT');
     gotoxy(64,2);  write('Object editor');
     gotoxy(64,4);  write('->Per muoverti');
     gotoxy(67,5);  write('le 4 frecce');
     gotoxy(64,7);  write('->pi— in fretta');
     gotoxy(67,8);  write('con lo shift');
     gotoxy(64,10);  write('->ON/OFF punto');
     gotoxy(67,11);  write('tasto >INS<');
     gotoxy(64,13); write('->Per finire');
     gotoxy(67,14); write('tasto >ESC<');
     x:=0; y:=0;
     square(bigX-2,bigY-2,33,33,1);
     square(bigX-18,bigY-18,65,65,1);
     for i:=-2 to 5 do for j:=-2 to 5 do
         box(i*8+bigX,j*8+bigY,6,lit(i+x0,j+y0,what));
     lx:=x0; ly:=y0;
     locked:=false ;
     writing:=false;
     pencolor:=-1;
     old:=what ;
     repeat
           modif := false ;
           moved := false ;
           square(x*8+bigX+1,y*8+bigY+1,3,3,-1) ;
           square(x0*8+7,y0*8+7,33,33,-1) ;
           square((x+x0)*8+10,(y+y0)*8+10,3,3,-1);
           gotoxy(50,21); write(x+x0:4,y+y0:4) ;
           gotoxy(50,23); write(x0 div 4:4,y0 div 4:4) ;
           if locked then begin
              draw(lx*8+12,ly*8+12,(x+x0)*8+12,(y+y0)*8+12,-1);
              gotoxy(65,21); write(lx:4,ly:4);
              gotoxy(65,23); write(lx div 4:4, ly div 4:4);
           end else begin
              gotoxy(65,21); write('':8);
              gotoxy(65,23); write('':8);
           end;
           gotoxy(23,23);
           if writing then write('TRACCIA') else write('SPOSTA ');
           gotoxy(31,23);
           case pencolor of
                -1:write('INVERTE');
                 0:write('SPEGNE ');
                 1:write('ACCENDE');
           end {case} ;
           ch:=keyboard;
           square(x*8+bigX+1,y*8+bigY+1,3,3,-1);
           square(x0*8+7,y0*8+7,33,33,-1);
           square((x+x0)*8+10,(y+y0)*8+10,3,3,-1);
           if locked then draw(lx*8+12,ly*8+12,(x+x0)*8+12,(y+y0)*8+12,-1);
           temp:=what ;
           case ch of
      { <-}    'a':begin lt; pix; end;
      { ->}    'd':begin rt; pix; end;
      { ^ }    'w':begin up; pix; end;
      { V }    'x':begin dn; pix; end;
               'q':begin up; lt; pix; end;
               'e':begin up; rt; pix; end;
               'z':begin dn; lt; pix; end;
               'c':begin dn; rt; pix; end;
      {INS}    'v':pixel(x0+x,y0+y) ;
      {s->}    '6':rt4;
      {s<-}    '4':lt4;
      {sV}     '2':dn4;
      {s^}     '8':up4;
               '7':begin up4; lt4; end;
               '9':begin up4; rt4; end;
               '1':begin dn4; lt4; end;
               '3':begin dn4; rt4; end;
               '+':pencolor:=1;
               '-':pencolor:=0;
               '*':pencolor:=-1;
               #13:writing:=not writing ;
               '0':for i:=0 to 3 do for j := 0 to 3 do pixel(x0+i,y0+j) ;
               'g':locked:=false;
               '.':begin locked:=true; lx:=x+x0; ly:=y+y0; end;
               'L':if locked then segm(lx,ly,x0+x,y0+y);
               'K':if locked then begin segm(lx,ly,x0+x,y0+y); lx:=x0+x; ly:=y0+y; end;
               'Q':if locked then
                      for i:=0 to abs(x0+x-lx) do
                          for j:=0 to abs(y0+y-ly) do
                              pixel(lx+i*sgn(x0+x-lx),ly+j*sgn(y0+y-ly));
               'C':if locked then circl ;
               'R':if locked then begin
                      segm(lx,ly,x+x0,ly);
                      segm(x+x0,ly,x+x0,y+y0);
                      segm(x+x0,y+y0,lx,y+y0);
                      segm(lx,y+y0,lx,ly);
                   end;
               #8: begin
                      what:=old ;
                      modif:=true ;
                      SetFillStyle(SolidFill,0);
                      bar(8,8,15+xdim*8,15+ydim*8);
                      display3all(what);
                   end;
               '>':begin
                      shiftR ;
                      modif:=true ;
                      SetFillStyle(SolidFill,0);
                      bar(8,8,15+xdim*8,15+ydim*8);
                      display3all(what);
                   end;
               '<':begin
                      shiftL ;
                      modif:=true ;
                      SetFillStyle(SolidFill,0);
                      bar(8,8,15+xdim*8,15+ydim*8);
                      display3all(what);
                   end;
           end;
     if modif then begin
        display(what);
        old:=temp ;
     end;
     if moved or modif then
     for i:=-2 to 5 do for j:=-2 to 5 do
         box(i*8+bigX,j*8+bigY,6,lit(i+x0,j+y0,what));
     until (ch=#27 {ESC});
     DirectVideo:=true;
end;

function GetYesNo: boolean ;
var ch:char;
begin
     repeat
           ch:=upcase(readkey);
     until ch in ['S','N'] ;
     writeln(ch);
     GetYesNo:=(ch='S');
end;

function SelectFile:string;
{ permette all'utente di scegliere un file gi… esistente vedendolo
  nella directory }
type str12 = string[12] ;
     pfrec = ^frec ;
     frec = record
                  name:str12 ;
                  link:pfrec
            end ;
var s0,s1,s2:string[60];
    dr, ct, i:integer;
    s:SearchRec ;
    names, p1: pfrec ;
begin
     dr:=0;
     GetDir(dr,s0);
     s2:=curdir;
     chdir(s2);
     writeln ;
     write('Va bene la directory "',s2,'" ? ') ;
     if not GetYesNo then begin
        repeat
          Write('In quale directory lavori [ d:path | . ] ? -> ');
          readln(s2) ;
          {$i-} chdir(s2); {$i+}
        until IoResult=0 ;
     end;
     dr:=0; GetDir(dr,s2);
     curdir:=s2;
     if s2[length(s2)]<>'\' then s2:=s2+'\' ;
     writeln;
     WriteLn('Lista delle immagini nella directory "',s2,'" :');
     FindFirst( '*.icn', AnyFile, s ) ;
     ct:=0 ;
     names:=nil ;
     while DosError=0 do begin
           inc(ct) ;
           p1:=names ;
           new(names) ;
           names^.name:=s.name ;
           names^.link:=p1 ;
           FindNext(s) ;
     end ;
     if ct=0 then begin
        writeln(^G'* * Non ci sono files del tipo giusto in questa directory * *');
        Write('Dimmi il nome dell''immagine -> ');
        readln(s1);
     end else begin
       p1:=names ;
       for i:=1 to ct do begin
         Write( i:3,') - ',copy(p1^.name,1,pos('.',p1^.name)-1):8,'':5 ) ;
         p1:=p1^.link ;
       end ;
       writeln ;
       repeat
          Write('Scegli un numero ( 0 = nuovo nome ) -> ') ;
          {$i-} readln(dr) {$i+}
       until (dr in [0..ct]) and (IoResult=0) ;
       if dr=0 then begin
          Write('Dimmi il nome dell''immagine -> ');
          readln(s1);
       end else begin
          p1:=names ;
          for i:=1 to dr-1 do p1:=p1^.link ;
          s1:=p1^.name ;
       end ;
       while names<>nil do begin
             p1:=names ;
             names:=names^.link ;
             dispose(p1) ;
       end ;
     end ;
     SelectFile:=s2+s1 ;
     ChDir(s0);
end ;

{ MAIN BODY }

var it:image;
    x0,y0:integer;
    filename:string;
    ch:char;
    io:inout;
    ctr:integer;
    ok:boolean ;
    y:word;

begin
  y:=0;
  Getdir(y,curdir);
  ctr:=1;
  graphscreen;
  textcolor(White); TextBackground(blue);
  ClrScr;
  textcolor(LightRed);
  WriteLn('* * * SANCTI GEORGII CASTRUM - Jena Soft 1988 - Gestione immagini oggetti * * *');
  textcolor(white);
  repeat
     writeln;
     if ParamCount<ctr then begin
        WriteLn('Scegli il nome dell''immagine da elaborare' ) ;
        filename:=SelectFile;
     end else begin
        filename := ParamStr(ctr);
        writeln('Elaboro l''immagine nø ',ctr,': "',filename,'".');
        inc(ctr);
     end ;
     writeln;
     if pos('.',filename)=0 then filename:=filename+'.icn';
     writeln('Guardo se esiste "',filename,'".');
     assign(io,filename);
     {$i-} reset(io); {$i+}
     if ioresult=0 then begin
        writeln('Trovato: lo carico in memoria');
        read(io,it);
        close(io);
     end else begin
        write(^G'NON trovato: creo una nuova immagine bianca ? ') ;
        if GetYesNo then Clear(it);
     end;
     writeln; writeln('Premi <ÄÙ per procedere, ESC per lasciar perdere ...');
     repeat
        ch:=readkey;
     until (ch=#13) or (ch=#27) ;
     if ch=#13 then begin
        SetGraphMode(ATT400hi);
        x0:=((xdim+1) shr 3)shl 2; y0:=((ydim+1)shr 3)shl 2;
        edit(x0,y0,it);
        RestoreCrtMode;
        ClrScr;
        repeat
          ok:=true ;
          write('Devo registrare la nuova versione ? ');
          if GetYesNo then begin
              Write('Va bene se uso "',filename,'" ? ');
              if not GetYesNo then begin
                 writeln;
                 WriteLn('Dimmi il nuovo nome da usare');
                 filename:=SelectFile;
                 if pos('.',filename)=0 then filename:=filename+'.icn';
              end;
              assign(io,filename);
              Writeln('Scrivo l''immagine "',filename,'" su disco');
              {$i-} rewrite(io); {$i+}
            if IoResult<>0 then begin
               writeln;
               writeln(^G'* * Ci sono errori: NON riesco a scrivere * *');
               ok:=false;
            end else begin
               write(io,it) ;
               close(io);
            end ; { if ioresult..}
          end ; { if Getyesno..}
        until ok ;
     end ; { if ch=..}
     writeln ;
     write('Continui con un''altra immagine ? ');
  until not GetYesNo ;
  Writeln;
  TextColor(LightCyan);
  Writeln(^G^G^G'Jena-Soft ti ringrazia per la collaborazione. Bye.');
  writeln;
  TextBackground(Black); textcolor(White);
end.