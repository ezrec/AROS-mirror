{Depacker, by E. de Vos}
[^$10=~][]#
^a:
[1_a;=~]   {while a<>EOF}
[169 a;=$~
 [	{if char!=escapecode}
   a;,
 ]?
 [	{if char=escapecode}
   ^c:	{read counter}
   ^a:	{read char}
   [0c;=~]  {while counter!=0}
   [
     a;,	{write the character}
     c;1-c:	{decrement counter}
   ]#
 ]?
 ^a:	{ a is read }
]#
