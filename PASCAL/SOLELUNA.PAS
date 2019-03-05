program soleluna;
{ spezza soleluna.byt in sole.byt e luna.byt }

uses drivers, graph ;

type astro = array[0..26,0..3] of byte ;
     img = record
                 dx,dy:word ;
                 it:astro ;
           end ;

var inf:file of astro;
    astri : array[1..2] of astro ;
    outf : file ;
    i:integer ;
    size:word ;
    pt:pointer ;
    solu:array[1..2] of img ;
    gd,gm : integer ;

begin
     gd:=att400; gm:=att400hi; initgraph(gd,gm,'');
     assign(inf,'soleluna.byt');
     reset(inf);
     read(inf,astri[1],astri[2]) ;
     close(inf) ;
     for i:=1 to 2 do begin
         solu[i].dx:=31; solu[i].dy:=26 ;
         solu[i].it:=astri[i] ;
         PutImage(0,0,solu[i],normalput);
         if (i=1) then assign(outf,'sole.byt') else assign(outf,'luna.byt') ;
         rewrite(outf,1) ;
         size:=ImageSize(0,0,31,26) ;
         GetMem(pt,size) ;
         GetImage(0,0,31,26,pt^) ;
         BlockWrite(outf,pt^,size) ;
         close(outf) ;
         FreeMem(pt,size) ;
     end ;
end .