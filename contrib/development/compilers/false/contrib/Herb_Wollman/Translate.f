{ Translate bbb ccc <file   Replaces character b with c }
{ To replace all As with Bs,   Translate 065 066 <file }
^48-10*^48-+10*^48-+b:^%^48-10*^48-+10*^48-+c:
ß[^$$1_=~][b;=[%c;]?,]#
