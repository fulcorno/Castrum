unit scroll ;

{ procedure per scrivere nella pergamena con caratteri gotici }

INTERFACE

uses graph,globals,dos,drivers,printer ;

procedure InitPergam ;  { inizializza la finestra }

var perg:text ; { TextDeviceDriver della pergamena }

implementation

const Lpos = 4 ;
      BIM = #27'L' ;

type chrrow = array [0..1] of byte ;       { 1 riga scansione = 16 bit }
     character = array[0..19] of chrrow ;  { 1 carattere = 20 righe scansione }
     bigfnt = array [char] of character ;  { font = 256 caratteri }
     widths = array [char] of 0..16 ;      { larghezza effettiva di ogni carattere }
     pergamena = array[301..399,0..79]of byte ;  { bitmap }

var font : ^bigfnt ;          { puntatori alle strutture linkate insieme }
    ScrollImg :^pergamena ;
    Wchars : ^widths ;
    sx1,sy1,           { TL pergamena }
    sx2,sy2,           { BR pergamena }
    sx3,sy3,           { TL gotici (in CARATTERI) }
    sx4,sy4:integer ;  { BR gotici (   "   "    ) }

{ Text Device Driver per la pergamena }

var buffer : array[0..21,0..79] of byte ; { alto 20+1 pixel+ 1 riga bianca }
    posB, posP : byte ;
    s:string ;

procedure send;
 { scrolla in su di una riga di 20+1 pixel mostrando il contenuto di "buffer" }
var i,j,i1,i2,grf:integer ;
    b,v,k1,k2,b1:byte ;
begin
     if (statoAUX.stampante=s_solotxt)or
        (statoAUX.stampante=s_graftxt)then writeln(lst);
         if badr[0]=badr[1] then begin
            grf:=2;
            for i:=0 to 10 do for j:=sx3 to sx4 do
                buffer[i*2,j]:=buffer[i*2,j] AND buffer[i*2+1,j] ;
         end else grf:=1 ;
         for i := 0 to 20 div grf do begin
             for j := sy3 to sy4-1 do
                 move(mem[$B800:badr[j+1]+sx3],
                      mem[$b800:badr[j]+sx3], sx4-sx3+1) ;
                 move(buffer[i*grf,sx3], mem[$b800:badr[sy4]+sx3], sx4-sx3+1) ;
         end ; {for i}
         if (statoAUX.stampante=s_sologoth)or
            (statoAUX.stampante=s_grafgoth) then begin
              for b:=0 to 2 do begin
                  b1:=b*8 ;
                  write(lst,#27'3'#24);
                  write(lst,BIM,chr((posB-sx3+1)*8 mod 256),
                            chr((posB-sx3+1)*8 div 256)) ;
                  for i1:=sx3 to posB do begin
                    k1:=$80;
                    for i2:=0 to 7 do begin
                      v:=0 ;
                      k2:=$80 ;
                      for j:=b1 to b1+7 do begin
                          if (j<=20) and ((buffer[j,i1] and k1)=0)
                             then v:=v or k2 ;
                          k2:=k2 shr 1 ;
                      end ;
                      write(lst,chr(v)) ;
                      k1:=k1 shr 1 ;
                    end ;
                  end ;
                  writeln(lst);
              end ;
              write(lst,#27'2');
         end;
         posP:=lpos ;
         posB:=sx3 ;
         fillchar(buffer,sizeof(buffer), $ff);
end; {send}

procedure OutCh(c:char) ;
var w,j:byte ;
    shape:character ;
    t:chrrow ;
type bufrow=array[0..79]of byte ;
var p:^bufrow ;

begin
       w:= Wchars^[c] ; { sua larghezza }
       shape:=font^[c] ;{ sua immagine }
       { aggiungi il carattere alla posP:posB }
       for j := 0 to 19 do begin
         t:=shape[j] ;
         p:=addr(buffer[j]) ;
         { sovrappongo l'immagine }
         p^[posB] := buffer[j,posB] and (not (t[0] shr posP)) ;
         p^[posB+1] := buffer[j,posB+1] and
                    (not lo( ( t[0] shl (8-posP) ) or ( t[1] shr posP ) )) ;
         p^[posB+2] := buffer[j,posB+2] and (not lo(t[1] shl (8-posP))) ;
       end ; {for j}
       inc(posP,w) ;          { sposta il cursore }
       inc(posB,posP shr 3);
       posP:=posP and 7 ;
end;

{$F+}  { richiesto dal Turbo4 }

function ScrollOpen(var F:textRec):integer ;
begin
     with F do begin
          if mode = fmInput then ScrollOpen:=104
          else begin
               mode:=fmOutput ;
          end ;
     end ;
     { inizializza il buffer }
     fillchar (buffer, sizeof(buffer), $ff) ;
     { inizializza il cursore }
     posB:=sx3;
     posP:=lpos;   { 1^ pixel = sx3*8 + lpos }
     s:='';
     ScrollOpen:=0;
end ;

function ScrollOut (var F:textRec) :integer ;
var i,j,ls,ct:integer ;
    c:char ;
begin
   with F do begin
      for i:= 0 to BufPos-1 do begin
          c:=BufPtr^[i] ;
          if (statoAUX.stampante=s_solotxt)
             or(statoAUX.stampante=s_graftxt) then write(lst,c) ;
          if (c=' ')or(c=#13) then begin
                      ls:=length(s) ;
                      ct:=0;
                      for j:=1 to ls do inc(ct,Wchars^[s[j]]) ;
                      if (posB*8+posP+ct)>sx4*8-lpos then send ;
                      for j:=1 to ls do OutCh(s[j]) ;
                      outch(' ');
                      s:='';
          end ;
          if c=#13 then begin
                        send;
                        posB:=sx3;
                        posP:=lpos;
          end ;
          if not (c in [#10,#13,#32]) then s:=s+c ;
      end ; {for}
      BufPos:=0 ;
   end ; {with}
   ScrollOut:=0;
end ;

{$F-}

procedure AssignPerg(var F:text) ;
begin
     with TextRec(F) do begin
          mode:=fmClosed ;
          BufPtr:=@Buffer ;
          BufSize:=sizeof(Buffer) ;
          OpenFunc:= @ ScrollOpen ;
          InOutFunc:= @ ScrollOut ;
          FlushFunc := @ ScrollOut ;
          CloseFunc := @ ScrollOut ;
          name[0]:= #0 ;
     end ;
end ;

procedure InitPergam ;
{ mostra l'immagine di copyright }
var y,x:integer ;
begin
     for y:=sy1 to sy2 do
         move(Scrollimg^[y],ptr($B800,Badr[y])^,80) ;
     if badr[0]=badr[1] then for y:=sy1 to sy2 do
        if not odd(y) then for x:=0 to 79 do
           mem[$b800:badr[y]+x]:=mem[$b800:badr[y]+x] and scrollimg^[y,x] ;
end ;

procedure bigfont ; external ;
procedure pergIMG ; external ;
procedure FntDim ; external ;
{$L 20x16.obj }
{$L pergam.obj }
{$L fntdim.obj }

begin {init code}
     { Definisce le dimensioni delle finestre }
     sx1 := 0 ;   sy1 := 301 ;       { TL pergamena }
     sx2 := 639;  sy2 := 399 ;       { BR     "     }
     sx3 := 2 ;   sy3 := sy1+6 ;     { TL scritte }
     sx4 := 77 ;  sy4 := sy2-9 ;    { BR    "    }
     { attiva la finestra }
     ScrollImg := @ pergIMG ;
     { attiva il font gotico }
     font := @ bigfont ;
     Wchars := @ FntDim ;
     { attiva il TDD }
     AssignPerg(perg) ;
     rewrite(perg);
end . { unit scroll }
