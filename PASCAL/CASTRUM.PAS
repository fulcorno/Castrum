program SanctiGeorgiiCastrum ;
{ Avventura nel castello di San Giorgio Canavese - Jena Soft 1988 }

uses graph, drivers, funckeys, status, globals, musica, agisci ;
var
    s:string ;
    az:shortint ;
    o1,o2:integer ;
    again:boolean ;
    h:integer ;
    i:t_barrette ;
    mus:file ;
    pt:pointer ;

(* N.B. Le linee inizianti con {**} sono PROVVISORIE per il demo. Non
        compariranno nella versione finale                             *)

begin
     writeln('Loading music');
     assign(mus,'ballo.mca');
     reset(mus,1);
     getmem(pt,filesize(mus));
     blockread(mus,pt^,filesize(mus));
     close(mus) ;
     play(pt) ;
     LoadVariables('') ;
     InitGraphScreen(0) ;
     scompari(0);
     initeverything ;
     h:=0;
     again:=true ;
     BuildObjTables ;
     OutStatus ;
{**} stato.dovesei:=1 ;
     compari(150) ;
     azione(-11,0,0) ;
     while again do begin
{**}     UpdateTempo(10);
         OutSoleLuna ;
         OutWatch ;
         MenuSelect(az,o1,o2) ;
         if az<>0 then azione(az,o1,o2);
{**}     for i:= punti to antipatia do with stato do begin
{**}         barrette[i]:=barrette[i]+random-0.4 ;
{**}         if barrette[i]>100 then barrette[i]:=10+random(10) ;
{**}         if barrette[i]<0 then barrette[i]:=random(100) ;
{**}     end ;
         OutStatus ;
{**}     if az=-128 then again:=false ;
     end ;
     scompari(150);
     restoreCRTmode ;
end . { prog 'test' }
