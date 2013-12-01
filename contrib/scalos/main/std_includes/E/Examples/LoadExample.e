MODULE	'exec/types','preferences','scalos/preferences'

CONST ID=$4d41494e,TEXT=$54455854

PROC main()
DEF 	prefname=NIL, prefhandle:PTR TO CHAR, tag,prefstruct:PTR TO prefsstruct,temp[128]:STRING,preffile,en=0

preffile:='example.prefs'



	IF (preferencesbase:=OpenLibrary('preferences.library',39)) <> NIL 
	
		/* First we need to allocate a PrefsHandle by name */
		IF (prefhandle:= AllocPrefsHandle(prefname)) <> NIL
		
			WriteF('PrefsHandle successfully allocated\n')

			/* Try to load preferences from file */
			ReadPrefsHandle(prefhandle, preffile)

			/* We need an ID and Tag value to refer to a specific preference.
			 * so we create that here (you could always just put them in the library calls).
			 * Remember that the ID must be created from 4 ASCII characters and the Tag
			 * cannot be 0
			 */

			tag:=1
			en:=0
			/* Use FindPreferences to check that the item has been stored */
			IF (prefstruct:=FindPreferences(prefhandle, ID, tag)) <> NIL
				WriteF('Preference data found\n')

				/* And to show that the data was actually stored, we get it back
				 * using GetPreferences
				 */
				GetPreferences(prefhandle, ID, tag, temp, 128)
				WriteF('ID: MAIN - Contents of preference data:\n\t\s\n', temp)
			ENDIF

			WHILE (GetEntry(prefhandle, TEXT, tag, temp, 128, en) >0)
				-> GetPreferences(prefhandle, TEXT, tag, temp, 128)
				WriteF('ID TEXT[\d]: Contents of preference data:\n    \s\n', en,temp)
				en++
			ENDWHILE

			/* Finally, we free the PrefsHandle */
			FreePrefsHandle(prefhandle)
		ENDIF

		CloseLibrary(preferencesbase)
	ENDIF
ENDPROC


