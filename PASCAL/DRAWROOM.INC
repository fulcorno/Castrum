{ SANCTI GEORGII CASTRUM - - procedure per disegnare le stanze }

const ptview=2;
var back:backgroundarray;

procedure InitRoom;
var i:integer;
begin
     defineworld(1,-11,-7,11,7);
     selectworld(1);
     definewindow(1,0,0,39,199);
     selectwindow(1);
     setheaderon;
     setheadertobottom;
     settextBIG;
     selectsmallfont;
     for i:=0 to 3 do begin
                           back[i shl 1]:=$aa;
                           back[1+i shl 1]:=$55;
     end;
     defineheader(1,'VisÅal ''dla stÉnsia.');
     setbackground8(back);
     drawborder;
end;

procedure DrawRoom(a,b:integer; hdr:WrkString);

var i:integer;

(**)procedure draw3d(csi1,eta1,csi2,eta2:real; wall:integer);
    var x1,y1,z1,x2,y2,z2:real;
    begin
         z1:=eta1; z2:=eta2;
         case wall of
              1: begin x1:=a; x2:=a; y1:=b*(1-csi1); y2:=b*(1-csi2); end;
              2: begin x1:=a*(csi1-1); x2:=a*(csi2-1); y1:=b; y2:=b; end;
              3: begin x1:=-a; x2:=-a; y1:=b*(csi1-1); y2:=b*(csi2-1); end;
              4: begin x1:=a*(1-csi1); x2:=a*(1-csi2); y1:=-b; y2:=-b; end;
         end {case} ;
         drawline(ptview*x1/(ptview-z1),ptview*y1/(ptview-z1),
                  ptview*x2/(ptview-z2),ptview*y2/(ptview-z2) );
(**)end;{draw3d}

(**)procedure parete(wall,tipo:integer);

    (**)procedure quad;
        begin
             setcolorwhite;
             draw3d(0,0,2,0,wall); draw3d(2,0,2,1,wall);
             draw3d(2,1,0,1,wall); draw3d(0,1,0,0,wall);
             draw3d(0,1.02,2,1.02,wall);
    (**)end; {quad}

    (**)procedure freccia(s:real);
        begin
             draw3d(s,0,s,0.4,wall); draw3d(s-0.1,0.3,s+0.1,0.3,wall);
             draw3d(s,0.4,s-0.1,0.3,wall); draw3d(s,0.4,s+0.1,0.3,wall);
    (**)end; {freccia}

    (**)procedure door(s,d,h:real);
        begin
         setcolorwhite;
         draw3d(s-d,0,s-d,h,wall); draw3d(s-d,h,s+d,h,wall);
         draw3d(s+d,h,s+d,0,wall);
         setcolorblack;
         draw3d(s-d,0,s+d,0,wall);
         freccia(s);
    (**)end; {door}

    (**)procedure portone(s,d,h:real);
        begin
         setcolorwhite;
         draw3d(s-d,0,s-d,0.75*h,wall); draw3d(s-d,0.75*h,s-d/2,h,wall);
         draw3d(s-d/2,h,s+d/2,h,wall);  draw3d(s+d/2,h,s+d,0.75*h,wall);
         draw3d(s+d,0.75*h,s+d,0,wall);
         setcolorblack;
         draw3d(s-d,0,s+d,0,wall);
         freccia(s);
    (**)end; {portone}

    (**)procedure cancello(s,d,h:real);
        begin
         setcolorwhite;
         draw3d(s-d,0,s-d,0.75*h,wall); draw3d(s-d,0.75*h,s-d/2,h,wall);
         draw3d(s-d/2,h,s+d/2,h,wall);  draw3d(s+d/2,h,s+d,0.75*h,wall);
         draw3d(s+d,0.75*h,s+d,0,wall); draw3d(s-d*0.75,h*7/8,s-d*0.75,0,wall);
         draw3d(s+d*0.75,h*7/8,s+d*0.75,0,wall); draw3d(s-d*0.75,h*7/8,s+d*0.75,h*7/8,wall);
         draw3d(s-d,h*5/8,s+d,h*5/8,wall); draw3d(s-d,h*3/8,s+d,h*3/8,wall);
         draw3d(s-d,h/8,s+d,h/8,wall); draw3d(s-d/4,0,s-d/4,h,wall);
         draw3d(s+d/4,0,s+d/4,h,wall); draw3d(s,0,s,h,wall);
         setcolorblack;
         draw3d(s-d,0,s+d,0,wall);
         freccia(s);
    (**)end; {cancello}

    (**)procedure finestra(s,t,l,m:real);
        begin
         setcolorwhite;
         draw3d(s-l,t-m,s+l,t-m,wall); draw3d(s+l,t-m,s+l,t+m,wall);
         draw3d(s+l,t+m,s-l,t+m,wall); draw3d(s-l,t+m,s-l,t-m,wall);
         draw3d(s,t-m,s,t+m,wall);     draw3d(s-l,t,s+l,t,wall);
    (**)end; {finestra}

    (**)procedure specchio(s,t,l,m:real);
        begin
         setcolorwhite;
         draw3d(s-l,t-m,s+l,t-m,wall); draw3d(s+l,t-m,s+l,t+m,wall);
         draw3d(s+l,t+m,s-l,t+m,wall); draw3d(s-l,t+m,s-l,t-m,wall);
         draw3d(s-l/2,t,s,t+m/2,wall); draw3d(s-l/2,t-m/2,s+l/2,t+m/2,wall);
         draw3d(s,t-m/2,s+l/2,t,wall);
    (**)end; {specchio}

    (**)procedure dashes(da,al:real; n:integer);
        var i:integer;
        begin setcolorblack;
              for i:=0 to n-1 do draw3d(da+(al-da)*i/n,0,da+(al-da)*i/n+(al-da)/n/2,0,wall);
              freccia((da+al)/2);
    (**)end; {dashes}

    (**)procedure muro(da,al:real);
        var i:integer;
        begin
             setcolorwhite;
             draw3d(da,0,da,1,wall); draw3d(da,1,al,1,wall);
             draw3d(al,1,al,0,wall); draw3d(al,0,da,0,wall);
             draw3d(da,1.02,al,1.02,wall);
    (**)end; {muro}

    (**)procedure mattoni(da,al:real);
        var j,k,n:integer;
            w:real;
        begin
             setcolorwhite;
             muro(da,al);
             n:=round((al-da)*5);
             for j:=1 to 3 do draw3d(da,j/4,al,j/4,wall);
             for j:=0 to 3 do for k:=0 to n-1 do begin
                 w:=((j and 1)/2+k)*(al-da)/n+da;
                 draw3d(w,j/4,w,(j+1)/4,wall);
             end;
    (**)end; {mattoni}

    (**)procedure rottami(da,al:real);
        var i:integer;
            h,h1:real;
        begin
             setcolorblack;
             h:=0.0;
             for i:=0 to 7 do   draw3d(i/8*(al-da)+da,-(i and 1),
                                (i+1)/8*(al-da)+da,-((i+1)and 1),wall);
             freccia((al+da)/2);
    (**)end; {rottami}

    (**)procedure portasciapa(s,d,h:real);
        begin
             setcolorwhite;
             draw3d(s-d,0,s-d,h,wall); draw3d(s-d,h,s+d,h,wall);
             draw3d(s+d,h,s+d,0,wall); draw3d(s-d,0,s+d,h,wall);
             draw3d(s-d,h,s+d,0,wall); draw3d(s-2*d,h/2,s+2*d,h/2,wall);
             setcolorblack;
             draw3d(s-d,0,s+d,0,wall);
             freccia(s);
    (**)end; {porta-sciapÖ}

    (**)procedure merli;
        begin
             setcolorwhite;
             draw3d(0,0,2,0,wall); draw3d(2,0,2,1,wall); draw3d(2,1,13/7,0.8,wall);
             draw3d(13/7,0.8,12/7,1,wall); draw3d(12/7,1,12/7,0.6,wall);
             draw3d(12/7,0.6,10/7,0.6,wall); draw3d(10/7,0.6,10/7,1,wall);
             draw3d(10/7,1,9/7,0.8,wall); draw3d(9/7,0.8,8/7,1,wall);
             draw3d(8/7,1,8/7,0.6,wall); draw3d(8/7,0.6,6/7,0.6,wall);
             draw3d(6/7,0.6,6/7,1,wall); draw3d(6/7,1,5/7,0.8,wall);
             draw3d(5/7,0.8,4/7,1,wall); draw3d(4/7,1,4/7,0.6,wall);
             draw3d(4/7,0.6,2/7,0.6,wall); draw3d(2/7,0.6,2/7,1,wall);
             draw3d(2/7,1,1/7,0.8,wall); draw3d(1/7,0.8,0,1,wall); draw3d(0,1,0,0,wall);
    (**)end; {merli}

    (**)procedure armadio(s,d,h:real);
        begin
             setcolorwhite;
             draw3d(s-d,0,s-d,h,wall); draw3d(s-d,h,s+d,h,wall);
             draw3d(s+d,h,s+d,0,wall); draw3d(s,h/2,s+d,h/2,wall);
    (**)end; {armadio}

    (**)procedure caminetto(s:real);
        begin
             setcolorwhite;
             draw3d(s+0.1,0.7,s-0.1,0.7,wall); draw3d(s-0.1,0.7,s-0.3,0.5,wall);
             draw3d(s-0.3,0.5,s-0.3,0.4,wall); draw3d(s-0.3,0.4,s+0.3,0.4,wall);
             draw3d(s+0.3,0.4,s+0.3,0.5,wall); draw3d(s+0.3,0.5,s+0.1,0.7,wall);
             draw3d(s-0.2,0,s-0.2,0.4,wall);   draw3d(s+0.2,0,s+0.2,0.4,wall);
    (**)end; {caminetto}

    (**)procedure scaffale(s,d,h:real);
        var i:integer;
        begin
             setcolorwhite;
             draw3d(s-d,0,s-d,h,wall); draw3d(s+d,0,s+d,h,wall);
             for i:=0 to 4 do draw3d(s-d,h/4*i,s+d,h/4*i,wall);
    (**)end; {scaffale}

    (**)procedure balcone;
        var i:integer;
        begin
             setcolorwhite;
             draw3d(0,0,2,0,wall); draw3d(2,0,2,0.5,wall);
             draw3d(2,0.5,0,0.5,wall); draw3d(0,0.5,0,0,wall);
             for i:=1 to 7 do draw3d(i/4,0.1,i/4,0.4,wall);
    (**)end; {balcone}

    var i:integer;
    begin {parete}
      case tipo of
          1:dashes(0,2,10);
          2:quad;
          3:begin quad; door(0.5,0.2,0.8); end;
          4:begin quad; door(1.0,0.2,0.8); end;
          5:begin quad; door(1.5,0.2,0.8); end;
          6:begin quad; door(0.5,0.2,0.8); door(1.5,0.2,0.8); end;
          7:begin quad; portone(1.0,0.4,0.9); end;
          8:begin quad; finestra(0.5,0.5,0.2,0.3); end;
          9:begin quad; finestra(1.0,0.5,0.2,0.3); end;
         10:begin quad; finestra(1.5,0.5,0.2,0.3); end;
         11:begin quad; finestra(0.6,0.5,0.3,0.3);
                  finestra(1.4,0.5,0.3,0.3); end;
         12:begin quad; finestra(0.4,0.5,0.2,0.3);
                  finestra(1.0,0.5,0.2,0.3); finestra(1.6,0.5,0.2,0.3); end;
         13:begin quad; portone(1.0,0.3,0.8);
                  finestra(1.6,0.5,0.2,0.3); end;
         14:begin quad; door(1.0,0.2,0.8); finestra(0.4,0.5,0.2,0.3);
                  finestra(1.6,0.5,0.2,0.3); end;
         15:begin quad; door(1.2,0.2,0.8); finestra(0.5,0.5,0.2,0.3); end;
         16:begin quad; specchio(0.2,0.4,0.1,0.2);
                  finestra(0.6,0.5,0.2,0.3); specchio(1.0,0.4,0.1,0.2);
                  finestra(1.4,0.5,0.2,0.3); specchio(1.8,0.4,0.1,0.2); end;
         17:begin quad; door(1.0,0.2,0.8); specchio(0.4,0.4,0.2,0.2);
                  specchio(1.6,0.4,0.2,0.2); end;
         18:begin quad; specchio(0.5,0.4,0.2,0.3);
                  specchio(1.5,0.4,0.2,0.3); end;
         19:rottami(0,2);
         20:begin quad; finestra(0.5,0.5,0.2,0.3); door(1.6,0.2,0.8); end;
         21:begin quad; door(0.4,0.2,0.8); finestra(1.5,0.5,0.2,0.3); end;
         22:begin dashes(0,0.7,4); muro(0.7,1.3); dashes(1.3,2.0,4); end;
         23:begin quad; door(0.4,0.2,0.7); finestra(1.1,0.5,0.2,0.3);
                  finestra(1.7,0.5,0.2,0.3); end;
         24:begin quad; door(1.0,0.2,0.7); door(1.6,0.2,0.7); end;
         25:begin muro(0,0.7); dashes(0.7,1.3,4); muro(1.3,2.0); end;
         26:begin muro(0,1); dashes(1,2,6); end;
         27:begin quad; portasciapa(1,0.2,0.7); end;
         28:begin quad; door(1.0,0.2,0.7); finestra(1.6,0.5,0.2,0.3); end;
         29:begin muro(0,1.3); door(0.4,0.2,0.7); dashes(1.3,2,5); end;
         30:mattoni(0,2);
         31:begin quad; portone(1,0.6,0.9); end;
         32:begin quad; cancello(1,0.3,0.8); end;
         33:merli;
         34:begin quad; armadio(1,0.2,0.6); end;
         35:begin quad; scaffale(1,0.2,0.5); end;
         36:begin quad; caminetto(1); end;
         37:begin quad; caminetto(0.5); door(1.5,0.2,0.7); end;
         38:begin quad; door(0.5,0.2,0.7); caminetto(1.6); end;
         39:begin quad; door(1,0.2,0.7); armadio(1.6,0.2,0.6); end;
         40:begin quad; caminetto(0.4); door(1,0.2,0.7); armadio(1.6,0.2,0.6); end;
         41:begin quad; door(1,0.2,0.7); caminetto(1.6); end;
         42:begin quad; caminetto(1.5); end;
         43:begin quad; caminetto(0.5); end;
         44:balcone;
      end; (* CASE tipo ... *)
(**)end; {parete}

begin {drawroom}
     SelectWorld(1);
     SelectWindow(1);
     selectscreen(2);
     SetHeaderOn;
     SetHeaderToBottom;
     SetTextBig;
     SelectSmallFont;
     DefineHeader(1,hdr);
     SetBackground8(back);
     DrawBorder;
     for i:=1 to 4 do parete(i,1+random(44));
     selectscreen(1);
     copywindow(2,1,0,0);
end;{drawroom}

