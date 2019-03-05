PROGRAM loader;

{$i olivetti.inc}

TYPE
     rows=array[0..31] OF BYTE;
     imm1=array[0..255] OF ^rows;
     wrkstring=STRING[80];

VAR

    photo_code      :imm1;
    codename        :wrkstring;
    grafica         :INTEGER;

PROCEDURE getmemory(VAR where:imm1);
VAR
    i:BYTE;
BEGIN
     FOR i:=0 TO 255 DO NEW(where[i])
END{getmemory};

PROCEDURE getimage(name:wrkstring;VAR where:imm1);
VAR
    getin:FILE OF rows;
    j,i  :BYTE;
BEGIN
     ASSIGN(getin,name);
     RESET(getin);
     FOR i:=0 TO 255 DO
             READ(getin,where[i]^);
     CLOSE(getin)
END{getimage};

function menu:integer;
var ans:integer;
BEGIN
     CLRSCR;
     REPEAT
       WRITELN('Scegli la risoluzione grafica :  1 = 320*200');
       WRITELN('                                 2 = 640*200');
       WRITE('                                 3 = 640*400 (solo Olivetti) ');
       READLN(ans);
     UNTIL (ans>=1) AND (ans<=3);
     menu:=ans;
     WRITELN
END{menu};

PROCEDURE drawing(VAR image:imm1);
VAR
  riga,colonna :BYTE;
  incr,x,y     :INTEGER;

BEGIN
  screen(grafica);
  y:=0;
  FOR riga:=0 TO 255 DO
      BEGIN
      y:=y+1;
      x:=1;
      FOR colonna:=0 TO 31 DO
          FOR incr:=0 TO 7 DO
              BEGIN
                IF ((image[riga]^[colonna]) and (128 SHR incr))<>0 THEN
                        plot (x,y,1);
                x:=x+1
              END
      END;
REPEAT UNTIL KEYPRESSED
END{drawing};

PROCEDURE options;

BEGIN

END{options};

BEGIN { MAIN }
     getmemory(photo_code);
     WRITE('Picture file name: ');
     READLN(codename);
     getimage(codename,photo_code);
     grafica:=menu;
     drawing(photo_code);
     options
END { MAIN }.
