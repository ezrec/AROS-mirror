MODULE	'exec/types','preferences','scalos/preferences'

CONST ID=$4d41494e

PROC main()
DEF 	prefname=NIL, prefhandle:PTR TO CHAR, tag, prefstruct:PTR TO prefsstruct,
	temp[128]:STRING,preffile

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

			/* Use FindPreferences to check that the item has been stored */
			IF (prefstruct:=FindPreferences(prefhandle, ID, tag)) <> NIL
				WriteF('Preference data found\n')

				/* And to show that the data was actually stored, we get it back
				 * using GetPreferences
				 */
				GetPreferences(prefhandle, ID, tag, temp, 128)
				WriteF('Contents of preference data:\n    \s\n', temp)
			ENDIF


			/* Now we set a new value for the data */
			WriteF('Enter new string for pref (blank string to not change value) and press return:\n    ')
			ReadStr(stdout,temp)

			IF (StrLen(temp) > 0)
			
				SetPreferences(prefhandle, ID, tag, temp, StrLen(temp)+1)

				/* Save the preferences out again. Run this program again if you want to check the changes */
				WritePrefsHandle(prefhandle, preffile)
			ENDIF

			/* Finally, we free the PrefsHandle */
			FreePrefsHandle(prefhandle)
		ENDIF

		CloseLibrary(preferencesbase)
	ENDIF
ENDPROC


