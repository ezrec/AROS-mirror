{chkbrack.f}
{checks brackets}
0a:0b:0c:
[^$1_=~]
[$'(=
 [a;1+a:]?
 $')=
 [a;1-a:]?
 $'[=
 [b;1+b:]?
 $']=
 [b;1-b:]?
 $'{=
 [c;1+c:]?
 $'}=
 [c;1-c:]?
]
#"#of () "
a;0=~$~["is ok.
"]?["NOT ok !!!
"]?"#of [] "
b;0=~$~["is ok.
"]?["NOT ok !!!
"]?"#of {} "
c;0=~$~["is ok.
"]?["NOT OK !!!
"]?


