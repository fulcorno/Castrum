Unit Status ;
{ disegna e gestisce la finestra dello STATUS }
{ procedure correlate alla visualizzazione dell'orologio }
{ e alla visione di sole e luna nel cielo }

INTERFACE

USES graph, drivers, globals ;

Procedure InitQuadranti ;

Procedure OutWatch ;           { disegna le lancette dell'orologio }

Procedure OutSoleLuna ;          { disegna gli astri alla posizione
                                  specificata dal tempo reale simulato }

Procedure OutStatus ;
Procedure OutInventari ;

Procedure InitStatus ;


IMPLEMENTATION

{ 1) gestione orologio }

const
     sx= 168 ; sy= 200 ;  { TL finestra orologio }
     dx= 60 ;  dy= 50 ;   { dimensioni finestra / 2 }
     rh= 20 ;  rm= 32 ;   { lunghezze lancette }
     asp=0.88 ;           { rapporto X/Y del video }

var pt:pointer ; { alla bitmap dell'orologio }
    oldtime : word ;

procedure WatchImg ; external ; {$L watch.obj}

procedure InitWatch ;  { pubblica }
{ quadrante vuoto }
begin
     SetViewPort(sx,sy,sx+2*dx-1,sy+2*dy-1,true) ;
     PutImage(0,0,pt^,NormalPut)
end ; { InitWatch }

procedure OutWatch ;  { pubblica }
var ang,ang2:real;
    ix,iy,ix2,iy2:integer ;
    newtime:word ;
begin
  newtime:=packtempo(stato.tempo) ;
  if newtime>oldtime then with stato.tempo do begin
     { calcola i segmenti }
     ang:=pi/2-(ore+min/60)/6*pi ;
     ang2:=pi/2-min/30*pi ;
     ix:=round(rh*cos(ang)) ;
     iy:=round(rh*asp*sin(ang)) ;
     ix2:=round(rm*cos(ang2)) ;
     iy2:=round(rm*asp*sin(ang2)) ;
     SetColor(0) ;
     SetLineStyle(SolidLn,0,3) ;
     InitWatch ; { cancella }
     Line(dx,dy,dx+ix,dy-iy) ;
     Line(dx,dy,dx+ix2,dy-iy2) ;
     oldtime:=newtime ;
  end {with}
end ; { OutWatch }

{ 2) gestione orizzonte }

const sxh=288 ; syh=230 ;  { TL of horizon }

type astro = record
                   Dx,Dy:word
                  { seguirebbero i bytes di bitmap, che non manipolo
                    e quindi non definisco }
             end ;

var PtHor: pointer ;
    PtSole, PtLuna : ^astro ;
    oldsolu : word ;

{ immagini bitmap degli oggetti }
procedure horizon ; external ; {$L horizon.obj}
procedure sole ; external ; {$L sole.obj}
procedure luna ; external ; {$L luna.obj}

procedure ShowHorizon ;
begin
     SetViewPort(0,0,639,399,true);
     setcolor(1);
     PutImage(sxh,syh,PtHor^,NormalPut)
end; { ShowHorizon }

procedure OutSoleLuna ;
{ disegna gli astri all'interno della cornice }
const cx=64 ;  cy=53 ;  { centro di rorazione rispetto (sx,sy) }
      ix=16 ;  iy=13 ;  { centro dell'astro rispetto al suo spigolo }

var dx,dy:integer ;
    ang:real ;
    newsolu:word ;

begin
     newsolu:=packtempo(stato.tempo) ;
     if newsolu>oldsolu+5 then with stato.tempo do begin
          ShowHorizon ; { cancella }
          SetViewPort(sxh,syh,sxh+127,syh+52,ClipOn) ;
          setcolor(1) ;
          ang:=-pi/2-(ore+min/60)/12*pi ;
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
          oldsolu:=newsolu ;
     end {with}
end; { OutSoleLuna }

procedure InitQuadranti ; { pubblica }
{ disegna i 2 quadranti vuoti }
begin
     ShowHorizon ;
     InitWatch ;
end ;  { InitQuadranti }

{ 3) Barrette, cotillons e oggetti della finestra di Status }

const
     sx0 = 420 ;   sy0 = 0 ;   { TL finestra }
     dx0 = 219 ;   dy0 = 299 ; { dim finestra }

     sx1 = 8 ;    sy1 = 30 ;   { indicatori }
     dx1 = 203 ;  dy1 = 94 ;

     sx2 = 8 ;    sy2 = 128 ;  { figurine }
     dx2 = 203 ;  dy2 = 50 ;

     sx3 = 8 ;    sy3 = 182 ;  { inventari }
     dx3 = 203 ;  dy3 = 114 ;
     delX = 52 ;  delY = 5 ;
     dimX = 144 ; dimY= 49 ;
     ofsY = 55 ;

var statImg : pointer ;
    OLDbarrette : array [t_barrette] of shortint ;

procedure statusIMG; external ;
{$L status.obj }

procedure bord (x,y, lx,ly, r :integer) ;
{ rettangolo con angoli smussati }
begin
     line(x+r,y,x+lx-r,y) ;
     line(x+r,y+ly,x+lx-r,y+ly) ;
     line(x,y+r,x,y+ly-r) ;
     line(x+lx,y+r,x+lx,y+ly-r) ;
     ellipse(x+r,y+r,90,180,r,r) ;
     ellipse(x+r,y+ly-r,180,270,r,r) ;
     ellipse(x+lx-r,y+ly-r,270,0,r,r) ;
     ellipse(x+lx-r,y+r,0,90,r,r) ;
end ; { bord }

procedure barrette ;
{ disegna gli indicatori % }
var x,x2,y:integer;
    s:string[4] ;
    i:t_barrette ;
begin
     SetViewPort(sx0+sx1+8,sy0+sy1+8,sx0+sx1+dx1-8,sy0+sy1+dy1-7,ClipOn) ;
     SetColor(0) ;
     SetLineStyle(SolidLn,0,NormWidth) ;
     SetTextJustify(RightText,CenterText) ;
     setusercharsize(2,5,1,5);
     SelectFont(6,0) ;
     y:=1 ;
     for i:=punti to antipatia do begin
         x:=trunc(stato.barrette[i]) ;
         if x<>OldBarrette[i] then begin
            OldBarrette[i]:=x ;
            str(x:3,s) ;
            x2:=x div 2 ;
            SetFillPattern(fills[grigio],1) ;
            SetFillStyle(UserFill,1) ;
            bar(91,y+1,91+x2,y+5) ;
            SetFillStyle(SolidFill,1) ;
            bar(91+x2+1,y+1,91+50,y+5) ;
            bar(91+50+3,y-1,175,y-2+textheight('0')) ;
            OutTextXY(175,y+3,s) ;
         end ; { if }
         inc(y,10) ;
     end ; { for }
end ;  { barrette }

Procedure Elenco(var d:t_list_objs) ;  { scrive un elenco di nomi di oggetto }
var i,j:integer ;
    s:str25 ;
    st:string ;
begin
     st:='' ;
     for i:=1 to ListObjSize do begin
         if (d[i]<>0) and (d_visibi in obj^[d[i]].stato) then begin
         { *** necessario  <<$B+>> *** }
            s:= NomeObj(d[i],0 ) ;
            for j:=1 to length(s) do s[j]:=_8to7[s[j]] ;
            s[1]:=upcase(s[1]);
            st:=st+s+' ' ;
         end ;
     end ;
     MoveTo(0,0) ;
     Setcolor(0);
     while st<>'' do begin
           i:=pos(' ',st+' ');
           s:=copy(st,1,i-1);
           st:=copy(st,i+1,255);
           If TextWidth(s)+GetX > dimX then MoveTo(0,GetY+TextHeight('A') ) ;
           OutText(s+' ') ;
     end ;
end ;  { elenco }

Procedure OutStatus ; { pubblica }
type icimg=record
     x,y:word;
     z:t_icona ;
     end;
var a:icimg ;
begin
     barrette ;
     SetViewPort(sx0+sx2,sy0+sy2, sx0+sx2+dx2, sy0+sy2+dy2, true);
     setcolor(1);
     a.x:=31;
     a.y:=27;
     a.z:=icona^[24+ord(stato.posizione)];
     PutImage(11,6,a,notput);
     a.z:=icona^[20+ord(stato.velocita)];
     putimage(61,6,a,notput);
     a.z:=icona^[38+ord(statoAUX.stampante)];
     putimage(111,6,a,notput);
end; { OutStatus }

Procedure OutInventari ;
begin
     { inventari }
     setcolor(0);
     SetFillPattern(fills[bianco],1);
     SetTextJustify(LeftText,TopText);
     SetUserCharSize(1,1,1,1);
     SelectFont(2,0);
     SetViewPort(sx0+sx3+delX,sy0+sy3+delY,sx0+sx3+delX+dimX,sy0+sy3+delY+dimY,ClipOn);
     bar(0,0,dimX,dimY);
     elenco(myobjs) ;
     SetViewPort(sx0+sx3+delX,sy0+sy3+delY+ofsY,sx0+sx3+delX+dimX,sy0+sy3+delY+dimY+ofsY,ClipOn);
     bar(0,0,dimX,dimY);
     elenco(l_objs);
end ; { OutInventari }

PROCEDURE InitStatus ;  { pubblica }
type icimg=record
       x,y:word;
       z:t_icona ;
     end;
     allimg=record
       x,y:word ;
       z:array[0..30000]of byte ;
     end;
var a:icimg ;
    i:t_barrette ;
    i1,i2,ddx:integer ;
begin
     SetViewPort(sx0,sy0,sx0+dx0,sy0+dy0,true);
{ correzione per 640*200: rendo visibili le scritte in corsivo }
     if badr[0]=badr[1] then begin
        ddx:=allimg(statimg^).x div 8 +1 ;
        for i1:=sy1+8 to sy1+dy1-8 do if not odd(i1) then
            for i2:=sx1 div 8 to 91 div 8 + 1 do
                allimg(statimg^).z[i1*ddx+i2]:=
                allimg(statimg^).z[i1*ddx+i2] AND
                allimg(statimg^).z[(i1+1)*ddx+i2] ;
     end;
     PutImage(0,0,statimg^,NormalPut);
{ icone inventari }
     a.x:=31;
     a.y:=27;
     a.z:=icona^[12] ;
     putimage(sx3+8,sy3+8,a,notput);
     a.z:=icona^[13] ;
     putimage(sx3+8,sy3+dy3 div 2 + 6,a,notput);
{ omino stilizzato }
     SetViewPort(sx0+sx2+161,sy0+sy2+6,sx0+sx2+161+31,sy0+sy2+6+27,true);
     setcolor(0) ;
     ellipse(16,14,0,360,
       round(stato.individuo[suo_peso]/stato.individuo[sua_altezza]*5),6);
     setfillstyle(emptyfill,1) ;
     floodfill(16,14,0);
     line(15,20,12,25);
     line(17,20,21,25);
     line(8,8,25,8);
     circle(16,5,3);
{ simboli "%" }
     SetViewPort(sx0+sx1+8,sy0+sy1+8,sx0+sx1+dx1-8,sy0+sy1+dy1-8,ClipOn) ;
     SetColor(0) ;
     SetTextJustify(LeftText,TopText) ;
     setusercharsize(1,1,1,1);
     SelectFont(2,0) ;
     for i:=punti to antipatia do OutTextXY(dx1-16-8-2,10*ord(i),'%') ;
end ; { InitStatus }

begin { init code }
     PtHor:=@horizon ;
     PtSole:=@sole;
     PtLuna:=@luna;
     pt:=@WatchImg ;
     statimg:=@statusIMG ;
     FillChar(OldBarrette,sizeof(oldbarrette),$ff);
     oldtime:=0 ;
     oldsolu:=0 ;
end . { unit STATUS }
