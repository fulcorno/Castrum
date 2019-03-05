unit ROOM ;

{ SANCTI GEORGII CASTRUM - - per disegnare le stanze }

INTERFACE

USES graph, drivers, globals, scroll, objects ;

Procedure DrawRoom ;  {disegna il LUOGO # stato.dovesei }

Procedure DrawArrows ; { disegna la Rosa dei Venti }

Procedure InitRoom ;

var PassedThrough : indice_porta ;  { comunica all'UNIT la posizione dove
                                      bisogna disegnare l'omino }

IMPLEMENTATION

CONST poly_size = 30 ; { n§ vertici max per poligono }

TYPE
    PolyPoints = array [1..poly_size] of PointType ;
    Point3dType = RECORD
                _x,_y,_z : real
                  END ;
    Poly3dPoints = array [1..poly_size] of Point3dType ;
    POLY = RECORD
           len : word ;
           R3 : Poly3dPoints ;
           thick,
           style,
           color : integer ;
           pattern : FillIndex ;
         END ;
    stato_porta = (sp_niente, sp_chiuso, sp_aperto) ;

CONST { room-drawing constants }

      ptview = 30 ;   { altezza del punto di vista in spanne }

      { lunghezza assi (x,y) in spanne }
      MX = 65; MY = 55;

      { dimensioni tipiche stanza }
      typ_x = 20 ;
      typ_y = 15 ;       { 4x3x2.8 m^3 }
      typ_z = 13 ;

VAR
   { le seguenti variabili sono dichiarate come globali a quest'UNIT
     per motivi di efficienza:
      - si evita di passarle continuamente come parametri
      - vengono settate e modificate solo dalle routines pubbliche
   }
   luogo_loc : t_luogo ;  { contiene una COPIA LOCALE della stanza che
                              si sta disegnando }
   stanza_loc : t_stanza ;
   kx,ky,hx,hy : real ;  { fattori di scala per la grafica }
   loc_x,loc_y,loc_z : real ;  { valori locali, corretti e riscalati,
                                 di dim_(xyz) }


   { geometry }
   sx1, sy1,            { top-left  (TL) of drawing }
   sx2, sy2,            { bottom-right (BR) of drawing }
   sx3, sy3,            { TL of title }
   sx4, sy4: integer;   { BR of title }

{ funzioni SIN e COS velocizzate (cioŠ precalcolate) }
type trigon = array[0..359] of real ;
var Asin,Acos:trigon ;

function sin(x:real):real ;
var i:integer ;
begin
     i:=trunc(x/pi*180) ;
     sin:=Asin[i mod 360] ;
end ;

function cos(x:real):real ;
var i:integer ;
begin
     i:=trunc(x/pi*180) ;
     cos:=Acos[i mod 360] ;
end ;


PROCEDURE RoomBackground ;
begin
     SetViewPort(0,0,GetMaxX,GetMaxY,ClipOn) ;
     SetFillPattern( fills[grigio], 1 ) ;
     SetFillStyle ( UserFill, 1 ) ;
     SetColor (1);
     bar ( sx1+1,sy1+1, sx2-1, sy2-1 ) ;
     SetLineStyle (SolidLn, 0, NormWidth );
     rectangle ( sx1,sy1, sx2,sy2 );
end;

PROCEDURE RoomHeader ( txt:str40 ) ;
begin
     SetViewPort ( sx3,sy3, sx4,sy4, ClipOn ) ;
     SetFillStyle ( SolidFill, 1 ) ;
     bar ( 0,0, sx4-sx3,sy4-sy3 ) ;
     SetColor(0) ;
     putpixel (0,0,0);  putpixel (sx4-sx3,0,0);
     putpixel (0,sy4-sy3,0);  putpixel (sx4-sx3,sy4-sy3,0);
     SetTextJustify (CenterText,CenterText) ;
     setusercharsize(1,1,1,1) ;
     SelectFont(DefaultFont, usercharsize) ;
     if TextWidth(txt)>sx4-sx3-8 then Internal_Error('P:t:>','RoomHeader',length(txt)); {->illegal parameter:'txt':too long }
     OutTextXY ( (sx4-sx3) shr 1, (sy4-sy3) shr 1+1, txt ) ;
end;

PROCEDURE proietta(P3:Point3dType; var csi,eta:real) ;
{
   Restituisce in (csi,eta) le coordinate del punto (x,y,z) proiettato
   sul piano z=0 a partire dal punto (0,0,ptview)
}
var tmp:real;
begin
    with p3 do begin
     tmp := ptview / (ptview-_z) ;
     csi := tmp * _x ;
     eta := tmp * _y ;
   end
end;

PROCEDURE muro( xw,yw, wall:shortint; var P3:Point3dType ) ;
{
asse yw Â                      Parametri:
        ³                         - numero della parete (wall)
100ÚÄÄÄÄÄÄÄÄÄ¿  (muro)            - dimensioni del pavimento (dim_x,dim_y)
   ³    ³    ³                    - altezza della stanza (dim_z)
ÄÄ ÀÄÄÄÄÄÄÄÄÄÙ ÄÄÄ> asse xw       - coordinate SUL MURO di un punto (xw,yw)
  -100  ³   100
Restituisce le coordinate spaziali                ÚÄÄÄ(4)ÄÄÄ¿  ¿
P3(_x,_y,_z) del punto (xw,yw), tenendo           ³         ³  Ã dim_y
conto delle dimensioni della stanza              (3)       (1) Ù
(lx,ly,hr) e di quale muro si tratta.             ³         ³
                                                  ÀÄÄÄ(2)ÄÄÄÙ
                                                       ÀÄÂÄÄÙ
                                                        dim_x
}
begin
   with luogo_loc, stanza_loc, P3 do begin
         _z:=yw*loc_z/100;
         case wall of
              1: begin _x:=loc_x; _y:=-loc_y*xw/100; end;
              2: begin _x:=-loc_x*xw/100; _y:=-loc_y; end;
              3: begin _x:=-loc_x; _y:=loc_y*xw/100; end;
              4: begin _x:=loc_x*xw/100; _y:=loc_y; end;
         end {case} ;
    end ;
end;

PROCEDURE muro2 (xw,yw,zw, wall:shortint; var P3:Point3dType ) ;
{ Š simile a 'muro', con la differenza che l'asse zw Š ortogonale
  al muro e diretto verso il centro della stanza. Il centro stesso
  ha coordinate (xw,yw,zw) = (0,0,1). Si pu• pensare a 'muro' come:
     muro ( xw,yw , ... ) == muro2 ( xw,yw, (zw=0) , ... )          }
BEGIN
   with luogo_loc, stanza_loc, P3 do begin
         _z:=yw/100*loc_z;
         case wall of
              1: begin _x:=loc_x*(1-zw/100); _y:=-loc_y*xw/100; end;
              2: begin _x:=-loc_x*xw/100; _y:=-loc_y*(1-zw/100); end;
              3: begin _x:=-loc_x*(1-zw/100); _y:=loc_y*xw/100; end;
              4: begin _x:=loc_x*xw/100; _y:=loc_y*(1-zw/100); end;
         end {case} ;
    end ;
END ; {muro2}

FUNCTION coordX ( csi:real ) : integer ;
{ passa da coordinate logiche  [ -MX <= csi <= MX ] a coordinate
  fisiche in pixel  [ 0 <= x <= sx2-sx1 ] : ** presume che sia settata
  la corretta ViewPort **                                              }
begin
     coordX := trunc( kx*csi+hx ) ;
end;

FUNCTION coordY ( eta:real ) : integer ;
{ passa da coordinate logiche  [ -MY <= eta <= MY ] a coordinate
  fisiche in pixel  [ 0 <= y <= sy2-sy1 ] : ** presume che sia settata
  la corretta ViewPort **                                              }
begin
     coordY := trunc( ky*eta+hy ) ;
end;

PROCEDURE draw3d( csi1,eta1,csi2,eta2:shortint; wall:integer );
{ disegna un segmento su di un muro nell'opportuna prospettiva e con
  i parametri particolari (spessore,stile,...) settati                }
var P1,P2 : Point3dType ; { coord. spaziali }
    a1,b1,a2,b2 : real ; { coord. logiche nella finestra }
begin
     muro ( csi1,eta1,wall, P1 ) ;
     muro ( csi2,eta2,wall, P2 ) ;
     proietta ( P1, a1,b1 ) ;
     proietta ( P2, a2,b2 ) ;
     line ( coordX(a1),coordY(b1), coordX(a2),coordY(b2) ) ;
end;{draw3d}

PROCEDURE draw3d2 (csi1,eta1,zeta1, csi2,eta2,zeta2:shortint; wall:integer) ;
{ Š l'equivalente di 'draw3d', usando 'muro2' }
var P1,P2 : Point3dType ; { coord. spaziali }
    a1,b1,a2,b2 : real ; { coord. logiche nella finestra }
begin
     muro2 ( csi1,eta1,zeta1,wall, P1 ) ;
     muro2 ( csi2,eta2,zeta2,wall, P2 ) ;
     proietta ( P1, a1,b1 ) ;
     proietta ( P2, a2,b2 ) ;
     line ( coordX(a1),coordY(b1), coordX(a2),coordY(b2) ) ;
end;{draw3d}

procedure polygon3d (var p:poly) ;
{ disegna il poligono p con le caratteristiche specificate
  convenzioni di chiamata:
  1) i punti sono p.len
  2) le loro coordinate sono in p.R3
  3) la campitura Š contenuta in p.pattern ( se = 0 non campisce )
  4) lo spessore del bordo Š p.thick
  5) il tratteggio del bordo Š p.style
  6) il colore del bordo Š p.color
}
var p2 : PolyPoints ;
    i : word ;
    csi,eta: real ;
begin
     with p do begin
          for i := 1 to len do begin
              proietta ( R3[i], csi,eta ) ;
              with p2[i] do begin
                   x := CoordX(csi) ;
                   y := CoordY(eta) ;
              end ;
          end ;
          SetColor(color) ;
          SetLineStyle (style,0,thick) ;
          SetFillPattern ( fills[pattern] , color ) ;
          SetFillStyle (UserFill, color ) ;
          if (len>0) then
             if (pattern=dummy) then
                DrawPoly( len, p2 )
             else BEGIN
                  FillPoly ( len, p2 ) ;
                  if color=0 then BEGIN { il TURBO non disegna il bordo }
                     p2[len+1]:=p2[1] ; { chiude il poligono }
                     DrawPoly (len+1, p2) ;
                  END
             END
     end
end ;

PROCEDURE init (var q:poly; _color, _style, _thick:integer; _pattern : FillIndex) ;
{ inizializza un nuovo poligono con i parametri indicati }
    BEGIN
         with q do BEGIN
              len := 0 ;
              color := _color ;
              style := _style ;
              thick := _thick ;
              pattern := _pattern
         END
END ;

PROCEDURE Pavimento ;
{ disegna il pavimento con il pattern stanza_loc.terreno }
var p : poly ;
    i : integer ;
begin
     with p, stanza_loc do begin
          init ( p, 0, SolidLn, NormWidth, terreno) ;
          len := 4 ;
          for i := 1 to 4 do muro ( -100,0, i, R3[i] ) ;
          polygon3d (p);
     END ;
end ; {pavimento}


PROCEDURE parete(wall:integer);
{ disegna la parete nella direzione 'wall' attingendo agli array globali
  per conoscerne forma, dimensioni, colori e tipi porte }

var p,q,man:poly ;    {p globale, q locale, man omino (oppure []) }
    i:integer;
    s_p1, s_p2:stato_porta ; { per sapere se disegnarla aperta o chiusa }
    num_frecce : byte ;

(**)procedure add (csi,eta:shortint ; var q:poly) ;
    { aggiunge un punto al poligono q, date le sue coordinate sul muro }
    begin
         with q do begin
              inc(len) ;
              muro (csi,eta,wall, r3[len]) ;
         end
     end ; {add}

(**)PROCEDURE add2 (csi,eta,zeta: shortint ; var q:poly) ;
    { Š l'equivalente di add, usando muro2 }
    BEGIN
         with q do begin
              inc(len) ;
              muro2 (csi,eta,zeta,wall, r3[len]) ;
         end
(**)END ; { add2 }

(**)procedure go;
    { disegna il poligono p }
    begin
         if p.len>0 then polygon3d(p) ;
         p.len := 0;
(**)end; {go}

(**)procedure quad;
    { cornice rettangolare del muro }
    var i:integer ;
    begin
              add (100,0, p ) ;  add (100,100, p ) ;
              add (-100,100,  p ); add (-100,0,  p ) ;
(**)end; {quad}

(**)procedure FRECCIA(s:shortint);
    { indica la possibile uscita }
    var i:indice_porta ;
    begin
         inc(num_frecce) ;
         with luogo_loc.pareti[wall] do
              if num_frecce=1 then i:=n_porta1
                              else i:=n_porta2 ;
         init( q, 0, SolidLn, NormWidth, nero ) ;
         add2 ( s-7, 0, -15, q ) ; add2 ( s, 0, -25, q ) ;
         add2 ( s+7, 0, -15, q ) ; add2 ( s, 0, -35, q ) ;
         polygon3d(q) ;
         SetLineStyle (SolidLn,0,ThickWidth) ;
         SetColor(0);
         draw3d2(s,0,0,s,0,-25,wall) ;
         if i=PassedThrough then begin { omino }
            init(man,0,solidln,NormWidth,bianco);
            add(s+10,0,man); add(s+20,0,man); add(s+20,40,man);
            add(s+30,40,man); add(s+40,40,man); add(s+20,60,man);
            add(s+10,60,man); add(s+10,80,man); add(s-10,80,man);
            add(s-10,60,man); add(s-20,60,man); add(s-40,40,man);
            add(s-30,40,man); add(s-20,40,man); add(s-20,0,man);
            add(s-10,0,man); add(s,40,man);
         end ;
(**)end; {freccia}

(**)procedure DOOR(s,d,h:shortint; s_p:stato_porta );
    const r=-30 ;
    begin
         add (s-d,0,p) ; add (s-d,h,p) ;
         add (s+d,h,p) ; add (s+d,0,p) ;
         freccia(s);
         if s_p<>sp_niente then begin
            init(q,1,SolidLn,NormWidth,G2) ;
            add(s-d,0,q); add(s-d,h,q);
            if s_p=sp_chiuso then begin
               add(s+d,h,q); add(s+d,0,q);
            end else begin { porta aperta }
               add2(s,h,r,q); add2(s,0,r,q);
            end ;
            polygon3d(q);
         end ;
(**)end; {door}

(**)procedure PORTAVETRI(s,d,h:shortint; s_p:stato_porta );
    const r=-30 ;
    begin
         add (s-d,0,p) ; add (s-d,h,p) ;
         add (s+d,h,p) ; add (s+d,0,p) ;
         freccia(s);
         if s_p<>sp_niente then begin
            init(q,1,SolidLn,NormWidth,G6) ;
            add(s-d,0,q); add(s-d,h,q);
            if s_p=sp_chiuso then begin
               add(s+d,h,q); add(s+d,0,q);
            end else begin { porta aperta }
               add2(s,h,r,q); add2(s,0,r,q);
            end ;
            polygon3d(q);
         end ;
(**)end; {portavetri}

(**)procedure PORTONE(s,d,h:shortint; s_p:stato_porta);
    begin
         add(s-d,0,p); add(s-d,3*h div 4,p); add(s-d div 2,h,p);
         add(s+d div 2,h,p); add(s+d,3*h div 4,p); add(s+d,0,p);
         freccia(s);
         if s_p=sp_chiuso then begin
            init(q,1,solidln,normWidth,g4);
            add(s-d,0,q); add(s-d,3*h div 4,q); add(s-d div 2,h,q);
            add(s+d div 2,h,q); add(s+d,3*h div 4,q); add(s+d,0,q);
            polygon3d(q) ;
         end else if s_p=sp_aperto then begin
            init(q,1,solidln,normWidth,g4);
            add(s-d,0,q); add(s-d,h,q); add2(s-d div 2,h,-d div 2,q);
            add2(s-d div 2,0,-d div 2,q);
            polygon3d(q) ;
            init(q,1,solidln,normWidth,g4);
            add(s+d,0,q); add(s+d,h,q); add2(s+d div 2,h,-d div 2,q);
            add2(s+d div 2,0,-d div 2,q);
            polygon3d(q) ;
         end ;
(**)end; {portone}

(**)procedure CANCELLO(s,d,h:shortint; s_p:stato_porta);
    var i:integer;
    begin
         add(s-d,0,p); add(s-d,3*h div 4,p); add(s-d div 2,h,p);
         add(s+d div 2,h,p); add(s+d,2*h div 4,p); add(s+d,0,p);
         freccia(s);
         if s_p=sp_chiuso then begin
            SetLinestyle(SolidLn,0,NormWidth) ;
            SetColor(1);
            for i:= -1 to 1 do  draw3d(s+d*i div 2,0,s+d*i div 2,h,wall) ;
            for i:=  1 to 3 do  draw3d(s-d,h*i div 4,s+d,h*i div 4,wall) ;
         end ;
(**)end; {cancello}

(**)procedure FINESTRA(s,t,l,m:shortint);
    const d=3 ;
    begin
         go ;
         init ( q, 0, SolidLn, NormWidth, nero ) ;
         add(s-l,t-m,q); add(s-l,t+m,q);
         add(s+l,t+m,q); add(s+l,t-m,q);
         polygon3d(q);
         init ( q, 0, SolidLn, NormWidth, G15 ) ;
         add2(s-l,t-m,d,q); add2(s-l,t+m,d,q);
         add2(s+l,t+m,d,q); add2(s+l,t-m,d,q);
         polygon3d(q);
         setlinestyle(UserBitLn,$aaaa,NormWidth);
         draw3d2(s,t-m,d,s,t+m,d,wall); draw3d2(s-l,t,d,s+l,t,d,wall);
(**)end; {finestra}

(**)procedure FINESmura(s,t,l,m:shortint);
    const d=3 ;
    begin
         go ;
         init ( q, 1, SolidLn, NormWidth, nero ) ;
         add(s-l,t-m,q); add(s-l,t+m,q);
         add(s+l,t+m,q); add(s+l,t-m,q);
         polygon3d(q);
         init ( q, 0, SolidLn, NormWidth, mattoni ) ;
         add2(s-l,t-m,d,q); add2(s-l,t+m,d,q);
         add2(s+l,t+m,d,q); add2(s+l,t-m,d,q);
         polygon3d(q);
(**)end; {finesmura}

(**)procedure GRATA(s,t,d,h:shortint) ;
    var i:integer ;
    const r=3 ;
    begin
         go ;
         init ( q, 0, SolidLn, NormWidth, nero ) ;
         add(s-d,t-h,q); add(s-d,t+h,q);
         add(s+d,t+h,q); add(s+d,t-h,q);
         polygon3d(q);
         init(q,0,SolidLn,NormWidth,bianco) ;
         add2(s-d,t-h,r,q); add2(s+d,t-h,r,q);
         add2(s+d,t+h,r,q); add2(s-d,t+h,r,q);
         polygon3d(q) ;
         SetLineStyle(UserBitLn, $7777, NormWidth) ;
         for i:=-2 to 2 do begin
             draw3d(s+i*d div 3,t-h,s+i*d div 3,t+h,wall) ;
             draw3d(s-d,t+i*h div 3,s+d,t+i*h div 3,wall) ;
         end ;
(**)end; {grata}

(**)procedure SPECCHIO(s,t,l,m:shortint);
    const d=3 ;
    begin
         go ;
         init ( q, 0, SolidLn, NormWidth, bianco ) ;
         add(s-l,t-m,q); add(s-l,t+m,q);
         add(s+l,t+m,q); add(s+l,t-m,q);
         polygon3d(q);
         init ( q, 1, SolidLn, NormWidth, G2 ) ;
         add2(s-l,t-m,d,q); add2(s-l,t+m,d,q);
         add2(s+l,t+m,d,q); add2(s+l,t-m,d,q);
         polygon3d(q);
         SetLineStyle(UserBitLn,$aaaa,NormWidth) ;
         draw3d2(s-l div 2,t,d,s,t+m div 2,d,wall);
         draw3d2(s-l div 2,t-m div 2,d,s+l div 2,t+m div 2,d,wall);
         draw3d2(s,t-m div 2,d,s+l div 2,t,d,wall);
(**)end; {specchio}

(**)procedure quadro(s,t,l,m:shortint);
    const d=3 ;
    begin
         go ;
         init ( q, 1, SolidLn, NormWidth, g3 ) ;
         add(s-l,t-m,q); add(s-l,t+m,q);
         add(s+l,t+m,q); add(s+l,t-m,q);
         polygon3d(q);
         init ( q, 0, SolidLn, NormWidth, bianco ) ;
         add2(s-l,t-m,d,q); add2(s-l,t+m,d,q);
         add2(s+l,t+m,d,q); add2(s+l,t-m,d,q);
         polygon3d(q);
         { disegnino idiota }
         draw3d2(s-l div 2,t+m div 2,d,s-l div 2,t,d,wall);
         draw3d2(s-l div 2,t,d,s+l div 2,t+m div 2,d,wall);
         draw3d2(s,t,d,s+l div 2,t,d,wall);
         draw3d2(s+l div 2,t,d, s+l div 2,t-m div 2,d, wall);
         draw3d2(s+l div 2,t-m div 2,d, s,t-m div 2,d, wall);
         draw3d2(s,t-m div 2,d, s,t,d,wall);
(**)end; { quadro }

(**)procedure DASHES(da,al:shortint);
    begin
         SetColor(0);
         SetLineStyle(UserBitLn,$f8f8,ThickWidth);
         draw3d(da,0,al,0,wall);
         freccia((da+al) div 2);
(**)end; {dashes}

(**)procedure MURO(da,al:shortint);
    var i:integer;
    begin
         go ;
         add (al,0,p); add (al,100,p); add (da,100,p); add (da,0,p);
(**)end; {muro}

(**)procedure MATTONI(da,al:shortint);
    var j,k,n,w:integer;
    begin
         muro(da,al);
         p.pattern:=bianco;
         go;
         SetLineStyle(SolidLn,0,NormWidth);
         SetColor(0);
         n:=round((al-da) div 20);
         for j:=0 to 4 do draw3d(da,j*25,al,j*25,wall);
         for j:=0 to 3 do for k:=0 to n-1 do begin
             w:=round(((j and 1)/2+k)*(al-da)/n+da);
             draw3d(w,j*25,w,(j+1)*25,wall);
         end;
(**)end; {mattoni}

(**)procedure ROTTAMI(da,al:shortint);
    var i:integer;
        h,h1:real;
    begin
         init(q,0,SolidLn,NormWidth,stanza_loc.terreno) ;
         add2(da,0,0, q) ;
         for i := 1 to 9 do add2(da+(al-da)*i div 10,random(20)-10,-random(30), q) ;
         add2(al,0,0, q) ;
         polygon3d(q) ;
         freccia((al+da)div 2);
(**)end; {rottami}

(**)procedure PORTASCIAPA(s,d,h:shortint);
    begin
         add(s-d,0,p); add(s-d,h,p); add(s+d,h,p); add(s+d,0,p);
         freccia(s);
         SetLineStyle(SolidLn,0,NormWidth);
         SetColor(1);
         draw3d(s-d,h,s+d,0,wall);
         draw3d(s+d,h,s-d,0,wall);
         draw3d(s-d,h div 2,s+d,h div 2,wall);
(**)end; {porta-sciap…}

(**)procedure MERLI;
    begin
         add(-100,0,p); add(100,0,p); add(100,100,p);
         add(85,80,p); add(71,100,p);
         add(71,60,p); add(42,60,p);
         add(42,100,p); add(28,80,p);
         add(14,100,p); add(14,60,p);
         add(-14,60,p); add(-14,100,p);
         add(-28,80,p); add(-42,100,p);
         add(-42,60,p); add(-71,60,p);
         add(-71,100,p); add(-85,80,p); add(-100,100,p);
(**)end; {merli}

(**)procedure ARMADIO(s,d,h:shortint);
    const r=5; { rientranza }
    begin
         go;
         init ( q, 1, SolidLn, NormWidth, nero ) ;
         add (s-d,0,q) ;  add (s-d,h,q) ;
         add (s+d,h,q) ;  add (s+d,0,q) ;
         polygon3d(q) ;
         draw3d2(s-d,0,0,s-d,0,r,wall); draw3d2(s+d,0,0,s+d,0,r,wall);
         draw3d2(s-d,h,0,s-d,h,r,wall); draw3d2(s+d,h,0,s+d,h,r,wall);
         init ( q, 1, SolidLn, NormWidth, G2 ) ;
         add2 (s-d,0,r,q) ;  add2 (s-d,h,r,q) ;
         add2 (s+d,h,r,q) ;  add2 (s+d,0,r,q) ;
         polygon3d(q) ;
         draw3d2(s,h div 2,r,s+d,h div 2,r,wall) ;
(**)end; {armadio}

(**)procedure ARMADIOmuro(s,t,d,h:shortint);
    const r=5; { rientranza }
    begin
         go;
         init ( q, 1, SolidLn, NormWidth, nero ) ;
         add (s-d,t-h,q) ;  add (s-d,t+h,q) ;
         add (s+d,t+h,q) ;  add (s+d,t-h,q) ;
         polygon3d(q) ;
         draw3d2(s-d,t-h,0,s-d,t-h,r,wall); draw3d2(s+d,t-h,0,s+d,t-h,r,wall);
         draw3d2(s-d,t+h,0,s-d,t+h,r,wall); draw3d2(s+d,t+h,0,s+d,t+h,r,wall);
         init ( q, 1, SolidLn, NormWidth, G2 ) ;
         add2 (s-d,t-h,r,q) ;  add2 (s-d,t+h,r,q) ;
         add2 (s+d,t+h,r,q) ;  add2 (s+d,t-h,r,q) ;
         polygon3d(q) ;
         draw3d2(s,t+h div 2,r,s+d,t+h div 2,r,wall) ;
(**)end; {armadio}

(**)procedure CAMINETTO(s:shortint);
    const d=15; d2=20; {semi-larghezze}
          r=10; r2=15; {profondit…}
          h1=40; h2=50; h3=60; {altezze}
    begin
         go;
         init(q,0,SolidLn,NormWidth, bianco ) ;
         add(s-d,0,q); add(s-d,h1,q); add(s+d,h1,q); add(s+d,0,q);
         polygon3d(q);
         q.len:=0;
         add2(s-d,0,r,q); add2(s-d,h1,r,q); add2(s+d,h1,r,q); add2(s+d,0,r,q);
         polygon3d(q); q.len:=0;
         draw3d2(s-d,0,0,s-d,0,r,wall); draw3d2(s+d,0,0,s+d,0,r,wall);
         add(s-d2,h1,q); add(s-d2,h2,q); add(s,h3,q);
         add(s+d2,h2,q); add(s+d2,h1,q); polygon3d(q); q.len:=0;
         add2(s-d2,h1,r2,q); add2(s-d2,h2,r2,q);
         add2(s+d2,h2,r2,q); add2(s+d2,h1,r2,q); polygon3d(q); q.len:=0;
         add(s,h3,q); add(s+d2,h2,q); add2(s+d2,h2,r2,q);
         polygon3d(q); q.len:=0;
         add(s,h3,q); add(s-d2,h2,q); add2(s-d2,h2,r2,q);
         polygon3d(q); q.len:=0;
         add(s,h3,q); add2(s-d2,h2,r2,q); add2(s+d2,h2,r2,q); polygon3d(q);
(**)end; {caminetto}

(**)procedure SCAFFALE(s,d,h:shortint);
    const r=10 ;
    var i,t:integer;
    begin
         go;
         init( q, 0, SolidLn, NormWidth, G14 ) ;
         add(s-d,0,q); add(s-d,h,q);
         add(s+d,h,q); add(s+d,0,q);
         polygon3d(q);
         for i:=0 to 4 do BEGIN
            q.len:=0; { == init( ..come sopra.. ) }
            t:=h*i div 4;
            add(s-d,t,q); add2(s-d,t,r,q);
            add2(s+d,t,r,q); add(s+d,t,q);
            polygon3d(q);
         END;
         draw3d2(s-d,0,r,s-d,h,r,wall); draw3d2(s+d,0,r,s+d,h,r,wall);
(**)end; {scaffale}

(**)procedure BALCONE;
    const h1=10; h2=50; h3=60; {altezze}
          r=10; {profondit…}
          d=6; {diam. colonne}
    var i,j,x,dx,dy:integer;
    begin
         go;
         init(q,0,SolidLn,NormWidth, G15 );
         {basamento}
         add(-100,0,q); add(-100,h1,q);
         add(100,h1,q); add(100,0,q);
         polygon3d(q); q.len:=0;
         add2(-100,h1,-2*r,q); add(-100,h1,q);
         add(100,h1,q); add2(100,h1,-2*r,q);
         polygon3d(q); q.len:=0;
         q.pattern:=dummy;
         {colonne}
         for i := -2 to 2 do BEGIN
             x:=i*33;
             for j:=0 to 4 do BEGIN
                 dx:=round(d*cos(2/5*pi*j));
                 dy:=round(d*sin(2/5*pi*j));
                 draw3d2(x+dx,h1,dy-r, x+dx,h2,dy-r, wall) ;
                 add2(x+dx,h1,dy-r,q);
             END;
             inc(q.len);
             q.r3[q.len]:=q.r3[1] ;
             polygon3d(q);
             q.len:=0;
         END;
         q.pattern := G14 ;
         {ringhiera}
         add(-100,h2,q); add(-100,h3,q);
         add(100,h3,q); add(100,h2,q);
         polygon3d(q); q.len:=0;
         add2(-100,h3,-2*r,q); add(-100,h3,q);
         add(100,h3,q); add2(100,h3,-2*r,q);
         polygon3d(q); q.len:=0;
(**)end; {balcone}

(**)procedure POGGIOLO(s,d:shortint) ;
    begin
         init(q,0,SolidLn,NormWidth,lisca);
         add2(s-d,0,0,q); add2(s+d,0,0,q);
         add2(s+d,0,-30,q); add2(s-d,0,-30,q);
         polygon3d(q);
(**)end; {poggiolo}

(**)procedure SALITA(s,d:shortint);
    var i:integer ;
    begin
         init(q,0,SolidLn,NormWidth,bianco) ;
         for i:=0 to 4 do begin
             q.len:=0;
             add2(s+d,(i+1)*16,-i*16,q);
             add2(s-d,(i+1)*16,-i*16,q);
             add2(s-d,i*16,-i*16,q);
             add2(s+d,i*16,-i*16,q);
             polygon3d(q);
             q.len:=2; { i primi 2 punti li mantengo uguali }
             add2(s-d,(i+1)*16,-(i+1)*16,q);
             add2(s+d,(i+1)*16,-(i+1)*16,q);
             polygon3d(q) ;
         end;
(**)end; {salita}

(**)procedure DISCESA(s,d:shortint);
    var i:integer ;
    begin
         init(q,0,SolidLn,NormWidth,bianco) ;
         for i:=4 downto 0 do begin
             q.len:=0;
             add2(s+d,-i*12,-i*25,q);
             add2(s-d,-i*12,-i*25,q);
             add2(s-d,-i*12,-(i+1)*25,q);
             add2(s+d,-i*12,-(i+1)*25,q);
             polygon3d(q) ;
         end;
(**)end; {discesa}

(**)procedure ANELLO(s,h:shortint); { anello attaccato al muro }
    const r=20 ;
    var i:integer ;
        a,a1:real ;
        sn,cs,sn1,cs1:shortint ;
    begin
         go;
         SetLineStyle(SolidLn,0,ThickWidth) ;
         setcolor(0);
         a1:=(-130)/180*pi;
         sn1:=round(r*sin(a1));
         cs1:=round(r*cos(a1));
         for i:=-12 to 13 do begin
             a:=(i*10)/180*pi;
             sn:=round(r*sin(a));
             cs:=round(r*cos(a));
             draw3d2(s+sn1,h,r-cs1, s+sn,h,r-cs, wall) ;
             a1:=a; cs1:=cs; sn1:=sn;
         end;
    end;

(**)procedure BALDACCHINO(s:shortint);
    const d=30 ; {larghezza}
          t=10 ; {spessore}
          h=70 ; {altezza da terra}
          r=40 ; {rientranza}
    var i,h1:shortint;
    begin
         go;
         for i:=0 to 1 do begin
             h1:=h+i*t;
             if i=0 then
                init(q,0,SolidLn,NormWidth,G4)
             else
                init(q,0,solidLn,NormWidth,quadri) ;
             add(s-d,h1,q); add(s+d,h1,q);
             add2(s+d,h1,3*r div 4,q) ;
             add2(s+ d div 2,h1,r,q);
             add2(s- d div 2,h1,r,q);
             add2(s-d,h1,3*r div 4,q);
             polygon3d(q);
         end;
(**)end; {baldacchino}

(**)procedure buco(s,h,r:shortint);
    var i:integer;
        a:real;
    const n=20 ;
    begin
         init(q,0,solidln,normwidth,nero);
         go ;
         for i:= 0 to n do begin
             a:=2*pi/n*i ;
             add(round(s+r*cos(a)),round(h+r*sin(a)),q);
         end ;
         polygon3d(q);
(**)end ; { buco }

(**)procedure cesso ;
    const y=30 ;
          y2=55 ;
          dy=8 ;
    var i:integer ;
    begin
         go;
         { tazza }
         init(q,0,solidln,normwidth,bianco);
         add2(20,y,0,q); add2(20,y,20,q);
         add2(15,y,30,q); add2(-15,y,30,q);
         add2(-20,y,20,q); add2(-20,y,0,q);
         polygon3d(q);
         init(q,0,solidln, normwidth, grigio);
         add2(15,y,5,q); add2(15,y,20,q);
         add2(10,y,25,q); add2(-10,y,25,q);
         add2(-15,y,20,q); add2(-15,y,5,q);
         polygon3d(q);
         { bastone }
         init(q,0,solidln,normwidth,bianco);
         add(-5,y,q); add(-5,y2,q);
         add(5,y2,q); add(5,y,q);
         polygon3d(q);
         { vaschetta }
         for i:=0 to 1 do begin
             q.len:=0; { == init(q,0,solidln,normwidth,bianco) }
             add(-20,y2+i*dy,q); add2(-20,y2+i*dy,10,q);
             add2(20,y2+i*dy,10,q); add(20,y2+i*dy,q);
             polygon3d(q);
         end ;
(**)end ; { cesso }

(**)procedure vasca ;
    const x=40; y=30; z=30;
    begin
         go;
         init(q,0,solidln,normwidth,bianco);
         { base }
         add2(-x,0,0,q); add2(-x,0,z,q);
         add2(x,0,z,q); add2(x,0,0,q);
         polygon3d(q);
         q.len:=0;
         { retro }
         add(-x,0,q); add(-x,y,q);
         add(x,y,q); add(x,0,q);
         polygon3d(q);
         q.len:=0;
         { sinistra }
         add(-x,0,q); add(-x,y,q);
         add2(-x,y,z,q); add2(-x,0,z,q);
         polygon3d(q);
         q.len:=0;
         { destra }
         add(x,0,q); add(x,y,q);
         add2(x,y,z,q); add2(x,0,z,q);
         polygon3d(q);
         q.len:=0;
         { fronte }
         add2(-x,0,z,q); add2(-x,y,z,q);
         add2(x,y,z,q); add2(x,0,z,q);
         polygon3d(q);
(**)end ; { vasca }

(**)procedure castlamunt ;
    const x=30; y=50; z=30; x1=6;
          y1=65; y2=80; xx=15;
    begin
         go;
         init(q,0,solidln,normwidth,bianco);
         { fronte }
         add2(x,0,z,q); add2(x,y,z,q);
         add2(-x,y,z,q); add2(-x,0,z,q);
         polygon3d(q);
         { finestra }
         init(q,0,solidln,normwidth,grigio);
         add2(x div 2, y div 4,z,q); add2(x div 2,3*y div 4,z,q);
         add2(-x div 2, 3*y div 4, z,q); add2(-x div 2,y div 4,z,q);
         polygon3d(q);
         { top }
         init(q,0,solidln,normwidth,g6);
         add(-x,y,q); add(x,y,q);
         add2(x,y,z,q); add2(-x,y,z,q);
         polygon3d(q);
         { cannone }
         init(q,0,solidln,thickwidth,g12);
         add(-x1,y,q); add(x1,y,q);
         add(x1,100,q); add(-x1,100,q);
         polygon3d(q);
         { radiatore }
         init(q,0,solidln,normwidth,righe);
         add(-xx,y1,q); add(-xx,y2,q);
         add(xx,y2,q); add(xx,y1,q);
         polygon3d(q);
(**)end ; { castlamunt }

(**)procedure stuva ;
    const x=30; y=50; z=30; x1=6;
    begin
         go;
         init(q,0,solidln,normwidth,g14);
         { fronte }
         add2(x,0,z,q); add2(x,y,z,q);
         add2(-x,y,z,q); add2(-x,0,z,q);
         polygon3d(q);
         { top }
         init(q,0,solidln,normwidth,g6);
         add(-x,y,q); add(x,y,q);
         add2(x,y,z,q); add2(-x,y,z,q);
         polygon3d(q);
         { cannone }
         init(q,0,solidln,thickwidth,g12);
         add(-x1,y,q); add(x1,y,q);
         add(x1,100,q); add(-x1,100,q);
         polygon3d(q);
(**)end ; { stuva }

(**)procedure altare ;
    const x=70; y=55; z=20;
          dx=10; dy=8;
          xt=20; y1=63; y2=80;
    begin
         go;
         { fronte }
         init(q,0,solidln,normwidth,bianco);
         add2(x,0,z,q); add2(x,y,z,q);
         add2(-x,y,z,q); add2(-x,0,z,q);
         polygon3d(q);
         { decorazione fronte }
         init(q,0,solidln,normwidth,quadri);
         add2(x-dx,dy,z,q); add2(x-dx,y-dy,z,q);
         add2(-x+dx,y-dy,z,q); add2(-x+dx,dy,z,q);
         polygon3d(q);
         { top }
         init(q,0,solidln,normwidth,g15);
         add(-x,y,q); add(x,y,q);
         add2(x,y,z,q); add2(-x,y,z,q);
         polygon3d(q);
         { tabernacolo }
         init(q,0,solidln,thickwidth,quadri);
         add(-xt,y1,q); add(xt,y1,q);
         add(xt,y2,q); add(-xt,y2,q);
         polygon3d(q);
(**)end ; { altare }

(**)procedure transenna(s:shortint);
    const x=8; y=50;
    var i:integer;
    begin
         go;
         { base }
         init(q,1,solidln,thickwidth,g8);
         add(s-x,0,q); add(s+x,0,q);
         add2(s+x,0,80,q); add2(s-x,0,80,q);
         polygon3d(q);
         { sbarre }
         for i:=0 to 4 do draw3d2(s,0,i*20, s,y,i*20, wall) ;
         { top }
         init(q,1,solidln,thickwidth,g8);
         add(s-x,y,q); add(s+x,y,q);
         add2(s+x,y,80,q); add2(s-x,y,80,q);
         polygon3d(q);
(**)end; { transenna }

(**)procedure ringhiera(da,al:shortint);
    const h=50; h1=65;
    var n,i:integer;
    begin
         { estremi }
         setlinestyle(solidln,0,thickwidth);
         setcolor(0);
         draw3d(da,0, da,h1, wall);
         draw3d(al,0, al,h1, wall);
         { corrimano }
         draw3d(da,h, al,h, wall);
         { raggi }
         setlinestyle(solidln,0,normwidth);
         n:=(al-da) div 10 ;
         for i:= 1 to n-1 do
             draw3d(da+i*(al-da)div n,0, da+i*(al-da)div n,h, wall ) ;
(**)end; { ringhiera }

(**)procedure lavabo;
    const h=50; x=30; z=30; z0=20;
    begin
         go;
         { sostegno }
         init(q,0,solidln,normwidth,bianco);
         add(-x,0,q); add(x,0,q); add(x,h,q); add(-x,h,q);
         polygon3d(q);
         { vasca }
         q.len:=0 ;
         add(-x,h,q); add2(-x,h,z0,q); add2(-x div 2,h,z,q);
         add2(x div 2,h,z,q); add2(x,h,z0,q); add(x,h,q);
         polygon3d(q);
         draw3d2(-x div 2,h,0, -x div 2,h,z0, wall);
         draw3d2(x div 2,h,0, x div 2,h,z0, wall);
         draw3d2(-x div 2,h,z0, x div 2,h,z0, wall);
         { rubinetti }
         setcolor(1);
         setlinestyle(solidln,0,thickwidth) ;
         draw3d(-x div 2,h+10, -x div 2,h+20, wall);
         draw3d(x div 2,h+10, x div 2,h+20, wall);
(**)end; { lavabo }

(**)function GetDoorStatus(np:indice_porta) : stato_porta ;
    { stabilisce come deve essere disegnata la porta #np in base al suo
      tipo ed alle sue condizioni istantanee                            }
     begin
     with porta^[np] do begin
          case tipo of
               p_passaggio: GetDoorStatus:=sp_niente ;
               p_ostruito:  GetDoorStatus:=sp_chiuso ;
          else {p_porta} begin
                if stato=p_aperto then GetDoorStatus:=sp_aperto
                else {chiusa normalmente o a chiave} GetDoorStatus:=sp_chiuso ;
              end  {if}
          end {case}
     end {with}
(**)end; {GetDoorStatus}

begin {parete}
     with p, luogo_loc, pareti[wall] do begin { prepara il muro standard }
        color := 1 ;
        thick := ThickWidth ;
        style := SolidLn ;
        len := 0 ;
        pattern := tipo ;
        if n_porta1<>0 then begin
           s_p1:=GetDoorStatus(n_porta1);
           if n_porta2<>0 then s_p2:=GetDoorStatus(n_porta2);
        end;
     end ;
     man.len:=0 ; { non disegnare l'uomo per default }
     num_frecce:=0 ;
     case luogo_loc.pareti[wall].forma of
          1:dashes(-1,1);
          2:quad;
          3:begin quad; door(-50,22,80,s_p1); end;
          4:begin quad; door(0,20,80,s_p1); end;
          5:begin quad; door(50,20,80,s_p1); end;
          6:begin quad; door(-50,20,80,s_p1); door(50,20,80,s_p2); end;
          7:begin quad; portone(0,40,90,s_p1); end;
          8:begin quad; finestra(-50,50,20,30); end;
          9:begin quad; finestra(0,50,20,30); end;
         10:begin quad; finestra(50,50,20,30); end;
         11:begin quad; finestra(-40,50,30,30);
                  finestra(40,50,30,30); end;
         12:begin quad; finestra(-60,50,20,30);
                  finestra(0,50,20,30); finestra(60,50,20,30); end;
         13:begin quad; portone(0,30,80,s_p1);
                  finestra(60,50,20,30); end;
         14:begin quad; door(0,20,80,s_p1); finestra(-60,50,20,30);
                  finestra(60,50,20,30); end;
         15:begin quad; door(20,20,80,s_p1); finestra(-50,50,20,30); end;
         16:begin quad; specchio(-80,40,10,20);
                  finestra(-40,50,20,30); specchio(0,40,10,20);
                  finestra(40,50,20,30); specchio(80,40,10,20); end;
         17:begin quad; door(0,20,80,s_p1); specchio(-60,40,20,20);
                  specchio(60,40,20,20); end;
         18:begin quad; specchio(-50,40,20,30);
                  specchio(50,40,20,30); end;
         19:rottami(-100,100);
         20:begin quad; door(60,20,80,s_p1); finestra(-50,50,20,30); end;
         21:begin quad; door(-60,20,80,s_p1); finestra(50,50,20,30); end;
         22:begin dashes(-100,-30); muro(-30,30); dashes(30,100); end;
         23:begin quad; door(-60,20,70,s_p1); finestra(10,50,20,30);
                  finestra(70,50,20,30); end;
         24:begin quad; door(0,20,70,s_p1); door(60,20,70,s_p2); end;
         25:begin muro(-100,-30); dashes(-30,30); muro(30,100); end;
         26:begin muro(-100,0); dashes(0,100); end;
         27:begin quad; portasciapa(0,20,70); end;
         28:begin quad; door(0,20,70,s_p1); finestra(60,50,20,30); end;
         29:begin muro(-100,30); door(-60,20,70,s_p1); dashes(30,100); end;
         30:mattoni(-100,100);
         31:begin quad; portone(0,60,90,s_p1); end;
         32:begin quad; cancello(0,30,80,s_p1); end;
         33:merli;
         34:begin quad; armadio(0,20,60); end;
         35:begin quad; scaffale(0,30,50); end;
         36:begin quad; caminetto(0); end;
         37:begin quad; door(50,20,70,s_p1); caminetto(-50); end;
         38:begin quad; door(-50,20,70,s_p1); caminetto(60); end;
         39:begin quad; door(0,20,70,s_p1); armadio(60,20,60); end;
         40:begin quad; door(0,20,70,s_p1); caminetto(-60); armadio(60,20,60); end;
         41:begin quad; door(0,20,70,s_p1); caminetto(60); end;
         42:begin quad; caminetto(50); end;
         43:begin quad; caminetto(-50); end;
         44:balcone;
         45:begin quad; grata(0,50,20,20); end;
         46:begin quad; portavetri(0,40,80,s_p1); end;
         47:begin quad; poggiolo(0,30); door(0,30,70,s_p1);
                  finestra(-60,50,20,30); finestra(60,50,20,30); end;
         48:begin quad; grata(-60,50,20,30); grata(60,50,20,30); end;
         49:begin quad; finestra(-60,50,25,40); finestra(60,50,25,40); end;
         50:begin quad; salita(0,40); door(0,40,70,s_p1); end;
         51:begin quad; discesa(0,40); door(0,40,70,s_p1); end;
         52:begin quad; salita(-50,40); discesa(50,40);
                  door(-50,40,70,s_p1); door(50,40,70,s_p2); end;
         53:begin quad; salita(-50,40); door(-50,40,70,s_p1);
                  door(50,30,70,s_p2); end;
         54:begin quad; door(-50,30,70,s_p1); grata(20,50,20,30);
                  grata(70,50,20,30); end;
         55:begin quad; anello(-50,50); anello(50,50); end;
         56:begin quad; door(-50,30,70,s_p1); baldacchino(30); end ;
         57:begin quad; poggiolo(-50,30); poggiolo(50,30);
                  door(-50,30,70,s_p1); door(50,30,70,s_p2); end ;
         58:begin quad; poggiolo(0,30); door(0,30,70,s_p1);  end;
         59:begin quad; buco(0,70,15); end;
         60:begin quad; cesso; end;
         61:begin quad; vasca end;
         62:begin quad; castlamunt end;
         63:begin quad; armadiomuro(0,50,20,30) end;
         64:begin quad; stuva end;
         65:begin quad; door(10,36,70,s_p1); caminetto(-60) end;
         66:begin quad; door(50,40,70,s_p1); baldacchino(-40); end;
         67:begin quad; discesa(40,40); door(40,40,70,s_p1); end;
         68:begin quad; altare end;
         69:begin quad; transenna(-30) end;
         70:begin quad; door(-55,30,60,s_p1); transenna(30);
                  finestra(70,50,20,20); end;
         71:begin dashes(-100,0); muro(0,100); end ;
         72:begin discesa(-70,30); dashes(-100,-40); muro(-40,40);
                  dashes(40,100); end;
         73:begin dashes(-100,-40); muro(-40,40); salita(70,30);
                  dashes(40,100); end;
         74:begin quad; discesa(-50,30); salita(50,30); door(-50,30,70,s_p1);
                  door(50,30,70,s_p2); end;
         75:begin quad; armadio(-50,30,60); caminetto(50); end;
         76:begin quad; door(25,35,70,s_p1); armadio(-45,25,60); end;
         77:begin quad; portasciapa(25,40,70); caminetto(-45); end;
         78:begin quad; door(0,36,70,s_p1); quadro(-70,50,20,20);
                  quadro(70,50,20,20); end;
         79:begin quad; salita(-50,30); door(-50,30,70,s_p1);
                  finesmura(50,50,30,30); end;
         80:begin quad; salita(-50,30); door(-50,30,70,s_p1); end;
         81:begin discesa(-70,30); dashes(-100,-40); dashes(40,100);
                  ringhiera(-40,40); end;
         82:begin dashes(-100,-40); salita(70,30);
                  dashes(40,100); ringhiera(-40,40); end;
         83:begin discesa(-70,30); dashes(-100,-40);
                  salita(70,30); dashes(40,100); ringhiera(-40,40); end;
         84:begin dashes(-100,-30); muro(-30,100); finestra(0,50,20,30);
                  finestra(60,50,20,30); end;
         85:begin quad; finestra(0,15,35,15); end;
         86:begin quad; grata(-10,50,30,30); scaffale(60,25,70); end;
         87:begin discesa(70,30); dashes(40,100); ringhiera(-100,40); end;
         88:begin quad; grata(-60,50,20,30); grata(0,50,20,30);
                  grata(60,50,20,30); end;
         89:begin quad; lavabo; end;
         90:begin quad; salita(50,40); door(50,40,70,s_p1); end;
         98:begin salita(0,100); freccia(0); end ;
         else Internal_Error('P:t:R','Parete',0) ; {->illegal parameter:'tipo':out of range }
      end; (* CASE tipo ... *)
      polygon3d(p) ;
      polygon3d(man) ;
end; {parete}

procedure DrawRoom ;
var i:byte;                       { visibile dall'esterno !! }
begin {drawroom}
     RoomBackground ;
     luogo_loc:=luogo^[stato.dovesei];
     stanza_loc:=stanza^[luogo_loc.n_stanza] ;
     RoomHeader ( stanza_loc.nome ) ;
     if g_soffitto in statoAUX.grafica then OutObject(-ord(stanza_loc.tipo)-1)
        else OutObject(0) ;
     if (p_stanze in statoAUX.prolix) and
        (PackTempo(stato.tempo)-stanza_loc.memoria> {***} 60 {***} ) and
        (stanza_loc.descrizione <> 0) then
             writeln(perg,frase^[stanza_loc.descrizione]^) ;
     { riscala }
     with luogo_loc, stanza_loc do begin
       loc_x:=sqrt(dim_x*typ_x);
       loc_y:=sqrt(dim_y*typ_y);
       loc_z:=sqrt(dim_z*typ_z);
     end ;
     SetViewPort (sx1+2,sy1+2, sx2-2,sy2-2, true ) ;
     SetColor(1) ;
     pavimento ;
     for i:=1 to 4 do parete(i);
     stanza^[luogo_loc.n_stanza].memoria := PackTempo(stato.tempo) ;
end;{drawroom}

procedure InitRoom ; { pubblica }
begin
     RoomBackground ;
     RoomHeader('');
end ;

Procedure DrawArrows ; { disegna la Rosa dei Venti }
begin
     RoomBackground ;
     RoomHeader('punti cardinali') ;
     SetViewPort (sx1+2,sy1+2, sx2-2,sy2-2, true ) ;
     setcolor(1) ;
     setlinestyle (solidln,0,3);
     line(coordX(-MX*0.7),coordY(0),coordX(MX*0.7),coordY(0)) ;
     line(coordX(0),coordY(-MY*0.65),coordX(0),coordY(MY*0.65)) ;
     circle(coordX(0),coordY(0),30);
     circle(coordX(0),coordY(0),40);
     line(coordX(MX*0.7),coordY(0),coordX(MX/2),coordY(MY/2));
     line(coordX(MX*0.7),coordY(0),coordX(MX/2),coordY(-MY/2));
     SetUserCharSize(1,1,1,1);
     SelectFont(10,0) ;
     setcolor(0) ;
     SetTextJustify(LeftText,CenterText);
     OutTextXY(coordX(MX*0.75),coordY(0),'N') ;
     SetTextJustify(RightText,CenterText);
     OutTextXY(coordX(-MX*0.75),coordY(0),'S') ;
     SetTextJustify(CenterText,CenterText);
     OutTextXY(coordX(0),coordY(MY*0.75),'W') ;
     SetTextJustify(CenterText,CenterText);
     OutTextXY(coordX(0),coordY(-MY*0.75),'E') ;
end ;


var i:integer ;

begin {inizializing code}
     sx1:=0; sy1:=0;
     sx2:=316; sy2:=187 ;
     sx3:=sx1+1; sy3:=sy2+2;
     sx4:=sx2-1; sy4:=198 ;
     kx := (sx2-sx1)/(2*MX) ;
     hx := MX*kx ;
     ky := (sy1-sy2)/(2*MY) ;
     hy := MY*ky+(sy2-sy1) ;
     for i:=0 to 90 do Asin[i]:=system.sin(i/180*pi);
     for i:=90 to 180 do Asin[i]:=Asin[180-i] ;
     for i:=180 to 359 do Asin[i]:=-Asin[360-i] ;
     for i:=0 to 359 do Acos[i]:=sin((i+90)/180*pi);
     PassedThrough:=0 ;
end . { unit 'room' }
