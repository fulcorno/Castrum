Program OverView;
{visione d'insieme della disposizione delle finestre sullo schermo
 di lavoro di Sancti Georgii Castrum}

{$i graphix.inc}
{$i drawroom.inc}
{$i panico.inc}
{$i pergam.inc}
{$i oggetti.inc}
{$i funckeys.inc}
{$i watch.inc}
{$i horizon.inc}
{$i timer.inc}

var i,hrs,mnt:integer;
    ctr,tmp:real;
    ch:char;
    Frase:WrkString;
    testo:text;

procedure appari(d:integer);
begin
     hirescolor(8); delay(d);
     hirescolor(7); delay(d);
     hirescolor(15); delay(d);
end;

procedure scompari(d:integer);
begin
     hirescolor(7); delay(d);
     hirescolor(8); delay(d);
     hirescolor(0); delay(d);
end;


begin  {MAIN}
     initgraphic;
     assign(testo,'demo.txt');
     reset(testo);
     scompari(1);
     gotoxy(25,25); writeln('PER FAVORE ATTENDI UN ISTANTE');
     appari(150);
     selectscreen(2);

     (* window 1: immagine della stanza *)
     InitRoom;

     (* window 2: STATUS *)
     defineworld(2,0,0,1,298);
     selectworld(2);
     definewindow(2,52,0,79,298);
     selectwindow(2);
     setheadertotop;
     SelectBigFont;
     settextBIG;
     defineheader(2,'FnâstrÜ d''lâ §furmÑsiñn');
     drawborder;

     (* window 3: narratore *);
     DisegnaPergamena;

     (* window 5: finestra dell'oggetto *)
     DrawObj;

     (* window 6: orizzonte *)
     InitHorizon;

     (* window 7: orologio *)
     InitWatch;

     (* window 8: titolo tasti funzione *)
     InitMenuTitle;

     (* window 4: tasti funzione *)
     InitMenu;

     (* movimento delle lancette dell'orologio  CONTEMPORANEO allo
        scrolling dei messaggi e alla comparsa del panico (!!!)    *)
     scompari(150);
     copyscreen;
     selectscreen(1);
     appari(150);
     i:=0;
     ResetCounter;
     repeat
           ctr:=HardCounter;
           tmp:=ctr/7;
           hrs:=trunc(tmp/60);
           mnt:=round(tmp-hrs*60);
           orologio(hrs,mnt);
           soleluna(hrs+mnt/60);
           i:=succ(i);
           if eof(testo) then reset(testo);
           readln(testo,frase);
           ScrollUP(frase);
           if (i mod 7)=0 then DrawRoom(i mod 5+1,i mod 3+1,
                          'Stansia nÅmer '+chr(48+ord((i mod 100) div 10))+chr(48+ord(i mod 10)));
           if (i mod 20)=0 then PANICO;
     until keypressed;
     read(kbd,ch);
     scompari(150);
     leavegraphic;
end.
