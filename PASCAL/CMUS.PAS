{***********************************************************************
     CMUS.EXE - Music Compiler for 'MUSICA.TPU'
 ***********************************************************************
 Copyright Jena Soft - Author: Fulvio Corno 1988

Compila un file di testo .MUS in un file di tipo .MCA (Music Compiled Array),
adatto ad essere eseguito direttamente dalla Play di MUSICA.TPU .

Convenzioni per il file .MUS :
                         <--\
              R>>            +--+      (anche minuscole)
       -D>>- -----       <--/   |     D, R, M, F, S, L, Z : note Do..Si
  Z>                      <--\  |   MODIFICABILI CON :
 ----  ----- -----       -L>- | +---Suffisso '>>': 2 ottave sopra
                        S>    +-----Suffisso '>' : 1 ottava sopra
----------------------F>----- | +---Suffisso '<' : 1 ottava sotto
                    M>        | |   Suffisso '<<': 2 ottave sotto
------------------R>--------- | |   Prefisso '*' : bequadro (=annulla #,·)
                D>        <--/  |   Prefisso '#' : 1/2 tono sopra
--------------Z--------------   |   Prefisso '$' : 1/2 tono sotto (sinonimo ·)
            L  <<=== 440 Hz     |
----------S------------------   |   =N:    N =   1   2   4   8   16
        F                       |     =>durata  4/4 2/4 1/4 1/8 1/16
------M----------------------   |
    R                           |   Prefisso numerico: estensione durata
 -D-  ---- ---- ---- ----       |    Es: =4 3#D>
       Z<                 <--\  |         |  \-- questo Do# sopra dura 3/4
           -L<- ---- ----    +--+         \-- ogni nota in unitÖ di 1/4
                 S<          |
                     -F<- <--/      Suffisso '.': estensione durata del 50%
                                     Es: =4 2#D>. dÖ lo stesso risultato
                                          (2*(1/4))+50% = 3/4
Altri comandi :

%n : sceglie il tempo : <n> = numero di note da 1/4 in 1 minuto
@[$|·|#][D..Z]:imposta un diesis o bemolli in chiave (Es: @$ZM --> Si· e Mi·)
              ( vale fino alla successiva impostazione )
@* : annulla ogni precedente @[$|·|#]
&f : sceglie la frequenza 'f' del La fondamentale ( *L ) (in HERTZ)
&* &> &>> &< &<< : Esegue un &f rispettivamente alla frequenza naturale di
                   *L *L> *L>> *L< *L<< ( 440, 880, 1760, 220, 110 )
! : tutto ciï che segue '!' ä ignorato tranne per un caso:
!% : se c'ä un commento '!%' il brano verrÖ ripartirÖ da capo una volta finito

? : una pausa si indica con ? e viene trattata come ogni nota
  ( ä quindi valido 2? ed anche ?.. )

^ : seleziona il modo STACCATO: ogni nota ä seguita da una pausa di 1/18 sec.
- : seleziona il modo LEGATO: non ci sono pause tra le note
-^ : seleziona il legato-puntato: c'ä appena uno stacco tra le 2 note.
' : STACCA le due note che separa

'' : Le linee vuote sono ignorate

Defaults : ogni brano comincia automaticamente con:
&* @* %120 =4 ^
I defaults si possono alterare con gli switches opzionali come segue :
Switches == /&f /@[$|#][D..Z] /%n /=n /[^|-]
N.B. non si possono usare &> e simili perchä i simboli '<' e '>' vengono
     interpretati dal DOS come I/O REDIRECTION .

Sintassi :  CMUS [Switches] <source>[.MUS] [<destination>[.MCA]]
 (Se manca <destination>, si assume uguale a "<source>.MCA")

Oppure : CMUS ? (mostra queste note esplicative a video)

L'autore ä a vostra disposizione :  '' +----------------+
       Fulvio Corno                    |   Jena  Soft   |
       via C.I.Giulio 24               |    Canaväis    |
 10090 S.Giorgio Canavese (TO)         | Software house |
       Tel. (0124) 32160               +----------------+  ,,

*************************************************************************}

Program CompilaMusica ; { CMUS.EXE }

{$i musica.inc}

type
    note = ( do_, re_, mi_, fa_, so_, la_, si_ ) ;

    chiave = set of note ;

const nomi:array[note] of char = 'DRMFSLZ' ;
      clock = 18.20648 {Hz} ;
      offset:array[note] of shortint =  { semitoni di distanza da *L }
          ( -9, -7, -5, -4, -2, 0, 2 ) ;
      errors:array[0..12] of string[50] =
             ( 'E0: Compilazione conclusa senza errori',
               'E1: Manca il parametro <source>',
               'E2: Valore illegale per =n',
               'E3: Valore illegale per %N',
               'E4: Valore illegale per &f',
               'E5: Nome nota illegale (non DRMFSLZ)',
               'E6: Formato della chiave illegale',
               'E7: Direttiva non riconosciuta',
               'E8: Troppi nomi di file',
               'E9: <source> e <destination> sono identici',
               'E10: Non riesco a leggere <source>',
               'E11: Non riesco a scrivere su <destination>',
               'E12: Suffisso illegale per una nota ' ) ;

var
   inp : text ; { file <source>.MUS }
   out : file of nota ; { file <destination>.MCA }
   source,dest:string ;
   tempo:integer ;  { %n: frequenza di default (in (1/4)/min ) }
   durata:integer ; { =N: durata di ogni nota }
   fondamentale : real ;
   diesis,bemolli : chiave ;
   line:word ;  { linea a cui si ä arrivati a scorrere il file }
   frase,parola :string;
   ln2a1su12 : real ;
   last:nota ;
   count:word ;

const stacca:nota=(freq:0; dura:1) ;

procedure OutError(n:byte) ;
begin
     write(errors[n]) ;
     if line=0 then writeln(' nella linea di comando')
     else writeln(' alla linea ',line) ;
     halt(n) ;
end ;

procedure SelectDefaults ;
begin
     diesis:=[] ;
     bemolli:=[] ;
     fondamentale := 440.0 ;
     durata:= 4 ;
     tempo:=120 ;
     source:='';
     dest:='';
end ;

function Ch2Note(c:char):note ;
begin
     c:=upcase(c);
     if pos(c,nomi)>0 then ch2note:=note(pos(c,nomi)-1)
     else OutError(5)
end ;

procedure Controllo(s:string) ;
var v:word ;
    e:integer ;
    tmp:chiave ;
    s1:string ;
begin
     s1:=copy(s,2,255);
     val(s1,v,e) ;
     case s[1] of
          '=':if e=0 then durata:=v else Outerror(2);
          '%':if e=0 then tempo:=v else OutError(3);
          '&':if e=0 then fondamentale:=v
              else if s1='*' then fondamentale:=440.0
              else if s1='<' then fondamentale:=220.0
              else if s1='>' then fondamentale:=880.0
              else if s1='>>' then fondamentale:=1760.0
              else if s1='<<' then fondamentale:=110.0
              else OutError(4) ;
          '@':begin
                   diesis:=[] ;
                   bemolli:=[] ;
                   if (s1[1]='$') or (s1[1]='·') then
                      for e:=2 to length(s1) do
                          bemolli:=bemolli+[ch2note(s1[e])]
                   else if s1[1]='#' then
                        for e:=2 to length(s1) do
                            diesis:=diesis+[ch2note(s1[e])]
                   else if s1<>'*' then OutError(6) ;
                   if (diesis=[])and(bemolli=[])and(s1<>'*') then OutError(6);
              end;
          else OutError(7) ;
     end; {case}
end;

procedure GetParameters ;
var i:integer ;
    s:string ;
begin
     if (ParamCount = 1) and (ParamStr(1) = '?') then begin
        {DisplayHelp ;}
        Halt(255); { l'errore ä il + grave: l'utente non sa usare il comando }
     end ;
     if ParamCount = 0 then Outerror(1) ;
     SelectDefaults ;
     for i :=1 to ParamCount do begin
           s:=paramStr(i) ;
           if s[1]='/' then controllo(copy(s,2,255))
           else if source='' then source:=s
           else if dest='' then dest:=s
           else OutError(8);
     end;
     if (source='') and (dest='') then OutError(1);
     if dest='' then dest:=source ;
     if pos('.',source)=0 then source:=source+'.MUS' ;
     if pos('.',dest)=0 then dest:=dest+'.MCA' ;
     if dest=source then OutError(9) ;
end ;

procedure OutNota(s:string);
var what:nota ;
    dura_glb,dura_loc,dura_dot:real ;
    s1:string ;
    i:byte ;
    err:integer ;
    altera:integer;
    c:char ;
    freq_loc : real ;
    questa : note ;
begin
     dura_glb:=(4*60)/(tempo*durata) ; { durata della nota in secondi }
     i:=1;
     s1:='';
     while pos(s[i],'0123456789')<>0 do begin
           s1:=s1+s[i] ;
           inc(i)
     end ;
     if s1<>'' then val(s1,dura_loc,err) { per come ä costruita s1 --> err=0 }
     else dura_loc:=1 ;
     s:=copy(s,length(s1)+1,255);
     altera:=0 ;
     if s='''' then what:=stacca
     else if s='-' then what:=n_lega
     else if s='^' then what:=n_stac
     else if s='-^' then what:=n_lest
     else begin
          if s[1]='?' then begin
             freq_loc:=0.0;
             s1:=copy(s,2,255);
          end else begin
              if (s[1]='$') or (s[1]='·') then altera:=-1 ;
              if s[1]='#' then altera:=1 ;
              if pos(s[1],'#$·*')<>0 then c:=s[2] else c:=s[1] ;
              questa:=ch2note(c) ;
              if (s[1]<>'*')and(altera=0) then begin
                 if questa in diesis then altera:=1
                 else if questa in bemolli then altera:=-1
              end ;
              freq_loc:=fondamentale*exp((altera+offset[questa])*ln2a1su12) ;
              s1:=copy(s,pos(c,s)+1,255);
          end ;
          dura_dot:=1 ;
          for i:=1 to length(s1) do
              case s1[i] of
                   '<':freq_loc:=freq_loc/2 ;
                   '>':freq_loc:=freq_loc*2 ;
                   '.':dura_dot:=dura_dot*2 ;
                   else OutError(12);
              end;
          dura_loc:=dura_loc*(2-1/dura_dot) ;
          what.freq:=trunc(freq_loc);
          what.dura:=trunc(clock*dura_glb*dura_loc) ;
          if what.dura=0 then what.dura:=1 ;
     end ;
     write(out,what);
     inc(count) ;
end;

begin { main body }
     line:=0;
     count:=0;
     ln2a1su12:=ln(2)/12 ;
     stacca.dura:=1;
     stacca.freq:=0;
     GetParameters ;
     writeln('Compilo ',source,' in ',dest) ;
     { apri i files }
     assign(inp,source);
     assign(out,dest);
     {$I-}
     reset(inp);
     if ioresult<>0 then OutError(10);
     rewrite(out);
     if ioresult<>0 then OutError(11);
     {$I+}
     last.dura:=0 ;
     last.freq:=0 ;
     while not eof(inp) do begin
           readln(inp,frase) ;
           inc(line) ;
           while frase<>'' do begin
                 if frase[1]='!' then begin
                    if frase='!%' then last.freq:=1 ;
                    frase:=''
                 end else begin
                      parola:=copy(frase,1,pos(' ',frase+' ')-1) ;
                      if parola <>'' then
                         if pos(parola[1],'=%&@')<>0 then controllo(parola)
                         else OutNota(parola)
                 end ;
                 frase:=copy(frase,pos(' ',frase+' ')+1,255);
           end ; {while}
           write(^M,'Linea ',line:4,' --> ',count:5,' note') ;
     end ;
     writeln;
     write(out,last);
     close(inp);
     close(out);
     OutError(0);
end.
