{$E-,F+,G+,I-,N+,O+,Q-,R-,S-,V-}
Unit Tunplas;

INTERFACE

Uses Blue_Cob,Crt;

Procedure CalcPlas( plas : Pointer);
Procedure CalcTunnel( Int : Word);
Procedure Trash_it_256;
Procedure DoTunnel( plas : Pointer; move : Word);
Procedure CalcWorm( seg3 : Pointer);
Procedure CalcRain( plas : Pointer; Intensity : Byte);
Procedure DoWorm( plas, seg3 : Pointer; move : Word);

IMPLEMENTATION

Procedure CalcPlas( plas : Pointer);
Var a, x, x2, x3, y, y2 : Word;
  Begin
    Randomize;
    For x:=0 To $ffff Do
      If Random(800)=0 Then Mem[seg(segmen^):x]:=Random(30)*Random(20);
    For a:=0 To 14 Do
    Begin
      For x:=0 To $ffff Do
      Begin
        x2:=0;
        For y:=1 To 5 Do
          For y2:=1 To 5 Do
            x2:=x2+Mem[seg(segmen^):x+(y-3)*256+y2-3];
        x3:=Mem[seg(segmen^):x];
        If a>10 Then x2:=x2 div 25
          Else x2:=x2 div 25+Random(4)*Random(10*x3)-Random(4)*Random(5*x3);
        If a=11 Then x2:=x2+Random(20)*Random(20);
        Mem[seg(segmen2^):x]:=x2;
      End;
      Zeiger:=segmen;
      segmen:=segmen2;
      segmen2:=Zeiger;
      Write('²');
    End;
    asm
      les di,plas
      push ds
      lds si,segmen
      mov cx,4000h
    db 66h
      rep movsw
      pop ds
    end;
  End;

Procedure CalcTunnel( Int : Word);
Var x, y, col, Winkel : Word;
  Begin
    For y:=0 To 99 Do
      For x:=0 To 159 Do
      Begin
        Winkel:=Lo(Round(Arctan386(1.5*(99.5-y)/(159.5-x))
                                      *(256 Shl (Int-1))/pi));
        col:=Round( 12000/Sqrt((159.5-x)*(159.5-x)+1.5*(99.5-y)*(99.5-y)) );
        Memw[seg(segmen2^):2*(320*y+x)]:=(col And 255) shl 8 +winkel;
     {} Memw[seg(segmen2^):2*(320*y+(319-x))]:=(col and 255) shl 8 -winkel;
   {} End;
  End;

Procedure DoTunnel( plas : Pointer; move : Word);assembler;
  asm
    les di, segmen
    mov ax, word ptr [segmen2+2]
  db 08eh          {mov }
  db 0e8h          {gs,ax}
    mov ax, move
    push ds
    lds si, plas
    mov dx, si
    mov cx, 32000  {z„hler}
@lp1:
    mov bx, dx
  db 65h
    mov si, [bx]   {Addresse holen}
    add si, ax     {Bewegung drauf}
    movsb          {Byte kopieren}
    add dx, 2
    push di        {di sichern}
    mov bx, 63999  {"von hinten" auch!}
    sub bx, di
    inc bx         {wegen "inc di" bei "movsb"}
    mov di, bx     {neuer Offset}
    dec si         {auch wegen "inc si" bei "movsb"}
    movsb
    pop di         {di holen}
    dec cx
    jnz @lp1
    pop ds
  end;

Procedure CalcWorm( seg3 : Pointer);
Var
  x, x2, y, y2, mo, col, peri : Word;
  r : Double;
  Begin
    {For x:=0 To 3120 Do Sinus^[x]:=Round( 1024*Sin386((x*pi/180)/3120*360) );}
    r:=26.4;
    Repeat
      r:=r+0.1*Sqrt(Sqrt(Sqrt(r)));
      peri:=Round(869*(r/240));
      For x:=0 To peri Do
      Begin
        y2:=Round(0.6*(110.5+4000/r
                  +r{Sinus^[Trunc(780+3120/(peri*2)*x)]/1024}
                  *Cos386(x*pi/peri) ));
        If (y2>=0) And (y2<200) Then
        Begin
          x2:=Round(159.5+r{*Sinus^[Trunc(3120/(peri*2)*x)]/1024}
                           *Sin386(x*pi/peri) );
          If (x2>159) And (x2<320) Then
          Begin
            col:=Round(x/peri*512) And 255;
         {} Memw[seg(segmen2^):2*(160*y2+(319-x2))]:=(256*(Round(2*r) and 255))+257-col;
         {} Memw[seg(seg3^):2*(160*y2+x2-160)]:=(256*(Round(2*r) and 255))+col;
          End;
        End;
      End;
      If (Round(r*1000) mod 114=0)
         And (r<240) Then Write('²');
    Until (r>=260);
  End;

Procedure Trash_It_256;
  Begin
    Zeiger:=segmen;
    segmen:=segmen2;
    segmen2:=Zeiger;
    asm
      push ds
      les di,segmen      {Schreib-Speicher" laden}
      lds si,segmen2
      xor di,di
      mov cx,0ffffh
    db 66h
      inc cx

@lop:
      xor ax,ax
      xor dx,dx
      mov al,[si+1]      {Pixelfarbe rechts neben dem Zielpixel nach al}
      mov dl,[si-1]      {Pixelfarbe links neben dem Zielpixel nach bl}
      add ax,dx          {Pixelfarben addieren}
      mov dl,[si-257]    {...und so weiter...[der Offset der Pixel berechnet
                          sich aus 320*Y+X ...min=0...max=63999]}
      add ax,dx
      mov dl,[si-256]
      add ax,dx
      mov dl,[si-255]
      add ax,dx
      mov dl,[si+255]
      add ax,dx
      mov dl,[si+256]
      add ax,dx
      mov dl,[si+257]
      add ax,dx
      add ax,2
      shr ax,3           {Summe der Farben per shiften um 3 nach rechts durch
                          8 teilen}
      stosb              {in den "original-Punkt"}
      inc si             {n„chstes Pixel}
    db 66h
      dec cx             {   "       "  }
      jnz @lop           {solange, bis cx=0...dann sind alle, bis auf die
                          Pixel in Zeile 1 berechnet worden}
      pop ds
    end
  End;

Procedure Klecks( x, y, farbe : Byte);assembler;
  asm
    les di,segmen
    mov ah,y
    mov al,x
    mov di,ax
    mov al,farbe
    mov dx,10
@lp1:
    mov cx,10
    rep stosb
    add di,246
    dec dx
    jnz @lp1
  end;

Procedure CalcRain( plas : Pointer; Intensity : Byte);
Var x, y, i, a : Word;
  Begin
    Randomize;
    For i:=0 To 160 Do              {schon mal vorbereiten, damit dann nicht}
    Begin                            {erst langsam der Screen voller wird}
      If i mod 16=0 Then Write('²');
      For a:=0 To 2 Do
        Klecks( Random(246), Random(246), Intensity);
      trash_it_256;
    End;
    trash_it_256;
    asm
      les di, plas
      push ds
      lds si, segmen
      mov cx, 4000h
    db 66h
      rep movsw
      pop ds
    end;
  End;

Procedure DoWorm( plas, seg3 : Pointer; move : Word);assembler;
  asm
    les di, segmen  {}
    mov ax, word ptr [segmen2+2]
  db 08eh          {mov... }
  db 0e8h          {...gs,ax}
    mov ax, word ptr [seg3+2]
  db 8eh           {mov... }
  db 0e0h          {...fs,ax}
    mov ax, move
    push ds
    lds si, plas
    mov bx, si
    mov dx, si
    mov ch, 200    {Z„hler der Zeilen}
@lp1:
    mov cl, 160    {Spalten}
@lpl:
  db 65h
    mov si, [bx]   {Adresse laden}
    add si, ax     {Bewegung drauf}
    movsb          {Pixel kopieren}
    add bx, 2      {n„chste Adresse}
    dec cl         {Z„hler senken}
    jnz @lpl
    push bx        {linken Offset sichern}

    mov cl, 160    {Spalten}
    mov bx, dx     {rechten laden}
@lpr:
  db 64h
    mov si, [bx]
    add si, ax
    movsb
    add bx, 2
    dec cl
    jnz @lpr
    mov dx, bx      {rechten sichern}
    pop bx          {linken laden}

    dec ch
    jnz  @lp1
    pop ds
  end;


End.
