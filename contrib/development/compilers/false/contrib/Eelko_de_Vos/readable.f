{readable.f}
{gets a textfile and makes it readable (cuts off after Xth pos at space) }
^0b:0c:
[d:'0 d;>~'91+d;>&]
[1_c:b;10*b:d;'0-b;+b:^]
#"amount: "b;."
"c;
[0a:
 [d;$1_=~]
 [$10=$
  [0a:10,       { a:=0 ; println }
  ]?
  ~[
     $' =a;b;>&$        {is it a ' ' textchar and a>b?}
     ["
"    0a:
     ]?
     ~[d;,
     ]?
  ]?
  {print char}
  { increment counter. Beware of TAB's! }
  '	=[a;7+a:]?
  a;1+a:
 ^d:]#
]?
~
[
"readable: required argument missing.
Usage: readable number <filename_in [>filename_out]
       number tells where to split up the lines.
"
]?

