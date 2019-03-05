program PULISCI ;

{$C-}

{$i olivetti.inc}

const dim = 150 ;
      xd    :integer    = 3 ;
      yd    :integer    = 1 ;
      lim   :integer    = 12 ;
      noise :real       = 0.8 ;
      spss  :integer    = 2 ;
      slop  :real       = 0.02 ;
      dist  :integer    = 10 ;

type imm = array[0..dim,0..dim] of boolean ;

var ogg,og1:imm ;
    ch:char ;
    gen:integer ;

procedure Display(var what:imm; x0,y0:integer) ;
var i,j:integer;
begin
     for i := 0 to dim do
         for j := 0 to dim do
             Plot (x0+i,y0+j,ord(what[i,j]))
end ;

procedure Genera(var what:imm) ;
var i,j:integer ;
begin
     for i:= 0 to dim do
         for j := 0 to dim do
             what[i,j] := (round(j+i*slop+noise*random)mod dist)<spss
end ;

function min(a,b:integer):integer ;
begin
     if a<b then min := a
            else min := b
end;

function max(a,b:integer):integer ;
begin
     if a>b then max := a
            else max := b
end;

procedure Trasforma(entra:imm; var esce:imm) ;
var x,y,i,j,ct:integer;
begin
     for x := 0 to dim do begin
         gotoxy(38,18);
         write(x*100 div dim : 3,'%');
         for y := 0 to dim do begin
             ct:=0 ;
             for j := max(y-yd,0) to min(y+yd,dim) do
                 for i := max(x-xd,0) to min(x+xd,dim) do
                     if entra[i,j] then ct:=ct+1+ord(j=y) ;
             esce [x,y] := ( ct > lim ) ;
        end ;
     end ;
end ;

procedure Trasponi (entra:imm; var esce:imm) ;
var i,j:integer ;
begin
     for i := 0 to dim do
         for j := 0 to dim do
             esce[i,j] := entra[j,i]
end;

procedure Simmetrica (entra:imm; var esce:imm) ;
var i,j:integer ;
begin
     for j:=0 to dim do
         for i := j to dim do
             esce[i,j] := entra[j,i]
end;

const l1=25; l2=6; l3=10; l4=5;
      pr = '->';

begin
  repeat
     screen(3);
     writeln('RIPULITORE DI OGGETTI',^M,^M) ;
     writeln('Dimensioni':l1, dim:l2, 'pixel':l3);
     write('Larghezza intorno':l1, xd:l2, 'pixel':l3, pr:l4);
     readln(xd);
     write('Altezza intorno':l1, yd:l2, 'pixel':l3, pr:l4);
     readln(yd);
     write('Valore di soglia':l1, lim:l2, 'unit…':l3, pr:l4);
     readln(lim);
     write('Spaziatura':l1, dist:l2, 'pixel':l3, pr:l4);
     readln(dist);
     write('Spessore':l1, spss:l2, 'pixel':l3, pr:l4);
     readln(spss);
     write('Fattore di disturbo':l1, noise:l2:1, '':l3, pr:l4);
     readln(noise);
     write('Pendenza':l1, slop:l2:3, 'tan(phi)':l3, pr:l4);
     readln(slop);
     write('(O)rizzontale, (V)erticale  oppure  (M)isto ?':l1+l2+l3, pr:4);
     repeat
           read(Kbd,ch);
           ch := upcase(ch) ;
     until (ch = 'O') or (ch='V') or (ch='M');
     writeln(ch);
     writeln('OK.':l1);
     genera(ogg);
     if (ch='V') then trasponi(ogg,ogg);
     if (ch='M') then simmetrica(ogg,ogg);
     display(ogg,100,200) ;
     og1 := ogg;
     gen:=0;
     repeat
           gen:=gen+1;
           trasforma(og1,og1) ;
           gotoxy(48,24);
           write('Generazione ',gen:2);
           display(og1, 200+dim,200 ) ;
     until keypressed ;
     read(kbd,ch);
     gotoxy(10,24);
     writeln('ANCORA ?');
     read(Kbd,ch);
     screen(0);
  until upcase(ch)<>'S';
end.
