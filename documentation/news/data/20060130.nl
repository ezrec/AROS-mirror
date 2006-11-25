=============
Status update
=============

:Author:   Paolo Besser
:Date:     2006-01-30

Dit is de eerste status update van 2006. Om te beginnen willen we alle
AROS gebruikers, sponsors en enthousiastelingen bedanken voor hun steun
en inzet! Aan het begin van 2005 had `AROS-Exec`__ ongeveer 250 leden,
dat is inmiddels verdubbeld naar meer dan 500 (wat nog altijd groeit).
Gedurende deze maanden heeft AROS enkele hoofddoelen bereikt zoals
netwerk ondersteuning, CVS naar Subversion migratie en een PPC port.
Meer en meer applicaties zijn geport of toegevoegd en veel bugs zijn 
verholpen. Ook is er nu een AROS smaak die boven op het klassieke 
AmigaOS draait, in een poging deze te moderniseren. We zijn er in ieder geval erg
zeker van dat 2006 een zeer interessant jaar gaat worden voor AROS. Maar
genoeg nu, tijd voor het laatste nieuws...

Georg Steger heeft aanpassingen gemaakt aan de input instellingen voor de
muis, wat de muis tracking en aanwijzer precisie heeft verbeterd.

Matthias Rustler heeft de documentatie bijgewerkt voor gebruikers en 
ontwikkelaars, waarbij handige tips zijn toegevoegd en oude overbodige 
informatie is weggehaald. U zult de resultaten van zijn werk overal op
de website vinden. Matthias heeft ook ExecuteStartup ingestuurd, een 
kleine tool die applicaties start die zich in de SYS:WBstartup map bevinden.

__ http://aros-exec.org

Netwerk voorzieningen
---------------------

Dankzij Nick Andrews en Jack Patton heeft AROS nu ook IRC en Telnet 
cliënten. Om het netwerken op de hosted versie van AROS mogelijk te 
maken, heeft Michal Schulz een .tap interface hidd ingebouwd voor 
alle Unix doelen. 