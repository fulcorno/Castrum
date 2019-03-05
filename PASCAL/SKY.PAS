unit Sky ;
{ visione di sole e luna nel cielo }

INTERFACE

Uses drivers, graph ;

Procedure OutSoleLuna(ora,min:byte) ; { disegna gli astri alla posizione
                                        specificata dall'ora }

IMPLEMENTATION

const sx=288 ; sy=230 ;  { TL of horizon }

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
     PutImage(sx,sy,PtHor^,NormalPut)
end;

procedure OutSoleLuna(ora,min:byte) ;
{ disegna gli astri all'interno della cornice }
const cx=64 ;  cy=53 ;  { centro di rorazione rispetto (sx,sy) }
      ix=16 ;  iy=13 ;  { centro dell'astro rispetto al suo spigolo }

var dx,dy:integer ;
    ang:real ;

begin
     ShowHorizon ; { cancella }
     SetViewPort(sx,sy,sx+127,sy+52,ClipOn) ;
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

begin { init code }
     PtHor:=@horizon ;
     PtSole:=@sole;
     PtLuna:=@luna;
     ShowHorizon ;
end .