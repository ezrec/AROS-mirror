/* rxtest4.rexx */

OPTIONS RESULTS

   IF ~SHOW(LIBRARIES,'rexxtricks.library') THEN
      IF ~ADDLIB('rexxtricks.library',9,-30,38) THEN
         quit('Cannot open rexxtricks.library!',10)

   IF ~SHOW('LIBRARIES','rexxreqtools.library') THEN
      IF ~ADDLIB('rexxreqtools.library',9,-30,0) THEN
         quit('Cannot open rexxreqtools.library!',10)

   IF ~SHOW('LIBRARIES','rexxdossupport.library') THEN
      IF ~ADDLIB('rexxdossupport.library',9,-30,0) THEN
         quit('Cannot open rexxdossupport.library!',10)

erg = rtezrequest("Test Request?","_Yes|_No","RxTest4.rexx",)
say "Result was " || erg
