{ SANCTI GEORGII CASTRUM - - gestione dell'orizzonte artificiale }

type astro=array[0..26,0..3]of byte;
     solu=(sole,luna);
var astri:array[solu]of ^astro;

procedure soleluna(ore:real);
var phi:real;
    x,y,j,k:integer;
    as:solu;
begin
     selectscreen(2);
     selectworld(6);
     selectwindow(6);
     restorewindow(6,0,0);
     setcolorwhite;
     phi:=-2*pi*ore/24-pi/2;
     for as:=sole to luna do begin
         x:=round(28*cos(phi))-16;
         y:=round(28*aspectglb*sin(phi))+13;
         for k:=0 to 26 do if y-k>0 then for j:= 0 to 31 do
             if odd((astri[as]^[k,j shr 3])shr (7-(j and 7)))
                then drawpoint(x+j,y-k);
     phi:=phi+pi;
     end;
     selectscreen(1);
     copywindow(2,1,35,218);
end;

procedure InitHorizon;
var into:file of astro;
begin
     defineworld(6,-68,-23,67,58);
     selectworld(6);
     loadwindow(6,35,218,'horizon.img');
     StoreWindow(6);
     assign(into,'soleluna.byt');
     reset(into);
     new(astri[sole]); new(astri[luna]);
     read(into,astri[sole]^,astri[luna]^);
     close(into);
end;