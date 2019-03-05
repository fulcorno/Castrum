(***********************************************************)
(* TURBO GRAPHIX TOOLBOX v 1.05A (c) BORLAND INTERNATIONAL *)
(*   ADATTATO E COMPATTATO PER " SANCTI GEORGII CASTRUM "  *)
(*          PROPRIETA' DELLA "JENA SOFT" 1988              *)
(***********************************************************)

const MaxWorldsGlb=10; MaxWindowsGlb=10; StringSizeGlb=80; RamScreenGlb:boolean=true;
      CharFile:string[StringSizeGlb]='4x6.fon'; MaxProcsGlb=27;  MaxErrsGlb=7;

type  CPUreg=record case integer of
             1:(ax,bx,cx,dx,bp,si,di,ds,es,flgs: integer);
             2:(al,ah,bl,bh,cl,ch,dl,dh: byte); end;
      wrkstring=string[StringSizeGlb];
      WorldType=record  x1,y1,x2,y2:real  end;
      WindowType=record
                   x1,y1,x2,y2:integer;  header:wrkstring;
                   drawn,top:boolean;    size:integer
                 end;
      worlds=array [1..MaxWorldsGlb] of WorldType;
      windows=array [1..MaxWindowsGlb] of WindowType;
      character=array [1..3] of byte; CharArray=array [32..126] of character;
      character8x8=array[0..7] of byte; CharArray8x8=array[byte] of character8x8;
      character16x8=array[0..15] of byte; CharArray16x8=array[byte] of character16x8;
      character20x16=array[0..19] of integer; CharArray20x16=array[byte] of character20x16;
      BackgroundArray=array[0..7]of byte; LineStyleArray=array[0..7]of boolean;
      ConversionArray=array [$80..$ff] of $00..$7f;

var   X1WldGlb,X2WldGlb,Y1WldGlb,Y2WldGlb,AxGlb,AyGlb,BxGlb,ByGlb:real;
      X1RefGlb,X2RefGlb,Y1RefGlb,Y2RefGlb,LinestyleGlb,MaxWorldGlb,MaxWindowGlb,WindowNdxGlb,
      X1Glb,X2Glb,Y1Glb,Y2Glb,XTextGlb,YTextGlb,VStepGlb,HeaderSizeGlb:integer;
      DirectModeGlb,ClippingGlb:boolean;
      SmallFont,MessageGlb,BrkGlb,HeaderGlb,TopGlb,GrafModeGlb:boolean;
      TextDistance,CntGlb,ColorGlb,ErrCodeGlb:byte;
      ErrorProc:array [0..MaxProcsGlb] of ^WrkString; ErrorCode:array [0..MaxErrsGlb] of ^WrkString;
      PcGlb:string[40];
      AspectGlb:real; GrafBase:integer; world:worlds; window:windows;
      CharSet:CharArray; CharSet8x8:^CharArray8x8; CharSet16x8:^CharArray16x8; CharSet20x16:^CharArray20x16;
      CharConversion:ConversionArray;  LineStyleArrayGlb:LineStyleArray;

const XMaxGlb=79;   XScreenMaxGlb=639;      YMaxGlb=399;
      IVStepGlb=4;  ScreenSizeGlb=16383;    HardwareGrafBase=$B800;
      MinForeground:integer=0;      MaxForeground:integer=15;
      MinBackground:integer=0;      MaxBackground:integer=0;
      AspectFactor=0.8128;  SaveStateGlb:integer=10;  ForegroundColorGlb:integer=15;

type  ScreenType=array [0..ScreenSizeGlb] of integer;
      ScreenPointer=^ScreenType;
      FontChar=array [0..7] of byte;  IBMFont=array [0..255] of FontChar;
      WindowStackRecord=record
                          W:WindowType;
                          Contents:ScreenPointer;
                        end;
      stacks=array [1..MaxWindowsGlb] of WindowStackRecord;

var   ScreenGlb:ScreenPointer;     ConOutPtrSave:integer;
      Font:IBMFont;      Stack:Stacks;
      DisplayType:(IBMPCjr,IBMCGA,IBMEGA,NoDisplay);
      BaseAddress:ARRAY[0..YMaxGlb] of integer;

procedure error(ErrProc,ErrCode:integer); FORWARD;

function HardwarePresent:boolean;
  begin
        MinForeground:=0;        MaxForeground:=15;
        MinBackground:=0;        MaxBackground:=0;
        DisplayType:=IBMCGA;     HardwarePresent:=true;
  end;

procedure AllocateRAMScreen;
  var test:^integer;
  begin
    new(ScreenGlb);
    while ofs(ScreenGlb^)<>0 do begin dispose(ScreenGlb); new(test); new(ScreenGlb); end;
  end;

procedure LeaveGraphic;
  var regs:record case integer of
             1:(ax,bx,cx,dx,bp,si,di,ds,es,flgs: integer);
             2:(al,ah,bl,bh,cl,ch,dl,dh: byte); end;
  begin
    regs.ax:=SaveStateGlb; intr($10,regs);
    if GrafModeGlb then ConOutPtr:=ConOutPtrSave; GrafModeGlb:=false;
  End;

procedure DC(C: byte);
var i:integer;
  begin
       for i:=0 to 7 do mem[GrafBase:BaseAddress[i+(YTextGlb-1)shl 3]+XTextGlb-1]:=CharSet8x8^[c,i] xor (ColorGlb xor $ff)
  end;

procedure DisplayChar(C: byte);
  begin
    if C=8 then begin
      if XTextGlb>1 then XTextGlb:=XTextGlb-1;
    end else if C=10 then begin
      if YTextGlb<50 then YTextGlb:=YTextGlb+1;
    end else if C=13 then XTextGlb:=1
    else begin  DC(C); if XTextGlb<80 then XTextGlb:=XTextGlb+1; end;
  end;

procedure SetIBMPalette(PaletteNumber,Color:integer);
  var regs:CPUreg;
  begin
    with regs do begin ah:=$0B; bl:=Color; bh:=PaletteNumber; end;
    intr($10,regs);
  end;

procedure SetForegroundColor(Color:Integer);
  begin SetIBMPalette(0,Color); ForegroundColorGlb:=Color; end;

procedure ClearScreen;
  begin fillchar(mem[GrafBase:0000],(ScreenSizeGlb+1) Shl 1,0); end;

procedure EnterGraphic;
  var regs:CPUreg; FontFile: file of IBMFont; y:integer;
  begin
    regs.ax:=$0f00;    intr($10,regs);
    if (regs.al<4) or (SaveStateGlb=10) then SaveStateGlb:=regs.al;
    regs.ax:=$0048;    intr($10,regs);
    SetForegroundColor(MaxForeground);
    if not GrafModeGlb then ConOutPtrSave:=ConOutPtr;
    ConOutPtr:=ofs(DisplayChar);    GrafModeGlb:=true;
    for y:=0 to YMaxGlb do BaseAddress[y]:=(Y and 3) shl 13 + 80*(Y shr 2);
  end;

procedure DP(X,Y: integer);
var adr,bit:integer;
  begin
       adr:=BaseAddress[y]+X shr 3; bit:=$80 shr (x and 7);
       if ColorGlb<>0 then mem[GrafBase:adr]:=mem[GrafBase:adr] or bit
          else mem[GrafBase:adr]:=mem[GrafBase:adr] and ($ff xor bit)
  end;

function PD(x,y:integer):boolean;
  begin
    PD:=(ColorGlb=0) xor (mem[GrafBase:BaseAddress[y] + x shr 3] and (128 shr (x and 7)) <> 0);
  end;

procedure SetBackground8(Background:BackgroundArray);
  var i:integer;
  begin
    for i:=Y1RefGlb to Y2RefGlb do
      FillChar(mem[GrafBase:BaseAddress[i]+X1RefGlb],X2RefGlb-X1RefGlb+1,Background[i and 7]);
  end;

procedure SetBackground(byt:byte);
  var bk:BackgroundArray;
  begin fillchar(bk,8,byt); SetBackground8(bk); end;

procedure DrawStraight(x1,x2,y:integer);
  var i,x:integer; DirectModeLoc:boolean;
  begin
    if (not((x1<0)or(x1>XMaxGlb shl 3+7))and not((x2<0)or(x2>XMaxGlb shl 3+7))and((y>=0)and(y<=YMaxGlb)))
    then begin
      DirectModeLoc:=DirectModeGlb;      DirectModeGlb:=true;
      if x1>x2 then begin  x:=x1;   x1:=x2;   x2:=x   end;
      if x2-x1<16 then  for x:=x1 to x2 do dp(x,y)
      else begin
        x1:=x1+8;
        for i:=(x1-8) to (x1 and -8) do dp(i,y); for i:=(x2 and -8) to x2 do dp(i,y);
        FillChar(Mem[GrafBase:BaseAddress[Y]+(X1 Shr 3)], (X2 Shr 3)-(X1 Shr 3),ColorGlb);
      end;
      DirectModeGlb:=DirectModeLoc;
     end
  end;

procedure SaveScreen(FileName:wrkstring);
  type PicFile=file of ScreenType;
  var picture:ScreenPointer;  PictureFile:PicFile; ioerr:boolean;
     procedure IOCheck;
       begin ioerr:=IOResult<>0; if ioerr then Error(27,5); end;
  begin
    ioerr:=false;  picture:=ptr(GrafBase,0);
    assign(PictureFile,FileName);
    {$I-} rewrite(PictureFile); {$I+}
    IOCheck;
    if not ioerr then begin
      {$I-} write(PictureFile,picture^); {$I+}
      IOCheck;
     end;
    if not ioerr then begin
      {$I-} close(PictureFile); {$I+}
      IOCheck;
     end;
  end;

procedure LoadScreen(FileName:wrkstring);
  type PicFile=file of ScreenType;
  var picture:ScreenPointer; PictureFile:PicFile;
  begin
    picture:=ptr(GrafBase,0);
    assign(PictureFile,FileName);
    {$I-} reset(PictureFile); {$I+}
    if IOResult<>0 then Error(11,5)
    else begin
      read(PictureFile,picture^); close(PictureFile);
     end;
  end;

procedure SwapScreen;
  const SS=$4000;
  var g:integer;
  begin
    if RamScreenGlb then begin
      g:=seg(ScreenGlb^);
      Inline($8B/$86/ g /$8E/$C0/$1E/$B8/ HardwareGrafBase /$8E/$D8/$B9/
             SS /$31/$DB/$8B/$07/$26/$87/$07/$89/$07/$43/$43/$E2/$F5/$1F);
     end;
  end;

procedure CopyScreen;
  var ToBase:integer;
  begin
    if RamScreenGlb then
     begin
      if GrafBase=HardwareGrafBase then ToBase:=seg(ScreenGlb^) else ToBase:=HardwareGrafBase;
      move(mem[GrafBase:0000],mem[ToBase:0000],(ScreenSizeGlb+1) Shl 1);
     end;
  end;

procedure InvertScreen;
  const SS=$4000;
  begin
    Inline($1E/$A1/ GrafBase /$8E/$D8/$B9/ SS /$31/$DB/$F7/$17/$43/$43/$E2/$FA/$1F);
  end;

procedure GotoXYTurbo(X,Y:integer);
  begin    GotoXY(X,Y);  end;

procedure GotoXY(X,Y:integer);
  begin
    if not GrafModeGlb then GotoXYTurbo(X,Y);
    XTextGlb:=X;  YTextGlb:=Y;
  end;

procedure ClrEOLTurbo;
  begin    ClrEOL;  end;

procedure ClrEOL;
  var temp:integer;
  begin
    if not GrafModeGlb then ClrEOLTurbo
    else begin
      temp:=XTextGlb;
      for XTextGlb:=temp to 80 do DC(32);
      XTextGlb:=temp;
     end;
  end;

procedure error { (ErrProc,ErrCode:integer) };
  type string2=string[2];
  var NLevels,PCValue,XLoc,YLoc:integer;
      ch:char;
  function HexString(byt:byte):string2;
    const hex:array [0..15] of char='0123456789ABCDEF';
    begin      HexString:=hex[byt shr 4] + hex[byt and 15];    end;
begin
  if not (ErrProc in [0..MaxProcsGlb]) then begin
    LeaveGraphic; writeln('FATAL ERROR 1: illegal procedure number ',ErrProc);
    halt; end;
  if not (ErrCode in [0..MaxErrsGlb]) then begin
    LeaveGraphic;
    writeln('FATAL ERROR 2: illegal error code ',ErrCode);  halt;   end;
  ErrCodeGlb:=ErrCode;
  if BrkGlb then LeaveGraphic;
  if MessageGlb or BrkGlb then begin
    XLoc:=XTextGlb; YLoc:=YTextGlb;
    GotoXY(1,24); ClrEOL;
    writeln('Turbo Graphix error #',ErrCode,' in procedure #',ErrProc);
    if MessageGlb then begin
      ClrEOL; write('(',ErrorCode[ErrCode]^,' in ',ErrorProc[ErrProc]^,')');
     end;
   end;
  if MessageGlb and BrkGlb then begin
    WriteLn; WriteLn('Traceback:');  NLevels:=0;
    repeat
      inline($89/$EB/$8B/$8E/ NLevels /$09/$C9/$74/$05/$8B/$6E/
             $00/$E2/$FB/$8B/$46/$02/$89/$DD/$89/$86/ PCValue );
      if PCValue<>0 then writeln(PcGlb,' : ',HexString(hi(PCValue-1)),HexString(lo(PCValue-1)));
      NLevels:=NLevels+1;
    until (NLevels>20) or (PCValue=0);
    halt;
   end
  else if BrkGlb { and not MessageGlb } then halt
  else if MessageGlb then  begin
    write('.  Hit enter: ');
    repeat read(Kbd,Ch) until (Ch=^M) or (Ch=^C);
    if Ch=^C then  begin LeaveGraphic;  halt; end;
    GotoXY(XLoc,YLoc);
   end;
end;

procedure SetBreakOff;
  begin  BrkGlb:=false  end;

procedure SetBreakOn;
  begin  BrkGlb:=true  end;

procedure SetWindowModeOff;
  begin  DirectModeGlb:=true  end;

procedure SetWindowModeOn;
  begin  DirectModeGlb:=false  end;

procedure SetClippingOn;
  begin  ClippingGlb:=true  end;

procedure SetClippingOff;
  begin  ClippingGlb:=false  end;

procedure SetMessageOn;
  begin  MessageGlb:=true  end;

procedure SetMessageOff;
  begin  MessageGlb:=false  end;

procedure SetHeaderOn;
  begin  HeaderGlb:=true  end;

procedure SetHeaderOff;
  begin  HeaderGlb:=false  end;

procedure SetHeaderToTop;
  begin  TopGlb:=true  end;

procedure SetHeaderToBottom;
  begin  TopGlb:=false  end;

procedure RemoveHeader(i:integer);
  begin
    if i in [1..MaxWindowsGlb] then
      with window[i] do begin drawn:=false; top:=true; header:=''; end
    else error(22,2);
  end;

procedure SetTextFar;
  begin TextDistance:=6; HeaderSizeGlb:=10 end;

procedure SetTextNear;
  begin TextDistance:=5; HeaderSizeGlb:=10 end;

procedure SetTextBIG;
  begin
       TextDistance:=8; if SmallFont then HeaderSizeGlb:=13 else HeaderSizeGlb:=21
  end;

procedure SelectBigFont;
  begin
       SmallFont:=FALSE; if TextDistance >=8 then HeaderSizeGlb:=21;
   end;

procedure SelectSmallFont;
  begin
       SmallFont:=TRUE; if TextDistance >=8 then HeaderSizeGlb:=13;
  end;

procedure SetColorWhite;
  begin  ColorGlb:=255  end;

procedure SetColorBlack;
  begin  ColorGlb:=0  end;

procedure SetScreenAspect(aspect:real);
  begin  if aspect<>0.0 then AspectGlb:=abs(aspect)  end;

procedure SetAspect(aspect:real);
  begin  if aspect<>0.0 then AspectGlb:=abs(aspect)*AspectFactor  end;

procedure SetLinestyle(ls:integer);
  var i:integer;
  const lsa:array [0..4] of byte=($FF,$88,$F8,$E4,$EE);
  begin
    if not (ls in [0..4]) then ls:=ls and $FF + $100;
    LineStyleGlb:=ls; if ls<5 then ls:=lsa[ls];
    for i:=0 to 7 do LineStyleArrayGlb[7-i]:=((ls shr i) and 1)<>0;
    CntGlb:=7;
  end;

procedure SetVStep(vs:integer);
  begin  if vs>0 then VStepGlb:=vs  end;

procedure DefineHeader(i:integer;hdr:wrkstring);
  begin
    if (i in [1..MaxWindowsGlb]) then window[i].header:=Hdr else error(3,2);
  end;

procedure SelectScreen(i:integer);
  begin
    if RamScreenGlb and (I=2) then GrafBase:=Seg(ScreenGlb^) else GrafBase:=HardwareGrafBase;
  end;

procedure DefineWorld(i:integer; X_1,Y_1,X_2,Y_2:real);
  begin
    if ((X_1<>X_2) and (Y_1<>Y_2)) and (i in [1..MaxWorldsGlb]) then
      with world[i] do begin
        x1:=X_1;y1:=Y_2;x2:=X_2;y2:=Y_1;
        if i>MaxWorldGlb then MaxWorldGlb:=i;
       end
    else if i in [1..MaxWorldsGlb] then error(1,3) else error(1,2);
  end;

procedure SelectWorld(i:integer);
  begin
    if (i in [1..MaxWorldGlb]) then
      with world[i] do begin
        X1WldGlb:=x1;  Y1WldGlb:=y1;  X2WldGlb:=x2;  Y2WldGlb:=y2;  end
    else error(2,2);
  end;

procedure ReDefineWindow(i,X_1,Y_1,X_2,Y_2:integer);
  begin
    if (i in [1..MaxWindowsGlb]) and (X_1<=X_2) and (Y_1<=Y_2) and
       (X_1>=0) and (X_2<=XMaxGlb) and (Y_1>=0) and (Y_2<=YMaxGlb) then
      with window[i] do begin
        x1:=X_1;  y1:=Y_1;  x2:=X_2;  y2:=Y_2;
        if i>MaxWindowGlb then MaxWindowGlb:=i;
       end
    else if i in [1..MaxWindowsGlb] then error(3,3) else error(3,2);
  end;

procedure DefineWindow(i,X_1,Y_1,X_2,Y_2:integer);
  begin
    ReDefineWindow(i,X_1,Y_1,X_2,Y_2);
    with window[i] do begin header:=''; top:=true; drawn:=false; end;
  end;

procedure SelectWindow(i:integer);
  begin
    if (i in [1..MaxWindowGlb]) then with window[i] do begin
        WindowNdxGlb:=i; X1RefGlb:=x1;   Y1RefGlb:=y1;   X2RefGlb:=x2;   Y2RefGlb:=y2;
        BxGlb:=((x2-x1) shl 3+7)/(X2WldGlb-X1WldGlb);   ByGlb:=(y2-y1)/(Y2WldGlb-Y1WldGlb);
        AxGlb:=(x1 shl 3)-X1WldGlb*BxGlb;   AyGlb:=y1-Y1WldGlb*ByGlb;
       end else error(4,2);
  end;

function WindowX(x:real):integer;
  begin  WindowX:=trunc(AxGlb+BxGlb*x)  end;

function WindowY(y:real):integer;
  begin  WindowY:=trunc(AyGlb+ByGlb*y)  end;

procedure InitGraphic;
  var fil:file of CharArray;  fil8x8:file of CharArray8x8;
      fil16x8:file of CharArray16x8;  fil20x16:file of CharArray20x16;
      filconv:file of ConversionArray;  tfile:text;
      test:^integer;  temp:WrkString;   i:integer;  ch:byte;
  begin
    EnterGraphic;
    GotoXY(1,1);    MessageGlb:=True;    BrkGlb:=False;
    GrafModeGlb:=False;  GetMem(ErrorProc[0],16);  GetMem(ErrorCode[0],24);
    ErrorProc[0]^:='InitGraphic';    ErrorCode[0]^:='ERROR.MSG missing';
    assign(tfile,'error.msg');    {$I-} reset(tfile); {$I+}
    if ioresult=0 then begin
      for i:=0 to MaxProcsGlb do begin
        readln(tfile,temp);
        if i<>0 then GetMem(ErrorProc[i],length(temp)+1);
        ErrorProc[i]^:=temp;
       end;
      for i:=0 to MaxErrsGlb do begin
        readln(tfile,temp);
        if i<>0 then GetMem(ErrorCode[i],length(temp)+1);
        ErrorCode[i]^:=temp;
       end;
      readln(tfile,PcGlb);
      close(tfile);
     end else begin
      GetMem(ErrorProc[1],14);
      ErrorProc[1]^:='** UNKNOWN **';
      for i:=2 to MaxProcsGlb do ErrorProc[i]:=ErrorProc[1];
      for i:=1 to MaxErrsGlb do ErrorCode[i]:=ErrorProc[1];
      error(0,0);
     end;
    for i:=1 to MaxWorldsGlb do DefineWorld(i,0,0,XScreenMaxGlb,YMaxGlb);
    MaxWorldGlb:=1;
    for i:=1 to MaxWindowsGlb do begin
      DefineWindow(i,0,0,XMaxGlb,YMaxGlb);
      with stack[i] do begin
        W.Size:=0; Contents:=Nil;
       end;
      RemoveHeader(i);
     end;
    MaxWindowGlb:=1;
    if CharFile<>'' then begin
      assign(fil,CharFile);
      {$I-} reset(fil); {$I+}
      if ioresult=0 then read(fil,CharSet) else error(0,1);
      close(fil);
     end;
    assign(fil8x8,'8x8.fon');
    {$I-} reset(fil8x8); {$I+}
    if ioresult=0 then begin
       new(CharSet8x8);  read(fil8x8,CharSet8x8^)
    end else CharSet8x8:=NIL;
    close(fil8x8);
    assign(fil16x8,'16x8.fon');
    {$I-} reset(fil16x8); {$I+}
    if ioresult=0 then begin
       new(CharSet16x8);  read(fil16x8,CharSet16x8^)
    end else CharSet16x8:=NIL;
    close(fil16x8);
    assign(fil20x16,'20x16.fon');
    {$I-} reset(fil20x16); {$I+}
    if ioresult=0 then begin
       new(CharSet20x16);  read(fil20x16,CharSet20x16^)
    end else CharSet20x16:=NIL;
    close(fil20x16);
    assign(filconv,'8to7bit.cvt');
    {$I-} reset(filconv); {$I+}
    if ioresult=0 then read(filconv,CharConversion) else for ch:=$80 to $ff do CharConversion[ch]:=$20;
    close(filconv);
    BrkGlb:=true;
    if RamScreenGlb then begin
      AllocateRAMScreen;  SelectScreen(2);  ClearScreen;
     end;
    SelectScreen(1);    WindowNdxGlb:=1;    SelectWorld(1);
    SelectWindow(1);    SetColorWhite;      SetClippingOn;
    SetAspect(1);    DirectModeGlb:=false;  SetMessageOn;
    SetHeaderOff;       SetHeaderToTop;     ErrCodeGlb:=0;
    SetLineStyle(0);  VStepGlb:=IVStepGlb;
    X1Glb:=0;  X2Glb:=0;  Y1Glb:=0;  Y2Glb:=0;
    SetTextFar;       SelectSmallFont;
  end;

procedure ResetWindows;
  var i:integer;
  begin
    for i:=1 to MaxWindowsGlb do begin  DefineWindow(i,0,0,XMaxGlb,YMaxGlb);
      RemoveHeader(i);   end;
    SelectWindow(1);
  end;

procedure ResetWorlds;
  var i:integer;
  begin
    for i:=1 to MaxWorldsGlb do  DefineWorld(i,0,0,XScreenMaxGlb,YMaxGlb);
    SelectWorld(1);  SelectWindow(WindowNdxGlb);
  end;

function clip(var x1,y1,x2,y2:integer):boolean;
  var ix1,iy1,ix2,iy2,dummy,X1Loc,X2Loc:integer;
      ClipLoc:boolean;
  function inside(x,xx1,xx2:integer):integer;
    begin
      inside:=0;  if x<xx1 then inside:=-1 else if x>xx2 then inside:=1;
    end;

  begin
    clip:=true;    ClipLoc:=true;
    if ClippingGlb then begin
        X1Loc:=X1RefGlb shl 3;  X2Loc:=X2RefGlb shl 3 +7;
      ix1:=inside(x1,X1Loc,X2Loc);   iy1:=inside(y1,Y1RefGlb,Y2RefGlb);
      ix2:=inside(x2,X1Loc,X2Loc);   iy2:=inside(y2,Y1RefGlb,Y2RefGlb);
      if (ix1 or ix2 or iy1 or iy2)<>0 then begin
        if x1<>x2 then begin
          if ix1<>0 then begin
            if ix1<0 then dummy:=X1Loc else dummy:=X2Loc;
            if y2<>y1 then y1:=y1+trunc((y2-y1)/(x2-x1)*(dummy-x1));
            x1:=dummy;
           end;
          if (ix2<>0) and (x1<>x2) then begin
            if ix2<0 then dummy:=X1Loc else dummy:=X2Loc;
            if y2<>y1 then y2:=y1+trunc((y2-y1)/(x2-x1)*(dummy-x1));
            x2:=dummy;
           end;
          iy1:=inside(y1,Y1RefGlb,Y2RefGlb); iy2:=inside(y2,Y1RefGlb,Y2RefGlb);
         end;
        if y1<>y2 then begin
          if iy1<>0 then begin
            if iy1<0 then dummy:=Y1RefGlb else dummy:=Y2RefGlb;
            if x1<>x2 then x1:=x1+trunc((x2-x1)/(y2-y1)*(dummy-y1));
            y1:=dummy;
           end;
          if iy2<>0 then begin
            if iy2<0 then dummy:=Y1RefGlb else dummy:=Y2RefGlb;
            if x1<>x2 then x2:=x1+trunc((x2-x1)/(y2-y1)*(dummy-y1));
            y2:=dummy;
           end;
         end;
        iy1:=inside(y1,Y1RefGlb,Y2RefGlb); iy2:=inside(y2,Y1RefGlb,Y2RefGlb);
        if (iy1<>0) or (iy2<>0) then ClipLoc:=false;
        if ClipLoc then begin
          ix1:=inside(x1,X1Loc,X2Loc); ix2:=inside(x2,X1Loc,X2Loc);
          if (ix2<>0) or (ix1<>0) then ClipLoc:=false;
         end;
        clip:=ClipLoc;
       end;
     end;
  end;

procedure DrawPoint(xr,yr:real);
  var x,y:integer;
  begin
    if DirectModeGlb then dp(trunc(xr),trunc(yr)) else begin
      x:=WindowX(xr);  y:=WindowY(yr);
      if ClippingGlb then begin
        if (x>=X1RefGlb shl 3) and (x<X2RefGlb shl 3+7) then
          if (y>=Y1RefGlb) and (y<=Y2RefGlb) then dp(x,y);
      end else dp(x,y); end;
  end;

function PointDrawn(xr,yr:real):boolean;
  begin
    if DirectModeGlb then PointDrawn:=PD(trunc(xr),trunc(yr))
    else PointDrawn:=PD(WindowX(xr),WindowY(yr));
  end;

procedure DrawLine(x1,y1,x2,y2:real);
  var X1Loc,Y1Loc,X2Loc,Y2Loc:integer;
  procedure DrawLineDirect(x1,y1,x2,y2:integer);
    var x,y,DeltaX,DeltaY,XStep,YStep,direction:integer;
    begin
      x:=x1;  y:=y1;  XStep:=1;  YStep:=1;
      if x1>x2 then XStep:=-1;  if y1>y2 then YStep:=-1;
      DeltaX:=abs(x2-x1);  DeltaY:=abs(y2-y1);
      if DeltaX=0 then direction:=-1 else direction:=0;
      while not ((x=x2) and (y=y2)) do begin
        if LinestyleGlb=0 then dp(x,y) else begin
          CntGlb:=(CntGlb+1) and 7; if LineStyleArrayGlb[CntGlb] then dp(x,y);
        end;
        if direction<0 then begin
          y:=y+YStep; direction:=direction+DeltaX;
        end else begin
          x:=x+XStep; direction:=direction-DeltaY;
         end;
       end;
    end;
  begin {drawline}
    if DirectModeGlb then DrawLineDirect(trunc(x1),trunc(y1),trunc(x2),trunc(y2))
    else begin
      X1Loc:=WindowX(x1); Y1Loc:=WindowY(y1); X2Loc:=WindowX(x2); Y2Loc:=WindowY(y2);
      if clip (X1Loc,Y1Loc,X2Loc,Y2Loc) then DrawLineDirect(X1Loc,Y1Loc,X2Loc,Y2Loc);
     end;
  end;

procedure DrawLineClipped(x1,y1,x2,y2:integer);
  begin
    if clip(x1,y1,x2,y2) then DrawLine(x1,y1,x2,y2);
  end;

procedure DrawCircleDirect(xr,yr,r:integer; DirectModeLoc: boolean);
  const n=14;
  type Circ = array [1..n] of integer;
  const x:Circ=(0,121,239,355,465,568,663,749,823,885,935,971,993,1000);
  var xk1,xk2,yk1,yk2,xp1,yp1,xp2,yp2:integer;
      xfact,yfact:real;
      i:integer;
  procedure DrawLinW(X1,Y1,X2,Y2:integer);
    var DrawIt: boolean;
    begin
      DrawIt:=DirectModeLoc;
      if not DrawIt then DrawIt:=Clip(X1,Y1,X2,Y2);
      if DrawIt then DrawLine(X1,Y1,X2,Y2);
    end;
  begin {DrCirDir}
    xfact:=abs(r*0.001); yfact:=xfact*AspectGlb;
    if xfact>0.0 then begin
      xk1:=trunc(x[1]*xfact+0.5); yk1:=trunc(x[n]*yfact+0.5);
      for i:=2 to n do begin
        xk2:=trunc(x[i]*xfact+0.5); yk2:=trunc(x[n-i+1]*yfact+0.5);
        xp1:=xr-xk1; yp1:=yr+yk1; xp2:=xr-xk2; yp2:=yr+yk2; DrawLinW(xp1,yp1,xp2,yp2);
        xp1:=xr+xk1; xp2:=xr+xk2; DrawLinW(xp1,yp1,xp2,yp2);
        yp1:=yr-yk1; yp2:=yr-yk2; DrawLinW(xp1,yp1+1,xp2,yp2+1);
        xp1:=xr-xk1; xp2:=xr-xk2; DrawLinW(xp1,yp1+1,xp2,yp2+1);
        xk1:=xk2; yk1:=yk2;
       end;
     end else dp(xr,yr);
  end;

procedure DrawCircle(X_R,Y_R,xradius:real);
  var DirectModeLoc:boolean;
  begin
    DirectModeLoc:=DirectModeGlb; DirectModeGlb:=True;
    if DirectModeLoc then DrawCircleDirect(trunc(X_R),trunc(Y_R),trunc(xradius),True)
    else DrawCircleDirect(WindowX(X_R),WindowY(Y_R),WindowX(X_R+xradius)-WindowX(X_R),False);
    DirectModeGlb:=DirectModeLoc;
  end;

procedure DrawCross(x1,y1,scale:integer);
  begin
    DrawLineClipped(x1-scale,y1,x1+scale+2,y1); DrawLineClipped(x1,y1-scale,x1,y1+scale+1);
  end;

procedure DrawSquareC(x1,y1,x2,y2:integer; fill:boolean);
  var i:integer;
  procedure DSC(x1,x2,y:integer);
    begin
      if clip(x1,y,x2,y) then if LineStyleGlb=0 then DrawStraight(x1,x2,y) else DrawLine(x1,y,x2,y);
    end; {dsc}
  begin
    if not fill then begin
      DSC(x1,x2,y1); DrawLineClipped(x2,y1,x2,y2); DSC(x1,x2,y2); DrawLineClipped(x1,y2,x1,y1);
     end else for i:=y2 to y1 do DSC(x1,x2,i);
  end;

procedure DrawSquare(X1,Y1,X2,Y2:real; fill:boolean);
  var i,x1loc,y1loc,x2loc,y2loc:integer; DirectModeLoc:boolean;
  procedure DS(x1,x2,y:integer);
    begin
      if LineStyleGlb=0 then DrawStraight(x1,x2,y) else DrawLine(x1,y,x2,y);
    end;{ds}
  procedure DSC(x1,x2,y:integer);
    begin if clip(x1,y,x2,y) then DS(x1,x2,y);  end;
  procedure DrawSqr(x1,y1,x2,y2:integer; fill:boolean);
    var i:integer;
    begin
      if not fill then begin
        DS(x1,x2,y1); DrawLine(x2,y1,x2,y2); DS(x1,x2,y2); DrawLine(x1,y2,x1,y1);
       end else for i:=y1 to y2 do DS(x1,x2,i);
    end;
  begin {drawsquare}
     if DirectModeGlb then DrawSqr(trunc(X1),trunc(Y1),trunc(X2),trunc(Y2),fill)
    else begin
      DirectModeLoc:=DirectModeGlb; DirectModeGlb:=true;
      x1loc:=WindowX(X1); y1loc:=WindowY(Y1); x2loc:=WindowX(X2); y2loc:=WindowY(Y2);
      if not fill then begin
        DSC(x1loc,x2loc,y1loc); DrawLineClipped(x2loc,y1loc,x2loc,y2loc);
        DSC(x1loc,x2loc,y2loc); DrawLineClipped(x1loc,y2loc,x1loc,y1loc);
       end else for i:=y1loc to y2loc do DSC(x1loc,x2loc,i);
      DirectModeGlb:=DirectModeLoc;
     end;
  end;

procedure DrawAscii(var x,y:integer; size,ch:byte);
  var x1ref,x2ref,xpos,ypos,xstart,ystart,xend,yend,xx,yy: integer;
      charbyte: byte;
  begin
    x1ref:=X1RefGlb shl 3; x2ref:=X2RefGlb shl 3+7;
    IF TextDistance < 8 THEN BEGIN
     if ch>=$80 then ch:=CharConversion[ch];
     for ypos:=0 to 5 do begin
       CharByte:=(CharSet[ch,(7-ypos) shr 1] shr ((ypos and 1) shl 2)) and $0F;
       for xpos:=0 to 3 do
         if (CharByte shr (3-xpos)) and 1<>0 then begin
           xstart:=x+xpos*size;  xend:=xstart+size-1; ystart:=y+1+(ypos-2)*size; yend:=ystart+size-1;
           if ClippingGlb then begin
             if xstart<x1ref then xstart:=x1ref;
             if xend>x2ref then xend:=x2ref;
             if ystart<Y1RefGlb then ystart:=Y1RefGlb;
             if yend>Y2RefGlb then yend:=Y2RefGlb;
            end;
           for yy:=ystart to yend do for xx:=xstart to xend do dp(xx,yy);
          end;
      end;
    END ELSE BEGIN
     IF SmallFont THEN BEGIN
       for ypos:=0 to 7 do begin
        CharByte:=CharSet8x8^[ch,ypos];
        for xpos:=0 to 7 do
          if (CharByte shr (7-xpos)) and 1<>0 then begin
            xstart:=x+xpos*size; xend:=xstart+size-1; ystart:=y+ypos*size; yend:=ystart+size-1;
            if ClippingGlb then begin
              if xstart<x1ref then xstart:=x1ref;
              if xend>x2ref then xend:=x2ref;
              if ystart<Y1RefGlb then ystart:=Y1RefGlb;
              if yend>Y2RefGlb then yend:=Y2RefGlb;
             end;
            for yy:=ystart to yend do for xx:=xstart to xend do dp(xx,yy);
           end;
        end;
     END ELSE BEGIN
       for ypos:=0 to 15 do begin
        CharByte:=CharSet16x8^[ch,ypos];
        for xpos:=0 to 7 do
          if (CharByte shr (7-xpos)) and 1<>0 then begin
            xstart:=x+xpos*size; xend:=xstart+size-1; ystart:=y+ypos*size; yend:=ystart+size-1;
            if ClippingGlb then begin
              if xstart<x1ref then xstart:=x1ref;
              if xend>x2ref then xend:=x2ref;
              if ystart<Y1RefGlb then ystart:=Y1RefGlb;
              if yend>Y2RefGlb then yend:=Y2RefGlb;
             end;
            for yy:=ystart to yend do for xx:=xstart to xend do dp(xx,yy);
           end;
        end;
     END;
    END;
      x:=x+size*TextDistance;
end;

procedure DrawText(x,y,scale:integer; txt:wrkstring);
  var LineStyleLoc,code,AsciiValue,StringLen,i,SymbolScale,SymbolCode:integer;
      DirectModeLoc:boolean;
  begin
    DirectModeLoc:=DirectModeGlb; DirectModeGlb:=true; LineStyleLoc:=LinestyleGlb;
    SetLineStyle(0); StringLen:=length(txt); i:=1;
    while i<=StringLen do begin
      AsciiValue:=ord(txt[i]); DrawAscii(x,y,scale,AsciiValue); i:=succ(i);
    end;
    DirectModeGlb:=DirectModeLoc; SetLineStyle(LineStyleLoc);
  end;

procedure DrawTextW(x,y:real; scale:integer; txt:wrkstring);
  begin
    if DirectModeGlb then DrawText(trunc(x),trunc(y),scale,txt) else DrawText(WindowX(x),WindowY(y),scale,txt);
  end;

procedure DrawBorder;
  var ClipLoc,DirectModeLoc:boolean;
      xl1,xl2:integer;
  procedure DrawHeaderBackground(y1,y2:integer);
    var i:integer;
    begin for i:=y1 to y2 do DrawStraight(xl1,xl2,i); end;
  procedure DrawHeader;
    var Y1Hdr,Y2Hdr,yl1,yl2:integer;
    begin
      with window[WindowNdxGlb] do begin
        if drawn then
          if top then begin
            ReDefineWindow(WindowNdxGlb,X1RefGlb,Y1RefGlb-HeaderSizeGlb,X2RefGlb,Y2RefGlb);
            SelectWindow(WindowNdxGlb);
           end else begin
            ReDefineWindow(WindowNdxGlb,X1RefGlb,Y1RefGlb,X2RefGlb,Y2RefGlb+HeaderSizeGlb);
            SelectWindow(WindowNdxGlb);
           end;
        if TopGlb then begin
          yl1:=Y1RefGlb+HeaderSizeGlb; yl2:=Y2RefGlb;
          Y1Hdr:=Y1RefGlb; Y2Hdr:=Y1RefGlb+HeaderSizeGlb-1;
         end else begin
          yl1:=Y1RefGlb; yl2:=Y2RefGlb-HeaderSizeGlb;
          Y1Hdr:=Y2RefGlb-HeaderSizeGlb+1; Y2Hdr:=Y2RefGlb;
         end;
        top:=TopGlb;
        ReDefineWindow(WindowNdxGlb,X1RefGlb,yl1,X2RefGlb,yl2);
        SelectWindow(WindowNdxGlb);
        DrawHeaderBackground(Y1Hdr,Y2Hdr);
        ColorGlb:=255-ColorGlb;
        DrawText(xl1+2+(xl2-xl1-length(header)*TextDistance) shr 1,Y1Hdr+3,1,header);
        DrawSquare(xl1,Y1Hdr,xl2,Y2Hdr,false);
        ColorGlb:=255-ColorGlb;
        DrawSquare(xl1,Y1RefGlb,xl2,Y2RefGlb,false);
        drawn:=true;
      end;
    end;
  begin {drawborder}
    DirectModeLoc:=DirectModeGlb; DirectModeGlb:=true; ClipLoc:=ClippingGlb;
    ClippingGlb:=false; xl1:=X1RefGlb shl 3; xl2:=X2RefGlb shl 3+7;
    with window[WindowNdxGlb] do
      if ((HeaderGlb) and (length(header)>0)) and (y2-y1>HeaderSizeGlb) and
         ((length(header)*TextDistance)<abs(xl2-xl1)-4) then DrawHeader
      else begin
        drawn:=false; DrawSquare(xl1,Y1RefGlb,xl2,Y2RefGlb,false);
      end;
    DirectModeGlb:=DirectModeLoc; ClippingGlb:=ClipLoc;
  end;

procedure MoveVer(delta:integer; FillOut:boolean);
  var direction,outer,FromBase,i,XLen,from,tu:integer;
  procedure MoveVer1(VStep: integer);
    begin
      XLen:=X2RefGlb-X1RefGlb+1;
      if direction=-1 then for i:=Y1RefGlb to Y2RefGlb do begin
          if i>0 then begin
            From:=BaseAddress[I]; Tu:=BaseAddress[I-VStep];
            Move(mem[GrafBase:from+X1RefGlb],mem[GrafBase:tu+X1RefGlb],XLen);
           end;
         end
      else
        for i:=Y2RefGlb downto Y1RefGlb do if i<YMaxGlb then begin
            From:=BaseAddress[I]; Tu:=BaseAddress[I+VStep];
            Move(mem[GrafBase:from+X1RefGlb],mem[GrafBase:tu+X1RefGlb],XLen);
           end;
      if not RamScreenGlb then FillOut:=false;
      if not FillOut then
        if direction=-1 then
          for I:=Y2RefGlb downto Y2RefGlb-VStep+1 do FillChar(mem[GrafBase:BaseAddress[I]+X1RefGlb],XLen,not ColorGlb)
        else
          for I:=Y1RefGlb to Y1RefGlb+VStep-1 do FillChar(mem[GrafBase:BaseAddress[I]+X1RefGlb],XLen,not ColorGlb)
      else begin
        if GrafBase=HardwareGrafBase then FromBase:=seg(ScreenGlb^) else FromBase:=HardwareGrafBase;
        if direction=-1 then for I:=Y2RefGlb downto Y2RefGlb-VStep+1 do
            Move(mem[FromBase:BaseAddress[I]+X1RefGlb],mem[GrafBase:BaseAddress[I]+X1RefGlb],XLen)
        else
          for I:=Y1RefGlb to Y1RefGlb+VStep-1 do
            Move(mem[FromBase:BaseAddress[I]+X1RefGlb],mem[GrafBase:BaseAddress[I]+X1RefGlb],XLen);
       end;
      ReDefineWindow(WindowNdxGlb,X1RefGlb,Y1RefGlb+VStep*direction,X2RefGlb,Y2RefGlb+VStep*direction);
      SelectWindow(WindowNdxGlb);
    end;
  begin
    if delta<>0 then begin
      direction:=1; if delta<0 then direction:=-1;
      with window[WindowNdxGlb] do
        if drawn then
          if top then Y1RefGlb:=Y1RefGlb-HeaderSizeGlb else Y2RefGlb:=Y2RefGlb+HeaderSizeGlb;
      if (Y1RefGlb+delta < 0) or (Y2RefGlb+delta > YMaxGlb) then Error(23,7)
      else begin
        for outer:=1 to abs(delta) div VStepGlb do MoveVer1(VStepGlb);
        if abs(delta) mod VStepGlb<>0 then MoveVer1(abs(delta) mod VStepGlb);
       end;
      with window[WindowNdxGlb] do
        if drawn then
          if top then Y1RefGlb:=Y1RefGlb+HeaderSizeGlb else Y2RefGlb:=Y2RefGlb-HeaderSizeGlb;
     end;
  end;

procedure MoveHor(delta:integer; FillOut:boolean);
  var direction,outer,FromBase,i,XLen,y:integer;
  begin
    if delta<>0 then begin
      direction:=1; if delta<0 then direction:=-1;
      with window[WindowNdxGlb] do
        if drawn then
          if top then Y1RefGlb:=Y1RefGlb-HeaderSizeGlb else Y2RefGlb:=Y2RefGlb+HeaderSizeGlb;
      if (X1RefGlb+delta < 0) or (X2RefGlb+delta > XMaxGlb) then Error(24,7)
      else begin
        for outer:=1 to abs(delta) do begin
          XLen:=X2RefGlb-X1RefGlb+1;
          for i:=Y1RefGlb to Y2RefGlb do begin
            Y:=BaseAddress[i];
            Move(mem[GrafBase:y+X1RefGlb],mem[GrafBase:y+X1RefGlb+direction],XLen);
            if not RamScreenGlb then FillOut:=false;
            if not FillOut then
              if direction<0 then Mem[GrafBase:y+X2RefGlb]:=(not ColorGlb) and $FF
              else Mem[GrafBase:y+X1RefGlb]:=(not ColorGlb) and $FF
            else begin
              if GrafBase=HardwareGrafBase then FromBase:=seg(ScreenGlb^) else FromBase:=HardwareGrafBase;
              if direction=-1 then Mem[GrafBase:y+X2RefGlb]:=Mem[FromBase:y+X2RefGlb]
              else Mem[GrafBase:y+X1RefGlb]:=Mem[FromBase:y+X1RefGlb];
             end;
           end;
          ReDefineWindow(WindowNdxGlb,X1RefGlb+direction,Y1RefGlb,X2RefGlb+direction,Y2RefGlb);
          SelectWindow(WindowNdxGlb);
         end;
       end;
      with window[WindowNdxGlb] do
        if drawn then
          if top then Y1RefGlb:=Y1RefGlb+HeaderSizeGlb else Y2RefGlb:=Y2RefGlb-HeaderSizeGlb;
     end;
  end;

procedure CopyWindow(from,tu:byte; x1,y1:integer);
  var XLen,YLen:integer;
      FromBase,ToBase,i:integer;
  begin
    if (x1<0) or (y1<0) then error(17,3) else begin
      with window[WindowNdxGlb] do
        if drawn then
          if top then Y1RefGlb:=Y1RefGlb-HeaderSizeGlb else Y2RefGlb:=Y2RefGlb+HeaderSizeGlb;
      if from=2 then FromBase:=seg(ScreenGlb^) else FromBase:=HardwareGrafBase;
      if tu=2 then ToBase:=seg(ScreenGlb^) else ToBase:=HardwareGrafBase;
      XLen:=X2RefGlb-X1RefGlb; YLen:=Y2RefGlb-Y1RefGlb;
      if x1+XLen>XMaxGlb then XLen:=XMaxGlb-x1; if y1+YLen>YMaxGlb then YLen:=YMaxGlb-y1;
      XLen:=XLen+1;
      for i:=0 to YLen do
        Move(mem[FromBase:BaseAddress[Y1RefGlb+i]+X1RefGlb],mem[ToBase:BaseAddress[Y1+i]+x1],XLen);
      with window[WindowNdxGlb] do
        if drawn then
          if top then Y1RefGlb:=Y1RefGlb+HeaderSizeGlb else Y2RefGlb:=Y2RefGlb-HeaderSizeGlb;
     end;
  end;

procedure SaveWindow(n:integer; FileName:wrkstring);
  type sector=array [0..127] of byte;
  var i,j,secptr,xlen:integer;
      W:WindowType;
      PictureFile:file of sector;
      sec1:array [0..1] of sector;
  begin
    W:=window[n]; assign(PictureFile,FileName);
    {$I-} rewrite(PictureFile); {$I+}
    if IOResult<>0 then Error(25,5)
    else begin
      move(w,sec1,sizeof(W));
      secptr:=sizeof(W);
      with W do begin
        if drawn then
          if top then y1:=y1-HeaderSizeGlb else y2:=y2+HeaderSizeGlb;
        xlen:=x2-x1+1;
        for i:=y1 to y2 do begin
          move(mem[GrafBase:BaseAddress[i]+x1],sec1[0,secptr],xlen);
          secptr:=secptr+xlen;
          if secptr>127 then begin
            Write(PictureFile,sec1[0]); move(sec1[1],sec1[0],128); secptr:=secptr-128;
           end;
         end;
        if secptr<>0 then Write(PictureFile,sec1[0]);
       end;
      close(PictureFile);
     end;
  end;

procedure LoadWindow(n,xpos,ypos:integer; FileName:wrkstring);
  type sector=array [0..127] of byte;
  var i,secptr,xlen:integer;
      W:WindowType;
      PictureFile:file of sector;
      sec1:array [0..1] of sector;
      second:boolean;
  begin
    assign(PictureFile,FileName);
    {$I-} reset(PictureFile); {$I+}
    if IOResult<>0 then Error(12,5)
    else begin
      Read(PictureFile,sec1[0]);
      move(sec1,W,sizeof(W));
      secptr:=sizeof(W); second:=false; window[n]:=W;
      with W do begin
        if drawn then
          if top then y1:=y1-HeaderSizeGlb else y2:=y2+HeaderSizeGlb;
        xlen:=x2-x1+1;
        if xpos>=0 then begin x2:=xpos+x2-x1; x1:=xpos; end;
        if ypos>=0 then begin y2:=ypos+y2-y1; y1:=ypos; end;
        if (x1<0) or (y1<0) or (x2>XMaxGlb) or (y2>YMaxGlb) then error(12,3)
        else begin
          for i:=y1 to y2 do begin
            if (secptr+xlen>127) and not second and not eof(PictureFile) then
             begin Read(PictureFile,sec1[1]); second:=true; end;
            move(sec1[0,secptr],mem[GrafBase:BaseAddress[i]+x1],xlen);
            secptr:=secptr+xlen;
            if secptr>127 then begin
              move(sec1[1],sec1[0],128); secptr:=secptr-128; second:=false;
             end;
           end;
         end;
       end;
      window[n]:=W; close(PictureFile);
     end;
  end;

function WindowSize(win:integer):integer;
  var WS: integer;
  begin
    WS:=-1;
    if not (win in [1..MaxWindowsGlb]) then error(13,2) else with window[win] do begin
      WS:=(y2-y1+1)*(x2-x1+1); if Drawn then WS:=WS+HeaderSizeGlb*(x2-x1+1);
      WS:=(WS+$03FF) And $FC00;
     end;
    WindowSize:=WS;
  end;

procedure ClearWindowStack(win:integer);
  begin
    if not (win in [1..MaxWindowsGlb]) then error(14,2) else with stack[win],W do begin
      if (Contents<>Nil) then freemem(Contents,Size);
      Contents:=nil; Size:=0;
     end;
  end;

procedure StoreWindow(win:integer);
  var i,XLen,y,y0,y9,A:integer;
      m:real;
  begin
    if not (win in [1..MaxWindowsGlb]) then error(15,2) else begin
      if stack[win].Contents<>Nil then ClearWindowStack(win);
      m:=maxavail; if m<0 then m:=m+65536.0;
      if WindowSize(win)>16.0*m then error(15,6)
      else with stack[win],W do begin
          W:=window[win]; Size:=WindowSize(win); getmem(Contents,Size);
          with W do begin
            y0:=y1; y9:=y2;
            if drawn then
              if top then y0:=y0-HeaderSizeGlb else y9:=y9+HeaderSizeGlb;
            XLen:=x2-x1+1; A:=0;
            for i:=y0 to y9 do begin
              Y:=BaseAddress[I];
              Move(mem[GrafBase:y+x1],mem[seg(Contents^):ofs(Contents^)+A],XLen);
              A:=A+XLen;
             end;
           end;
         end;
     end;
  end;

procedure RestoreWindow(win,DeltaX,DeltaY:integer);
  var i,XLen,y,y0,y9,A:integer;
      w1:WindowType;
  begin
    if not (win in [1..MaxWindowsGlb]) then error(16,2)
    else with stack[abs(win)] do begin
      W1:=W;
      if Contents=Nil then error(16,2) else with W1 do begin
        x1:=x1+DeltaX; x2:=x2+DeltaX; y1:=y1+DeltaY; y2:=y2+DeltaY;
        if (X1>=0) and (X1<=XMaxGlb) and (X2>=0) and (X2<=XMaxGlb) and
           (Y1>=0) and (Y1<=YMaxGlb) and (Y2>=0) and (Y2<=YMaxGlb) then begin
          XLen:=X2-X1+1; A:=0; y0:=y1; y9:=y2;
          if drawn then
            if top then y0:=y0-HeaderSizeGlb else y9:=y9+HeaderSizeGlb;
          for i:=y0 to y9 do begin
            Y:=BaseAddress[i];
            with stack[win] do Move(mem[seg(Contents^):ofs(Contents^)+A],mem[GrafBase:y+X1],XLen);
            A:=A+XLen;
           end;
          window[win]:=W1;
          if win=WindowNdxGlb then SelectWindow(win);
         end
        else error(16,3);
       end;
     end
  end;

procedure SaveWindowStack(FileName:wrkstring);
  var WindowFile:file;
      PointerFile:file of WindowType;
      i:integer;
  begin
    assign(WindowFile,FileName+'.stk');
    {$I-} rewrite(WindowFile); {$I+}
    if IOResult<>0 then Error(26,5)
    else begin
      for i:=1 to MaxWindowsGlb do with stack[i],W do if Contents<>Nil then
            blockwrite(WindowFile,Contents^,Size Shr 7);
      close(WindowFile);
      assign(PointerFile,FileName+'.ptr');
      {$I-} rewrite(PointerFile); {$I+}
      if IOResult<>0 then Error(26,5)
      else begin
        for i:=1 to MaxWindowsGlb do write(PointerFile,stack[i].W);
        close(PointerFile);
       end;
     end;
  end;

procedure LoadWindowStack(FileName:wrkstring);
  var WindowFile:file;
      PointerFile:file of WindowType;
      i,WS:integer;
  begin
    assign(PointerFile,FileName+'.ptr');
    {$I-} reset(PointerFile); {$I+}
    if IOResult=0 then begin
      for i:=1 to MaxWindowsGlb do read(PointerFile,stack[i].W);
      close(PointerFile);
      assign(WindowFile,FileName+'.stk');
      {$I-} reset(WindowFile); {$I+}
      if IOResult=0 then begin
        for i:=1 to MaxWindowsGlb do with stack[i],W do if Size<>0 then begin
              getmem(Contents,Size); blockread(WindowFile,Contents^,Size Shr 7);
             end
            else Contents:=nil;
        close(WindowFile);
       end
      else error(21,5);
     end
    else error(21,5);
  end;

procedure ResetWindowStack;
  var i:integer;
  begin  for i:=1 to MaxWindowsGlb do ClearWindowStack(i); end;

procedure InvertWindow;
  var i,j,b:integer;
  begin
    with window[WindowNdxGlb] do
      if drawn then
        if top then Y1RefGlb:=Y1RefGlb-HeaderSizeGlb else Y2RefGlb:=Y2RefGlb+HeaderSizeGlb;
    for i:=Y1RefGlb to Y2RefGlb do begin
      b:=BaseAddress[I];
      Inline($8B/$86/ b /$8B/$1E/ X1RefGlb /$8B/$0E/ X2RefGlb /$8B/$16/
            GrafBase /$1E/$8E/$DA/$29/$D9/$41/$01/$C3/$F6/$17/$43/$E2/$FB/$1F);
     end;
    with window[WindowNdxGlb] do
      if drawn then
        if top then Y1RefGlb:=Y1RefGlb+HeaderSizeGlb else Y2RefGlb:=Y2RefGlb-HeaderSizeGlb;
  end;
