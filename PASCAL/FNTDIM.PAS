program WriteFontDimensions ;

uses drivers,graph ;

type chrrow = array [0..1] of byte ;
     character = array[0..19] of chrrow ;
     bigfnt = array [char] of character ;
     widths = array [char] of byte ;
     pergamena = array[301..399,0..79]of byte ;

var font : ^bigfnt ;
    ScrollImg :^pergamena ;
    Wchars : widths ;


function lit(c:char; i,j:integer):boolean ;
var t:chrrow ;
    b:byte ;
    off:integer ;
begin
     t := font^[c,j] ;
     if (i<8) then begin
        off:=0 ;
        b:=t[0] ;
     end else begin
        off:=8 ;
        b:=t[1] ;
     end;
     lit := ( (b shl (i-off)) and $80 ) <> 0 ;
end ; {lit}

procedure bigfont ; external ;
{$L 20x16.obj }

var outfile : file of widths ;
    i,j,w:integer ;
    c:char ;
begin
     font := @ bigfont ;
     { determina le larghezze }
     for c:= chr(0) to chr(255) do begin
         w:=-1 ;
         for j:=15 downto 0 do
             if (j>w) then
                for i:=0 to 19 do
                    if lit(c,j,i) then w:=j ;
         Wchars [c] := w+2 ;
     end ;
     Wchars [' '] := 9 ;
     assign(outfile,'fntdim.byt') ;
     rewrite(outfile) ;
     write(outfile,Wchars) ;
     close(outfile) ;
end .