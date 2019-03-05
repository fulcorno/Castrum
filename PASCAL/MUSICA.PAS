unit Musica ;
{ installa un INTERRUPT DRIVER per suonare in background qualsiasi musica
  precedentemente "compilata" con CMUS.EXE }

 interface
{*********}

uses Dos, Crt ;

Procedure MusicOn ;    { installa il driver }
Procedure MusicOff ;   { de-installa il driver }
Procedure Play(music:pointer) ;  { inizia a suonare la musica specificata }
Function CurrentNote:integer ; { restituisce il # della nota che si sta
                             per suonare. Convenzione:
                                      # = 0 nessun brano in esecuzione
                                      # < 0 brano sospeso alla nota abs(#)
                                      # > 0 brano in esecuzione alla nota # }
Procedure GoToNote(n:word) ; { salta alla nota specificata }

implementation

const IntNo = $1c ; { numero dell'interrupt da dirottare }

{$i musica.inc}

type stat = (legato, legastacca, staccato ) ;

var SaveInt1C : pointer ; { vecchio indirizzo dell'int $1c }
    SaveExitAddr : pointer ; { vecchio valore di ExitProc }
    cur_note : word ; { nota che deve essere suonata. Se Š =0, non suona }
    countdown : word ; { quanto bisogna ancora aspettare per suonare la
                         prossima nota }
    MusicAddr : ^brano ;
    installed : boolean ;
    status : stat ;
    pausing:boolean ;

{F+} { richiesto per gli interrupt }
procedure BackGround ; interrupt ;
begin
  if MusicAddr <> NIL then begin
     if countdown=0 then begin
        if status<>legato then NoSound ;
        if pausing then begin
           inc(cur_note) ;
           with MusicAddr^[cur_note] do begin
             countdown:=dura ;
             if dura = 0 then { convenzione per COMANDO SPECIALE }
                case freq of
                  0:begin
                         NoSound ;
                         cur_note:=0 ;
                         MusicAddr := NIL ;
                    end ;
                  -1:cur_note:=0 ;
                  -3:status:=legato ;
                  -2:status:=legastacca ;
                  -4:status:=staccato ;
                  else cur_note:=freq-1 ;
             end {case} else begin
                 if status=staccato then pausing:=false ;
                 if freq=0 then NoSound else Sound(freq) ;
             end ; { if dura }
           end { with }
        end { if pausing } else begin
            pausing:=true ;
            countdown:=0 ;
        end ; { if pausing }
     end { if countdown } else dec(countdown) ;
  end ; { if musicaddr }
end ; { background }
{$F-}

procedure Install ;
begin
     SetIntVec(IntNo, @ BackGround ) ;
     installed := true ;
end; {install}

procedure DeInstall ;
begin
     SetIntVec(IntNo, SaveInt1C ) ;
     installed := false ;
end; {deinstall}

Procedure MusicOn ; { PUBBLICA }
begin
     CountDown := 0 ;
     if MusicAddr<> NIL then Install ;
end ;

Procedure MusicOff ; { PUBBLICA }
begin
     DeInstall ;
     NoSound ;
end ;

Procedure Play ( music : pointer ) ; { PUBBLICA }
begin
     MusicOff ;
     MusicAddr := music ;
     cur_note := 0 ;
     if musicaddr<>NIL then MusicOn ;
end ;

Function CurrentNote : integer ;  { PUBBLICA }
begin
     if MusicAddr = NIL then CurrentNote:=0
     else if installed then CurrentNote:=cur_note+ord(cur_note=0)
          else CurrentNote:= -cur_note-ord(cur_note=0)
end ;

Procedure GoToNote(n:word) ; { PUBBLICA }
begin
     MusicOff ;
     cur_note:=n-1 ;
     countdown:=0;
     if MusicAddr <> NIL then MusicOn ;
end ;

{$F+}
procedure ShutOff ;
begin
     ExitProc := SaveExitAddr ;
     MusicOff ;
end ;
{$F-}

begin { init code }
     GetIntVec(IntNo, SaveInt1C);
     SaveExitAddr := ExitProc ;
     ExitProc := @ ShutOff ;    { assicuriamoci si scollegare l'interrupt
                                  prima di finire il programma, per evitare
                                  un sicuro CRASH ! }
     cur_note := 0 ; { Posizionati all'inizio }
     MusicAddr := NIL ; { all'inizio non ci sono brani da suonare }
     installed:=false ;
     status:=legastacca ;
     pausing:=true ;
end .