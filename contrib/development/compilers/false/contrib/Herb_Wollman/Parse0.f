{ Parse separates the words in a file with Tabs }
9s:32d:ß {initialize}
[^$1_=~]     {c NotEOF}
[
 $d;=$~        {c =d ~=d}
   [%$c:$,0]?  {c =d }{store and emit}
   [$c;=$~     {c =c ~=c}
     [%$c:s;,0]? {store and emit s}
     []?         {donothing}
   ]?
%]#
