program function_keys{disegna la mascherina per i tasti funzione};

uses crt,graph;

const flname='b:keys.dat';
      outfile='tasti.img';
      dimension=28;

type pixel=(a,b,c,d,e);
     riga=array[0..95]of pixel;
     frase=string[80];

var  dati:text;
     img:file;
     height:integer;
     size:word;
     pt:pointer;
     i:integer;

procedure getrow(var dest:riga);
{legge una riga in DEST}
var ch:char;
    i,ct,n:integer;
    row:frase;

begin
     readln(dati,row);
     ct:=0;
     n:=0;
     while row<>'' do begin
           ch:=row[1];
           delete(row,1,1);
           if ('0'<=ch) and (ch<='9') then n:=10*n+ord(ch)-48;
           if ('a'<=ch) and (ch<='e') then begin
              if n=0 then n:=1;
              for i:=ct to ct+n-1 do
                  case ch of
                       'a':dest[i]:=a;
                       'b':dest[i]:=b;
                       'c':dest[i]:=c;
                       'd':dest[i]:=d;
                       'e':dest[i]:=e
                  end{case};{end for}
              ct:=ct+n;
              n:=0
           end;{if ...}
     end{while}
end{getrow};

procedure display(row:riga;var y:integer);
var x:integer;
begin
     for x:=0 to 95 do
         case row[x] of
              a:;
              b:if not(odd(x)) or not(odd(y)) then PutPixel(x,y,1);
              c:if not(odd(x+y)) then PutPixel(x,y,1);
              d:if not(odd(x)) and not(odd(y)) then PutPixel(x,y,1);
              e:PutPixel(x,y,1)
         end{case};{end for}
     inc(y);
end{display};

function disegnalo:integer;
var intro:array[0..14]of riga;
    exc:array[0..9]of riga;
    fine:array[0..8]of riga;
    body:riga;
    y,i,j:integer;
    name:frase;
begin
     DirectVideo:=false;
     gotoxy(1,18);
     assign(dati,flname);
     reset(dati);
     readln(dati,name);
     writeln(name,'intro');
     for i:=0 to 14 do getrow(intro[i]);
     readln(dati,name);
     writeln(name,'exc');
     for i:=0 to 9 do getrow(exc[i]);
     readln(dati,name);
     writeln(name,'fine');
     for i:=0 to 8 do getrow(fine[i]);
     readln(dati,name);
     writeln(name,'body');
     getrow(body);
     y:=0;
     for i:=0 to 14 do display(intro[i],y);
     for i:=1 to dimension-6 do display(body,y);
     for i:=1 to 4 do begin
         for j:=0 to 9 do display(exc[j],y);
         for j:=1 to dimension do display(body,y);
     end;{for}
     for i:=0 to 8 do display(fine[i],y);
     disegnalo:=y-1;
end{disegnalo};


var gm,gd:integer;

begin {MAIN}
     gd:=att400; gm:=att400hi;
     initgraph(gd,gm,'C:\turbo4');
     height:=disegnalo;
     writeln(height);
     size:=ImageSize(0,0,95,height);
     writeln(size,' bytes');
     GetMem(pt,size);
     GetImage(0,0,95,height,pt^);
     assign(img,outfile);
     rewrite(img,1);
     BlockWrite(img,pt^,size);
     close(img);
end.