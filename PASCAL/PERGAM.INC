{ SANCTI GEORGII CASTRUM - - finestra del narratore }

procedure DisegnaPergamena;
type  col=array[300..399] of byte;
var sx,dx:col;
    i:integer;
    infile:file of col;
begin
     setheaderoff;
     defineworld(3,0,99,639,0);
     selectworld(3);
     definewindow(3,0,300,79,399);
     selectwindow(3);
     SetBackground($ff);
     assign(infile,'scroll.byt');
     {$I-} reset(infile); {$I+}
     if ioresult <>0 then drawborder
        else begin
             read(infile,sx,dx);
             for i:=300 to 399 do begin
                 mem[grafbase:baseaddress[i]]:=sx[i];
                 mem[grafbase:baseaddress[i]+79]:=dx[i]
             end;
             SetColorBlack;
             DrawLine(8,93,632,93);
             SetColorWhite;
        end;
     close(infile)
end {DisegnaPergamena} ;

procedure ScrollUP(testo:WrkString);
var i,j:integer;
    temp:array[0..20,1..39]of integer;
begin
     fillchar(temp,1638,$ff);
     for i:=1 to length(testo) do
         for j:=0 to 19 do temp[j,i]:=CharSet20x16^[ord(testo[i]),j] xor $ffff;
     for i:=0 to 20 do begin
         for j:=8 to 90 do move( mem[grafbase:baseaddress[j+301]+1], mem[grafbase:baseaddress[j+300]+1],78);
         move(temp[i],mem[grafbase:baseaddress[391]+1],78);
     end;
end;
