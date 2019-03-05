program CartaMillimetrataCustom;

type regs=record case integer of
                 1:(AX,BX,CX,DX,BP,SI,DI,DS,ES,Flags:integer);
                 2:(AL,AH,BL,BH,CL,CH,DL,DH         :byte);
          end; {record}

procedure outch(c:char);
{Stampa un carattere <== SE E SOLO SE ==> la stampante Š pronta}
var rg:regs;
    ok:boolean;
begin
  repeat
     with rg do begin
          ah:=2;
          dx:=0;
          intr($17,rg);
          if (ah=$90) then begin
                      al:=ord(c);
                      ah:=0;
                      intr($17,rg);
                      ok:=true;
          end else ok:=false;
     end;
  if keypressed then halt;
  until ok;
end;

function lit(x,y:integer):boolean;
begin
     lit:=false;
     if (x mod 16 =0) or (y mod 12 =0) then lit:=true;
     if (x mod 4 =0) and (y mod 3 <=1) then lit:=true;
     if (y mod 3 =0) and (x mod 4 <=2) then lit :=true;
end;

var x,y,i,j,v,row,col:integer;

begin
     lstoutptr:=ofs(outch);
     write(lst,^['3'#24);
     for row:=0 to 33 do begin
         write(lst,^['L',chr(lo(641)),chr(hi(641)));
         for col:=0 to 640 do begin
             v:=0;
             for j:=0 to 7 do begin
                 x:=col;
                 y:=row shl 3 + 7 - j ;
                 if lit(x,y) and (y<=270)
                 then v:=v or (1 shl j);
             end;
             write(lst,chr(v));
         end;
         writeln(lst);
     end;
     writeln(lst,^['2');
end.
