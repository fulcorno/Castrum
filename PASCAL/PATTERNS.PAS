program TestFillPatterns ;

uses drivers, graph,crt ;

var i:fillINDEX ;
    c:char;
    x,y:integer;
    b:byte;
    s:string ;

begin
     InitGraphScreen ;
     setfillstyle(UserFill,0);
     setcolor(1);
     setTextStyle(DefaultFont,horizdir,1);
     setTextJustify(lefttext,toptext);
     for i:=succ(nero) to pred(dummy) do begin
         x:=( (ord(i)-1)mod 8 )*80 ;
         y:=( (ord(i)-1)div 8 )*80 ;
         setfillpattern(fills[i],1);
         bar(x,y,x+75,y+75) ;
         setfillpattern(fills[nero],1);
         bar(x,y,x+16,y+8) ;
         str(ord(i):2,s);
         OutTextXY(x,y,s);
     end ;
     c:=readkey ;
     cleardevice ;
     for i:=nero to pred(dummy) do begin
         setcolor(1) ;
         setfillpattern(Fills[i],0);
         bar(0,0, 127,200);
         for x:=0 to 7 do for y:=1 to 8 do begin
             b:= (fills[i][y] shr (7-x)) and 1;
             if b=0 then setfillpattern(fills[grigio],1)
                    else setfillpattern(fills[bianco],1) ;
             bar(200+20*x,20*(y-1),200+18+20*x,18+20*(y-1));
         end;
         str(ord(i):2,s) ;
         s:='Pattern '+s ;
         setfillpattern(fills[nero],1);
         bar(200,200,200+textwidth(s),200+textheight(s)) ;
         OutTextXY(200,200,s);
         c:=readkey;
     end ;
end.