===========================
Kort introduktion till AROS
===========================

:Authors:   Aaron Digulla, Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Copyright: Copyright © 1995-2009, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Almost finished, I think...


.. Include:: index-abstract.sv


Mål
===

AROS-projektets mål är att skapa ett operativsystem som:

1. Är så kompatibelt med AmigaOS 3.1 som möjligt.

2. Kan portas till olika former av hårdvaruarkitekturer och processorer, såsom
   x86, PowerPC, Alpha, Sparc, HPPA och andra.

3. Vara kompatibel på binär nivå när man kör den på Amiga-hårdvara och
   källkodskompatibel på annan hårdvara.
  
4. Kan köras som en fristående version som startar direkt från hårddisk eller
   som en emulerad version som öppnar ett fönster, vilket tillåter att man kan
   köra Amiga och övriga applikationer samtidigt.

5. Förbättrar funktionaliteten hos AmigaOS.

För att nå dessa mål använder vi ett antal olika arbetssätt. Allra främst
använder vi oss av Internet. Du kan vara med i vårat projekt även om bara kan
skriva en enda funktion i operativsystemet. Den mest uppdaterade versionen av
källkoden finns tillgänglig dygnet runt och patchar kan integreras när som
helst. En liten databas med öppna jobb säkerställer att samma jobb inte utförs
av flera personer samtidigt.


Historia
========

Runt 1993 såg situationen för Amiga värre ut än någonsin och hängivna
supportrar samlades för att diskutera vad som kunde göras för att rädda vår
älskade maskin. Vissa ansåg att det var viktigt att Amigan blev mer accepterad
i vidare kretsar och att dess framgångslöshet berodde på dess begränsade
spridning. Planer utvecklades för att uppnå detta mål. En av dessa planer var
att åtgärda buggarna i AmigaOS, en annan var att utveckla AmigaOS till ett
modernt operativsystem. Dessa var några av anledningarna till att
AROS-projektet startades.

När Amigan gick i graven verkade det vettigast att köpa ut AmigaOS källkod.
Tills dess fokuserade man på vad som faktiskt behövde göras. Som vad som
egentligen skulle räknas som buggar och vad var det bästa sättet att fixa dem?
Vad ska ett så kallat modernt OS ha för funktioner? Och hur skulle de
implementeras i OSet?

Två år senare så var debatten fortfarande inte avslutad och eftersom källkoden
fortfarande inte var tillgänglig så hade inte en enda rad kod skapats.
Diskussionerna tenderade att upprepas och urarta till bråk kring vad som var
och inte var genomförbart.

Vintern 1995 tröttnade Aaron Digulla på situationen och postade en RFC
("Request for comments") till AOS e-postlista och frågade vad som var det
minsta gemensamma kravet på OS:et skulle kunna vara. Flera alternativ
presenterades och slutsatsen blev att nästan alla ville ha ett öppet
operativsystem som var kompatibelt med AmigaOS 3.1 (Kickstart 40.68).
Alla fortsatta diskussioner om vad som kunde göras var nu baserat på detta mål
i åtanke.

Arbetet med AROS tog sin början.
