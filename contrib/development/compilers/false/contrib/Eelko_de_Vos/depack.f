{ depack.f }
{remove first until carriage return (redirection gives this shit) }
[^l:]
[l;10=~]#
{is first char a packcode?}
 l;'#=$
 [
{yes...}
  ^l:^b:[b;0>][l;,b;1-b:]#
 ]
 ?
 ~
 [
  l;,
 ]
 ?
{possible depacking first char done.
 now the rest.}
[^$a:1_=~]
[a;'#=$
 [
  ^a:^b:[b;0>][a;,b;1-b:]#
 ]
 ?
 ~
 [
  a;,
 ]
 ?
]
#
