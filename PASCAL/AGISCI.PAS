UNIT agisci ;
{ contiene le procedure per :
  - muoversi di stanza in stanza
  - prendere/posare oggetti
  - emettere descrizioni
  - modificare la Variabile di Stato per simulare lo stato fisico
  - usare i vari oggetti in tutti i modi possibili
}

INTERFACE

uses globals, scroll, funckeys, room, status, musica, objects ;

procedure BuildObjTables ;

procedure azione(az,o1,o2:integer);  { compie l'azione specificata }

procedure InitEveryThing ;

IMPLEMENTATION

procedure InitEveryThing ; { pubblica }
begin
     InitQuadranti ;
     InitStatus ;
     InitPergam ;
     InitRoom ;
     OutObject(0);
     OutMenu(1);
end ;

procedure OutMsg(n:indice_msg);
begin
     writeln(perg,messaggio(n))
end ; { OutMsg }

procedure LookForObjs(dove:integer ; var ls:t_list_objs );
var i:indice_obj ;
    c:integer ;
begin
     fillchar(ls,sizeof(ls),0);
     c:=0;
     for i:=1 to tot_objs do if obj^[i].dove = dove then begin
         inc(c) ;
         ls[c]:=i ;
     end ;
end ;

procedure BuildObjTables ; { pubblica }
var c:integer ;
    i:indice_dato ;
begin
     LookForObjs(-1,myobjs);
     LookForObjs(stato.dovesei,L_objs);
     with stato do begin
          trasporta[tot_peso]:=0.0 ;
          trasporta[tot_ingombro]:=0.0 ;
          c:=1;
          while myobjs[c]<>0 do begin
                i:=tipo2dato(obj^[myobjs[c]].tipo);
                trasporta[tot_peso]:=trasporta[tot_peso]+
                                           c_miria[dato^[i].dati[d_peso]] ;
                trasporta[tot_ingombro]:=trasporta[tot_ingombro]+
                                   dato^[i].dati[d_ingomb] ;
                inc(c);
          end ; { while }
     end ; { with stato }
     OutInventari ;
end ; { build obj tables }

procedure esci(parete,door:byte) ;     { parete :: 1..4 ; door :: 1..2 }
var np:indice_porta ;
    us:indice_luogo ;
begin
    with luogo^[stato.dovesei].pareti[parete] do begin
          if door=1 then begin np:=n_porta1 ;
                               us:=n_uscita1 ;
          end else if door=2 then begin
                               np:=n_porta2 ;
                               us:=n_uscita2 ;
          end else Internal_Error('P:r:d','esci',door) ; { parameter Door out of range }
          with porta^[np] do begin
               if (tipo=p_passaggio) or ((tipo=p_porta)and(stato=p_aperto))
               then begin
                    globals.stato.dovesei := us ;
                    PassedThrough := np ;
                    BuildObjTables ;
                    DrawRoom ;
               end else outmsg(5);
          end ;
     end ;
end ; { esci }

procedure Prendi(og:indice_obj) ;
begin
     if obj^[og].dove<>stato.dovesei then Internal_Error('P:r:o','prendi',og)
     else with obj^[og] do
          if not([d_visibi,d_sposta] <= stato ) then writeln(perg,messaggio(11)+' '+NomeObj(og,1),'.')
          else begin
               dove:=-1 ;
               if nasconde<>0 then obj^[nasconde].stato:=obj^[nasconde].stato - [d_visibi] ;
               BuildObjTables ;
               if p_azioni in statoAUX.prolix then writeln(perg,messaggio(12)+' '+nomeobj(og,1)+'.') ;
     end;
end ; { Prendi }

procedure Posa(og:indice_obj) ;
begin
     if obj^[og].dove<>-1 then Internal_Error('P:r:o','posa',og)
     else with obj^[og] do begin
               dove:=globals.stato.dovesei ;
               BuildObjTables ;
               if p_azioni in statoAUX.prolix then writeln(perg,messaggio(13)+' '+nomeobj(og,1)+'.');
     end;
end ; { Posa }

procedure apri(n:integer);
var par,por,np:integer ;
begin
     par:=(n+1) div 2 ;
     por:=(n+1) mod 2 + 1 ;
     with luogo^[stato.dovesei].pareti[par] do
          if por=1 then np:=n_porta1
          else np:=n_porta2 ;
     with porta^[np] do begin
          if tipo=p_passaggio then outmsg(6)
          else if tipo=p_ostruito then outmsg(10)
          else begin
               if stato=p_aperto then outmsg(6)
               else if stato=p_bloccato then outmsg(9)
               else begin
                         stato:=p_aperto ;
                         if p_azioni in statoAUX.prolix
                            then outmsg(14);
                         PassedThrough := np ;
                         DrawRoom ;
               end
          end
     end
end ; { apri }

procedure chiudi(n:integer);
var par,por,np:integer ;
begin
     par:=(n+1) div 2 ;
     por:=(n+1) mod 2 + 1 ;
     with luogo^[stato.dovesei].pareti[par] do
          if por=1 then np:=n_porta1
          else np:=n_porta2 ;
     with porta^[np] do begin
          if tipo=p_passaggio then outmsg(8)
          else if tipo=p_ostruito then outmsg(7)
          else begin
               if stato=p_chiuso then outmsg(7)
               else if stato=p_bloccato then outmsg(7)
               else begin
                         stato:=p_chiuso ;
                         if p_azioni in statoAUX.prolix
                            then outmsg(15);
                         PassedThrough := np ;
                         DrawRoom ;
               end
          end
     end
end ; { chiudi }

procedure descrporta(n:integer);
var par,por,np:integer ;
begin
     par:=(n+1) div 2 ;
     por:=(n+1) mod 2 + 1 ;
     with luogo^[stato.dovesei].pareti[par] do
          if por=1 then np:=n_porta1
          else np:=n_porta2 ;
     with porta^[np] do begin
          if descrizione=0 then outmsg(2)
          else writeln(perg,frase^[descrizione]^) ;
     end ;
end ;

procedure descrobj(n:indice_obj);
begin
     with obj^[n] do
          if descrizione=0 then writeln(perg,messaggio(3)+' '+nomeobj(n,1)+'.')
          else writeln(perg,frase^[descrizione]^) ;
end ;

procedure descrstanza ;
begin
     with stanza^[luogo^[stato.dovesei].n_stanza] do
          if descrizione=0 then outmsg(1)
          else writeln(perg,frase^[descrizione]^) ;
end ;

{R+} { per accorgersi di eventuali incongruenze }
procedure azione(az,o1,o2:integer);
var x,y,z:integer ;
begin
     case az of
        -1:prendi(o1);
        -2:posa(o1);
        -3:descrobj(o1);
        -4:descrstanza;
        -5:apri(o1);
        -6:chiudi(o1);
        -7:descrporta(o1);
        -8:stato.velocita:=t_velocita(o1-1) ;
        -9:stato.posizione:=t_posizione(o1-1) ;
        -10:begin
             if o1>=7 then begin
                 x:=NumeroPorte(stato.dovesei,o1-6) ;
                 if x=0 then writeln(perg,'Come posso uscire da una parete senza uscite ?')
                 else if x=1 then esci(o1-6,1)
                 else if x=2 then GotoMenu(-9-(o1-6)) ;
             end else   { o1 = (#porta-1) + 2*parete }
                 esci((-o1) div 2, 1+ (-o1) and 1 )  ;
            end ;
        -11:begin
                 DrawArrows ;
                 LedWait(3) ;
                 DrawRoom ;
            end ;
        -13:with statoAUX do case o1 of
                 1:prolix:=prolix+[p_azioni] ;
                 2:prolix:=prolix-[p_azioni] ;
                 3:prolix:=prolix+[p_stanze] ;
                 4:prolix:=prolix-[p_stanze] ;
                 5:prolix:=prolix+[p_oggetti] ;
                 6:prolix:=prolix-[p_oggetti] ;
            end ;
        -14:with statoAUX do case o1 of
                 1:grafica:=grafica+[g_soffitto] ;
                 2:grafica:=grafica-[g_soffitto] ;
                 3:grafica:=grafica+[g_oggetto] ;
                 4:grafica:=grafica-[g_oggetto];
                 5:grafica:=grafica+[g_fkeys];
                 6:grafica:=grafica-[g_fkeys];
            end ;
        -15:statoAUX.stampante:=t_stampante(o1-1) ;
        -18:if o1=10 then MusicOff
            else if o1=9 then MusicOn ;
        else if az>0 then with verbo^[az] do begin
             write(perg,spiega,' ',nomeobj(o1,1));
             if tipo<>0 then begin
                if prep<>0 then write(perg,' ',prepos[prep]);
                write(perg,' ',nomeobj(o2,1));
             end ;
             writeln(perg);
        end ;
     end ; { case }
end ;  { azione }
{$R-}

end . { unit agisci }
