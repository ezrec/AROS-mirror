/* $VER: mts2ct 1.1 $
Convert mts to Amiga catalog translation
© Stefan Haubenthal 2012

non Amiga:
foo.mts -> foo.mtb

Amiga:
en.mts  -> Regina.cd
foo.mts -> foo.ct -> foo.catalog

rx mts2ct <en.mts >T:Regina.cd
rx mts2ct <de.mts >T:deutsch.ct deutsch
rx mts2ct <sv.mts >T:svenska.ct svenska
rx mts2ct <tr.mts >T:türkçe.ct türkçe
rx mts2ct <pl.mts >T:polski.ct polski
rx mts2ct <pt.mts >T:português.ct português
rx mts2ct <no.mts >T:norsk.ct norsk
rx mts2ct <es.mts >T:español.ct español

catcomp Regina.cd CFILE locale.c
makedir Catalogs/deutsch ALL
catcomp Regina.cd deutsch.ct CATALOG Catalogs/deutsch/Regina.catalog
*/
if arg() then do
	say "## version $VER: Regina.catalog 3.5 ("translate(date(E),".","/")")"
	if arg(1)="polski" then
		codeset=5
	else
		codeset=0
	say "## codeset "codeset
	say "## language "arg(1)
end
do forever
	parse pull line
	if line="" then leave
	if abbrev(line, #) then say overlay(";", line)
	else do
		parse var line major","minor","line
		if arg() then
			say "MSG_"strip(major)"_"strip(minor)
		else
			say "MSG_"strip(major)"_"strip(minor)" (//)"
		say line
		say ";"
	end
end
