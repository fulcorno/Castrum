program MakeHorizon;
{disegna il bordo dell'orizzonte artificiale e salva la finestra}

uses crt, drivers, graph ;

var x,y,i:integer;
    x2,y2:real;
    as2:real;
    size:word ;
    pt:pointer ;
    out:file ;
const aspectglb=0.81 ;
      cx=64 ;
      cy=53 ;

begin
     x:=att400; y:=att400hi;
     initgraph(x,y,'');
     as2:=sqr(aspectGlb);
     setviewport(0,0,639,399,ClipOff);
     for x:=-60 to 60 do
         for y:=-6 to -1 do
             if odd(x+y) then putpixel(x+cx,-y+cy,1);
     for x:=61 to 62 do
         for y:=-6 to -1 do
             if odd(x) or odd(y) then begin putpixel(x+cx,-y+cy,1); putpixel(-x+cx,-y+cy,1) end;
     for x:=-62 to 62 do
         for y:=0 to 55 do begin
             x2:=sqr(x);
             y2:=sqr(y);
             if x2+y2/as2 <=20 then begin
                                       putpixel(x+cx,-y+cy,1);
                                       putpixel(x+cx,y+cy,1)
                                  end
             else if x2+y2/as2 <=3481 then begin
                                       if ((x+y) mod 4 =0) and ((x-y) mod 4=0) then putpixel(x+cx,-y+cy,1)
                                       end
             else if x2+y2/as2 <=3721 then begin
                                        if odd(x+y) then putpixel(x+cx,-y+cy,1)
                                        end
             else if x2+y2/as2 <=3969 then begin
                                       if odd(x) or odd(y) then putpixel(x+cx,-y+cy,1)
                                       end
         end;
     Line(-60+cx,7+cy,60+cx,7+cy);
     Line(-62+cx,5+cy,60+cx,5+cy);
     repeat until keypressed;
     size:=ImageSize(0,0,127,63) ;
     GetMem(pt,size) ;
     GetImage(0,0,127,63,pt^) ;
     putImage(320,200,pt^,NormalPut) ;
     assign(out,'horizon.byt');
     rewrite(out,1);
     BlockWrite(out,pt^,size) ;
     close(out) ;
     CloseGraph;
end.