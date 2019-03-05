program TestCHRFiles ;

uses graph ;

var pt:pointer ;
    f:file ;
    gm,gd:integer ;
    fnum,i,j:integer ;
    fs:word ;
    n:array[0..10]of integer ;
    s1:string ;

const s:array[1..10]of string[4] = ( 'bold','euro','goth','lcom','litt',
                                     'sans','scri','simp','trip','tscr' ) ;

begin
     gd:=att400; gm:=att400hi ;
     InitGraph(gd,gm,'c:\turbo4\');
     n[0]:=0;
     for i:=1 to 10 do begin
         assign(f,'c:\quattro\'+s[i]+'.chr');
         reset(f,1);
         fs:=filesize(f);
         getmem(pt,fs);
         blockread(f,pt^,fs);
         close(f) ;
         n[i]:=registerbgifont(pt) ;
     end;
     for i:=1 to 10 do begin
         ClearDevice ;
         SetTextStyle(n[i],horizdir,4);
         str(graphresult:2,s1);
         outtext('Font name: '+s[i]+' -> n^ '+s1);
         moveto(0,50);
         for j:=32 to 126 do begin
             if GetX>600 then moveto(0,getY+textheight('[{\|')+10);
             OutText(chr(j));
         end ;
         readln ;
     end ;
end .