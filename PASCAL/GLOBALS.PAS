UNIT globals ;

{ contiene tutte (e sole) le variabili completamente GLOBALI, cioŠ
  accessibili ad ogni modulo e modificabili secondo la necessit…, che
  concernono l'avventura in sŠ e non i suoi aspetti grafici e/o
  implementativi                                                       }

INTERFACE
uses graph, crt, drivers ;

{ Parte 0: costanti e types comunemente usati }

TYPE
     str40 = string[40] ;
     str25 = string[25] ;
     str5  = string [5] ;

{ Parte 1: si definiscono gli arrays da tenere nell'heap }

{ sez. 1a: dimensioni arrays }

CONST
   { formato .DCA: }
     max_porta   = 1000 ;
     max_luogo   =  600 ;
     max_stanza  =  250 ;
     max_panico  =  100 ;
     max_menu    =   30 ;
     max_dato    =  250 ;
     max_obj     =  400 ;
     max_verbo   =   50 ;
   { non compilate: }
     max_icona   =   60 ;
     max_frase   = 1000 ;
     max_msg     =  200 ;

{ sez. 1b: subranges per gli indici }

TYPE
     indice_porta   = 0..max_porta   ;        { se indice_xxx = 0 indica    }
     indice_luogo   = 0..max_luogo   ;        { una situazione particolare: }
     indice_stanza  = 0..max_stanza  ;        { la rispettiva variabile NON }
     indice_panico  = 0..max_panico  ;        { ha valore significativo     }
     indice_menu    = 0..max_menu    ;
     indice_dato    = 0..max_dato    ;
     indice_obj     = 0..max_obj     ;
     indice_verbo   = 0..max_verbo   ;
     indice_icona   = 0..max_icona   ;
     indice_frase   = 0..max_frase   ;
     indice_msg     = 0..max_msg     ;

{ sez. 1c: tipi enumerativi per la costituzione delle stanze e degli oggetti }

TYPE
     t_soffitto = ( s_piano,   s_botte,   s_crociera,        { possibili tipi }
                    s_butala,  s_cielo,   s_chiocciola,      { di stanza      }
                    s_sciapa,  s_stroc,   s_querc,
                    s_foresta, s_caverna, s_cassettoni   ) ;

     t_porte = ( p_passaggio, p_porta, p_ostruito ) ;    { possibili tipi di }
                                                         { porta             }

     t_passaggio = ( p_aperto, p_chiuso, p_bloccato ) ;  { diversi stati di  }
                                                         { una porta         }

     t_parametri = ( d_robust, d_nutrit, d_rusnen,    { attributi invarianti }
                     d_affila, d_traspa, d_resist,    { degli oggetti        }
                     d_marces, d_capaci, d_riflet, d_umidez, d_schegg,
                     d_incend, d_compat, d_peso  , d_ingomb, d_elasti,
                     d_equili  ) ;

     t_statobj = ( d_aperto, d_visibi, d_sposta,     { switch on/off per }
                   d_acceso, d_pieno , d_funzio ) ;  { ogni oggetto      }

{ sez. 1d: struttura interna delle diverse entit… }

TYPE
     t_tipobj = record    { identificatore univoco del tipo di oggetto }
                      tipo1,tipo2:byte
                end;

     t_dato = record     { caratteristiche di ogni tipo di oggetto }
                    tipo : t_tipobj ;        { quale tipo di obj descrive }
                    agget: byte ;            { aggettivi da usare }
                    NomeGlb : str25 ;        { nome di default }
                    dati : array [ t_parametri ] of 0..15 ;  {caratteristiche}
                    verbo_L, verbo_P : set of indice_verbo ; {azioni possibili}
              end ;

     t_obj = record   { il vero e proprio oggetto }
                   tipo : t_tipobj ;  { che caratteristiche ha ? }
                   agget : byte ;     { aggettivi }
                   NomeLoc : str25 ;  { se Š <> default }
                   descrizione : indice_frase ;
                   stato : set of t_statobj ; { simula 'array of bool' }
                   nasconde : indice_obj ;  {oggetto legato/nascosto/bloccato}
                   dove : integer ; { luogo in cui Š posto :
                                      > 0 corrisponde a indice_luogo
                                      = 0 oggetto non disponibile
                                      < 0 (=-1) posseduto             }
             end ;

     t_porta = RECORD    { descrizione della porta }
                     tipo : t_porte ;      { costante }
                     stato : t_passaggio ; { valore istantaneo }
                     n_panico : indice_panico ;  { possibile pericolo celato }
                     n_chiave : indice_obj ;     { se Š chiusa a chiave }
                     descrizione : indice_frase ;
               END ;

     t_parete = RECORD
                      tipo : FillIndex ;   { tappezzeria }
                      forma : byte ;       { tipo di parete }
                      n_porta1,  n_porta2  : indice_porta ;  { # porta }
                      n_uscita1, n_uscita2 : indice_luogo ;  {luogo collegato}
                END;

     t_luogo = RECORD   {  unit… base del castello  }
                     n_stanza : indice_stanza ; { a cui appartiene }
                     dim_x, dim_y : byte ; { in spanne }
                     pareti : array [1..4] of t_parete ;
               END ;

     t_stanza = RECORD
                      nome : str40 ;
                      tipo : t_soffitto ;  { per il disegno 3d esterno }
                      terreno : FillIndex ; { campitura pavimento }
                      disegno : byte ;      { ornamento pavimento }
                      dim_z : byte ; { in spanne }
                      memoria : word ; { tempo 'packed' dell'ultima volta
                                         che ci sei stato }
                      descrizione : indice_frase ;
                END ;

     t_tastofunz = RECORD
                         frase : str25 ;
                         disegno : indice_icona ;
                         nextmenu : shortint ;  { <-- se >0 altro menu
                                                         =0 esegui azione
                                                         <0 menu speciale }
                         azione : shortint ;    { <-- se >0 esegue azione
                                                         =0 non agisce
                                                         <0 funzione speciale }
                   END ;

     t_menu = RECORD
                    titolo : str40 ;
                    tasto : ARRAY [ 1..10 ] of t_tastofunz ;
              END ;

     t_verbo = RECORD
                     nome:str25 ;  { per scrivere nel tasto funz.(imperativo) }
                     spiega:str25 ; { per scrivere nella pergamena (2a sing.) }
                     tipo:byte ;   { con/senza 2^ parametro e di che tipo }
                     prep:byte ;   { eventuale preposizione 2^ parametro }
               END ;

     t_icona = array [0..27,0..3] of byte ;

     t_frase = ^ string ;

{ sez. 1e: definizione arrays veri e propri }

TYPE
     stanze = array [ indice_stanza ] of t_stanza ;
     luoghi = array [ indice_luogo  ] of t_luogo  ;
     porte  = array [ indice_porta  ] of t_porta  ;
   { panici = array [ indice_panico ] of t_panico ; <-- per il futuro }
     menus  = array [ indice_menu   ] of t_menu   ;
     dati   = array [ indice_dato   ] of t_dato   ;
     objs   = array [ indice_obj    ] of t_obj    ;
     verbi  = array [ indice_verbo  ] of t_verbo  ;
     icone  = array [ indice_icona  ] of t_icona  ;
     frasi  = array [ indice_frase  ] of t_frase  ;
     msgs   = array [ indice_msg    ] of indice_frase ;

{ sez. 1f: dimensioni data base }

VAR
   tot_stanze : indice_stanza ;  { tot_xxx == reale numero di xxx presenti }
   tot_luoghi : indice_luogo  ;
   tot_porte  : indice_porta  ;
 { tot_panici : indice_panico ; }
   tot_menus  : indice_menu   ;
   tot_dati   : indice_dato   ;
   tot_objs   : indice_obj    ;
   tot_verbi  : indice_verbo  ;
   tot_icone  : indice_icona  ;
   tot_frasi  : indice_frase  ;
   tot_msgs   : indice_msg    ;

{ sez. 1g: puntatori agli array globali (posti nell'heap per non superare
           i 64K di dati nel Data Segment)                                }

VAR
   stanza : ^ stanze ;
   luogo  : ^ luoghi ;
   porta  : ^ porte  ;
 { panico : ^ panici ; <-- per il futuro }
   menu   : ^ menus  ;
   dato   : ^ dati   ;
   obj    : ^ objs   ;
   verbo  : ^ verbi  ;
   icona  : ^ icone  ;
   frase  : ^ frasi  ;
   msg    : ^ msgs   ;


{ Parte 2 : definizione entit… componenti la "variabile di stato" }

{ sez. 2a: tabella di conversione Miria <-> 0..15 ed altre costanti
           fondamentali per lo Stato }

const
     c_miria : array [0..15] of real =
           ( 0, 0.003, 0.01, 0.05, 0.1, 0.2, 0.5, 0.7, 1.5, 3.0,
             6.0, 10.0, 20.0, 30.0, 50.0, 1e+37) ;
     ListObjSize = 25 ;

{ sez. 2b: tipi base con cui costituire lo status }

type
    t_orologio = record   { 'tempo reale simulato' }
                     giorno:byte ;
                     ore:0..23 ;
                     min:0..59 ;
                     sec:0..59 ;
                 end ;
    t_barrette = ( punti, energia, sonno, sete, fatica, morale, patologia,
                   antipatia ) ;
    t_individuo = ( suo_peso, sua_altezza, sua_eta ) ;
    t_trasporta = ( tot_peso, tot_ingombro ) ;
    t_posizione = ( in_piedi, abbassato, accosciato, a4gambe ) ;
    t_velocita  = ( punta_piedi, cammina, svelto, corri ) ;

{ sez. 2c: arrays atti a contenere le informazioni }

type
    ta_barrette = array [ t_barrette ] of real ;  { saranno sempre numeri tra 0
                                        e 100, ma li rappresento come REAL per
                                        avere maggior risoluzione numerica }
    ta_individuo = array [ t_individuo ] of real ;
    ta_trasporta = array [ t_trasporta ] of real ;

    t_status = record
                     individuo : ta_individuo ;    { dati giocatore }
                     trasporta : ta_trasporta ;    { glob. objs. trasportati }
                     barrette  : ta_barrette  ;    { indici 0..100 }
                     posizione : t_posizione  ;    { pos.fisica }
                     velocita  : t_velocita   ;    { vel. effettiva }
                     tempo     : t_orologio   ;    { tempo reale simulato }
                     dovesei   : indice_luogo ;    { dove ti trovi ora }
               end ;

    t_list_objs = array[1..ListObjSize] of indice_obj ;
                { elenco di oggetti (max ListObjSize) }

{ sez. 2d: variabili vere e proprie }

var
   stato : t_status ;
   myobjs : t_list_objs ;
   l_objs : t_list_objs ;

{ sez. 2e: tabelle di costanti numeriche di correzione }

const
     c_posizione : array [ t_posizione ] of real =  { moltiplicativi per }
                   ( 1.0, 0.75, 0.5, 0.3 ) ;    { l'altezza in funzione  }
                                                { della posizione fisica }

     c_velocita : array  [ t_velocita ] of real =    { moltiplicativi per }
                  ( 0.5, 1.0, 1.5, 2.0 ) ;    { trovare la reale velocit… }

{ sez. 2f: stringhe contenenti il nome dei componenti gli ENUM TYPE }

const
     n_barrette : array [ t_barrette ] of str25 =
         ( 'Punti', 'Energia', 'Sonno', 'Sete', 'Fatica', 'Morale',
           'Patologia', 'Antipatia' ) ;

     n_posizione : array [ t_posizione ] of str25 =
         ( 'in piedi', 'abbassato', 'accosciato', 'a quattro gambe' ) ;

     n_velocita : array [ t_velocita ] of str25 =
         ( 'muoverti in punta di piedi', 'camminare normalmente',
           'camminare velocemente', 'correre' ) ;  { hai deciso di ... }

     n_soffitto : array [ t_soffitto ] of str25 =
         ( 'soffitto piatto', 'soffitto a botte', 'soffitto a crociera',
           'soffitto a butala', 'luogo a cielo aperto', 'scala a chiocciola',
           'soffitto " sciapa'' "', 'soffitto " stroc "',
           'soffitto a " querc "', 'foresta', 'caverna',
           'soffitto a cassettoni' ) ;

     n_artDET : array [1..9] of string[7] = ( 'il ', 'la ', 'l''',
                'l''', 'lo ', 'le ', 'i ', 'la ', 'gli ' ) ;
     n_artIND : array [1..9] of string[7] = ( 'un ', 'una ', 'un ',
                'un''', 'uno ', 'delle ', 'dei ', 'della ', 'degli ' ) ;

     prepos : array[0..3] of string[6] = ( '', 'con', 'su', 'contro' ) ;

{ Parte 3: definizione delle "preferences" (controlli) }

{ sez. 3a: enum types }

type t_stampante = ( s_niente, s_solotxt, s_sologoth,
                     s_graftxt, s_grafgoth, s_solograf ) ; { stampante }
     t_grafica = ( g_soffitto, g_oggetto, g_fkeys) ;  { rendimento grafico }
     t_prolix = ( p_azioni, p_stanze, p_oggetti ) ;   { uso pergamena }

{ sez. 3b: collezione di switches }

type switches = record
        stampante : t_stampante ;
        grafica : set of t_grafica ;
        prolix  : set of t_prolix  ;
     end ;

{ sez. 3c: variabile di stato ausiliaria }

var statoAUX : switches ;

{ Parte 4: Definizioni (headers) delle procedure 'pubbliche' }

{ sez. 4a: procedure di servizio }

procedure Internal_Error ( code, proc:str25; val:integer ) ;
{ comunica all'utente un'inconsistenza interna del programma }

procedure LoadVariables(path:str40) ;
{ riempie gli ARRAY con i giusti dati prevevandoli dalla directory
  specificata nel parametro 'path'. }

{ sez. 4b: compiti di conversione }

function PackTempo(ora:t_orologio):word ;
{ trasforma un record di tipo 'orologio' in un numero di minuti, trascurando
  i secondi }

function MiriaTo015(miria:real):byte ;
{ codifica un valore in miria nel range 0..15 secondo la tabella c_miria }

function Tipo2Dato(t:t_tipobj):indice_dato ;
{ restituisce il # di dato che corrisponde al tipo_obj specificato }

{ sez. 4c: aggiornamento variabili di stato }

procedure UpdateTempo(secs:integer);
{ avanza l'orologio di "secs" secondi }

{ sez. 4d: Lettura delle informazioni implicite (esplicitazione) }

function NumeroPorte(n:indice_luogo; wall:byte) : byte ;
{ Restituisce il numero di porte esistenti nella parete specificata del luogo
  specificato. Restituisce 0, 1 o 2 }

function NomeObj(n:indice_obj; art:byte):str25 ;  { restituisce il nome
  (come stringa) dell'obj di cui si specifica il # d'ordine. Pu• essere il
  nome globale (di default) o, se esiste, quello locale. Viene preposto
  l'articolo (0=nessuno; 1=determinativo; 2=indeterminativo) }

function Messaggio(n:indice_msg):string ;
{ restituisce la stringa corrispondente al dato messaggio di sistema,
  esplicitata nelle macro %<qualcosa> }

IMPLEMENTATION

function PackTempo(ora:t_orologio):word ;  {pubblica}
begin
     with ora do
          PackTempo:=min+ore*60+giorno*1440
end ; { PackTempo }

procedure UpdateTempo(secs:integer); {pubblica}
var s:integer ;
begin
     with stato.tempo do begin
          s:=sec+secs ;
          inc(min,s div 60);
          inc(ore,min div 60);
          inc(giorno,ore div 24);
          sec:=s mod 60 ;
          min:=min mod 60 ;
          ore:=ore mod 24 ;
     end
end ; { UpdateTempo }

function MiriaTo015(miria:real):byte ;  { pubblica }
var i:0..15 ;
begin
     i := 0 ;
     while miria > c_miria[i] do inc(i) ;
     MiriaTo015 := i ;
end ; { MiriaTo015 }

Procedure CommandLineParameters ;
var i,e:integer ;
    v:real ;
begin
     with stato do begin { defualts }
          individuo[suo_peso] := 7 ;
          individuo[sua_altezza] := 7.5 ;
          individuo[sua_eta] := 30 ;
          for i:=1 to ParamCount do begin
              if i in [1..3] then begin
                 val(paramstr(i),v,e) ;
                 if e=0 then case i of
                    1:individuo[suo_peso]:=v ;
                    2:individuo[sua_altezza]:=v ;
                    3:individuo[sua_eta]:=v ;
                 end ;
              end ;
          end ;
     end ;
end ;


procedure LoadVariables(path:str40) ;  { pubblica }
type pptr=^pointer;
     pint=^integer;
var ico:file of t_icona ;
    fr:text ;
    s:string ;
    i:integer ;
    c:char ;

(**)procedure GetIn(name:str25; pt:pptr; tot:pint; len:word);
    var f:file;
        fl:word;
     begin
          assign(f,path+name+'.dca');
          reset(f,1);
          fl:=FileSize(f);
          tot^:=fl div len -1 ;
          GetMem(pt^,fl);
          BlockRead(f,pt^^,fl);
          close(f);
          writeln('Loaded ','"'+name+'"':8,' : ',fl:5,' bytes -> ',tot^:4,' items.') ;
(**)end;

begin  { LoadVariables }
{ 1) Lettura di tutti i files ".DCA" }
     GetIn('stanza',@stanza,@tot_stanze,sizeof(t_stanza));
     GetIn('luogo',@luogo,@tot_luoghi,sizeof(t_luogo));
     GetIn('porta',@porta,@tot_porte,sizeof(t_porta));
     GetIn('menu',@menu,@tot_menus,sizeof(t_menu));
     GetIn('dato',@dato,@tot_dati,sizeof(t_dato));
     GetIn('verbo',@verbo,@tot_verbi,sizeof(t_verbo));
     GetIn('obj',@obj,@tot_objs,sizeof(t_obj));
{ 2) Lettura del file FRASI.TXT }
     assign(fr,path+'frasi.txt');
     reset(fr);
     new(msg);
     Fillchar(msg^,sizeof(msg^),0);
     new(frase);
     Fillchar(frase^,sizeof(frase^),0);
     tot_frasi:=0;
     tot_msgs:=0;
     while not EOF(fr) do begin
           if not eoln(fr) then begin
              read(fr,c) ;
              if c='*' then readln(fr)
              else begin
                   readln(fr,i,s);
                   while s[1]=' ' do s:=copy(s,2,255) ;
                   inc(tot_frasi);
                   GetMem(frase^[tot_frasi],length(s)+1);
                   frase^[tot_frasi]^:=s ;
                   case upcase(c) of
                        'S':stanza^[i].descrizione:=tot_frasi ;
                        'P':porta^[i].descrizione:=tot_frasi ;
                        'O':obj^[i].descrizione:=tot_frasi ;
                        '.':begin
                                 if i>tot_msgs then tot_msgs:=i ;
                                 msg^[i]:=tot_frasi ;
                            end ;
                        else writeln('TXT Error: ',c,i,' ',s);
                   end ; {case}
              end ; {if c}
           end else readln(fr) ;
     end ; {while}
     close(fr) ;
{ 3) Lettura del file "icons.byt" contenente le icone dei FKEYS }
     assign(ico,path+'icons.byt');
     reset(ico);
     new(icona);
     while not eof(ico) do begin
           inc(tot_icone);
           read(ico,icona^[tot_icone]);
     end;
     close(ico);
end ; { LoadVariables }

procedure Internal_Error (code, proc:str25; val:integer) ; { pubblica }
begin
     RestoreCrtMode ;
     TextColor(LightRed) ;
     WriteLn('*** INTERNAL ERROR *** please report to Jena-Soft') ;
     TextColor(LightCyan) ;
     writeln('Error code ',code,' (',val,') in procedure ',proc) ;
     TextColor(White) ;
     Halt(1) ;
end ; { Internal_Error }

FUNCTION NumeroPorte(n:indice_luogo; wall:byte):byte ;  { pubblica }
begin
     with luogo^[n].pareti[wall] do
          if n_uscita1=0 then NumeroPorte:=0
          else if n_uscita2=0 then NumeroPorte:=1
          else NumeroPorte:=2
end ;

function Tipo2Dato(t:t_tipobj):indice_dato ;  { pubblica }
var i:indice_dato;
    found:boolean ;
begin
     i:=1 ;
     found:=false ;
     repeat
           with dato^[i] do if (tipo.tipo1=t.tipo1)and(tipo.tipo2=t.tipo2)
           then begin
              tipo2dato:=i ;
              found:=true ;
           end ;
           inc(i) ;
     until found or (i>tot_dati) ;
     if not found then Tipo2Dato:=0 ;
end ;

function NomeObj(n:indice_obj; art:byte):str25 ; { pubblica }
var i:indice_dato ;
    s1,s2:str25 ;
begin
     with obj^[n] do
          if NomeLoc<>''
          then case art of
                  0:NomeObj:=NomeLoc ;
                  1:NomeObj:=n_artDET[agget]+NomeLoc ;
                  2:NomeObj:=n_artIND[agget]+NomeLoc ;
               end
          else begin
               i:=tipo2dato(tipo);
               if i=0 then begin
                  str(tipo.tipo1,s1);
                  str(tipo.tipo2,s2);
                  NomeObj:='('+s1+','+s2+')=???' ;
               end else with dato^[i] do case art of
                    0:NomeObj:=Nomeglb ;
                    1:NomeObj:=n_artDET[agget]+NomeGlb ;
                    2:NomeObj:=n_artIND[agget]+NomeGlb ;
               end
          end ;
end ; { NomeObj }

function Messaggio(n:indice_msg):string ;  { pubblica }
var i:integer ;
    s:string ;
    t:str40 ;
begin
     s:=frase^[msg^[n]]^ ;
     repeat
           i:=pos('%',s);
           if (i>0)and(i<length(s)) then begin
              t:='';
              case upcase(s[i+1]) of
                   'S': t:=stanza^[luogo^[stato.dovesei].n_stanza].nome ;
              end;
              s:=copy(s,1,i-1)+t+copy(s,i+2,255);
           end
     until i=0;
     messaggio:=s ;
end ; { messaggio }

begin { inizializzazione delle variabili }
      tot_stanze := 0 ;
      tot_luoghi := 0 ;
      tot_porte  := 0 ;
      tot_menus  := 0 ;
      tot_dati   := 0 ;
      tot_objs   := 0 ;
      tot_verbi  := 0 ;
      tot_icone  := 0 ;
      tot_frasi  := 0 ;
      with stato.tempo do begin { l'avventura inizia alle 06:00 }
           giorno:=0 ;
           ore:=6 ;
           min:=0 ;
           sec:=0 ;
      end ;
      with stato do begin   { stato fisico iniziale }
           posizione:=in_piedi ;
           velocita:=cammina ;
           fillchar(barrette,sizeof(barrette),0);
      end ;
      CommandLineParameters ;
      with statoAUX do begin    { tutte le possibilit… attivate... }
           grafica:=[g_fkeys,g_soffitto,g_oggetto] ;
           prolix:=[p_azioni,p_stanze,p_oggetti] ;
           stampante:=s_niente ;   { ...tranne la stampante }
      end {with} ;
end { unit globals } .
