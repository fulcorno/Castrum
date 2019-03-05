{$N+} { use 8087 !! }
program LeggiLexicon ;

uses graph,crt,dos ;

type riga = array [0..63] of byte ;
     immg = array [0..399] of riga ;
     objimg = array[0..89,0..19] of byte ;

var badr:array[0..399]of word ;

procedure ReadImage( fname:string; var buffer:immg );
var f:file ;
    dum:array[0..7]of byte ;
begin
     assign(f,fname);
     reset(f,1);
     blockread(f,dum,8);
     blockread(f,buffer, sizeof(immg));
     close(f);
end ; { ReadImage }

procedure ViewImage(var buffer:immg; x,y:integer );
var i: integer ;
    c:char ;
begin
     for i:=0 to 399 do move( buffer[i],ptr($b800,badr[i+y]+x div 8)^, sizeof(riga) ) ;
end; { ViewImage }

{-------L-I-T-------}

function Lit(var buffer:immg; x,y:word) : boolean ;
begin
     lit := buffer[y,x shr 3] and (1 shl (7-(x and 7))) <>0 ;
end ; { Lit }

{----------B-L-I-T----------}

procedure blit(var b:byte; mask:byte; value:boolean);
begin
     if not value then b:=b and not mask
     else b:=b or mask ;
end;

{--------------------SCELTA---------FINESTRA-------------------}

procedure SelectWindow(var x0,y0,dx,dy:integer; var what:immg) ;
var first:boolean ;
    q:byte ;
    c:char ;
    ts:string[10] ;
begin
     SetGraphMode(att400hi);
     SetTextStyle(DefaultFont,HorizDir,1) ;
     SetTextJustify(LeftText,TopText) ;
     first:=false;
     q:=16 ;
     repeat
           viewimage(what,0,0);
           setcolor(0);
           setfillstyle(EmptyFill,0) ;
           bar(512,0,639,399) ;
           setlinestyle(userbitln,$aaaa,normwidth);
           rectangle(x0,y0,x0+dx,y0+dy);
           rectangle(x0-1,y0-1,x0+1,y0+1);
           setcolor(1);
           setlinestyle(userbitln,$5555,normwidth);
           rectangle(x0,y0,x0+dx,y0+dy);
           rectangle(x0-1,y0-1,x0+1,y0+1);
           MoveTo(530,16) ;
           str(x0,ts); OutText('('+ts+',');
           str(y0,ts); OutText(ts+') - (');
           str(x0+dx,ts); OutText(ts+',');
           str(y0+dy,ts); OutText(ts+')');
           MoveTo(530,32);
           if first then OutText('Expand')
                    else OutText(' Move') ;
           MoveTo(530,48);
           str(q,ts); OutText('Step '+ts) ;
           c:=readkey;
           if c=#0 then begin
              c:=readkey ;
              case c of
                   #$48:c:='8' ;
                   #$4B:c:='4' ;
                   #$4D:c:='6' ;
                   #$50:c:='2' ;
                   else c:=#0 ;
              end {case} ;
           end ;
           if first then
              case c of
                   '4':dec(dx,q);
                   '6':inc(dx,q);
                   '8':dec(dy,q);
                   '2':inc(dy,q);
              end
           else
              case c of
                 '4':dec(x0,q);
                 '6':inc(x0,q);
                 '8':dec(y0,q);
                 '2':inc(y0,q);
              end;
           case c of
              ' ':first:=not first ;
              '+':inc(q);
              '-':q:=abs(q-2)+1;
           end;
       until c=#13;
       if dx<0 then begin
          x0:=x0+dx;
          dx:=-dx;
       end;
       if dy<0 then begin
          y0:=y0+dy;
          dy:=-dy;
       end;
       restorecrtmode;
       clrscr;
end ; { proc SelectWindow }

{-----------------------PESCAGGIO------------------------}

procedure Pescaggio( var inp:immg; var outp:objimg;
                     x0,y0,dx,dy:integer; sc:extended ) ;
var i,j:shortint ;
    xx,yy:integer ;
    cl,mk:byte ;
    ddx,ddy:integer ;
begin
   xx := x0+dx div 2 ;
   yy := y0+dy div 2 ;
   ddx:=trunc(dx*sc/2) ;
   ddy:=trunc(dy*sc/2) ;
   for i:=-80 to 79 do begin
      cl:=(i+80)shr 3 ;
      mk:=1 shl (7-(i and 7)) ;
      for j:=-45 to 44 do
         blit( outp[j+45,cl], mk,
            not ( (abs(j)<ddy) and (abs(i)<ddx) ) or
            LIT(inp, trunc(xx+i/sc),trunc(yy+j/sc)) );
      write(^M,trunc((i+80)*100/160):2,'%');
   end
end ; { proc Pescaggio }

{--------------------DITHERING*4-----------------------}

procedure Dither( var inp:immg; var outp:objimg;
                  x0,y0,dx,dy:integer; sc:extended ) ;
var dim,thr,ptype,ctr:shortint ;
    i,j,k1,k2:shortint ;
    xx,yy,xc,yc,ddx,ddy:integer ;
    cl,mk:byte ;
begin
   dim := trunc(1/sc) ; { dimensione blocchetto }
   thr := sqr(1+2*dim) ; { pixel contenuti }
   { Output informazioni }
   writeln('Lato quadretto = ',2*dim+1) ;
   write('Valori soglia = ') ;
   for i:=1 to 5 do write(i*thr div 5,' ') ;
   writeln ;
   { Precalcoli }
   xx:=x0+dx div 2 ;
   yy:=y0+dy div 2 ;
   ddx:=trunc(dx*sc/2) ;
   ddy:=trunc(dy*sc/2) ;
   { cicli }
   for i:=-80 to 79 do begin
      cl:=(i+80)shr 3 ;
      mk:=1 shl (7-(i and 7)) ;
      for j:=-45 to 44 do begin
         ptype:=(4-((i and 1)or((j and 1) shl 1))) and 3 ;
         xc:=trunc(xx+i/sc) ;
         yc:=trunc(yy+j/sc) ;
         ctr:=0 ;
         for k1:= -dim to dim do
            for k2:= -dim to dim do
               if lit(inp,xc+k1,yc+k2) then inc(ctr) ;
         blit( outp[j+45,cl], mk, (ctr>thr*ptype div 5) OR
                                  not((abs(j)<ddy)and(abs(i)<ddx)) );
      end ;
      write(^M,trunc((i+80)*100/160):2,'%') ;
   end ;
end ;

{--------------------MEDIA----------------------}

procedure Media ( var inp:immg; var outp:objimg;
                  x0,y0,dx,dy:integer; sc:extended ) ;
var dim,thr,ctr:shortint ;
    i,j,k1,k2:shortint ;
    xx,yy,xc,yc,ddx,ddy:integer ;
    cl,mk:byte ;
begin
   dim := trunc(1/sc) ; { dimensione blocchetto }
   thr := sqr(1+2*dim) div 2 ; { pixel soglia }
   { Output informazioni }
   writeln('Lato quadretto = ',2*dim+1) ;
   { Precalcoli }
   xx:=x0+dx div 2 ;
   yy:=y0+dy div 2 ;
   ddx:=trunc(dx*sc/2) ;
   ddy:=trunc(dy*sc/2) ;
   { cicli }
   for i:=-80 to 79 do begin
      cl:=(i+80)shr 3 ;
      mk:=1 shl (7-(i and 7)) ;
      for j:=-45 to 44 do begin
         xc:=trunc(xx+i/sc) ;
         yc:=trunc(yy+j/sc) ;
         ctr:=0 ;
         for k1:= -dim to dim do
            for k2:= -dim to dim do
               if lit(inp,xc+k1,yc+k2) then inc(ctr) ;
         blit( outp[j+45,cl], mk, (ctr>thr) OR
                                  not((abs(j)<ddy)and(abs(i)<ddx)) );
      end ;
      write(^M,trunc((i+80)*100/160):2,'%') ;
   end ;
end ;

{------------------M-A-I-N-------------------}

var
    what : immg ;
    dir,s,s1:string[80] ;
    result:objimg ;
    x0,y0,dx,dy,i:integer;
    c:char;
    fatto:boolean ;
    scx,scy,sc:extended ;
    f:file ;
    srec:SearchRec ;

begin
   x0:=att400; y0:=att400hi;
   InitGraph(x0,y0,'c:\tp4'); RestoreCrtMode;
   for i:=0 to 399 do
      BAdr[i] :=(i and 3) shl 13 + 80*(i shr 2) ;
   clrscr;
   writeln('EXTRACT.EXE : riscalatore di immagini');
   writeln('Propriet… di Jena-Soft 1988.');
   repeat
      write('In che directory lavori ? ') ;
      readln(dir) ;
      FindFirst(dir+'\*.*',0,srec) ;
      while DosError=0 do begin
         write (srec.name,'':16-length(srec.name)) ;
         FindNext(srec) ;
      end ;
      writeln ;
      Write('Filename = ');
      readln(s);
      ReadImage(dir+'\'+s,what);
      x0:=256; y0:=200; dx:=0; dy:=0;
      repeat
         { Seleziona la finestra da riscalare }
         SelectWindow(x0,y0,dx,dy,what) ;
         { Informazioni sulla finestra }
         writeln('File = ',s);
         writeln('Window = (',x0,',',y0,') - (',x0+dx,',',y0+dy,')');
         if (dx=0) or (dy=0) then
         writeln('Scusa, ma questa NON Š una finestra !')
         else begin
            writeln('Scaling:');
            scx:=160/dx ;
            scy:=90/dy ;
            sc:=scx;
            if scy<sc then sc:=scy ;
            { sc == min ( DX/dx, DY/dy ) }
            writeln('Fattore di scala = ',sc:4:2);
            writeln ;
            writeln('Scegli il tipo di elaborazione:') ;
            writeln('- A - estrazione semplice (pescaggio)') ;
            writeln('- B - RIDUZIONE con binarizzazione sulla soglia del valor medio') ;
            writeln('- C - RIDUZIONE con dithering a 5 livelli di grigio sul valor medio') ;
            writeln('-ESC- non fare nulla') ;
            writeln ;
            write('--> _',^H) ;
            repeat
               c:=upcase(readkey) ;
            until (c in ['A'..'C']) or (c=#27) ;
            if c<>#27 then writeln(c)
                      else writeln('ESC') ;
            fatto:=false ;
            case c of
               'A' : begin
                        pescaggio ( what, result, x0,y0,dx,dy, sc ) ;
                        fatto:=true ;
                     end ;
               'B' : if sc<1.0 then begin
                        Media  ( what,result, x0,y0,dx,dy, sc ) ;
                        fatto:=true ;
                     end else writeln('Spiacente, la media ha senso solo in riduzione. Usa lo Smoothing.') ;
               'C' : if sc<1.0 then begin
                        Dither ( what,result, x0,y0,dx,dy, sc ) ;
                        fatto:=true ;
                     end else writeln('Spiacente, il dithering ha senso solo in riduzione. Usa lo Smoothing.') ;
            end {case} ;
         end ;
         if fatto then begin
            setgraphmode(att400hi);
            for i:=0 to 89 do move(result[i],ptr($b800,badr[i])^,20);
            setcolor(1);
            OutTextXY(0,150,'<CR> per procedere; un altro tasto per riprovare');
            c:=readkey;
            restorecrtmode;
         end else begin
            writeln('<CR> per abbandonare, un altro tasto per riprovare') ;
            c:=readkey ;
         end ;
      until (c=#13) and fatto ;
      if fatto then begin
         writeln('Con che nome salvo [.OGG] ( <CR> per non salvare )');
         readln(s1);
         if s1<>'' then begin
            writeln('Writing...');
            if pos('.',s1)=0 then s1:=s1+'.ogg' ;
            if (pos('\',s1)=0)and(pos(':',s1)=0) then s1:=dir+'\'+s1 ;
            assign(f,s1);
            rewrite(f,1);
            blockwrite(f,result,1800);
            close(f);
         end ;
      end ;
      writeln('Premi un tasto per continuare, <ESC> per smettere') ;
   until readkey = #27 ;
end.
