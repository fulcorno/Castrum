unit spooler ;
{ Text Device Driver per la stampante. Permette di stampare un testo in
  quasi-background: quando viene mandato il testo alla stampante, se lo
  spooler Š abilitato ed il buffer non Š pieno il testo viene memorizzato.
  Verr… poi stampato effettivamente solo pi— tardi, quando la procedura
  SPOOL verr… richiamata, e la stampa continua fino alla pressione di un
  tasto, o fino alla prima pausa prolungata della stampante.
  N.B. da usare IN ALTERNATIVA a "unit Printer"  }

interface

uses Dos, crt ;

var Lst:text ; { TDD collegato alla stampante }

procedure InitSpooler(BufSize:word; MaxDelay:integer) ;
{ BufSize: dimensioni in bytes del buffer. MaxDelay: tempo massimo disponibile
  dallo spooler se nessun tasto viene premuto prima di ritornare al chiamante
  (in millisecondi). Se MaxDelay=0 aspetta indefinitamente.
  Per Default ho un buffer di 10K e attesa 1ms. }

procedure Spool ;
{ manda i dati sulla stampante, fino allo svuotamento del buffer, alla
  pressione di un tasto o allo scadere di MaxDelay. }

IMPLEMENTATION

type superarray=array[0..65534] of char ;
var Pbuffer:^superarray ;
    size, head, tail:word ;
    Mdelay:integer ;

function PrnStat : byte ;
var r:registers ;
begin
     r.ah:=2 ;
     r.dx:=0 ;
     intr($17,r) ;
     PrnStat:=r.ah ;
end ;

procedure OutCh(c:char) ;
var r:registers ;
begin
     r.ah:=0 ;
     r.dx:=0 ;
     r.al:=ord(c) ;
     intr($17,r) ;
end ;

procedure incr(var w:word);
begin
     inc(w);
     if w>=size then dec(w,size) ;
end ;

function EOB : boolean;  { EndOfBuffer <==> tail+1 = head }
var w:word ;
begin
     w:=tail ;
     incr(w);
     EOB := (w=head) ;
end ;

{$F+} { TextDeviceDriver }

function PrnOut(var F:TextRec):integer ;
var p:word ;
begin
     with F do begin
          p:=0 ;
          if BufPos<>0 then repeat
              Pbuffer^[tail]:=bufptr^[p] ;
              incr(tail) ;
              inc(p) ;
              if EOB then incr(head) ; {registra solo gli ultimi 'size' chrs}
          until (p=bufPos) ;
          BufPos:=0 ;
          PrnOut:= 0 ;
     end
end ;

function DoNothing(var f:textrec):integer ;
begin
     DoNothing:=0
end ;

function PrnOpen(var F:TextRec):integer ;
begin
     with f do begin
          if mode=fmInput then PrnOpen:=12
          else begin
               mode:=fmOutput ;
               PrnOpen:=0 ;
          end
     end
end ;

{$F-}

procedure PrnAssign(var F:text);
begin
     with TextRec(F) do begin
          mode:=fmClosed ;
          bufsize:=sizeof(buffer) ;
          bufptr:=@buffer ;
          OpenFunc:=@PrnOpen ;
          CloseFunc:=@DoNothing ;
          InOutFunc:=@PrnOut ;
          FlushFunc:=@PrnOut ;
          name[0]:= #0 ;
     end
end ;

Procedure SPOOL ; { pubblica }
var count:integer ;
begin
     count:=0 ;
     while (not keypressed)and(count<=Mdelay)and(head<>tail) do begin
           if (PrnStat and $80)<>0 then begin
              OutCh(Pbuffer^[head]) ;
              incr(head) ;
              count:=0;
           end else if Mdelay<>0 then begin
               delay(1) ;
               inc(count) ;
           end ;
     end ;
end ;

procedure InitSpooler(BufSize:word; MaxDelay:integer) ;
begin
     if size<>0 then FreeMem(Pbuffer,size) ; { de-alloca }
     size:=BufSize ;
     GetMem(Pbuffer,size) ;
     Mdelay:=MaxDelay ;
     head:=0 ;
     tail:=0 ;
end ;

begin { init code }
     size:=0 ;
     Pbuffer:=NIL ;
     PrnAssign(Lst);
     rewrite(lst);
     InitSpooler(10240,1) ; { defaults }
end .