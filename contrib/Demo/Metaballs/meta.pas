{$E-,F+,G+,I-,N+,O+,Q-,R-,S-,V-}

Program Metaballs;

{ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ


               ÉÍÍÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÍÍÍÍÍÍÍÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÍÍ»
               ³  *******************************************  ³
               ³  ****     Writen By: "Blue Cobold"¸     ****  ³
               ³  ****  (C)opyright "Blue Cobold"¸ 2001  ****  ³
               ³  *******************************************  ³
               ÈÍÍÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÍÍ¼

    WENN DIR EINE UNIT FEHLT ODER DU HILFE BRAUCHST: BlueCobold@gmx.de
ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ}

Uses Crt, Blue_Cob;

Const balls = 5;    {max=9}

Var x, y, i : Word;
    ball    : Array [1..balls] of Record
                                    p    : Pointer;
                                    rand : Byte;
                                    dia  : Word;
                                  End;

Procedure Calc_Ball( hohe, breite : Word; po : Pointer);
Var x, y, col : Integer;
    e    : Single;
  Begin
    For y:=0 To hohe-1 Do
      For x:=0 To breite-1 Do
      Begin
        e:= Sqrt( Sqr(x-(breite+1)/2) + 1.5*Sqr(y-(breite+1)/2) );
        col:=Round(130+130*Cos386(e*pi/(hohe/2)));
        If col>255 Then col:=255;
        If e<(hohe div 2) Then
          Mem[ Seg(po^):(breite*y+x)]:=col
        Else Mem[ Seg(po^):(breite*y+x)]:=0;
      End;
  End;

Procedure Do_Ball( x, y, h, b : Word; po : Pointer);assembler;
  asm
    les di, segmen
    mov ax, 320
    mul y
    add ax, x
    mov di, ax
    push ds
    lds si, po
    xor si, si
    mov dx, h
  @lpy:
    mov cx, b
  @lpx:
    lodsb
    add al, es:[di]
    jnc @weiter
    mov al, 255
  @weiter:
    stosb
    dec cx
    jnz @lpx
    add di, 320
    sub di, b
    dec dx
    jnz @lpy
    pop ds
  end;

Begin
  Ab_in_13h;
  Randomize;
  For x:=0 To 127 Do
  Begin
    Farbe_setzen(x, 0, x shr 1, 0 );
    Farbe_setzen(x+128, x shr 1, 63, x shr 1);
  End;
  Farbe_setzen(255, 63, 63, 63);  {}
  For i:=1 To balls Do
  Begin
    ball[i].dia:=20+Abs(70-10*(i-1)){}{Round(60+30*Sin386(x*i)){};
    ball[i].rand:=Round( 20+20*Sin386(i*pi/balls*13));
    GetMem( ball[i].p, Sqr(ball[i].dia) );
    Calc_Ball( ball[i].dia, ball[i].dia, ball[i].p);
  End;
  x:=0;
  Repeat
    Loesche_13h;
    For i:=1 To balls Do
      Do_Ball( Round( (160-ball[i].dia shr 1)
                     +Sin386(x*pi/400+i*6)*(90-8*i)*Sin386((x+ball[i].rand)*pi/150)
                     +(20+6*i)*Cos386((x-ball[i].rand+940)*pi/150)),
               Round( (100-ball[i].dia shr 1)
                     +Cos386(x*pi/400+i*6)*(20+6*i)*Cos386((x+ball[i].rand)*pi/120)
                     +(20+4*i)*Sin386((x-ball[i].rand+1000)*pi/120)),
               ball[i].dia, ball[i].dia, ball[i].p);  {}
    Retrace;
    Bilde;
    Inc(x);
  Until Port[$60]=1;
  For i:=1 To balls Do GetMem( ball[i].p, Sqr(ball[i].dia) );
  Raus_aus_13h;
End.
