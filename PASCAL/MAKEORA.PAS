program Watch;
{disegna (e salva) l'immagine dell'orologio di S.G.Castrum}

uses drivers, graph ;

var x,y,i:integer;
    t,aspectGlb,AS2,phi:real;
    ax,ay,size:word ;
    pt:pointer ;
    fout:file ;

procedure DrawPoint(x,y:integer) ;
begin
     PutPixel(x,y,1)
end ;

procedure DrawLine(x1,y1,x2,y2:real);
begin
     line(round(x1),round(y1),round(x2),round(y2))
end ;

begin
     x:=att400; y:=att400hi; InitGraph(x,y,'');
     GetAspectRatio(ax,ay) ;
     AspectGlb:=ax/ay ;
     AS2:=sqr(AspectGlb);
     {quadrante}
     for x:=0 to 119 do
         for y:=0 to 99 do begin
             t:=sqr(x-60)+sqr(y-50)/as2;
             if t <=1225 then DrawPoint(x,y)
                else if (t<=2500) then begin if odd(x+y) then DrawPoint(x,y) end
                else if (t<=2809) and (odd(x) or odd(y)) then DrawPoint(x,y);
         end;
     {tacche orarie}
     SetFillStyle(SolidFill,0);
     bar(59,50-round(45*aspectglb),61,50-round(35*aspectglb));
     bar(95,49,105,50);
     bar(15,49,25,50);
     bar(59,50+round(35*aspectglb),60,50+round(45*aspectglb));
     setcolor(0);
     for i:=0 to 11 do
         if i mod 3 <>0 then begin
            phi:=i/6*pi;
            DrawLine(60+35*cos(phi),50-35*aspectglb*sin(phi),60+42*cos(phi),50-42*aspectglb*sin(phi));
         end;
     {corona "rolex"}
     SetLineStyle(UserBitLn,$aaaa,NormWidth);
     MoveTo(50,40);
     LineTo(70,40);
     LineTo(73,30);
     LineTo(65,37);
     LineTo(60,33);
     LineTo(55,37);
     LineTo(47,30);
     LineTo(50,40);
     {crocetta centrale}
     SetLineStyle(SolidLn,0,NormWidth);
     Line(59,50,61,50);
     Line(60,49,60,51);
     {scritta "swiss"}
     SetTextStyle(SmallFont,HorizDir,4);
     SetTextJustify(CenterText,CenterText);
     OutTextXY(60,70,'SWISS');
     SetColor(1);
     assign(fout,'watch.byt') ;
     rewrite(fout,1) ;
     size:=ImageSize(0,0,119,99) ;
     GetMem(pt,size) ;
     GetImage(0,0,119,99,pt^) ;
     PutImage(320,200,pt^,NormalPut);
     BlockWrite(fout,pt^,size);
     close(fout)
end.