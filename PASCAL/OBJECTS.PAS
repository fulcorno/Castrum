unit Objects ;
{ gestisce la "finestra dell'oggetto" }

INTERFACE

uses graph, drivers, globals ;

Procedure OutObject(n:integer) ; { disegna l'oggetto specificato nella
                                   finestra, eventualmente caricando
                                   l'immagine da disco                }

IMPLEMENTATION

type obj=array[0..89,0..19] of byte ;
     soffitti=array[t_soffitto] of obj ;
     pobj=^obj ;

const sx=0 ; sy=200 ;
      dx=159 ; dy=89 ;
      sy1=291 ; dy1=8 ;

var pt:pobj ;
    p_soffitti : ^ soffitti ;

procedure Header(s:string) ;
var i:integer ;
begin
     SetViewPort(sx,sy1,sx+dx,sy1+dy1,true) ;
     SetFillStyle(SolidFill,1) ;
     bar(1,0,dx-1,dy1) ;
     SetColor(0) ;
     PutPixel(1,0,0) ;
     PutPixel(dx-1,0,0) ;
     PutPixel(1,dy1,0) ;
     PutPixel(dx-1,dy1,0) ;
     setusercharsize(1,1,1,1) ;
     SelectFont(SmallFont, usercharsize ) ;
     SetTextJustify( CenterText, CenterText ) ;
     for i := 1 to length(s) do s[i]:=upcase(_8to7[s[i]]) ;
     if TextWidth(s)>dx-4 then Internal_Error('P:s:>','Header',length(s)) ;
     OutTextXY(dx div 2, dy1 div 2-1, s)
end ;

procedure display(p:pobj; s:string) ;
var i:integer ;
begin
     for i:= 0 to dy do
         move(p^[i], ptr($b800,Badr[sy+i]+ (sx shr 3))^, 20) ;
     header(s) ;
end ;

{ ****** PROVVISORIO: RISCRIVERE ***** }

procedure OutObject(n:integer) ;
var i:integer ;
begin
     if (n>=-12)and(n<=-1) then display( addr(p_soffitti^[t_soffitto(-n-1)]),
                                   n_soffitto[t_soffitto(-n-1)] )
     else display(pt,'Marchio "JENA-SOFT"') ;
end ;

{ ****** PROVVISORIO: ELIMINARE ******** }

procedure read_IT;
var fl:file of obj ;
    i,j:integer ;
begin
     new(pt) ;
     assign(fl,'marchio2.ogg');
     reset(fl);
     read(fl,pt^);
     close(fl);
     for i:=0 to 19 do for j :=0 to 89 do pt^[j,i]:=not pt^[j,i] ;
     new(p_soffitti);
     assign(fl,'soffitti.byt');
     reset(fl);
     for i:=0 to 11 do
         read(fl,p_soffitti^[t_soffitto(i)]) ;
     close(fl);
end ;

begin
     read_it ;
end .
