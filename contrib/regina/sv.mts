#
# Regina felmeddelanden - Svenska
# Written by Mark Hessling <M.Hessling@qut.edu.au>
#
# dutch - Gert van der Kooij <geko@wanadoo.nl>
# french - Mark <cotemark@globetrotter.net>
# german - <florian@grosse-coosmann.de>
# norwegian (bokmal) - Vidar Tysse <vtysse@broadpark.no>
# portuguese - brian <bcarpent@nucleus.com>
# russian - Oleg Kulikov <kulikov@xanadu.ru>
# spanish - Pablo Garcia-Abia <Pablo.Garcia@cern.ch>
# svenska - Jan-Erik L„rka <jan-erik@os2ug.se>
#
# Riktlinjer f”r ”vers„ttare
# --------------------------
# - Rader som b”rjar med # „r kommentarer
# - Ord skrivna med VERSALER skall inte ”vers„ttas
# - P† endel meddelanderader med fel finns m„rken †tf”ljda av tecknet |.
#   Dessa skall inte heller ”vers„ttas.
# - S„kerst„ll att ordningen p† ers„ttningar, d.v.s. %s, %d
#   bibeh†lls i ”vers„ttningen. Meddela mig om det „r grammatiskt om”jligt,
# - Det finns n†gra kommentarer i slutet av denna fil under rubriken:
#   "Extra generella fraser som beh”ver ”vers„ttas:"
#   ™vers„tt „ven dessa fraser.
# - Skicka de ”versatta meddelandena i en och samma fil, den h„r filen,
#   med namnet XX.mts d„r XX „r motsvarigheten till spr†kkoden enl. ISO.
#   Viktigt: Paketera filen f”r att s„kerst„lla att filen n†r mottagaren i dess
#            ursprungliga form
#   L„gg till en kommentar upptill av filen med spr†k och ditt namn samt e-postadress.
#   Ange om du vill att din e-postadress skall visas i dokumenatationen f”r Regina.
#   Jag beh†ller din e-postadress f”r att kunna kontakta dig i framtiden om behov uppst†r
#   av att uppdatera och l„gga till enskilda meddelanden, men publicerar endast ditt namn.
# - Till sist men p† inget s„tt minst; Tack!
#
  0,  1,Fel %s k”r %s, rad %d:|<value>,<source>,<linenumber>
  0,  2,Fel %s vid interaktiv s”kning:|<value>
  0,  3,Interaktiv s”kning. Avsluta avlusning med "Trace Off". ENTER f”r att forts„tta.
  2,  0,Problem uppstod vid avslutning
  2,  1,Problem uppstod vid avslutning: %s|<description>
  3,  0,Problem uppstod vid initiering
  3,  1,Problem uppstod vid initiering: %s|<description>
  4,  0,Program avbr”ts
  4,  1,Program avbr”ts med HALT: %s|<description>
  5,  0,Systemresurser utt”mda
  5,  1,Systemresurser utt”mda: %s|<description>
  6,  0,Ingen tr„ff p† "/*" eller kommentar
  6,  1,Ingen tr„ff p† avslutande kommentar till ("/*")
  6,  2,Ingen tr„ff p† enkelt kommentarstecken (')
  6,  3,Ingen tr„ff p† dubbelt kommentarstecken (")
  7,  0,F”rv„ntade WHEN eller OTHERWISE
  7,  1,SELECT p† rad %d skall f”ljas av WHEN; hittade "%s"|<linenumber>,<token>
  7,  2,SELECT p† rad %d skall f”ljas av WHEN, OTHERWISE eller END; hittade "%s"|<linenumber>,<token>
  7,  3,Alla WHEN-uttryck till SELECT p† rad %d ger resulatatet falskt; F”rv„ntade OTHERWISE p† rad|<linenumber>
  8,  0,Ov„ntat THEN eller ELSE
  8,  1,THEN saknar tillh”rande villkor IF eller WHEN
  8,  2,ELSE saknar tillh”rande villkor THEN
  9,  0,Ov„ntat WHEN eller OTHERWISE
  9,  1,WHEN saknar tillh”rande SELECT
  9,  2,OTHERWISE saknar tillh”rande SELECT
 10,  0,Ov„ntat END eller ingen kod som h”r ihop med END
 10,  1,END saknar tillh”rande DO eller SELECT
 10,  2,END som h”r till DO p† rad %d m†ste ha en symbol som f”ljs av en motsvarande kontrollvariabel (eller ingen symbol); hittade "%s"|<linenumber>,<token>
 10,  3,END som h”r till DO p† rad %d f†r inte f”ljas av en symbol eftersom det inte finns n†gon kontorllvariabel; Hittade "%s"|<linenumber>,<token>
 10,  4,END som h”r till SELECT p† rad %d f†r inte f”ljas av en symbol; hittade "%s"|<linenumber>,<token>
 10,  5,END f†r inte f”lja direkt p† THEN
 10,  6,END f†r inte f”lja direkt p† ELSE
 11,  0,[Kontrollstacken „r full]
 12,  0,[Frasen „r > 1024 tecken]
 13,  0,Felaktigt tecken i programmet
 13,  1,Felaktigt tecken i programmet "('%x'X)"|<hex-kodning>
 14,  0,Ofullst„ndig DO/SELECT/IF
 14,  1,DO-instruktionen m†ste †tf”ljas av ett matchande END
 14,  2,SELECT-instruktionen m†ste †tf”ljas av ett matchande END
 14,  3,THEN m†ste f”ljas av en instruktion
 14,  4,ELSE m†ste f”ljas av en instruktion
 15,  0,Ogiltig hexadecimal eller bin„r str„ng
 15,  1,Ogiltig placering av blanktecken p† plats %d i hexadecimal str„ng|<position>
 15,  2,Ogilitg placering av blanktecken p† plats %d in bin„r str„ng|<position>
 15,  3,Endast 0-9, a-f, A-F och blanktecken f†r anv„ndas i en hexadecimal str„ng; hittade "%c"|<char>
 15,  4,Endast 0, 1 och blanktecken f†r anv„ndas i bin„ra str„ngar; hittade "%c"|<char>
 16,  0,Kunde inte hitta
 16,  1,"%s" kunde inte hittas|<name>
 16,  2,Kan inte SIGNALera till "%s" d„rf”r att den finns inuti en grupp av IF, SELECT eller DO|<name>
 16,  3,Kan inte anrpoa "%s" d„rf”r att den finns inuti en grupp av IF, SELECT eller DO|<name>
 17,  0,Ov„ntad PROCEDURE
 17,  1,PROCEDURE f†r endast anv„ndas n„r den utg”r f”rsta instruktionen som k”rs efter ett internt anrop med CALL eller vid funktionsanrop
 18,  0,F”rv„ntade THEN
 18,  1,Nyckelordet IF p† rad %d m†ste f”ljas av ett matchande THEN; hittade "%s"|<linenumber>,<token>
 18,  2,Nyckelordet WHEN p† rad %d m†ste f”ljas av ett matchande THEN; hittade "%s"|<linenumber>,<token>
 19,  0,F”rv„ntade str„ng eller symbol
 19,  1,F”rv„ntade str„ng eller symbol efter nyckelordet ADDRES; hittade "%s"|<token>
 19,  2,F”rv„ntade str„ng eller symbol efter nyckelordet CALL; hittade "%s"|<token>
 19,  3,F”rv„ntade str„ng eller symbol efter nyckelordet NAME; hittade "%s"|<token>
 19,  4,F”rv„ntade str„ng eller symbol efter nyckelordet SIGNAL; hittade "%s"|<token>
 19,  6,F”rv„ntade str„ng eller symbol efter nyckelordet TRACE; hittade "%s"|<token>
 19,  7,F”rv„ntade symbol n„r m”nster tolkades; hittade "%s"|<token>
 20,  0,F”rv„ntade ett namn
 20,  1,Namn kr„vs; hittade "%s"|<token>
 20,  2,Hittade "%s" d„r endast ett namn „r giltigt|<token>
 21,  0,Felaktigt data i slutet av frasen
 21,  1,Frasen slutade vid ett ov„ntat m„rke; hittade "%s"|<token>
 22,  0,Ogiltig str„ng av tecken
 22,  1,Ogiltig str„ng av tecken '%s'X|<hex-kodning>
 23,  0,Ogiltig str„ng av data
 23,  1,Ogiltig str„ng av data '%s'X|<hex-kodning>
 24,  0,Ogiltigt anrop till TRACE
 24,  1,Tecknet till TRACE m†ste vara ett av f”ljande "%s"; hittade "%c"|ACEFILNOR,<value>
 25,  0,Ogiltigt nyckelord hittades
 25,  1,CALL ON m†ste f”ljas av n†got av nyckelorden %s; hittade "%s"|<keywords>,<token>
 25,  2,CALL OFF m†ste f”ljas av n†got av nyckelorden %s; hittade "%s"|<keywords>,<token>
 25,  3,SIGNAL ON m†ste f”ljas av n†got av nyckelorden %s; hittade "%s"|<keywords>,<token>
 25,  4,SIGNAL OFF m†ste f”ljas av n†got av nyckelorden %s; hittade "%s"|<keywords>,<token>
 25,  5,ADDRESS WITH m†ste f”ljas av n†got av nyckelorden INPUT, OUTPUT eller ERROR; hittade "%s"|<token>
 25,  6,INPUT m†ste f”ljas av n†got av nyckelorden STREAM, STEM, LIFO, FIFO eller NORMAL; hittade "%s"|<token>
 25,  7,OUTPUT must be followed by one of the keywords STREAM, STEM, LIFO, FIFO, APPEND, REPLACE or NORMAL; hittade "%s"|<token>
 25,  8,APPEND m†ste f”ljas av n†got av nyckelorden STREAM, STEM, LIFO or FIFO; hittade "%s"|<token>
 25,  9,REPLACE m†ste f”ljas av n†got av nyckelorden STREAM, STEM, LIFO or FIFO; hittade "%s"|<token>
 25, 11,NUMERIC FORM m†ste f”ljas av n†got av nyckelorden %s; hittade "%s"|<keywords>,<token>
 25, 12,PARSE m†ste f”ljas av n†got av nyckelorden %s; hittade "%s"|<keywords>,<token>
 25, 13,UPPER m†ste f”ljas av n†got av nyckelorden %s; hittade "%s"|<keywords>,<token>
 25, 14,ERROR m†ste f”ljas av n†got av nyckelorden STREAM, STEM, LIFO, FIFO, APPEND, REPLACE or NORMAL; hittade "%s"|<token>
 25, 15,NUMERIC m†ste f”ljas av n†got av nyckelorden %s; hittade "%s"|<keywords>,<token>
 25, 16,FOREVER m†ste f”ljas av n†got av nyckelorden %s; hittade "%s"|<keywords>,<token>
 25, 17,PROCEDURE m†ste f”ljas av nyckelordet EXPOSE eller ingenting; hittade "%s"|<token>
 26,  0,Felaktigt heltal
 26,  1,Storleken p† heltal m†ste vara inom omr†det f”r nuvarande inst„llning DIGITS (%d); hittade "%s"|<value>,<value>
 26,  2,V„rdet f”r upprepning av uttrycket i instruktionen DO m†ste vara noll eller ett positivt heltal; hittade "%s"|<value>
 26,  3,V„rdet av uttrycket FOR i instruktionen DO m†ste vara noll eller ett positivt heltal; hittade "%s"|<value>
 26,  4,Parametern f”r tolkning enl. mallen m†ste vara ett heltal; hittade "%s"|<value>
 26,  5,V„rdet p† NUMERIC DIGITS m†ste vara ett positivt heltal; hittade "%s"|<value>
 26,  6,V„rdet p† NUMERIC FUZZ m†ste vara noll eller ett positivt heltal; hittade "%s"|<value>
 26,  7,Numret som anv„nds i inst„llningen f”r TRACE m†ste vara ett heltal; hittade "%s"|<value>
 26,  8,Operand till h”ger om upph”jt i ("**") m†ste vara ett heltal; hittade "%s"|<value>
 26, 11,Resultet av %s %% %s skulle kr„va exponentiell notation f”r nuvarande inst„llning av NUMERIC DIGITS %d|<value>,<value>,<value>
 26, 12,Resulter av ber„kningen %% som anv„nds f”r %s // %s skulle kr„va exponentiell notation f”r nuvarande inst„llning av NUMERIC DIGITS %d|<value>,<value>,<value>
 27,  0,Ogiltig syntax f”r DO
 27,  1,Ogiltig anv„ndning av nyckelordet "%s" i villkoret DO|<token>
 28,  0,Ogiltig LEAVE eller ITERATE
 28,  1,LEAVE „r bara giltigt inom en upprepad DO-sats
 28,  2,ITERATE „r bara giltigt inom en upprepad DO-sats
 28,  3,Symbolen som kommer efter LEAVE ("%s") m†ste antingen matcha kontrollvariabeln f”r nuvarande upprepad DO-stats eller utel„mnas|<token>
 28,  4,Symbolen som kommer efter ITERATE ("%s") m†ste antingen matcha kontrollvariabeln f”r nuvarande upprepad DO-stats eller utel„mnas|<token>
 29,  0,Milj”variablens namn „r f”r l†ngt
 29,  1,Molj”variablens namn ”verstiger %d tecken; hittade "%s"|#Limit_EnvironmentName,<name>
 30,  0,Namnet eller str„ngen „r f”r l†ng
 30,  1,Namnet ”verstiger %d tecken|#Limit_Name
 30,  2,Ordagrann str„ng ”verstiger %d tecken|#Limit_Literal
 31,  0,Namnet startar med ett nummer eller "."
 31,  1,Kan inte tilldela ett v„rde till ett nummer; hittade "%s"|<token>
 31,  2,Variabel f†r inte starta med ett nummer; hittade "%s"|<token>
 31,  3,Variabel f†r inte starta med "."; hittade "%s"|<token>
 32,  0,[Felaktig anv„ndning av ordstam (stem)]
 33,  0,Ogiltigt resultat av uttryck
 33,  1,V„rdet av NUMERIC DIGITS "%d" m†ste ”verstiga v„rdet av NUMERIC FUZZ "(%d)"|<value>,<value>
 33,  2,V„rdet av NUMERIC DIGITS "%d" f†r inte ”verstiga %d|<value>,#Limit_Digits
 33,  3,Resultet av uttrycket efter NUMERIC FORM m†ste b”rja med "E" eller "S"; hittade "%s"|<value>
 34,  0,Logiskt v„rde „r inte "0" eller "1"
 34,  1,V„rdet av uttryck efter nyckelordet IF m†ste vara exakt "0" eller "1"; hittade "%s"|<value>
 34,  2,V„rdet av uttryck efter nyckelordet WHEN m†ste vara exakt "0" eller "1"; hittade "%s"|<value>
 34,  3,V„rdet av uttryck efter nyckelordet WHILE m†ste vara exakt "0" eller "1"; hittade "%s"|<value>
 34,  4,V„rdet av uttryck efter nyckelordet UNTIL m†ste vara exakt "0" eller "1"; hittade "%s"|<value>
 34,  5,V„rdet av uttrycket till v„nster om den logiska j„mf”relsen "%s" m†ste vara exakt "0" eller "1"; hittade "%s"|<operator>,<value>
 34,  6,V„rdet av uttrycket till h”ger om  den logiska j„mf”relsen "%s" m†ste vara exakt "0" eller "1"; hittade "%s"|<operator>,<value>
 35,  0,Ogiltigt uttryck
 35,  1,Ogiltigt uttryck hittades vid "%s"|<token>
 36,  0,Ingen matchning av "(" i uttrycket
 37,  0,Ov„ntat "," eller ")"
 37,  1,Ov„ntat ","
 37,  2,Ingen matchning av ")" i uttrycket
 38,  0,Ogiltig mall eller m”nster
 38,  1,Ogiltig mall vid tolkning hittades vid "%s"|<token>
 38,  2,Ogiltig position f”r tolkning vid "%s"|<token>
 38,  3,Instruktionen PARSE VALUE m†ste f”ljas av nyckelordet WITH
 39,  0,[Stacken „r ”verfull]
 40,  0,Ogiltigt anrop till rutin
 40,  1,Problem uppstod i extern rutin "%s"|<name>
 40,  3,Inte tillr„ckligt m†nga argument i anrop till "%s"; minsta antal som f”rv„ntas „r %d|<bif>,<argnumber>
 40,  4,F”r m†nga argument i anrop till "%s"; maximalt antal som f”rv„ntas „r %d|<bif>,<argnumber>
 40,  5,Saknar argument i anrop till "%s"; %d f”rv„ntas|<bif>,<argnumber>
 40,  9,%s argument %d exponent ”verstiger %d siffror; hittade "%s"|<bif>,<argnumber>,#Limit_ExponentDigits,<value>
 40, 11,%s argument %d m†ste vara ett nummer; hittade "%s"|<bif>,<argnumber>,<value>
 40, 12,%s argument %d m†ste vara ett heltal; hittade "%s"|<bif>,<argnumber>,<value>
 40, 13,%s argument %d m†ste vara noll eller positivt; hittade "%s"|<bif>,<argnumber>,<value>
 40, 14,%s argument %d m†ste vara positivt; hittade "%s"|<bif>,<argnumber>,<value>
 40, 17,%s argument 1, m†ste ha en heltalsdel i omr†det 0:90 och en decimaldel som inte „r st”rre „n .9; hittade "%s"|<bif>,<value>
 40, 18,%s omr„knat m†ste vara †r i omr†det 0001 till 9999|<bif>
 40, 19,%s argument 2, "%s", har inte formatet som beskrivs av argument 3, "%s"|<bif>,<value>,<value>
 40, 21,%s argument %d f†r inte vara tomt (null)|<bif>,<argnumber>
 40, 23,%s argument %d m†ste vara ett enkeltecken; hittade "%s"|<bif>,<argnumber>,<value>
 40, 24,%s argument 1 m†ste vara en bin„r str„ng; hittade "%s"|<bif>,<value>
 40, 25,%s argument 1 m†ste vara en hexadecimal str„ng; hittade "%s"|<bif>,<value>
 40, 26,%s argument 1 m†ste vara en giltig symbol; hittade "%s"|<bif>,<value>
 40, 27,%s argument 1, m†ste vara ett giltigt namn p† en filstr”m; hittade "%s"|<bif>,<value>
 40, 28,%s argument %d, valet m†ste starta med n†got av alternativen "%s"; hittade "%s"|<bif>,<argnumber>,<optionslist>,<value>
 40, 29,%s konverterat till formatet "%s" „r inte till†tet|<bif>,<value>
 40, 31,%s argument 1 ("%d") f†r inte ”verstiga 100000|<bif>,<value>
 40, 32,%s skillnaden mellan argument 1 ("%d") och argument 2 ("%d") f†r inte ”verstiga 100000|<bif>,<value>,<value>
 40, 33,%s argument 1 ("%d") m†ste vara mindre eller lika med argument 2 ("%d")|<bif>,<value>,<value>
 40, 34,%s argument 1 ("%d") m†ste vara mindre eller lika med antalet rader i programmet (%d)|<bif>,<value>,<sourceline()>
 40, 35,%s argument 1 kan inte uttryckas som ett heltal; hittade "%s"|<bif>,<value>
 40, 36,%s argument 1 m†ste vara ett namn p† en variabel som redan finns; hittade "%s"|<bif>,<value>
 40, 37,%s argument 3 m†ste vara ett namn som redan finns; hittade "%s"|<bif>,<value>
 40, 38,%s argument %d „r inte tillr„ckligt stor f”r att „ndra "%s"|<bif>,<argnumber>,<value>
 40, 39,%s argument 3 „r inte noll eller ett; hittade "%s"|<bif>,<value>
 40, 41,%s argument %d m†ste vara inom gr„nsen f”r filstr”mmen; hittade "%s"|<bif>,<argnumber>,<value>
 40, 42,%s argument 1; kan inte placera i denna filstr”m; hittade "%s"|<bif>,<value>
 40,914,[%s argument %d, m†ste vara en av "%s"; hittade "%s"]|<bif>,<argnumber>,<optionslist>,<value>
 40,920,[%s: L†g-niv† I/O-fel p† filstr”m; %s]|<bif>,<description>
 40,921,[%s argument %d, pekar p† placering "%s" i filstr”m; inkompatibel med filstr”m som „r ”ppen]|<bif>,<argnumber>,<value>
 40,922,[%s argument %d, f”r f† underkommandon; minimum expected is %d; hittade %d]|<bif>,<argnumber>,<value>,<value>
 40,923,[%s argument %d, f”r m†nga underkommandon; maximalt f”rv„ntat antal „r %d; hittade %d]|<bif>,<argnumber>,<value>,<value>
 40,924,[%s argument %d, ogiltig placering angiven; f”rv„ntade ett av "%s"; hittade "%s"]|<bif>,<argnumber>,<value>,<value>
 40,930,[RXQUEUE, funktion TIMEOUT, f”rv„ntade ett heltal mellan -1 och %d; hittade \"%s\"]|<value>,<value>
 40,980,Ov„ntad inmatning, antingen av ok„nd typ eller s† inneh†ller den ogiltigt data%s%s|: ,<location>
 40,981,Talet „r utanf”r till†tet omr†de%s%s|: ,<location>
 40,982,Str„ngen „r f”r stor f”r utrymme som gjorts iordning%s%s|: ,<location>
 40,983,Ogiltig kombination av typ/storlek%s%s|: ,<location>
 40,984,Saknar st”d f”r nummer av typen NAN, +INF, -INF%s%s|: ,<location>
 40,985,Strukturen „r f”r komplex f”r statiskt utrymme internt%s%s|: ,<location>
 40,986,Ett element saknas i strukturen%s%s|: ,<location>
 40,987,Ett v„rde saknas i strukturen%s%s|: ,<location>
 40,988,Namnet eller del av namn „r ogiltigt f”r tolken%s%s|: ,<location>
 40,989,Ett problem uppstod i kopplingen mellan Regina och GCI%s%s|: ,<location>
 40,990,Typen uppfyller inte kraven p† grundtyper (argument/returnerar v„rde)%s%s|: ,<location>
 40,991,Antalet arguments „r fel eller saknas%s%s|: ,<location>
 40,992,den interna stacken f”r GCI „r ”verfull%s%s|: ,<location>
 40,993,GCI r„knade f”r m†nga n„stlade LIKE-containers%s%s|: ,<location>
 41,  0,Fel vid aritmetisk ”vers„ttning
 41,  1,Ickenumeriskt v„rde ("%s") till v„nster om aritmetisk operation "%s"|<value>,<operator>
 41,  2,Ickenumeriskt v„rde ("%s") till h”ger om aritmetisk operation "%s"|<value>,<operator>
 41,  3,Ickenumeriskt v„rde ("%s") anv„nds med prefix-operator "%s"|<value>,<operator>
 41,  4,V„rdet av TO-uttrycket i en DO-stats m†ste vara numeriskt; hittade "%s"|<value>
 41,  5,V„rdet av BY-uttrycket i en DO-stats m†ste vara numersikt; hittade "%s"|<value>
 41,  6,V„rdet av uttrycket f”r kontrollvariabeln f”r DO-statsen m†ste vara numeriskt; hittade "%s"|<value>
 41,  7,Exponenten ”verstiger %d siffror; hittade "%s"|#Limit_ExponentDigits,<value>
 42,  0,F”r omfattande/komplicerad aritmetisk ber„kning
 42,  1,F”r omfattande aritmetisk ber„kning uppt„cktes vid "%s %s %s"; exponenten f”r resultatet kr„ver mer „n %d siffror|<value>,<operator>,<value>,#Limit_ExponentDigits
 42,  2,Aritmetisk komplicerad ber„kning uppt„cktes vid "%s %s %s"; exponenten f”r resultatet kr„ver mer „n %d siffror|<value>,<operator>,<value>,#Limit_ExponentDigits
 42,  3,Aritmetisk ber„kning inte m”jlig; division f†r inte g”ras med noll
 43,  0,Rutinen hittades inte
 43,  1,Kunde inte hitta rutinen "%s"|<name>
 44,  0,Funktionen returnerade inget data
 44,  1,Inget data returnerades fr†n funktionen "%s"|<name>
 45,  0,Inget data har angetts i funktionens RETURN-stats
 45,  1,Returv„rde f”rv„ntades i instruktionen f”r RETURN eftersom rutinen "%s" anropades som en funktion|<name>
 46,  0,Ogiltig variabelreferens
 46,  1,Extra m„rke ("%s") hittades i variabelreferens; ")" f”rv„ntades|<token>
 47,  0,Ov„ntad ben„mning
 47,  1,INTERPRET f”r data f†r inte inneh†lla ben„mningar; hittade "%s"|<name>
 48,  0,Systemtj„nst har slutat fungera
 48,  1,Fel i systemtj„nst: %s|<description>
 48,920, L†g-niv† I/O-fel p† filstr”m: %s %s: %s|<description>,<stream>,<description>
 49,  0,Tolkningsfel
 49,  1,Tolkningsfel: Fel i %s, rad %d: "%s". Var god rapportera felet!|<module>,<linenumber>,<description>
 50,  0,Ok„nd reserverad symbol
 50,  1,Ok„nd reserverad symbol "%s"|<token>
 51,  0,Ogiltigt funktionsnamn
 51,  1,Okommenterat funktionsnamn f†r inte sluta med punkt; hittade "%s"|<token>
 52,  0,Resultet som returnerades "%s" „r l„ngre „n %d tecken|<name>,#Limit_String
 53,  0,Felaktigt val
 53,  1,Str„ng eller symbol f”rv„ntades efter nyckelordet STREAM; hittade "%s"|<token>
 53,  2,Variabelreferens f”r„ntades efter nyckelordet STEM; hittade "%s"|<token>
 53,  3,Argument till STEM m†ste ha en punkt som sista tecken; hittade "%s"|<name>
 53,100,Str„ng eller symbol f”rv„ntades efter nyckelordet LIFO; hittade "%s"|<token>
 53,101,Str„ng eller symbol f”rv„ntades efter nyckelordet FIFO; hittade "%s"|<token>
 54,  0,Ogiltigt v„rde p† STEM
 54,  1,F”r detta STEM APPEND m†ste v„rdet av "%s" utg”ra antalet rader; hittade "%s"|<name>,<value>
#
# Alla felmeddelanden nedanf”r „r inte definierade av ANSI
#
 60,  0,[Kan inte †terg† till b”rjan av den tillf„lliga filen]
 61,  0,[Felaktig metod f”r s”kning i fil]
 64,  0,[Syntaktiskt fel vid inl„sning]
 64,  1,[Syntaktiskt fel p† rad %d]
 64,  2,[Generellt syntaktiskt fel p† rad %d, kolumn %d]|<linenumber>,<columnnumber>
 90,  0,[Icke-ANSI-karakt„ristisk anv„ndning med "OPTIONS STRICT_ANSI"]
 90,  1,[%s „r en ut”kning enl. Regina BIF]|<bif>
 90,  2,[%s „r en ut”kad instruktion enl. Regina]|<token>
 90,  3,[%s argument %d, valet m†ste b”rja med ett av "%s" n„r "OPTIONS STRICT_ANSI" anv„nds; hittade "%s"; a Regina extension]|<bif>,<argnumber>,<optionslist>,<value>
 93,  0,[Ogilitgt anrop till rutin]
 93,  1,[Kommandot STREAM %s m†ste f”ljas av ett av "%s"; hittade "%s"]|<token>,<value>,<value>
 93,  3,[Kommandot STREAM m†ste vara ett av "%s"; hittade "%s"]|<value>,<value>
 94,  0,[Fel i extern k” vid ”verf”ring]
 94,  1,[External queue timed out]
 94, 99,[Internt fel i extern k” vid ”verf”ring: %d "%s"]|<description>,<systemerror>
 94,100,[Generellt systemfel i extern k” vid ”verf”ring. %s. %s]|<description>,<systemerror>
 94,101,[Fel vid uppkoppling mot %s p† port %d: "%s"]|<machine>,<portnumber>,<systemerror>
 94,102,[Kan inte utl„sa IP-adress f”r %s]|<machine>
 94,103,[Ogiltigt format p† servernamn har angivits f”r k”: "%s"]|<queuename>
 94,104,[Ogiltigt format p† namn f”r k”: "%s"]|<queuename>
 94,105,[Kan inte starta ”verf”ring med Windows Socket: %s]|<systemerror>
 94,106,[Maximalt antal externa k”er har ”verskridits: %d]|<maxqueues>
 94,107,[Fel uppstod vid l„sning av socket: %s]|<systemerror>
 94,108,[Ogiltig v„xel angiven. M†ste vara en av "%s"]|<switch>
 94,109,[Kunde inte hitta k”n \"%s\"]|<queuename>
 94,110,[%s „r ogiltig f”r externa k”er]|<bif>
 94,111,[Funktionen %s f”r RXQUEUE kan inte anv„ndas med interna k”er]|<functionname>
 95,  0,[Begr„nsad anv„ndning i "s„kerhetsl„ge"]
 95,  1,[%s ogiltig i "s„kerhetsl„ge"]|<token>
 95,  2,[%s argument %d ogiltigt i "s„kerhetsl„ge"]|<bif>,<argnumber>
 95,  3,[%s argument %d: "%s", ogiltigt i "s„kerhetsl„ge"]|<bif>,<argnumber>,<token>
 95,  4,[STREAM argument 3: ™ppna fil f”r skriv†tkomst med WRITE „r inte till†tet i "s„kehetsl„ge"]
 95,  5,[Det „r inte till†tet att k”ra externa kommandon i "s„kerhetsl„ge"]
100,  0,[Ok„nt filsystemfel]
#
# Extra generella fraser som beh”ver ”vers„ttas:
# Texten inom () motsvarar namnen i error.c
#
# (err1prefix)   "Fel %d under k”rning av \"%.*s\" p† rad %d: %.*s",
# (suberrprefix) "Fel %d.%d: %.*s",
# (err2prefix)   "Fel %d under k”rning av \"%.*s\": %.*s",
# (erropen)      "Kan inte ”ppna spr†kfilen: %s",
# (errcount)     "Felaktigt antal meddelanden i spr†kfilen: %s",
# (errread)      "Kan inte l„sa fr†n spr†kfilen: %s",
# (errmissing)   "Text saknas i spr†kfilen: %s.mtb",
# (errcorrupt)   "Spr†kfilen: %s.mtb „r skadad",
