{countwl.f}
{count lines and words }
0a:0b:0c:
[^$1_=~]
[$10=
  [a;1+a:0c:]?
  $$d:' =
  d;'   =|
  d;10=
   |$
     [c;~[b;1+b:1_c:]?]?
     ~[0c:]?
  |
]a;1-a:b;1-b:
#"Number of words: "b;."
Number of lines: "a;."
"


