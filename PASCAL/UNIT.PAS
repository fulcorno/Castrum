Program _Unit ;

{ Manda sull'Output standard la parte 'interface' dell'unit specificata,
  tralasciando l'implementation. Descrizione:

SINTASSI
    UNIT  [-[d][t][c][n]]  [name[.PAS]] ...
          switches ^         ^ units da listare

Significato degli switches:
     -d(ate)       : indica la data di creazione della Unit
     -t(ime)       :    "    l'ora   "     "       "     "
     -c(apitalize) : trasforma in maiuscolo le keywords UNIT, INTERFACE, USES,
                     CONST, TYPE, VAR, FUNCTION, PROCEDURE, RECORD, END,
                     ARRAY, SET, FILE, OF.
     -n(ot heading): non stampa la linea di intestazione

N.B. Lo Switch va preceduto da '-', '/' oppure da '.' (vedi SwChars)e pu•
     essere indifferentemente maiuscolo o minuscolo .
     Gli Switches sono cumulativi, cioŠ se un determinato switch Š
     presente in un qualunque punto della riga di comando, esso
     sar… attivo per tutti i files specificati .

Possibili errori:
    File not found                   -> specified file doesn't exist
    File doesn't contain any unit    -> you can list only unit files
    Missing 'implementation'         -> unit is not complete
    Unrecognised switch              -> lo switch non Š disponibile
}

uses Crt, Dos ;

var F:text ;
    StdErr:text ;
    OutDate,OutTime,Caps,Head : boolean ;  { switches }

const terminator = 'IMPLEMENTATION' ;
      KWords: array[1..16] of string[15] =
            ( 'ARRAY', 'CASE', 'CONST', 'END', 'FILE', 'FUNCTION', 'INTERFACE',
              'OF', 'PROCEDURE', 'RECORD', 'SET', 'TYPE', 'UNIT', 'USES',
              'VAR', terminator ) ;
      separators=' :;,./+-*<>()[]{}^=$#@''' ;
      SwChars='-/.' ;

procedure GetSwitches ;
var i,j:byte ;
    s:string ;
    AllOk:boolean ;
begin
     OutDate:=false ;
     OutTime:=false ;
     Caps:=false ;
     Head:=true ;
     for i := 1 to ParamCount do begin
         allok:=true ;
         s:=ParamStr(i);
         if pos(s[1],SwChars)>0 then begin
            for j:=2 to length(s) do
                case upcase(s[j]) of
                     'D':OutDate:=true ;
                     'T':OutTime:=true ;
                     'C':Caps:=true ;
                     'N':Head:=false ;
                     else AllOk:=false ;
                end ;
            if (length(s)=1) or not AllOk then
               writeln(StdErr,'Unrecognised switch "',s,'" in parameter ',i );
         end ; { if pos }
     end ; {for i}
     if not Head then writeln(stderr,'Will not output headers')
        else begin
             if OutDate then writeln(stderr, 'Will output date');
             if OutTime then writeln(stderr, 'Will output time');
       end ;
     if Caps then writeln(stderr, 'Will capitalize keywords');
end ; { GetSwitches }

procedure Substitute(var s:string) ;
var s1,kw:string ;
    i,j:integer;
    lun:integer ;
begin
     s1:=s ;
     for i:=1 to length(s1) do s1[i]:=upcase(s1[i]) ;
     for i:=1 to 16 do begin
         kw:=kwords[i];
         lun:=length(kw);
         if pos(kw,s1)<>0 then
           for j:=1 to length(s1) do
             if copy(s1,j,lun)=kw then
                if ( (j=1) or (pos(s1[j-1],separators)<>0) ) and
                  ( (j+lun>=length(s1)) or (pos(s1[j+lun],separators)<>0) )
                    then move(kw[1],s[j],lun) ;
     end;
end ;

var ct:integer ;
    fname,row,tmp:string ;
    AtEnd,IsUnit:boolean ;
    time:longint ;
    created:DateTime ;

begin {main body}
     assign(Output,'');
     rewrite(Output);
     assignCRT(StdErr);
     rewrite(StdErr);
     textcolor(12);
     Writeln(stderr) ;
     WriteLn(StdErr, 'UNIT interface lister version 1.0');
     writeln(StdErr, 'property of Fulvio Corno');
     writeln(stderr) ;
     GetSwitches ;
     for ct :=1 to ParamCount do begin
         fname:=ParamStr(ct);
         if pos('.',fname)=0 then fname:=fname+'.pas';
         if pos(fname[1],SwChars)=0 then begin
            assign(F,fname);
            {$i-} reset(F) ; {$i+}
            if IoResult<>0 then WriteLn(StdErr,'File not found: ',fname)
               else begin
                    writeln(stderr,'Processing file ',fname);
                    if head then begin
                       writeln;
                       write('---> File: ',fname:20);
                       if OutDate or OutTime then begin
                          GetFTime(F,time);
                          UnpackTime(time,created);
                          write('':5,'Created ');
                          if OutDate then with created do
                             write(Day:2,'/',Month:2,'/',Year:4,'':5);
                          if OutTime then with created do
                             write(Hour:2,':',Min:2);
                       end;
                       writeln;
                       writeln;
                    end ;
                    AtEnd:=false ;
                    IsUnit:=false ;
                    while (not AtEnd) and (not eof(F)) do begin
                          readln(F,row);
                          tmp:=row ;
                          Substitute(tmp);
                          if Caps then row:=tmp;
                          if pos(terminator,tmp)<>0 then AtEnd:=true ;
                          if pos('UNIT',tmp)<>0 then IsUnit:=true ;
                          writeln(row);
                    end ;
                    close(F);
                    if not IsUnit then writeln(stderr,'File doesn''t contain any unit');
                    if not AtEnd then writeln(stderr,'Missing "implementation"');
            end;
         end;
     end;
end .