{
Do ASCII, Types the ASCII character for a value.
$VER: DoASCII 1.21 (31.03.95)
Peter Bengtsson <a94petbe@ida.his.se>
}
a;4-$1+;255&$10=["
[32mDoASC v1.21 By Peter Bengtsson[0m
This program will print the ASCII character
for a decimal value.

Syntax: DoASC <value1> <value2> ...
"]?[\1+\10=~][0b:[$;255&$$10=\32=|~][48-b;10*+b:1+]#b;$0=~[$." = "]?[]%,"
"]#
