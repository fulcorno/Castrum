unit watch ;
{ procedure correlate alla visualizzazione dell'orologio }

INTERFACE

USES drivers, graph ;

Procedure OutWatch(ora,min:byte) ; { disegna le lancette dell'orologio }

IMPLEMENTATION

const
     sx= 168 ; sy= 200 ;  { TL finestra orologio }
     dx= 60 ;  dy= 50 ;   { dimensioni finestra / 2 }
     rh= 20 ;  rm= 32 ;   { lunghezze lancette }

var pt:pointer ; { alla bitmap dell'orologio }
    asp:real ;   { rapporto X/Y del video }

procedure WatchImg ; external ; {$L watch.obj}

procedure InitWatch ;
{ quadrante vuoto }
begin
     SetViewPort(sx,sy,sx+2*dx-1,sy+2*dy-1,true) ;
     PutImage(0,0,pt^,NormalPut)
end ;

procedure OutWatch(ora,min:byte) ;
var ang,ang2:real;
    ix,iy,ix2,iy2:integer ;
begin
     { calcola i segmenti }
     ang:=pi/2-(ora+min/60)/6*pi ;
     ang2:=pi/2-min/30*pi ;
     ix:=round(rh*cos(ang)) ;
     iy:=round(rh*asp*sin(ang)) ;
     ix2:=round(rm*cos(ang2)) ;
     iy2:=round(rm*asp*sin(ang2)) ;
     SetColor(0) ;
     SetLineStyle(SolidLn,0,3) ;
     InitWatch ; { cancella }
     Line(dx,dy,dx+ix,dy-iy) ;
     Line(dx,dy,dx+ix2,dy-iy2)
end ;

procedure getasp ;
var x,y:word ;
begin
     GetAspectRatio(x,y) ;
     asp:=x/y
end ;

begin { init code }
     getasp ;
     pt:=@WatchImg ;
     InitWatch
end .