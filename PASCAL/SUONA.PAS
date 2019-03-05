program suona ;

uses musica, crt ;

var f:file ;
    p:pointer ;
    c:char ;

begin
     assign(f,paramstr(1)+'.mca');
     reset(f,1);
     GetMem(p,FileSize(f));
     BlockRead(f,p^,fileSize(f));
     close(f);
     play(p);
     repeat
           write(^m,CurrentNote:4)
     until keypressed ;
     c:=readkey ;
end.
