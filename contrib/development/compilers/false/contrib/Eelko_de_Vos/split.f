{split.f}
 ^a:[a;32=][^a:]#       { read operand for split: front/back }

 a;'f=a;'b=|^32=&$      { a has to be 'f' of 'b' followed by a space }
 [
  { read number in b }
   0b:^
     [c:'0 c;>~'91+c;>&]
     [b;10*b:c;'0-b;+b:^]
     #
  { number read in b }
  0c:
  a;'f=
  [ [^$d:1_=~c;b;=~&][c;1+c:d;,]#
  ]?
  a;'b=
  [ [^$d:1_=~c;b;=~][c;1+c:]#
    [^$d:1_=~][d;,]#
  ]?
 ]?
 ~[     {not right syntax }
   "split: Wrong arguments.
    SYNTAX:   Split f|b number <filename_in >filename_out
              f|b stands for what to put in filename_out: Front or Back
              number stands for where to split the file.
" ]?


