program insert ;
{ usage:
  ***> insert <icon name>[.icn] [pos#]
  description:
  ***> 1) <pos#> assente: aggiunge l'icona alla fine di icons.byt
       2) <pos#> presente: pone l'icona alla posizione specificata,
          sostituendo quella presente.
}

type icon = array [0..27,0..3] of byte ;

var ic, tot: file of icon ;
    nic : string ;
    n,e : integer ;
    tmp : icon ;

begin
     if paramCount<1 then begin
        writeln('usage: insert <icon name>[.icn] [pos#]');
        halt(255);
     end ;
     nic := ParamStr(1) ;
     if pos('.',nic)=0 then nic:=nic+'.icn' ;
     assign(ic,nic); reset(ic);
     assign(tot,'icons.byt'); reset(tot);
     if paramcount>1 then begin
        val(ParamStr(2),n,e);
        if e=0 then begin
           seek(tot,n-1) ;
           writeln('Positioned at ',n);
        end else begin
             writeln('Error: <',paramstr(2),'> unrecognised');
             halt(1);
        end ;
     end else begin
         seek(tot,filesize(tot)) ;
         writeln('Positioned at END');
     end ;
     writeln('Reading ',nic) ;
     read(ic,tmp) ;
     writeln('Writing to icons.byt');
     write(tot,tmp);
     close(ic);
     close(tot);
end.