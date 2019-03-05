{ SANCI GEORGII CASTRUM - - finestra del panico }

procedure DrawButton(x,y,dx,dy:integer; testo:WrkString);
begin
     setcolorwhite;
     DrawSquare(x+6,y+4,x+6+dx,y+4+dy,false);
     DrawSquare(x+3,y+2,x+3+dx,y+2+dy,false);
     DrawSquare(x,y,x+dx,y+dy,true);
     setcolorblack;
     DrawSquare(x+1,y+1,x+dx-1,y+dy-1,false);
     SetTextFar;
     DrawTextW(x+(dx-TextDistance*Length(testo))shr 1,y+dy shr 1,1,testo);
     SetColorWhite;
end;

procedure PANICO;
var i:integer;
    ch:char;

(**)procedure DispLine(n:integer; testo:WrkString);
    begin
         gotoxy(40-length(testo)shr 1,14+n shl 1);
         write(testo);
    end;
begin
     defineworld(9,0,239,351,0);
     selectworld(9);
     definewindow(9,18,80,61,319);
     selectwindow(9);
     setheaderoff;
     storewindow(9);
     selectscreen(2);
     setbackground(0);
     setcolorwhite;
     for i:=0 to 4 do drawsquare(i shl 1+1,i shl 1 +1,350 -i shl 1,238- i shl 1,false);
     DispLine(1,'I te ''n tå pastiss ''dla grîsa');
     DispLine(2,'Que ch''it fä ?');
     DispLine(3,'Decit mÉc an prâsa !!');
     DrawButton(90,100,100,20,'Tirte ''ntal pus');
     DrawButton(20,130,100,20,'Sauta andrer');
     DrawButton(160,130,100,20,'Ciama aiÅt');
     DrawButton(90,160,100,20,'Tacte a la corda');
     DrawSquare(230,170,329,219,false);
     copywindow(2,1,18,80);
     selectscreen(1);
     repeat until keypressed;
     while keypressed do read(kbd,ch);
     restorewindow(9,0,0);
     clearwindowstack(9);
end;
