procedure Timer(var ore,min:integer);
{lettura del timer interno}
var rg:CPUreg;
begin
     with rg do begin
          ah:=$2c;
          msdos(rg);
          ore:=ch; min:=cl;
     end;
end;

function HardCounter:real;
{lettura del contatore interno}
var rg:CPUreg;
    tm:real;
begin
     with rg do begin
          ah:=0;
          intr($1a,rg);
          tm:=65536.0*cx;
          if dx>=0 then tm:=tm+dx else tm:=tm+65536.0+dx;
          tm:=tm+65536.0*24*al;
     end;
     HardCounter:=tm;
end;

procedure ResetCounter;
{azzeramento del contatore}
var rg:CPUreg;
begin
     with rg do begin
          ah:=$01;
          cx:=0;
          dx:=0;
          intr($1a,rg);
     end;
end;