{ Dump prints Decimal and ASCII values of a file, written in FALSE for the Amiga }
[$9>~[32,]?$99>~[32,]?.32,]w:{print a number in 3 columns plus space }
[$31>~[%46]?,]x:{print char or .}
b62+v:{ 16 chars per line}
ßbz:[^$$1_=~][z;$y::z;4+$z:v;>[bz:[v;z;>][z;$;w;!4+z:]#bz:[v;z;>][z;$;x;!4+z:]#bz:10,]?]#
bz:[y;z;>][z;$;w;!4+z:]#[v;z;>]["    "z;4+z:]#bz:[y;z;>][z;$;x;!4+z:]#bz:10,
