{ pack.f }
{define packfunction}
[ [ {a;=last or "#"}
    a;l;=$
    [
      1b;+b:
    ]
    ?
    ~
    [
      b;1>l;'#=|$
      [{ next char was not the same, so just print it }
	"#"l;,b;1+,0p:a;l:0b:
      ]?
      ~
      [
	l;,l;,0p:a;l:0b:
      ]?
    ]
    ?
  ]
  ?
  ~
  [
    a;l;=a;'#=|$
    [{ same as previous! remember this one! }
      1b;+b:1_p:
	a;'#=[l;,'#l:0b:]?
    ]
    ?
    ~
    [{ print previous, remember this current one.}
      l;,a;l:0b:0p:
    ]
    ?
  ]
  ?
]f:
{remove first carriage return (because of redirection)}
[^l:]
[l;10=~]#
{ok. lets pack. l=last char}
0p: {p=packboolean ; b=counter ; a=char_read}
[^$a:1_=~]
[p;$
 f;!
]
#
p;f;!


