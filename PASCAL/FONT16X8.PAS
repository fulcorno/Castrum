program SaveFont16x8;
{copia i caratteri 16 x 8 dalla ROM nel file '16x8.FON'}

type CharArray16x8=array[byte] of array[0..15] of byte;

var chars:CharArray16x8;
    outfile: file of CharArray16x8;
    i,c:integer;

begin
     for c:=0 to 127 do
         for i:=0 to 15 do
             chars[c,i]:=mem[$09dd:$149c + c shl 4 + i ];
     for c:=128 to 255 do
         for i:=0 to 15 do
             chars[c,i]:=mem[$09dd:$1c9e + (c-128) shl 4 +i ];
     assign(outfile,'16x8.fon');
     rewrite(outfile);
     write(outfile,chars);
     close(outfile)
end.