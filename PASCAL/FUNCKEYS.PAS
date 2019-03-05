UNIT FuncKeys ;
{ Gestisce la finestra dei tasti funzione }

INTERFACE

uses dos,crt,graph, globals, drivers ;

PROCEDURE OutKeyTitle (s:string) ;     { disegna titolo tasti funzione }

PROCEDURE DelFKeys ;                   { disegna la mascherina vuota }

PROCEDURE OutFKey(n:byte; s:string) ;  { scrive in un tasto. Va a capo ad
                                         ogni carattere '@' (#64)         }

PROCEDURE OutMenu(n:shortint);         { Disegna l'intero menu' numero n
                                         (di tipo "speciale" se n<0)     }

FUNCTION ReadKeyboard : integer ;      { restituisce un codice indicante
                                         il tasto che Š stato premuto
                                         (** NON aspetta il tasto **) }

PROCEDURE RejectKey(key:integer) ;      { Mette il tasto 'key' in lista
                                          d'attesa, cioŠ sar… il primo in
                                          uscita da ReadKeyboard           }

PROCEDURE FlushKeyboard ;  { svuota il buffer di tastiera }

{ INTERPRETAZIONE dell'uscita di ReadKeyboard (R.K.) :
         R.K.   |  significato
      ----------|---------------------------------
         = 0    |  nessun tasto premuto
         > 0    |  carattere normale 1..255
         < 0    |  codice esteso tasti funzione & C.   }

const   { costanti con cui confrontare 'R.K.' per conoscere il tasto.
          k_xx : codice (esteso<0 o normale>0) del tasto xx }

     k_f1 = -59 ;   k_f2 = -60 ;
     k_f3 = -61 ;   k_f4 = -62 ;
     k_f5 = -63 ;   k_f6 = -64 ;      { tasti funzione }
     k_f7 = -65 ;   k_f8 = -66 ;
     k_f9 = -67 ;   k_f0 = -68 ;

     k_home = -71 ;   k_up  = -72 ;  k_pgup = -73 ;
     k_lt   = -75 ;                  k_rt   = -77 ;   { tastierino numerico }
     k_end  = -79 ;   k_dn  = -80 ;  k_pgdn = -81 ;
     k_ins  = -82 ;   k_del = -83 ;

     k_tab  =  9 ;   k_esc  = 27 ;   k_back =  8 ;
     k_ret  = 13 ;   k_bar  = 32 ;   k_star = 42 ;       { varŒ }
     k_plus = 43 ;   k_mins = 45 ;

PROCEDURE MenuSelect ( var verbo:shortint ; var obj1,obj2:integer ) ;
{ permette di compiere un passo nel " menu tree " verso un figlio, o di
  abortire verso il main. Restituisce il # dell'azione da compiere e i
  # degli oggetti su cui compierle. In generale obj# Š un indice_obj, ma
  (a seconda di 'verbo') pu• anche essere indice_porta, indice_stanza,
  indice_luogo, ...                                                       }

PROCEDURE GotoMenu (n:indice_menu) ;  { Forza un men— dall'esterno,
  indipendentemente dallo stato attuale del MENU TREE }

PROCEDURE LedWait(n:byte) ;  { attende la pressione di CAPS/NUM LOCK facendo
 lampeggiare il relativo LED. Si pu• anche premere un altro tasto, che rimarr…
 in sospeso nel buffer di tastiera (e quindi anche in quello di ReadKeyboard) }

IMPLEMENTATION

const
     wh=95 ;  { larghezza=96 pixel }
     ht=197 ; { altezza=198 pixel }
     sx=320; sy1=23;        { TL mascherina }
     sy2=0 ;                { TL titolo }
     wht= 28 ;              { altezza finestrella }
     blink=1000 ;

var pt:pointer ;    { indirizzo della bitmap della mascherina di tasti }

    cur_verbo : shortint ;  { azione scelta nei menus }
    cur_obj1, cur_obj2 : indice_obj ;  { parametri scelti da menu }
    cur_obj_ptr: ^ indice_obj ; { "alias" per cur_obj# (v. commenti oltre) }
    cur_menu : shortint ; { # menu corrente }
    menu_loc : t_menu ; { copia locale del menu in output }

    num_objs : array [1..10] of integer ; { associazione #obj <- funcKey }

    LastKey : integer ;    { memorizza il tasto per la coppia ReadKeyboard/
                             /RejectKey }

procedure Mascherina; external ;
{$L tasti.obj}

Procedure OutKeyTitle(s:string) ;
var i:byte ;
    s1,s2:str25;
begin
     for i:=1 to length(s) do s[i]:=_8to7[s[i]] ; { Niente accenti & C. }
     { Disegna lo sfondo del rettangolo per il titolo }
     SetViewPort(sx,sy2,sx+wh,sy1-2,true) ;
     SetFillStyle(SolidFill,1) ;
     bar(0,0,wh,sy1-sy2-1) ; { fondo bianco }
     SetLineStyle(UserBitLn,$5555,ThickWidth) ;
     SetColor(0) ;
     rectangle(0,0,wh,sy1-sy2-2) ;  { bordo grigio }
     SetLineStyle(SolidLn,0,1) ;
     setusercharsize(1,1,1,1) ;
     SelectFont(SmallFont,0) ;
     SetTextJustify(CenterText,TopText) ;
     { riconosci la 1^a e la 2^a riga, controlla la lunghezza e scrivi }
     i:=pos('@',s);
     s1:=copy(s,1,i-1);
     s2:=copy(s,i+1,255);
     OutTextXY(wh div 2, 1, s1 ) ;
     OutTextXY(wh div 2, TextHeight(s1)+1 , s2 ) ;
end ;

procedure DelFKeys ;
begin
     SetViewPort(sx,sy1,sx+wh,sy1+ht,true) ;
     PutImage(0,0,pt^,NormalPut) ;
     OutKeyTitle('');
end;

PROCEDURE OutFKey(n:byte; s:string) ;
var bx,by,y:integer ;
    ct,ln:byte ;
    s1:string ;
begin
     SetUserCharSize(7,8,1,1) ;
     SelectFont(SmallFont,UserCharSize) ;
     SetTextJustify(CenterText,TopText) ;
     setcolor(0) ;
     if odd(n) then bx:=9 else bx:=55 ;
     by:=9+((n-1)shr 1)*(10+wht) ;
     SetViewPort(sx+bx,sy1+by,sx+bx+31,sy1+by+wht-1,ClipOn) ;
     setfillstyle(SolidFill,1);
     bar(0,0,31,wht);
     ln:=length(s);
     ct:=1 ;
     y:=0 ;
     { scrive una parola per riga (righe separate da '@') }
     while ( (textheight('A')+y) < wht ) and (ct<=ln) do begin
           s1:='';
                 while (ct<=ln) and (s[ct]<>'@') do begin
                       s1:=s1+upcase(_8to7[s[ct]]) ;
                       inc(ct) ;
                 end ;
                 inc(ct) ;
           OutTextXY(15,y,s1) ;
           inc(y,textheight(s1)) ;
     end ;
end ;   { OutFKey }

procedure OutIcon(n:byte; c:indice_icona);
type fill = record
          x,y:word ;
          icon:t_icona ;
     end;
var img:fill ;
    bx,by:integer;
begin
     img.x:=31;
     img.y:=27;
     img.icon:=icona^[c] ;
     if odd(n) then bx:=9 else bx:=55 ;
     by:=9+((n-1)shr 1)*(10+wht) ;
     SetViewPort(sx+bx,sy1+by,sx+bx+31,sy1+by+wht-1,ClipOn) ;
     setfillstyle(SolidFill,1);
     bar(0,0,31,wht);
     setcolor(1);
     PutImage(0,0,img,notPUT);
end;

procedure FlashKey(n:byte);
type fill = record
          x,y:word ;
          icon:t_icona ;
     end;
var img:fill ;
    bx,by,i:integer;
begin
     img.x:=31;
     img.y:=27;
     if odd(n) then bx:=9 else bx:=55 ;
     by:=9+((n-1)shr 1)*(10+wht) ;
     SetViewPort(sx+bx,sy1+by,sx+bx+31,sy1+by+wht-1,ClipOn) ;
     FillChar(img.icon,sizeof(t_icona),$ff);
     for i:=1 to 50 do PutImage(0,0,img,XORput);
end ;

procedure MakeKey(n:byte; az,nm:shortint; ic:indice_icona; des:str25 ) ;
begin
     with menu_loc.tasto[n] do begin
          azione:=az ;
          nextmenu:=nm ;
          disegno:=ic ;
          frase:=des ;
     end ;
end ;

procedure Lista(var ls:t_list_objs; var ctr:integer; nm:integer ) ;
var i:integer ;
    j:byte ;
    s:str25 ;
begin
     for i:=1 to ListObjSize do
         if (ls[i]<>0)and(d_visibi in obj^[ls[i]].stato)
            then begin
                 inc(ctr) ;
                 if ctr<=10 then begin
                    s:=nomeObj(ls[i],0);
                    if length(s)>6 then s:=copy(s,1,6)+'@'+copy(s,7,255);
                    if length(s)>13 then s:=copy(s,1,13)+'@'+copy(s,14,255);
                    num_objs[ctr]:=ls[i] ;
                    MakeKey(ctr, 0,nm, 0,s);
                 end ;
     end ;
end ;  { Lista }

procedure ListaVerbi(og:indice_obj; where:byte; var ctr:integer ) ;
var i:indice_verbo ;                { ^  1|2 }
    j:indice_dato ;
    v:set of indice_verbo ;
begin
     j:=tipo2dato(obj^[og].tipo) ;
     with dato^[j] do if where=1 then v:=verbo_L
                         else v:=verbo_P ;
     for i:=1 to tot_verbi do
         if i in v then begin
               inc(ctr) ;
               if ctr<=10 then with verbo^[i] do MakeKey(ctr, i,-tipo, 0,nome);
         end ;
end ;  { ListaVerbi }


PROCEDURE OutMenu(n:shortint);         { pubblica }
var i:integer ;
    x,y,z:integer ;
begin
     DelFkeys;
     FillChar(num_objs,sizeof(num_objs),0);
     FillChar(menu_loc,sizeof(menu_loc),0);
     if n>0 then menu_loc:=menu^[n]
     else if n<0 then case n of
         -1,-2,-3:begin
                 if cur_verbo<=0 then menu_loc.titolo := 'Scegli un@OGGETTO'
                 else menu_loc.titolo:=prepos[verbo^[cur_verbo].prep]+'@cosa?';
                 x:=0 ;
                 if cur_verbo=0 then y:=n-3 else y:=0 ;
                 if (n=-1)or(n=-3) then lista(L_objs,x,y);
                 if (n=-2)or(n=-3) then lista(myobjs,x,y);
             end ;
         -4,-5,-6:begin
                 menu_loc.titolo := 'Che cosa@ci fai?' ;
                 x:=0 ;
                 if (n=-4)or(n=-6) then listaVerbi(cur_obj1,1,x);
                 if (n=-5)or(n=-6) then ListaVerbi(cur_obj1,2,x);
             end ;
         -7:begin { lista delle porte nella stanza }
              if cur_verbo=-7 then menu_loc.titolo:='Quale PORTA@vuoi GUARDARE ?'
              else if (cur_verbo=-5)or(cur_verbo=43) then
                   menu_loc.titolo:='Quale PORTA@vuoi APRIRE ?'
              else if (cur_verbo=-6)or(cur_verbo=44) then
                   menu_loc.titolo:='Quale PORTA@vuoi CHIUDERE ?' ;
              for x:=1 to 4 do begin
                  y:=NumeroPorte( stato.dovesei ,x) ;
                  if y=1 then MakeKey(x*2-1, 0,0, 44+x,'') ;
                  if y=2 then begin
                                   MakeKey(x*2-1, 0,0, x*2+47,'');
                                   MakeKey(x*2, 0,0, x*2+48,'');
                              end ;
              end ;
            end ;
         -10,-11,-12,-13:begin { 2 porte nella parete (-n-9) }
               menu_loc.titolo:='Da quale PORTA@vuoi USCIRE ?' ;
               x:=-n-9 ;
               case x of
                 1:begin y:=2; z:=4 end;
                 2:begin y:=4; z:=3 end;
                 3:begin y:=3; z:=1 end;
                 4:begin y:=1; z:=2 end;
               end ;
               MakeKey(y, -10,0, 47+x*2,'') ;
               MakeKey(z, -10,0, 48+x*2,'') ;
               num_objs[y]:=-x*2 ;
               num_objs[z]:=-(x*2+1) ;
            end ;
         else Internal_Error('P:r:n','OutMenu',n); { Parameter N out of range }
     end { case } ;
     with menu_loc do begin
         OutKeyTitle(titolo);
         for i:=1 to 10 do with tasto[i] do begin
            if (disegno<>0)and(frase='') then OutIcon(i,disegno)
            else if (disegno=0)and(frase<>'') then OutFKey(i,frase)
            else if (disegno<>0) or (frase<>'') then
                 if g_fkeys in statoAUX.grafica
                 then OutIcon(i,disegno)
                 else OutFKey(i,frase) ;
         end ;
     end ;
end ; { OutMenu }

{ *** procedure per pilotare i LEDs della tastiera *** }

procedure WriteKbd(v:byte) ;
const stat=$64 ;
      kscan=$60 ;
begin
     while (port[stat] and 2)<>0 do {NOP} ;
     port[kscan]:=v ;
end ; { WriteKbd }

procedure LedOn(l:byte) ;
begin
     WriteKbd($13);
     WriteKbd(l or $80);
end ; { LedOn }

procedure LedOff(l:byte) ;
begin
     WriteKbd($13);
     WriteKbd(l);
end ; { LedOff }

procedure RestoreLeds ;
var R:registers ;
begin
     r.ah:=2 ;
     intr($16,r);
     if (r.al and 32)=0 then LedOff(2) else LedOn(2);
     if (r.al and 64)=0 then LedOff(1) else LedOn(1);
end ; { RestoreLeds }

Function KbdStat:byte ;
var r:registers ;
begin
     r.ah:=2 ;
     intr($16,r);
     KbdStat := r.al ;
end ; { KbdStat }

procedure LedWait(n:byte) ;  { pubblica }
var flip:boolean ;
    ctr:integer ;
begin
     flip:=true ;
     ctr:=1 ;
     repeat
           dec(ctr) ;
           if ctr=0 then begin
              ctr:=blink ;
              if flip then ledon(n) else ledoff(n) ;
              flip:=not flip ;
           end ;
     until ((KbdStat and $0f)<>0) ;
     RestoreLeds ;
end ; { LedWait }

FUNCTION ReadKeyboard : integer ; { pubblica }
var c:char ;
begin
     if LastKey<>0 then ReadKeyboard:=LastKey else begin
        if KeyPressed then begin
           c:=ReadKey ;
           if c=#0 then begin
              c:=ReadKey ;
              ReadKeyboard := - ord(c) ;
           end else ReadKeyboard := ord(c) ;
        end else ReadKeyboard := 0 ;
     end ;
     LastKey:=0 ;
end ; { ReadKeyboard }

procedure RejectKey(key:integer); {pubblica}
begin
     LastKey:=key ;
end ; { RejectKey }

PROCEDURE FlushKeyboard ; { pubblica }
begin
     repeat until ReadKeyboard = 0 ;
end ; { FlushKeyboard }

procedure ResetTree ;
begin
     cur_menu:=1 ;
     cur_verbo:=0;
     cur_obj1:=0;
     cur_obj2:=0;
     cur_obj_ptr := addr(cur_obj1);  { si inizia sempre a scegliere il
                                       1^ oggetto }
end; { ResetTree }

procedure Simula(s_verbo:shortint; s_obj1,s_obj2:integer ) ;
begin
     cur_menu:=0 ;
     cur_verbo:=s_verbo ;
     cur_obj1:=s_obj1 ;
     cur_obj2:=s_obj2 ;
end ; { simula }

PROCEDURE GotoMenu (n:indice_menu) ; { pubblica }
begin
     ResetTree ;
     cur_menu:=n ;
     OutMenu(n) ;
end ;

PROCEDURE MenuSelect ( var verbo:shortint ; var obj1,obj2:integer ) ;
var rk,key,i:integer ;                           { pubblica }
    lst_menu:shortint ;
begin
     if cur_menu=0 then begin
        ResetTree ;
        OutMenu(cur_menu);
     end ;
     lst_menu:=cur_menu ;
     rk:=ReadKeyboard ;
     key:=k_f1-rk+1 ;
     if (key>=1) and (key<=10) then with menu_loc.tasto[key] do begin
        if (azione<>0) or (nextmenu<>0) or (disegno<>0) or (frase<>'')
        then begin
           FlashKey(key);
           cur_menu := nextmenu ;
           if azione<>0 then cur_verbo:=azione ;
           cur_obj_ptr ^ := num_objs [key] ;  { oggetto scelto }
           if cur_obj_ptr^<>0 then cur_obj_ptr:=addr(cur_obj2)
{ se ho effettivamente scelto un oggetto, mi preparo per scegliere
  il secondo, aggiornando l'alias (se era gi… il secondo, nulla
  varia). Il metodo Š abbastanza "sporco", ma funziona. }
              else cur_obj_ptr^:= key ;
{ se non c'Š un argomento pronto,
  trasmetto in uscita il numero di tasto premuto nell'ultimo
  menu selezionato }
        end ;
     end else case rk of
         k_esc:ResetTree ;   { ESC => annulla azione }
         k_rt:simula(-10,7,0) ;  { --> }
         k_dn:simula(-10,8,0) ;  { .|, }
         k_lt:simula(-10,9,0) ;  { <-- }
         k_up:simula(-10,10,0);  { '|` }
         k_tab:begin         { |<--   TAB => switcha tra frasi e icone }
                 lst_menu:=0 ;
                 with statoAUX do
                 if g_fkeys in grafica then grafica:=grafica-[g_fkeys]
                                       else grafica:=grafica+[g_fkeys] ;
               end ;
         k_pgup:if stato.posizione > in_piedi  { PgUp => alza la POSIZIONE }
                   then dec(stato.posizione) ;
         k_pgdn:if stato.posizione < a4gambe   { PgDn => abbassa la POSIZIONE }
                   then inc(stato.posizione) ;
         k_plus:if stato.velocita < corri      { '+, => aumenta la VELOCITŽ }
                   then inc(stato.velocita) ;
         k_mins:if stato.velocita > punta_piedi { '-, => diminuisce VELOCITŽ }
                   then dec(stato.velocita) ;
         k_star:simula(-15,1,0) ;               { '*, => spegne la stampante }
         k_ret:simula(-128,0,0);   { ferma il gioco }
     end ;
     if cur_menu=0 then begin    { non c'Š un menu successivo ==> si deve
                                   compiere una qualche azione            }
        verbo:=cur_verbo ;
        obj1:=cur_obj1 ;  { aggiornati indirettamente tramite il loro }
        obj2:=cur_obj2 ;  { comune "alias" cur_obj_ptr                }
        ResetTree ;
        Lst_menu:=1 ;
        cur_menu:=0 ;
     end else verbo:=0 ;  { indica che NON ci sono azioni da compiere al
momento per le routines esterne: si Š solo passati ad un altro menu,
eventualmente con qualche azione "pendente", che per• non Š ancora
attuabile e quindi viene 'custodita' localmente  }
     if lst_menu<>cur_menu then OutMenu(cur_menu);
end ; { MenuSelect }

begin {init code}
      pt:=@Mascherina ;
      ResetTree ;
      RejectKey(0);
end. { unit FuncKeys }
