{Packer, by E. de Vos}
[^$10=~][]#
^a:	{ a is char-read }
1c:	{ counter:=1}
[1_a;=~]   {while a<>EOF}
[^	    {read nextchar}
 n:	      {n = nextchar}
 n;a;=$~      {cmp (char,nextchar)}
 [		{if nextchar!=char}
  c;3>$~	  {if counter>=3}
  [		  {if counter<3}
    a; ,	    {write char}
    2 c;=	    {if counter=2 then write char}
    [a;,]?
    1c:		  {counter:=1}
    n;a:
  ]?
  [169 ,	    {write escapecode}
   c; ,		    {write amount}
   a; ,		    {write char}
   n;a:		    {a:=nextchar}
   1c:
  ]?
 ]?
 [		{if nextchar=char}
   1 c; + c:	  {increment counter}
 ]?
]#
