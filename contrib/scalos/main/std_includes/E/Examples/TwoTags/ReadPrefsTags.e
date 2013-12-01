MODULE	'exec/types','preferences','scalos/preferences'

CONST ID=$54455354,ID_DOCS=$444f4353

/* ID name = TEST
   ID_DOCS name = DOCS
   Tags stored inside "ExampleTags.prefs" file
*/

PROC main()
DEF 	prefname=NIL, prefhandle:PTR TO CHAR, tag,temp[128]:STRING,preffile,en,prefstruct:PTR TO prefsstruct

preffile:='ExampleTags.prefs'



	IF (preferencesbase:=OpenLibrary('preferences.library',39)) <> NIL 
	
		/* First we need to allocate a PrefsHandle by name */
		IF (prefhandle:= AllocPrefsHandle(prefname)) <> NIL
		
			WriteF('PrefsHandle successfully allocated\n')

			/* Try to load preferences from file */
			ReadPrefsHandle(prefhandle, preffile)
			tag:=1

			/* Use FindPreferences to check that the item has been stored */
			IF (prefstruct:=FindPreferences(prefhandle, ID, tag)) <> NIL
				WriteF('Preference data found\n')

				/* And to show that the data was actually stored, we get it back
				 * using GetPreferences
				 */
				GetPreferences(prefhandle, ID, tag, temp, 128)
				WriteF('ID: TEST Tag\d -  Contents of preference data:\n\t\s\n', tag,temp)
			ENDIF


			/* We need an ID and Tag value to refer to a specific preference.
			 * so we create that here (you could always just put them in the library calls).
			 * Remember that the ID must be created from 4 ASCII characters and the Tag
			 * cannot be 0
			 */

			tag:=2 /* tag entry: ID_DOCS */
			en:=0

			/* Use GetEntry to check all items stored in tag 2 */

			WHILE (GetEntry(prefhandle, ID_DOCS, tag, temp, 128, en) >0)
				WriteF('ID: DOCS - Tag\d[\d]: \s\n', tag,en,temp)
				en++
			ENDWHILE


			/* Finally, we free the PrefsHandle */
			FreePrefsHandle(prefhandle)
		ENDIF

		CloseLibrary(preferencesbase)
	ENDIF
ENDPROC

