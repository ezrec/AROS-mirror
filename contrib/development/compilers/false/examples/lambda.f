{ lambda calculus in FALSE
  [] blocks are lambda functions already, so it isn't all that hard... }

[\%]a:		{ true }
[%]b:		{ false }
[a;b;@!]d:	{ not }
[b;@@\!]e:	{ and }
[\a;\!]f:	{ or }
[$d;!\@!]g:	{ xnor }

[$a;=["true"10,]?b;=["false"10,]?]z:	{ print true/false for funcs a/b }

"not testcase:
"
a;d;!z;!
b;d;!z;!

"and testcase:
"
a;a;e;!z;!		{ this would be: print(and(true,true)) }
a;b;e;!z;!
b;a;e;!z;!
b;b;e;!z;!

"or testcase:
"
a;a;f;!z;!
a;b;f;!z;!
b;a;f;!z;!
b;b;f;!z;!

"xnor testcase:
"			{ equality }
a;a;g;!z;!
a;b;g;!z;!
b;a;g;!z;!
b;b;g;!z;!
