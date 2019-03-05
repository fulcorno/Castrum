program Font20x16;
{crea il font di caratteri gotico}

uses dos,crt,graph ;

const grafbase= $B800 ;

type carattere=array[0..19]of word;
     fonttype=array[0..255]of carattere;
     wrkstring=string[80] ;

var fnt:fonttype;
    ch:char;
    chnum:byte;
    diskfont:file of fonttype;
    gd,gm:integer;

function baseaddress(y:integer):integer ;
begin
     baseaddress:=(Y and 3) shl 13 + 80*(Y shr 2)
end;

function Hex(n:integer):WrkString;
const dig:array[0..15]of char = '0123456789ABCDEF';
begin
     Hex:=dig[n shr 12]+dig[(n shr 8) and $f]+dig[(n shr 4) and $f]+dig[n and $f];
end;

function strg(n,d:integer):wrkstring;
var t:wrkstring;
begin
     str(n:d,t);
     strg:=t;
end;

function Dec(h:WrkString):integer;
var d,e:integer;
begin
     val('$'+h,d,e);
     if e<>0 then d:=0;
     dec:=d;
end;

procedure left(var n:word);
begin
     n := ( ( lo(n) shl 1 ) or ((n and $8000) shr 15) ) + ( ( hi(n) shl 1 ) shl 8 ) ;
end ;

procedure right(var n:word);
begin
     n := ( (n shr 1) and $00ff ) + ( ( (hi(n) shr 1) or ((n and $0001) shl 7) ) shl 8 ) ;
end ;

function MODIFY(num:byte):boolean;
var shape:carattere;
    ch:char;
    ans:wrkstring;
    i:integer;

(**)procedure show(chi:carattere);
    var i,j,v,w:integer;
    (**)function lit(x,y:integer):boolean;
        begin
             if j<=7 then lit:=(((chi[i] and $ff)shr (7-j))and 1)=1
                        else lit:=(((chi[i] shr 8)shr (15-j))and 1)=1;
        end;
    begin
         setfillstyle(solidfill,0);
         bar(0,0,200,160);
         setfillstyle(solidfill,1);
         bar(310,10,346,50);
         setlinestyle(DottedLn,0,1);
         for i:=0 to 20 do line(0,i*8,128,i*8);
         for j:=0 to 16 do line(j*8,0,j*8,160);
         w:=-1 ;
         for i:=0 to 19 do begin
           setcolor(1);
           OutTextXY(17*8,i shl 3,strg(i,2)+' '+hex(chi[i]));
           v:=chi[i] xor $ffff;
           move(v,mem[grafbase:baseaddress(20+i)+40],2);
           for j:=0 to 15 do begin
             if j>w then if lit(i,j) then w := j ;
             setfillstyle(solidfill,ord(lit(i,j)));
             bar(j shl 3+1,i shl 3+1,j shl 3+6,i shl 3 +6)
           end;
         end;
         setlinestyle(solidln,0,NormWidth);
         line(8*(w+1),0,8*(w+1),160) ;
    end;

(**)procedure clrline;
    begin
         setfillstyle(solidfill,0);
         bar(0,312,639,350);
         gotoxy(1,21);
    end;

begin
     modify:=false;
     ClearDevice;
     outtextxy(0,232,'CARATTERE IN EDITING: '+strg(num,3)+'  "'+chr(num)+'"');
     shape:=fnt[num];
     repeat
           show(shape);
           outtextxy(0,248,'[I]mmetti  [C]orreggi  [P]rendi  [A]ggiorna  [E]sci  [S]alva tutto');
           outtextxy(0,260,'[<]/[>]-shifta  [R]itocca');
           ch:=readkey;
           case upcase(ch) of
                'I':for i:=0 to 19 do begin
                        clrline;
                        outtextxy(0,312,'Riga '+strg(i,2)+'     ');
                        readln(ans);
                        clrline;
                        shape[i]:=dec(ans);
                    end;
                'C':begin
                         clrline;
                         outtextxy(0,312,'Quale riga ?   '); readln(i);
                         clrline;
                         outtextxy(0,312,'Riga '+strg(i,2)+'     '); readln(ans);
                         clrline;
                         shape[i]:=dec(ans);
                    end;
                'P':begin
                         clrline;
                         outtextxy(0,312,'Da quale carattere ?  '); readln(i);
                         clrline;
                         shape:=fnt[i];
                    end;
                'A':begin
                         assign(diskfont,'20x16.fon');
                         rewrite(diskfont);
                         write(diskfont,fnt);
                         close(diskfont);
                    end;
                'E':fnt[num]:=shape;
                'S':modify:=true;
                '<':begin
                         for i:=0 to 19 do left(shape[i]) ;
                    end;
                '>':begin
                         for i:=0 to 19 do right(shape[i]) ;
                    end;
           end {case} ;
     until upcase(ch) in ['E','S'];
end;


begin
     gd:=att400;
     gm:=att400hi;
     initgraph(gd,gm,'c:\turbo4');
     repeat
       assign(diskfont,'20x16.fon');
       {$i-} reset(diskfont); {$i+}
       if ioresult=0 then read(diskfont,fnt);
       close(diskfont);
       repeat
             cleardevice;
             outtextxy(0,350,'Carattere # ? '); ch:=readkey;
       until modify(ord(ch));
       outtextxy(0,350,'Posso salvarlo ? (S/N)');
       ch:=readkey;
       if upcase(ch)='S' then begin
          assign(diskfont,'20x16.fon');
          rewrite(diskfont);
          write(diskfont,fnt);
          close(diskfont);
       end;
     until false;  {  ^C per smettere  }
end.
