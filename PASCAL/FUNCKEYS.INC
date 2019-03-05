{ SANCTI GEORGII CASTRUM - - gestione tasti funzione }

procedure WriteMenuTitle(a,b:wrkstring);
begin
     SelectWorld(8);
     SelectWindow(8);
     RestoreWindow(8,0,0);
     SetColorBlack;
     SetTextNear;
     DrawTextW(5,4,1,a);
     DrawTextW(5,11,1,b);
     SetColorWhite;
end;

procedure InitMenuTitle;
begin
     DefineWorld(8,0,18,95,0);
     SelectWorld(8);
     DefineWindow(8,40,199,51,217);
     SelectWindow(8);
     SetHeaderOff;
     SetLineStyle($aa);
     drawsquare(2,0,95,18,true);
     SetLineStyle(0);
     DrawSquare(4,2,91,16,true);
     StoreWindow(8);
     WriteMenuTitle('Que ch''åt fä''?','Dç'', brÅt FagnÑn!');
end;

procedure ClearMenu;
begin
     SelectWorld(4);
     RestoreWindow(4,0,0);
end;

procedure WriteMenu(key,row:integer; cosa:wrkstring);
var i,j:integer;
begin
     SelectWorld(4);
     SelectWindow(4);
     i:=1; if odd(key) then i:=0;
     j:=(key-1) shr 1;
     SetColorBlack;
     SetTextNear;
     DrawTextW(10+i*46,12+j*38+row*7,1,cosa);
     SetColorWhite;
end;

procedure InitMenu;
var i,k:integer;
begin
     DefineWorld(4,0,198,95,0);
     SelectWorld(4);
     LoadWindow(4,40,0,'keys.img');
     SelectWindow(4);
     StoreWindow(4);
     ClearMenu;
     for i:=1 to 10 do for k:=0 to 3 do
         WriteMenu(i,k,'F'+chr(48+i div 10)+chr(48+i mod 10)+'-r'+chr(48+k));
end;