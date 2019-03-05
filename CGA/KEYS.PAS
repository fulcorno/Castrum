program function_keys{disegna la mascherina per i tasti funzione};

uses graph,crt,dos ;

const flname='keys.dat';
      dimension=28;

type pixel=(a,b,c,d,e);
     riga=array[0..95]of pixel;
     frase=string[80];
var  dati:text;
     out:file ;
     height:integer;
     gd,gm : integer ;
     size:word ;
     img:pointer ;
     errcode: integer ;

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
var x,y1:integer;
begin
     y1 := y {div 2} ;
     for x:=0 to 95 do
         case row[x] of
              a:;
              b:if not(odd(x)) or not(odd(y1)) then PutPixel(x,y1,1);
              c:if not(odd(x+y1)) then PutPixel(x,y1,1);
              d:if not(odd(x)) and not(odd(y1)) then PutPixel(x,y1,1);
              e:PutPixel(x,y1,1)
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
     assign(dati,flname);
     reset(dati);
     gotoxy(1,20);
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


{MAIN}
begin
     DirectVideo := False ;
     gd := att400 ; gm := att400hi ;
     initgraph(gd,gm,'');
     errcode := graphresult ;
     if(errcode<>grok) then begin
                       writeln(grapherrormsg(errcode)) ;
                       readln ;
     end ;
     height:=disegnalo {div 2};
     writeln(height);
     size := imagesize (0,0, 95, height) ;
     getmem (img,size) ;
     getimage(0,0,95,height,img^) ;
     putimage(160,0,img^,NormalPut) ;
{     assign (out,'tasti.byt') ;
     rewrite(out,1) ;
     blockwrite(out,img^,size) ;
     close(out) ;         }
     freemem (img,size) ;
end.
