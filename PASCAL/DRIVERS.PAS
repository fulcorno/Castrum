unit drivers ;
{ carica i fonts, il driver att400 e tutti i dati e le proceudre di
  uso generale per la gestione della grafica e della tastiera.
  Implementa l'emulazione 640*400 tramite 640*200. I risultati sono
  ovviamente penosi... }

INTERFACE
Uses crt, graph ;

VAR BAdr:array [0..399] of word ; { indirizzi delle 400 righe dello
                                    schermo grafico                  }
    _8to7:array [char] of char ; { tabella di conversione per eliminare
                                   le lettere accentate e i caratteri >=$80 }

type FillIndex = ( nero, G1, G2, G3, G4, G5, G6, G7, G8, G9, G10,
                   G11, G12, G13, G14, G15, bianco, grigio, rombi,
                   quadri, mattoni, lisca, quadrombi, esagoni,
                   cerchi, frecce, lago, rombi2, erba, diagonal,
                   quadri2, zigzag, pianella, parquet, righe, palle,
                   ottagon, quadri3, foglie, fiori, bracci, dummy ) ;

const Fills : array [ FillIndex ] of FillPatternType = (
              ( $00, $00, $00, $00, $00, $00, $00, $00 ),  { nero }
              ( $00, $44, $00, $00, $00, $44, $00, $00 ),  { G1 }
              ( $00, $44, $22, $00, $00, $44, $22, $00 ),  { G2 }
              ( $00, $44, $66, $00, $00, $44, $66, $00 ),  { G3 }
              ( $00, $66, $66, $00, $00, $66, $66, $00 ),  { G4 }
              ( $11, $66, $66, $00, $11, $66, $66, $00 ),  { G5 }
              ( $88, $66, $66, $11, $88, $66, $66, $11 ),  { G6 }
              ( $99, $66, $66, $88, $99, $66, $88, $88 ),  { G7 }
              ( $99, $66, $66, $99, $99, $66, $66, $99 ),  { G8 }
              ( $99, $66, $66, $dd, $99, $66, $66, $dd ),  { G9 }
              ( $bb, $66, $66, $dd, $bb, $66, $66, $dd ),  { G10 }
              ( $bb, $66, $ee, $dd, $bb, $66, $ee, $dd ),  { G11 }
              ( $bb, $77, $ee, $dd, $bb, $77, $ee, $dd ),  { G12 }
              ( $bb, $77, $ff, $dd, $bb, $77, $ff, $dd ),  { G13 }
              ( $bb, $ff, $ff, $dd, $bb, $ff, $ff, $dd ),  { G14 }
              ( $ff, $ee, $ff, $ff, $ff, $ee, $ff, $ff ),  { G15 }
              ( $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff ),  { bianco }
              ( $aa, $55, $aa, $55, $aa, $55, $aa, $55 ),  { grigio }
              ( $77, $aa, $dd, $aa, $77, $aa, $dd, $aa ),  { rombi }
              ( $db, $bd, $42, $db, $db, $42, $bd, $db ),  { quadri }
              ( $00, $7f, $7f, $7f, $00, $f7, $f7, $f7 ),  { mattoni }
              ( $fb, $2a, $ef, $a8, $bf, $a2, $fe, $8a ),  { lisca }
              ( $bb, $bb, $38, $d7, $ef, $d7, $38, $bb ),  { quadrombi }
              ( $7f, $7f, $be, $c9, $f7, $f7, $c9, $be ),  { esagoni }
              ( $c7, $bb, $45, $55, $45, $bb, $c7, $ff ),  { cerchi }
              ( $d6, $ba, $7c, $ee, $d6, $ba, $7c, $ee ),  { frecce }
              ( $ff, $bf, $47, $7b, $ff, $fb, $f5, $ce ),  { lago }
              ( $7e, $bd, $db, $e7, $e7, $db, $bd, $7e ),  { rombi2 }
              ( $ff, $fb, $b7, $b7, $d5, $e5, $83, $e7 ),  { erba }
              ( $f8, $f1, $e3, $c7, $8f, $1f, $3e, $7c ),  { diagonal }
              ( $ef, $c7, $83, $01, $83, $c7, $ef, $ff ),  { quadri2 }
              ( $18, $3c, $7e, $ff, $ff, $e7, $c3, $81 ),  { zigzag }
              ( $7f, $7f, $7f, $7f, $7f, $7f, $7f, $00 ),  { pianella }
              ( $a0, $af, $a0, $af, $a0, $af, $a0, $af ),  { parquet }
              ( $cc, $cc, $cc, $cc, $cc, $cc, $cc, $cc ),  { righe }
              ( $fe, $c6, $82, $82, $82, $c6, $fe, $00 ),  { palle }
              ( $c1, $be, $7f, $7f, $7f, $7f, $7f, $be ),  { ottagon }
              ( $f0, $f0, $f0, $f0, $0f, $0f, $0f, $0f ),  { quadri3 }
              ( $f1, $e1, $c1, $c3, $87, $9f, $bf, $bf ),  { foglie }
              ( $ff, $ef, $c7, $ab, $01, $ab, $ef, $c7 ),  { fiori }
              ( $ff, $ab, $ab, $c7, $6d, $6d, $ab, $c7 ),  { bracci }
              ( 0,0,0,0,0,0,0,0 )                          { dummy }
           ) ;

PROCEDURE InitGraphScreen(mode:byte) ;  { attiva il modo grafico
                                  mode=1 --> 640*400
                                  mode=2 --> 640*200
                                  mode=0 --> autodetect   }

PROCEDURE LoadFont(n:integer; path:string); { rende residente il font }

Procedure SelectFont(n,size:integer);  { fa una SetTextStyle, eventualmente
                                         caricando prima il font }

procedure scompari(time:word) ;    { generano la dissolvenza da schermo }
procedure compari(time:word) ;     { nero a schermo bianco e v/v        }

{ DUPLICAZIONE DELLE PROCEDURE DI Graph.TPU PER EMULARE LE 400 RIGHE }

function  GetY : integer;
function  GetMaxY : integer;

procedure SetViewPort(x1, y1, x2, y2 : integer; Clip : boolean);

procedure PutPixel(X, Y : integer; Pixel : word);
function  GetPixel(X, Y : integer) : word;

procedure LineTo(X, Y : integer);
procedure LineRel(Dx, Dy : integer);
procedure MoveTo(X, Y : integer);
procedure MoveRel(Dx, Dy : integer);
procedure Line(x1, y1, x2, y2 : integer);

procedure Rectangle(x1, y1, x2, y2 : integer);
procedure Bar(x1, y1, x2, y2 : integer);

procedure DrawPoly(NumPoints : word; var PolyPoints);
procedure FillPoly(NumPoints : word; var PolyPoints);

procedure FloodFill(X, Y : integer; Border : word);

procedure Circle(X, Y : integer; Radius : word);
procedure Ellipse(X, Y : integer;
                  StAngle, EndAngle : word;
                  XRadius, YRadius  : word);

procedure PutImage(X, Y : integer; var BitMap; BitBlt : word);

procedure OutTextXY(X, Y : integer; TextString : string);
procedure SetUserCharSize(MultX, DivX, MultY, DivY : word);
function  TextHeight(TextString : string) : word;

IMPLEMENTATION

const fonts:array[1..10]of string[4] =
      ( 'trip', 'litt', 'sans', 'goth', 'bold',
        'euro', 'lcom', 'scri', 'simp', 'tscr' ) ;
var loaded:array[0..10]of boolean ;
    table:array[0..10]of integer ;
    emulation:boolean ;

procedure ATT ; external; {$L att.obj}

function  GetY : integer;
begin
     if emulation then GetY:=2*graph.gety
                  else gety:=graph.gety
end ;

function  GetMaxY : integer;
begin
     if emulation then GetMaxY:=2*graph.getmaxy
                  else getMaxY:=graph.getmaxy
end ;

procedure SetViewPort(x1, y1, x2, y2 : integer; Clip : boolean);
begin
     if emulation then graph.SetViewPort(x1,y1 div 2,x2,y2 div 2,clip)
                  else graph.setviewport(x1,y1,x2,y2,clip)
end ;

procedure PutPixel(X, Y : integer; Pixel : word);
begin
     if emulation then graph.putpixel(x,y div 2,pixel)
                  else graph.putpixel(x,y,pixel)
end ;

function  GetPixel(X, Y : integer) : word;
begin
     if emulation then getpixel:=graph.getpixel(x,y div 2)
                  else getpixel:=graph.getpixel(x,y)
end ;

procedure LineTo(X, Y : integer);
begin
     if emulation then graph.lineto(x,y div 2)
                  else graph.lineto(x,y)
end;

procedure LineRel(Dx, Dy : integer);
begin
     if emulation then graph.linerel(dx,dy div 2)
                  else graph.linerel(dx,dy)
end ;

procedure MoveTo(X, Y : integer);
begin
     if emulation then graph.moveto(x,y div 2)
                  else graph.moveto(x,y)
end;

procedure MoveRel(Dx, Dy : integer);
begin
     if emulation then graph.moverel(dx,dy div 2)
                  else graph.moverel(dx,dy)
end ;

procedure Line(x1, y1, x2, y2 : integer);
begin
     if emulation then graph.line(x1,y1 div 2,x2,y2 div 2)
                  else graph.line(x1,y1,x2,y2)
end ;

procedure Rectangle(x1, y1, x2, y2 : integer);
begin
     if emulation then graph.rectangle(x1,y1 div 2,x2,y2 div 2)
                  else graph.rectangle(x1,y1,x2,y2)
end;

procedure Bar(x1, y1, x2, y2 : integer);
begin
     if emulation then graph.bar(x1, y1 div 2, x2,y2 div 2)
                  else graph.bar(x1,y1,x2,y2)
end ;

var buffer:pointer ;

procedure dimezzaY(numpoints:word; var polypoints) ;
type poly=array[0..1000]of PointType ;
var i:integer ;
begin
  if numpoints<=100 then
     for i:=0 to numpoints do begin
         poly(buffer^)[i].y:=poly(polypoints)[i].y div 2 ;
         poly(buffer^)[i].x:=poly(polypoints)[i].x ;
     end ;
end ;

procedure DrawPoly(NumPoints : word; var PolyPoints);
begin
     if emulation then begin
        dimezzaY(numpoints,polypoints) ;
        graph.drawpoly(numpoints,buffer^)
     end else graph.drawpoly(numpoints,polypoints)
end;

procedure FillPoly(NumPoints : word; var PolyPoints);
begin
     if emulation then begin
        dimezzaY(numpoints,polypoints) ;
        graph.fillpoly(numpoints,buffer^)
     end else graph.fillpoly(numpoints,polypoints)
end;

procedure FloodFill(X, Y : integer; Border : word);
begin
     if emulation then graph.floodfill(x,y div 2,border)
                  else graph.floodfill(x,y,border)
end;

procedure Circle(X, Y : integer; Radius : word);
begin
     if emulation then graph.circle(x, y div 2,radius)
                  else graph.circle(x,y,radius)
end;

procedure Ellipse(X, Y : integer; StAngle,EndAngle:word; XRadius,YRadius:word);
begin
     if emulation then graph.ellipse(x,y div 2,stangle,endangle,xradius,yradius div 2)
                  else graph.ellipse(x,y,stangle,endangle,xradius,yradius)
end;

procedure PutImage(X, Y : integer; var BitMap; BitBlt : word);
type img=record
        dx,dy:word;
        data:array[0..30000]of byte ;
     end;
var p:^img;
    sz,rs:word ;
    i,j:integer ;
begin
     if not emulation then graph.putimage(x,y,bitmap,bitblt)
        else begin
             sz:=4+img(bitmap).dy*img(bitmap).dx div 8 + 100 ;
             getmem(p,sz);
             p^.dx:=img(bitmap).dx ;
             p^.dy:=img(bitmap).dy div 2;
             rs:=p^.dx div 8 + 1 ;
             for i:=0 to p^.dy do begin
                 move(img(bitmap).data[2*i*rs],p^.data[i*rs],
                      rs ) ;
                 if (rs=4)and(img(bitmap).dy=27)and(bitblt=NotPut) { le icone }
                    then for j:=0 to rs-1 do
                         p^.data[i*rs+j]:=p^.data[i*rs+j] OR
                                          img(bitmap).data[(i+i+1)*rs+j] ;
             end;
             graph.putimage(x,y div 2,p^,bitblt);
             freemem(p,sz);
     end
end;

procedure OutTextXY(X, Y : integer; TextString : string);
begin
     if emulation then graph.outtextxy(x,y div 2,textstring)
                  else graph.outtextxy(x,y,textstring)
end;

procedure SetUserCharSize(MultX, DivX, MultY, DivY : word);
begin
     if emulation then graph.setusercharsize(multx,divx,multy,divy*2)
                  else graph.setusercharsize(multx,divx,multy,divy)
end;

function  TextHeight(TextString : string) : word;
begin
     if emulation then textheight:=2*graph.textheight(textstring)
                  else textheight:=graph.textheight(textstring)
end;

{ fine emulazione }

PROCEDURE InitGraphScreen(mode:byte) ;   { pubblica }
var gd,gm,y:integer ;
begin
     if (mode=1)or(mode=0) then begin
        gd:=att400 ;  gm:=att400hi ;  {640*400}
        InitGraph(gd,gm,'');
        emulation:=false;
        for Y:=0 to 399 do BAdr[y] :=(Y and 3) shl 13 + 80*(Y shr 2) ;
     end ;
     if (GraphResult<0)or(mode=2) then begin
        for y:=0 to 399 do BAdr[y]:= ((y div 2)and 1)shl 13+
            ((y div 2)and -2)shl 5+((y div 2)and -2)shl 3 ;
        emulation := true ;
        gd:=att400 ; gm:=att400Med ;  {640*200}
        InitGraph(gd,gm,'');
     end ;
end ; { InitGraphScreen }

procedure LoadFont(n:integer; path:string); { pubblica }
var w:longint ;
    ptr:pointer ;
    f:file ;
begin
     loaded[n]:=true ;
     assign(f,path+fonts[n]+'.chr');
     reset(f,1) ;
     w:=FileSize(f);
     GetMem(ptr,w);
     BlockRead(f,ptr^,w);
     close(f);
     table[n]:=RegisterBGIfont(ptr) ;
end ; { LoadFont }

Procedure SelectFont(n,size:integer); { pubblica }
begin
     if not(loaded[n]) then LoadFont(n,'');
     SetTextStyle(table[n],HorizDir,size);
end; { SelectFont }

procedure compari(time:word) ;  { pubblica }
begin
     setbkcolor(8);
     delay(time);
     setbkcolor(7);
     delay(time);
     setbkcolor(15);
end ;

procedure scompari(time:word) ; { pubblica }
begin
     setbkcolor(7);
     delay(time);
     setbkcolor(8);
     delay(time);
     setbkcolor(0);
end ;

var y:integer ;
type aoc=array[1..64]of char;
const s1:aoc='CueaaaaceeeiiiAAEeEooouuyoUcLYPfaiounN^^?--24!<>***||||\\||\///\';
      s2:aoc='\--|-+||\/--|-+----\\//++//*****abcpSsutFTOd#0e^=~><||-~^..Vn2* ';

begin { init code }
     y:=registerBGIdriver(@att) ;
     for y:=0 to 31 do _8to7[chr(y)]:=' ' ;
     for y:=32 to $7f do _8to7[chr(y)]:=chr(y) ;
     for y:=1 to 64 do begin
         _8to7[chr(127+y)]:=s1[y];
         _8to7[chr(191+y)]:=s2[y];
     end;
     for y:=1 to 10 do loaded[y]:=false ;
     loaded[0]:=true ;
     for y:=0 to 10 do table[y]:=0;
     GetMem(buffer,100*sizeof(pointType)) ;
end .  { unit DRIVERS }
