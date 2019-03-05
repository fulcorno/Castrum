program tastiera;
{serve per conoscere il valore associato ai vari tasti dal turbopascal}


procedure caratt;
var ch:char;
begin
     read(kbd,ch);
     case ch of
          #0..#26:write('^',chr(ord(ch)+64),' ');
          #28..#31:write('^',chr(ord(ch)+64),' ');
          #32..#126:write(ch,' ');
          #127:write('DEL ');
          #128..#159:write('|^',chr(ord(ch)-64),' ');
          #160..#255:write('|',chr(ord(ch)-128),' ');
     end {case};
     if (ch =^[) and keypressed then begin
                                          caratt;
                                          write ('ESC + ')
                                          end
        else write(ord(ch),' ');
end;

begin
     repeat
     caratt;
     writeln;
     until false;
end.
