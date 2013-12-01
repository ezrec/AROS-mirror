MODULE	'exec/types','preferences','scalos/preferences'

CONST ID=$54455354,ID_DOCS=$444f4353

/* ID name = TEST
   ID_DOCS name = DOCS
   Tags to write inside "ExampleOneTag.prefs" file

*/
PROC main()
DEF 	prefname=NIL, prefhandle:PTR TO CHAR, tag,temp[128]:STRING,preffile,en

preffile:='ExampleOneTag.prefs'



	IF (preferencesbase:=OpenLibrary('preferences.library',39)) <> NIL 
	
		/* First we need to allocate a PrefsHandle by name */
		IF (prefhandle:= AllocPrefsHandle(prefname)) <> NIL
		
			WriteF('PrefsHandle successfully allocated\n')

			/* We need an ID and Tag value to refer to a specific preference.
			 * so we create that here (you could always just put them in the library calls).
			 * Remember that the ID must be created from 4 ASCII characters and the Tag
			 * cannot be 0
			 */
			tag:=1
			en:=0
			/* Preference tag1 and ID = TEST */
			temp:='This is contents of preferences data stored with SetPreferences() for                                     ID: TEST)'
			SetPreferences(prefhandle, ID, tag, temp, StrLen(temp)+1)

			/* Now, add some entries in with ID: DOCS */
			
			temp:='preferences.library/--background--\tpreferences.library/--background--'
			SetEntry(prefhandle, ID_DOCS, tag, temp, StrLen(temp)+1, en++)
			WriteF('SetEntry: Tag\d[\d]\n\t\s\n',tag,en,temp)

			temp:='PURPOSE:\tThe preferences.library provides a convenient way to store the'
			SetEntry(prefhandle, ID_DOCS, tag, temp, StrLen(temp)+1, en++)
			WriteF('SetEntry: Tag\d[\d]\n\t\s\n',tag,en,temp)

			temp:='\tpreferences for your program. All internal management and I/O of your'
			SetEntry(prefhandle, ID_DOCS, tag, temp, StrLen(temp)+1, en++)
			WriteF('SetEntry: Tag\d[\d]\n\t\s\n',tag,en,temp)

			temp:='\tdata is handled by the library, and is controlled via a simple interface'
			SetEntry(prefhandle, ID_DOCS, tag, temp, StrLen(temp)+1, en++)
			WriteF('SetEntry: Tag\d[\d]\n\t\s\n',tag,en,temp)


			temp:='\twhich makes use of identifiers and tags to access the data. Multiple'
			SetEntry(prefhandle, ID_DOCS, tag, temp, StrLen(temp)+1, en++)
			WriteF('SetEntry: Tag\d[\d]\n\t\s\n',tag,en,temp)


			temp:='\tprograms can access the data (held only once in memory) at the same time'
			SetEntry(prefhandle, ID_DOCS, tag, temp, StrLen(temp)+1, en++)
			WriteF('SetEntry: Tag\d[\d]\n\t\s\n',tag,en,temp)

			temp:='\tas access is arbitrated through the use of semaphores.'
			SetEntry(prefhandle, ID_DOCS, tag, temp, StrLen(temp)+1, en++)
			WriteF('SetEntry: Tag\d[\d]\n\t\s\n',tag,en,temp)

			/* Save the preferences */
			WritePrefsHandle(prefhandle, preffile)

			/* Finally, we free the PrefsHandle */
			FreePrefsHandle(prefhandle)
		ENDIF

		CloseLibrary(preferencesbase)
	ENDIF
ENDPROC


