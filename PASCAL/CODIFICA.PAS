PROGRAM codifica (INPUT,OUTPUT);
{Visualizza un'immagine precedentemente digitalizzata e la converte in B/N}

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

PROCEDURE getmemory(VAR where:imm);
VAR
    i:BYTE;
BEGIN
     FOR i:=0 TO 255 DO NEW(where[i])
END{getmemory};

PROCEDURE putmemory(VAR where:imm1);
VAR
    i:BYTE;
BEGIN
     FOR i:=0 TO 255 DO NEW(where[i])
END{putmemory};

PROCEDURE getimage(name:wrkstring;VAR where:imm);
VAR
    getin:FILE OF col;
    i  :BYTE;
BEGIN
     ASSIGN(getin,name);
     RESET(getin);
     FOR i:=0 TO 255 DO
             READ(getin,where[i]^);
     CLOSE(getin)
END{getimage};

PROCEDURE display(VAR what:imm;limit:BYTE;var code:imm1);
{0<=limit<=255}

VAR
    i,j :BYTE;

(**)FUNCTION value(a,b:INTEGER):BYTE;
        BEGIN
           IF (a>=0) AND (a<=255) AND (b>=0) AND (b<=255)
              THEN value:=what[a]^[b]
              ELSE value:=0
        END;{value}

(**)FUNCTION light(i,j:INTEGER):BOOLEAN;
        VAR
          ct :INTEGER;
        BEGIN
          ct:=value(i,j);
          CASE scanner OF
               2 : BEGIN
                     ct:=ct+ ( value(i+1,j)+value(i-1,j)+value(i,j+1)+value(i,j-1) ) SHR 3;
                     ct:=ct+ ( value(i-1,j-1)+value(i-1,j+1)+value(i+1,j-1)+value(i+1,j+1) ) SHR 3;
                     light:= (ct >= (limit SHL 1))
                   END;
               1 : light:= (ct >= limit)
          END
        END{light};

(**)PROCEDURE code_it;
        VAR
          i1,j1,z1 :INTEGER;
          ct       :INTEGER;
        BEGIN
          FOR i1:=0 TO 255 DO
              FOR j1:=0 TO 31 DO
                  BEGIN
                    code[i1]^[j1]:=0;
                    FOR z1:=0 TO 7 DO
                        BEGIN
                          ct:=j1*8+z1;
                          IF light(ct,i1) THEN code[i1]^[j1]:=code[i1]^[j1]+(128 SHR z1)
                        END
                    END
       END{code_it};

(**)PROCEDURE putimage(name:wrkstring;VAR where:imm1);
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
               IF light(i,j)=TRUE THEN plot(i,j,1)
             END;
     REPEAT UNTIL KEYPRESSED;
     TEXTMODE;
     CLRSCR;
     WRITELN;
     WRITE('Codifico questa immagine (S/N) ? ');
           READLN(answer);
           IF UPCASE(answer)='S' THEN
                                     BEGIN
                                          WRITE('Nome file codificato ');
                                          READLN(codename);
                                          code_it;
                                          putimage(codename,code)
                                      END
END{display};

PROCEDURE menu(var scanner:integer);

BEGIN
     REPEAT
        WRITELN('Scegli la risoluzione grafica :  1 = 320*200');
        WRITELN('                                 2 = 640*200');
        WRITE('                                 3 = 640*400 (solo Olivetti) ');
        READLN(grafica);
     UNTIL (grafica>=1) AND (grafica<=3);
     REPEAT
       WRITELN;
       WRITE('Scegli il tipo di scanning (1 = controllo sul pixel, 2 = media pesata) ');
       READLN(scanner);
     UNTIL (scanner>=1) AND (scanner<=2);
     WRITELN
END{menu};

BEGIN { MAIN }
  REPEAT
     getmemory(photo);
     putmemory(photo_code);
     WRITE('Picture file name: ');
     READLN(filename);
     getimage(filename,photo);
     CLRSCR;
     menu(scanner);
     WRITELN('Batti il limite di soglia (1..255); 0=fine');
     WRITE('Soglia: ');
     READLN(lim);
     IF lim >0 THEN display(photo,lim,photo_code);
     WRITELN;
     WRITE('Ripeto l''elaborazione ? (S/N) ');
     READLN(answer)
  UNTIL UPCASE(answer)='N'
END{ MAIN }.
