{$E-,I-,Q-,R-,S-,X-}
Uses Blue_Cob,Crt,Tunplas;

{ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ}

Var x, xx, y     : Word;
    retr         : Boolean;
    ch           : Char;
    YWert        : Integer;
    plas,segmen3 : Pointer;
    XWerte       : Array[0..255] of Integer;

{ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ}

Const str =  ^j^j+'    "Black Hole" v1.1"'+^j^j^m
             +'    (C)opyright "Blue Cobold"ธ 2001'+^j^j^m
             +'    Fr Info''s bin ich zu erreichen unter'+^j^m
             +'    BlueCobold@gmx.de';
      quest = ^j^j^j^j+'    Retrace-Synchronisation? (J/N)';

Begin
  Ab_in_13h;
  asm mov ax,3; int 10h end;
  GetMem(plas,$ffff);
  GetMem(segmen3,$ffff);
  FillChar(plas^,$ffff,0);
  WriteLn(^j^j^j'                           ษอออออออออออป');
  WriteLn('    Berechne die Texturen: บ           บ');
  WriteLn('                           ศอออออออออออผ');
  GotoXY(29,5);
  CalcRain(plas,255);
  GotoXY(5,5);
  Write('Berechne das Loch:     บ           ');
  GotoXY(29,5);
  CalcWorm(segmen3);
  ClrScr;
  Write(quest);
  Repeat ch:=Upcase(ReadKey) Until ch In ['J','N'];
  If ch='J' Then retr:=True Else retr:=False;
  asm mov ax,13h; int 10h end;
  asm cli end;  {}
  For x:= 0 To 63 Do
  Begin
    Farbe_setzen(     x,    x,    0,    0);
    Farbe_setzen(  x+64, 63-x,    0,    0);
    Farbe_setzen( x+128,    x,    x,    x);
    Farbe_setzen( x+192, 63-x, 63-x, 63-x);
  End;
  x:=0;
  Start;
  Repeat
    Inc(x);
    DoWorm(plas,segmen3,x*510);
    If retr Then Retrace;{}
    Bilde;
  Until Port[$60]=1;
  asm sti end;
  Raus_aus_13h;
  Write(str,^j^j^j^m, x/(Result/18.21):10:2,' Fps');
  FreeMem(plas,$ffff);
  FreeMem(segmen3,$ffff);
End.
