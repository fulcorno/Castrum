{ SANCTI GEORGII CASTRUM - - Gestione dell'orologio }

procedure orologio(ore,min:integer);
var phi:real;
    i:integer;
begin
     SelectScreen(2);
     SelectWorld(7);
     SelectWindow(7);
     RestoreWindow(7,0,0);
     SetColorBlack;
     i:=60*ore+min;
     phi:=2*pi*i/60; DrawLine(0,0,35*sin(phi),AspectGlb*35*cos(Phi));
     Phi:=Phi/12;    DrawLine(0,0,25*sin(Phi),AspectGlb*25*cos(Phi));
     SetColorWhite;
     SelectScreen(1);    CopyWindow(2,1,20,200);
end;

procedure InitWatch;
begin
     defineworld(7,-60,-50,59,49);
     selectworld(7);
     loadwindow(7,20,200,'watch.img');
     setheaderoff;
     StoreWindow(7);
end;
