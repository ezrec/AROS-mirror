{remcom.f}
{removes comments like this...}
0a:0c:0d:0e:
[^$$'"=[e;1+e:]?b:1_=~]
[{print <CRT> every 50th char}
 b;10=~[d;49>c;~&e;1&1=~&["
"0d:]?]?
 a;$~
  [b;123=$~
   [{print char}
    {num?}
    b;'01->'91+b;>&b;''=|b;'"=|$
    [{ja}b;,d;1+d:1_c:
    ]?
    ~[{no, is it space | CRT | TAB and last char was no num?}
        b;' =b;'        =|b;10=|b;''=|b;'"=|c;~&$
        [{no-> print}0c:]?
        ~[b;,d;1+d:0c:]?{no spaces or similar}
     ]?
    {print char end}]?
   [1_a:]?
  ]?
  [b;125=
   [0a:]?
  ]?
]#"
"


