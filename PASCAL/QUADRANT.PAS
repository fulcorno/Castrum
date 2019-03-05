unit quadranti ;
{ procedure correlate alla visualizzazione dell'orologio }
{ e alla visione di sole e luna nel cielo }

INTERFACE

USES drivers, graph ;

Procedure InitQuadranti ;

Procedure OutWatch(ora,min:byte) ; { disegna le lancette dell'orologio }

Procedure OutSoleLuna(ora,min:byte) ; { disegna gli astri alla posizione
                                        specificata dall'ora }

IMPLEMENTATION

{ 1) gestione orologio }

const
     sx= 168 ; sy= 200 ;  { TL finestra orologio }
     dx= 60 ;  dy= 50 ;   { dimensioni finestra / 2 }
     rh= 20 ;  rm= 32 ;   { lunghezze lancette }
     asp=0.88 ;           { rapporto X/Y del video }

var pt:pointer ; { alla bitmap dell'orologio }

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

{ 2) gestione orizzonte }

const sxh=288 ; syh=230 ;  { TL of horizon }

type astro = record
                   Dx,Dy:word
                  { seguirebbero i bytes di bitmap, che non manipolo
                    e quindi non definisco }
             end ;

var PtHor: pointer ;
    PtSole, PtLuna : ^astro ;

procedure horizon ; external ; {$L horizon.obj}
procedure sole ; external ; {$L sole.obj}
procedure luna ; external ; {$L luna.obj}

procedure ShowHorizon ;
begin
     SetViewPort(0,0,639,399,true);
     setcolor(1);
     PutImage(sxh,syh,PtHor^,NormalPut)
end;

procedure OutSoleLuna(ora,min:byte) ;
{ disegna gli astri all'interno della cornice }
const cx=64 ;  cy=53 ;  { centro di rorazione rispetto (sx,sy) }
      ix=16 ;  iy=13 ;  { centro dell'astro rispetto al suo spigolo }

var dx,dy:integer ;
    ang:real ;

begin
     ShowHorizon ; { cancella }
     SetViewPort(sxh,syh,sxh+127,syh+52,ClipOn) ;
     setcolor(1) ;
     ang:=-pi/2-(ora+min/60)/12*pi ;
     dx:=round(35*cos(ang)) ;
     dy:=round((35*0.81)*sin(ang)) ;
     if Dy+iy>0 then begin
        if Dy+iy < 26 then PtSole^.dy:=dy+iy ; { riduce artificialmente
                                                 l'altezza dell'immagine
                                                 "ingannando" la PutImage }
        PutImage(cx-ix+dx,cy-iy-dy,PtSole^,OrPut) ;
        PtSole^.dy:=26 ; { rimettiamo le cose subito a posto !! }
     end ;
     if -Dy+iy>0 then begin
        if -Dy+iy < 26 then PtLuna^.dy:=-dy+iy ; { v.sopra }
        PutImage(cx-ix-dx,cy-iy+dy,PtLuna^,OrPut);
        PtLuna^.dy:=26 ;
     end ;
end;

procedure InitQuadranti ; { pubblica }
{ disegna i 2 quadranti vuoti }
begin
     ShowHorizon ;
     InitWatch ;
end ;

begin { init code }
     PtHor:=@horizon ;
     PtSole:=@sole;
     PtLuna:=@luna;
     pt:=@WatchImg ;
end .


