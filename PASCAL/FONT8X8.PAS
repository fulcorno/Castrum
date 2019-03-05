program SaveFont8x8;
{copia i caratteri 8 x 8 dalla ROM nel file '8x8.FON'}

type CharArray8x8=array[byte] of array[0..7] of byte;

var chars:CharArray8x8;
    outfile: file of CharArray8x8;
    i,c:integer;

begin
     for c:=0 to 127 do
         for i:=0 to 7 do
             chars[c,i]:=mem[$f000:$fa6e + c shl 3 + i ];
     for c:=128 to 255 do
         for i:=0 to 7 do
             chars[c,i]:=mem[$09dd:$108d + (c-128) shl 3 +i ];
     assign(outfile,'8x8.fon');
     rewrite(outfile);
     write(outfile,chars);
     close(outfile)
end.