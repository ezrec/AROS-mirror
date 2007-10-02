===========================
Kort introduktion till AROS
===========================

:Authors:   Aaron Digulla, Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Copyright: Copyright © 1995-2002, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Almost finished, I think...


.. raw:: html

   <h1>Introduction<br><img style="width: 238px; height: 2px;" alt="spacer" src="/images/sidespacer.png"></h1>

AROS är ett portabelt och fritt operativsystem för desktop med syfte på
att vara kompatibelt med AmigaOS 3.1, och samtidigt förbättra det i
många områden. Källkoden är tillgänglig under en open source-licens, vilket
tillåter alla friheten att förbättra det.


Mål
===

AROS-projektets mål är att skapa ett operativsystem som:

1. Är så kompatibelt med AmigaOS 3.1 som möjligt.

2. Kan portas till olika former av hårdvaruarkitekturer och processorer,
   såsom x86, PowerPC, Alpha, Sparc, HPPA och andra.

3. Bör vara kompatibel på binär nivå när man kör den på Amiga-hårdvara
   och källkodskompatibel på annan hårdvara.
  
4. Kan köras som en fristående version som startar direkt från hårddisk eller som
   en emulerad version som öppnar ett fönster vilket tillåter att man kan köra Amiga
   och övriga applikationer samtidigt.

5. Förbättrar funktionaliteten hos AmigaOS

För att nå dessa mål använder vi ett antal olika tekniker. First och främst
använder vi Internet. Du kan deltaga i vårat projekt även om bara kan skriva
en enda funktion i operativsystemet. Den mest uppdaterade versionen av källkoden
finns tillgänglig dygnet runt och patchar kan integreras när som helst. En liten
databas med öppna jobb säkerställer att samma jobb inte utförs av flera personer
samtidigt.


Historia
========

Runt 1993 såg situationen för Amiga värre ut än någonsin och hängivna supportrar
samlades för att diskutera vad som kunde göras för att öka acceptansen för vår
älskade maskin. Ganska så snart var det uppenbart vad huvudorsaken
av misslyckandet berodde på: spridningen, eller snarare avsaknaden av detta.
Amiga borde ha en mer utbredd marknad för att bli mer attraktiv för alla att
använda och utveckla i.  Planer utvecklades för att uppnå detta mål. En av
dessa planer var att åtgärda buggarna i AmigaOS, en annan var att utveckla
AmigaOS till ett modernt operativsystem. AOS-projektet var skapat.

Men vad var egentligen en bug? Hur skulle buggarna åtgärdas? Vad är det för
finesser som ett så kallat *modernt* operativsystem måste ha? Hur skulle de
implementeras i AmigaOS?

Två år senare så var debatten fortfarande aktuell och inte en enda rad kod
hade skrivits (åtminstone så hade ingen sett någon kod). Diskussioner
handlade fortfarande om "Vi måste ha ..." då en annan svarade "läs de gamla mailen"
eller "detta är omöjligt att utföra, eftersom ..." som följdes av "Du har fel
för att ..." och så vidare.

Vintern 1995, var Aaron Digulla trött på situationen och postade en RFC 
(reguest for comments) till AOS e-postlista där han frågade vad den minimala
gemensamma grunden för operativystemet borde vara. Flera alternativ presenterades
och sammanfattningsvis så ville i princip alla se ett öppet operativsystem som
är kompatibelt med AmigaOS 3.1 (Kickstart 40.68), under vilka fortsatta diskussioner
kunde baseras, för att kontrollera vad som var möjligt och inte.

Arbetet startade och så var AROS fött.
