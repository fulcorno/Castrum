PROGRAM TV_Image (INPUT,OUTPUT);
{visualizza un'immagine precedentemente digitalizzata.}

{$i screen.inc}

TYPE
     col=array[0..255] OF BYTE;
     imm=array[0..255] OF ^col;
     rows=array[0..31] OF BYTE;
     imm1=array[0..255] OF ^rows;
     wrkstring=STRING[80];

VAR
    photo           :imm;
    photo_code      :imm1;
    filename        :wrkstring;
    codename        :wrkstring;
    lim             :INTEGER;
    grafica,scanner :INTEGER;
    answer          :CHAR;
    point           :BOOLEAN;

PROCEDURE getmemory(VAR where:imm);
VAR
    i:BYTE;
BEGIN
     FOR i:=0 TO 255 DO NEW(where[i])
END;

PROCEDURE putmemory(VAR where:imm1);
VAR
    i:BYTE;
BEGIN
     FOR i:=0 TO 255 DO NEW(where[i])
END;

PROCEDURE getimage(name:wrkstring;VAR where:imm);
VAR
    getin:FILE OF col;
    j,i  :BYTE;
BEGIN
     ASSIGN(getin,name);
     RESET(getin);
     FOR i:=0 TO 255 DO
             READ(getin,where[i]^);
     CLOSE(getin)
END;

PROCEDURE display(VAR what:imm;limit:BYTE;code:imm1);
{0<=limit<=255

              . x .          * = 0..255
              x * x          x = 0..127 (/2)
              . x .          . = 0..63  (/4)
}

VAR
    i,j :BYTE;

(**)FUNCTION value(a,b:INTEGER):BYTE;
          BEGIN
             IF (a>=0) AND (a<=255) AND (b>=0) AND (b<=255)
                THEN value:=what[a]^[b]
                ELSE value:=0
          END;{value}

   PROCEDURE light(i,j:INTEGER);
       VAR
           ct :INTEGER;
       BEGIN
          ct:=value(i,j);
          point:=FALSE;
          CASE scanner OF
               2 : BEGIN
                     ct:=ct+ ( value(i+1,j)+value(i-1,j)+value(i,j+1)+value(i,j-1) ) SHR 3;
                     ct:=ct+ ( value(i-1,j-1)+value(i-1,j+1)+value(i+1,j-1)+value(i+1,j+1) ) SHR 3;
                     IF ct >= (limit SHL 1) THEN point:=TRUE
                   END;
               1 : IF ct >= limit THEN point:=TRUE
          END
    END{light};

   PROCEDURE code_it;
     VAR
       i1,j1,z1 :INTEGER;
       ct       :INTEGER;
     BEGIN
       FOR i1:=0 TO 255 DO
           FOR j1:=0 TO 31 DO
               FOR z1:=0 TO 7 DO
                   BEGIN
                      ct:=j1*8+z1;
                      light(ct,i1);
                      IF point=TRUE THEN code[j1]^[i1]:=code[j1]^[i1]+(128 SHR z1);
                   {writeln(code[j1]^[i1],'  ',point,' ',value(ct,i1),i1:4,j1:4,z1:4);}
                   END
       END{code_it};

   PROCEDURE putimage(name:wrkstring;VAR where:imm1);
     VAR
       getout:FILE OF rows;
       j,i   :BYTE;
     BEGIN
       ASSIGN(getout,name);
       REWRITE(getout);
       FOR i:=0 TO 255 DO
         WRITE(getout,where[i]^);
       CLOSE(getout)
     END{putimage};

BEGIN {display}
     screen(grafica);
     FOR i:=0 TO 255 DO
         FOR j:=0 TO 255 DO
             BEGIN
               light(i,j);
               IF point=TRUE THEN plot(i,j,1)
             END;
     REPEAT UNTIL KEYPRESSED;
     TEXTMODE;
     CLRSCR;
     WRITELN;
     WRITE('Codifico questa immagine (S/N) ? ');
           READLN(answer);
           IF (answer='S') OR (answer='s') THEN
                                              BEGIN
                                                WRITE('Nome file codificato ');
                                                READLN(codename);
                                                code_it;
                                                putimage(codename,photo_code)
                                              END
END{display};

PROCEDURE menu;

BEGIN
     WRITELN('Scegli la risoluzione grafica :  1 = 320*200');
     WRITELN('                                 2 = 640*200');
     WRITE('                                 3 = 640*400 (solo Olivetti) ');
     READLN(grafica);
     IF (grafica<1) OR (grafica>3) THEN menu;
     WRITELN;
     WRITE('Scegli il tipo di scanning (1 = controllo sul pixel, 2 = media pesata) ');
     READLN(scanner);
     IF (scanner<1) OR (scanner>2) THEN menu ELSE WRITELN
END;

BEGIN { MAIN }
     getmemory(photo);
     putmemory(photo_code);
     WRITE('Picture file name: ');
     READLN(filename);
     getimage(filename,photo);
     REPEAT
           CLRSCR;
           menu;
           WRITELN('Batti il limite di soglia (1..255); 0=fine');
           WRITE('Soglia: ');
           READlN(lim);
           IF lim >0 THEN display(photo,lim,photo_code)
    UNTIL lim=0
END.
