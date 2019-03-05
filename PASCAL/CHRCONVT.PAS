program CharConversionTable;
{costruisce la tavola di conversione per i caratteri da 8 a 7 bit}
{necessita della presenza di un operatore alla tastiera}

type ConvArray=array[128..255] of 0..127;

var Conversion:ConvArray;
    outfile:file of ConvArray;
    i:integer;
    ch:char;


begin
     for i:=128 to 255 do begin
       repeat
         write('Equivalente del carattere "',chr(i),'" (',i:3,') : ');
         read(Kbd,ch);
         writeln(ch);
         if ch = ^[ then i:=i-1;
       until ch <> ^[ ;
       Conversion[i]:=ord(ch)
     end;
     assign(outfile,'8to7bit.cvt');
     rewrite(outfile);
     write(outfile,Conversion);
     close(outfile)
end.
