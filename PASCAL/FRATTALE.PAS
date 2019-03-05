{$N+}
program Disegna_la_pergamena_con_tecnica_frattale ;

uses crt, drivers, graph ;

type real=double;

var h:real ;

const dir='c:\turbo4\' ;

procedure lineaX(xx1,yy1,xx2,yy2:integer; col:word; scale:real) ;

     procedure genera(x1,x2:integer; y1,y2:real) ;
          var y:real ;
              x:integer ;
          begin
          if (x2-x1)<=1 then PutPixel(trunc((x1+x2)/2+0.5),trunc((y1+y2)/2+0.5),col)
          else if not keypressed then begin
               y:=scale*(random-0.5)*exp(2*h*ln((x2-x1)/(xx2-xx1)))+(y1+y2)/2 ;
               x:=(x1+x2) div 2 ;
               genera(x1,x,y1,y) ;
               genera(x,x2,y,y2) ;
          end
     end ; {genera}

begin { lineaX }
{$ifdef fast}
     line(xx1,yy1,xx2,yy2) ;
{$else}
     genera(xx1,xx2,yy1,yy2) ;
{$endif}
end ;

procedure lineaY(xx1,yy1,xx2,yy2:integer; col:word; scale:real) ;

     procedure genera(y1,y2:integer; x1,x2:real) ;
          var x:real ;
              y:integer ;
          begin
          if (y2-y1)<=1 then PutPixel(trunc((x1+x2)/2+0.5),trunc((y1+y2)/2+0.5),col)
          else if not keypressed then begin
               x:=scale*(random-0.5)*exp(2*h*ln((y2-y1)/(yy2-yy1)))+(x1+x2)/2 ;
               y:=(y1+y2) div 2 ;
               genera(y1,y,x1,x) ;
               genera(y,y2,x,x2) ;
          end
     end ; {genera}

begin { lineaY }
{$ifdef fast}
     line(xx1,yy1,xx2,yy2) ;
{$else}
     genera(yy1,yy2,xx1,xx2) ;
{$endif}
end ;

type riga=array[0..79] of byte ;

var
    fl:file of riga ;
    y:integer ;
    tmp:riga ;

begin
     RandSeed := 2 ;
     InitGraphScreen ;
     for y:=1 to 10 do LoadFont(y,dir);
     h:=0.2 ;
     lineaX(8,305,632,305,1,5) ;  { limite sup. }
     ellipse(631,310,0,90,8,6) ;   { angolo TR }
     lineaY(638,310,625,392,1,3) ; { limite dx }
     line(625,392,634,392) ;
     ellipse(634,395,270,90,5,3) ; { bordo dx rotolo }
     lineaX(8,398,634,398,1,2) ;   { limite inf. }
     ellipse(8,392,180,270,7,6) ;  { bordo BL }
     lineaY(15,310,1,392,1,3) ;    { limite sx }
     ellipse(8,310,90,270,7,6) ;     { bordo TL }
     line(8,316,15,316) ;
     floodfill(350,320,1) ;
     setcolor(0) ;
     ellipse(7,310,300,45,7,7) ; { rotolo sup. }
     ellipse(6,311,270,90,5,5) ;
     ellipse(6,310,90,270,3,4) ;
     ellipse(6,311,270,90,2,3) ;
     ellipse(6,310,90,270,1,2) ;
     lineaX(8,392,626,392,0,2) ;  { rotolo inf. }
     ellipse(8,395,270,90,4,3) ;
     ellipse(8,394,90,270,4,2) ;
     ellipse(7,395,270,90,3,1) ;
     { scritte interne }
     SetViewPort(16,306,639-16,389,ClipOn) ;
     SetUserCharSize(18,10,5,4) ;
     SetTextStyle(4,HorizDir,UserCharSize) ;
     SetTextJustify(CenterText,CenterText) ;
     OutTextXY(303,15,'Sancti Georgii Castrum') ;
     SetUserCharSize(2,3,4,5) ;
     SetTextJustify(LeftText,CenterText) ;
     SetTextStyle(7,HorizDir,UserCharSize);
     moveto(10,45);
     OutText('Gioco di avventura e fantasia di ');
     SetTextStyle(1,HorizDir,4) ;
     OutText('Jena-Soft') ;
     SetTextStyle(3,horizdir,usercharsize);
     OutText(' 1988');
     SetUserCharSize(7,10,3,4) ;
     SetTextStyle(10,HorizDir,UserCharSize) ;
     SetTextJustify(CenterText,BottomText) ;
     OutTextXY(303,78,'Fulvio Corno  Stefano Roletti  Paolo Galetto') ;
{$ifndef debug}
     assign(fl,'pergam.byt') ;
     rewrite(fl) ;
{$endif}
     for y:=301 to 399 do begin
         move(ptr($B800,Badr[y])^,tmp,80) ;
{$ifndef debug}
         write(fl,tmp);
{$endif}
         move(tmp,ptr($B800,Badr[y-300])^,80) ;
     end ;
{$ifndef debug}
     close(fl) ;
{$endif}
     repeat until keypressed ;
end .