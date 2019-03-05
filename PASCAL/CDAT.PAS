{$R+,S+,I+,D+,T-,F-,V+,B-,N-,L+ }
program CompileDAT ; { CDAT.EXE }
{ compila un file *.DAT nel corrispondente *.DCA (data compiled array)
  ## Exclusive property of Jena Soft - 1988 ##
  Usato per la creazione del database Sancti Georgii Castrum

  Sintassi :    CDAT <nome> ...
  dove <nome> Š il nome di un Global Array dell'unit Globals            }

USES globals,drivers ;

const reclen = 20 ; { # max righe di un record in .dat }
      separatori:set of char = [' ', '.', ',', '*', '='] ;

var inp:text ;
    out:file ;
    rec:array[1..reclen] of string ;
    row,chr:byte ; { cursore nella lettura del record }
    totrows:word;

procedure ERROR(msg:string);
begin
     writeln('*** ERROR ***');
     writeln(msg);
     halt ;
end ;

procedure OpenFiles(var f:text; var g:file; fnam:string ) ;
begin
     assign(f,fnam+'.dat');
     assign(g,fnam+'.dca');
     writeln('Opening '+'input'+' file: ',fnam,'.dat');
     reset(f) ;
     writeln('Opening '+'output'+' file: ',fnam,'.dca');
     rewrite(g,1) ;
     totrows:=0;
end ; { AssignFile }

procedure readxx(var f:text; var s:string);
begin
     repeat
           readln(f,s);
           inc(totrows);
           write(^M,'Line ',totrows:4);
     until s[1]<>'*' ;
end ;

procedure GetRecord ;
{ legge un record nelle stringhe di REC }
var i:integer ;
    s:string ;
begin
     for i:=1 to reclen do rec[i]:='' ;
     i:=0;
     readxx(inp,s);
     while s<>'' do begin
           if s[1]<> '*' then begin { NON Š un commento }
              inc(i);
              rec[i]:=s;
           end ;
           readxx(inp,s);
     end ;
     row:=1 ;
     chr:=1 ;
end ;

function Next:char ;
{ restituisce il carattere che sta per essere letto }
begin
     if chr>length(rec[row]) then Next:= #0  { non c'Š pi— niente sulla riga }
        else next:=rec[row,chr] ;
end ;

procedure SkipChar ;
begin
     if next<>#0 then inc(chr) ;
end ;

procedure SkipLine ;
begin
     chr:=1 ;
     inc(row) ;
end ;

function parola:string ;
{ isola una sequenza separata da separatori }
var s:string ;
begin
     s:='' ;
     while next in separatori do skipchar ;
     while (not(next in separatori))and(next<>#0) do begin
           s:=s+next ;
           skipchar ;
     end ;
     while next=' ' do skipchar ;
     parola:=s ;
end ;

function LeggiInt:integer ;
{ interpreta la prossima "cosa" come un integer }
var s:string ;
    v,e:integer ;
begin
     s:=parola ;
     val(s,v,e) ;
     if e=0 then LeggiInt:= v
        else Error('Illegal INTEGER format') ;
end ;

function LeggiStr:string ;
{ mette il resto della riga in una stringa }
begin
     while next=' ' do skipchar ;
     LeggiStr:=copy(rec[row],chr,255);
     SkipLine ;
end ;

procedure LoadStanza ;
{ Carica l'array stanza^ }
var i:indice_stanza ;
begin
     new(stanza);
     fillchar(stanza^,sizeof(stanza^),0);
     tot_stanze:=0;
     repeat
           GetRecord ;
           i:=LeggiInt ;
           if i>tot_stanze then tot_stanze:=i ;
           if i<>0 then
              with stanza^[i] do begin
                nome := LeggiStr ;
                dim_z:= LeggiInt ;
                tipo:=t_soffitto(LeggiInt);
                terreno:=fillindex(LeggiInt);
                if next=#0 then disegno:=0 else disegno:=LeggiInt ;
                memoria:= 0 ; { visitata a t=-ì }
                SkipLine ;
                if next=#0 then descrizione:=0 else descrizione:=LeggiInt ;
           end ;
     until i=0 ;
     blockwrite(out,stanza^,(1+tot_stanze)*sizeof(t_stanza));
end ; { LoadStanza }

procedure LoadLuogo ;
{ carica l'array luogo^ }
var i:indice_luogo ;
    j:byte ;
begin
     new(luogo);
     fillchar(luogo^,sizeof(luogo^),0);
     tot_luoghi:=0;
     repeat
           GetRecord ;
           i:=LeggiInt;
           if i>tot_luoghi then tot_luoghi:=i ;
           if i<>0 then
              with luogo^[i] do begin
                n_stanza:=LeggiInt ;
                SkipLine ;
                dim_x:=LeggiInt;
                dim_y:=LeggiInt;
                SkipLine ;
                for j:=1 to 4 do with pareti[j] do begin
                    forma:=LeggiInt;
                    tipo:=FillIndex(LeggiInt);
                    if next<>#0 then begin {1}
                       n_porta1:=LeggiInt ;
                       n_uscita1:=LeggiInt ;
                       if next<>#0 then begin {1b}
                          n_porta2:=LeggiInt ;
                          n_uscita2:=LeggiInt ;
                       end {1b} else begin {2}
                                n_porta2:=0 ;
                                n_uscita2:=0 ;
                       end {2}
                    end {1} else begin {3}
                                  n_porta1:=0 ;
                                  n_uscita1:=0 ;
                                  n_porta2:=0 ;
                                  n_uscita2:=0 ;
                             end {3} ;
                SkipLine ;
                end ; {for}
           end ; {with}
     until i=0 ;
     blockwrite(out,luogo^,(1+tot_luoghi)*sizeof(t_luogo));
end; { loadluogo }

procedure LoadPorta ;
{ carica l'array porta^ }
var i:indice_porta ;
begin
     new(porta);
     fillchar(porta^,sizeof(porta^),0);
     tot_porte:=0 ;
     repeat
           GetRecord ;
           i:=LeggiInt ;
           SkipLine ;
           if i>tot_porte then tot_porte:=i ;
           if i<>0 then
              with porta^[i] do begin
                tipo:=t_porte(LeggiInt);
                if tipo=p_porta then stato:=t_passaggio(LeggiInt);
           end ;
     until i=0 ;
     blockwrite(out,porta^,(1+tot_porte)*sizeof(t_porta));
end ; { LoadLuogo }

procedure LoadMenu ;
{ carica l'array menu^ }
var i:indice_menu ;
    n:integer ;
    s:string ;
begin
     new(menu);
     fillchar(menu^,sizeof(menu^),0);
     tot_menus:=0;
     repeat
           GetRecord ;
           i:=LeggiInt;
           if i>tot_menus then tot_menus:=i;
           if i<>0 then
              with menu^[i] do begin
                titolo:=LeggiStr ;
                for n:=1 to  10 do with tasto[n] do begin
                    frase:='';
                    azione:=0;      { defaults }
                    nextmenu:=0;
                    disegno:=0;
                end ;
                while next<>#0 do begin
                      n:=LeggiInt ;
                      with tasto[n] do begin
                         if next='#' then begin
                            skipchar ;
                            disegno:=LeggiInt ;
                         end ;
                         azione:=LeggiInt;
                         nextmenu:=LeggiInt;
                         frase:=LeggiStr;
                      end;
                end ;
           end ;
     until i=0 ;
     blockwrite(out,menu^,(1+tot_menus)*sizeof(t_menu));
end; { LoadMenu }

procedure LoadDato ;
{ carica l'array dato^ }
var i,k:word ;
    j:t_parametri ;
begin
     new(dato) ;
     fillchar(dato^,sizeof(dato^),0);
     tot_dati:=0 ;
     repeat
           GetRecord ;
           inc(tot_dati);
           i:=LeggiInt ;
           if i>0 then with dato^[tot_dati] do begin
              tipo.tipo1:=i;
              tipo.tipo2:=leggiInt ;
              agget:=LeggiInt ;
              NomeGlb:=LeggiStr ;
              fillchar(dati,sizeof(dati),0);
              while next<>#0 do begin
                    j:=t_parametri(LeggiInt);
                    if next='.' then SkipChar else Error('Missing "-" in object '+nomeglb);
                    if next='?' then begin
                       SkipChar;
                       writeln(^M,'Missing ',ord(j),' in ',nomeglb);
                    end else dati[j]:=LeggiInt ;
              end ;
              SkipLine ;
              verbo_P:=[] ;
              if next<>'#' then while next<>#0 do verbo_P:=verbo_P+[LeggiInt] ;
              SkipLine ;
              verbo_L:=[] ;
              if next<>'#' then while next<>#0 do verbo_L:=verbo_L+[LeggiInt] ;
              SkipLine ;
           end;
     until i=0 ;
     blockwrite(out,dato^,(1+tot_dati)*sizeof(t_dato));
end; { LoadDato }

procedure LoadVerbo ;
var i:indice_verbo ;
begin
     new(verbo);
     Fillchar(verbo^,sizeof(verbo^),0);
     tot_verbi:=0;
     repeat
           GetRecord ;
           i:=LeggiInt;
           if i>tot_verbi then tot_verbi:=i ;
           if i>0 then with verbo^[i] do begin
              tipo:=LeggiInt ;
              prep:=LeggiInt ;
              nome:=LeggiStr ;
              spiega:=LeggiStr ;
           end ;
     until i=0 ;
     blockwrite(out,verbo^,(1+tot_verbi)*sizeof(t_verbo));
end ; { LoadVerbo }

procedure LoadObj ;
var i:indice_obj ;
begin
     new(obj);
     fillchar(obj^,sizeof(obj^),0);
     tot_objs:=0 ;
     repeat
           GetRecord ;
           i:=LeggiInt ;
           if i>tot_objs then tot_objs:=i ;
           if i>0 then with obj^[i] do begin
              descrizione := 0 ;
              tipo.tipo1:=LeggiInt ;
              tipo.tipo2:=LeggiInt ;
              if Next<>#0 then begin
                 agget:=LeggiInt ;
                 NomeLoc:=LeggiStr ;
              end else begin
                  NomeLoc:='' ;
                  SkipLine ;
              end ;
              stato:=[] ;
              if Next<>'#' then while Next<>#0 do
                           stato:=stato+[t_statobj(LeggiInt)] ;
              SkipLine ;
              dove:=LeggiInt ;
              if Next<>#0 then nasconde:=LeggiInt else nasconde:=0 ;
           end ;
     until i=0 ;
     blockwrite(out,obj^,(1+tot_objs)*sizeof(t_obj)) ;
end ; { LoadObj }

var name:string ;
    ctr,j:integer ;

begin { MAIN BODY }
     writeln('Jena Soft  --  .DAT => .DCA  Compiler  --  S.G.CASTRUM ');
     for ctr:=1 to ParamCount do begin
         name:=ParamStr(ctr);
         for j:=1 to length(name) do name[j]:=upcase(name[j]) ;
         writeln;
         writeln('Item to process --> ',name);
         OpenFiles(inp,out,name);
         if name='STANZA' then LoadStanza ;
         if name='LUOGO' then LoadLuogo ;
         if name='PORTA' then LoadPorta ;
         if name='MENU' then LoadMenu ;
         if name='DATO' then LoadDato ;
         if name='VERBO' then LoadVerbo ;
         if name='OBJ' then LoadObj ;
         WriteLn(^M,'Done without errors. ',filesize(out),' bytes written');
         close(inp);
         close(out);
     end;
end.
