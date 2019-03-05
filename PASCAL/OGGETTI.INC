{ SANCTI GEORGII CASTRUM - - gestione finestra dell'oggetto }

procedure DrawObj;
type row=array[0..19]of byte;
     ogg=array[0..89]of row;
var r:row;
    it:ogg;
    imp:file of ogg;
    i,j:integer;
begin
     defineworld(5,0,89,159,0);
     selectworld(5);
     setheaderon;
     setheadertobottom;
     definewindow(5,0,200,19,299);
     selectwindow(5);
     settextfar;
     defineheader(5,'Fiura at campagna');
     drawborder;
     assign(imp,'flower.obj');
     reset(imp); read(imp,it); close(imp);
     for i:=0 to 89 do begin
         for j:=0 to 19 do r[j]:=it[i,j] xor $ff;
         move(r,mem[grafbase:baseaddress[200+i]],20);
     end;
     setcolorblack;
     DrawSquare(1,1,158,88,false);
     setcolorwhite;
end;
