
	How to translate the MUIBuilder catalog into your
	preferred language ?

	Very easy !!!

	Just rename 'MUIBuilder.ct' to 'XXXXXX.ct' ( XXXXXX is your
	language name ).

	Then translate the English sentences to the new ones.

	You just have to know, how the hotkeys are coded :

		objects shorcuts : _This is a message
		here : t is the hot-key of 'This is a message' !

		Menus shortcuts : Q\00Quit
		here : Q is the shortcut of 'Quit' !

	When the translation is finished, you just have to build
	the catalog file using Catcomp or Flexcat ( for example ):

	With Catcomp, just type in a shell :

	> Catcomp MUIBuilder.cd XXXXXXXXX.ct CATALOG MUIBuilder.catalog
	> copy MUIBuilder.catalog locale:catalogs/XXXXXXXX

	With Flexcat :

	> Flexcat MUIBuilder.cd XXXXXXXXX.ct CATALOG MUIBuilder.catalog
	> copy MUIBuilder.catalog locale:catalogs/XXXXXXXX

	The most difficult task in this translation is to pay attention
	to the hotkeys : it's very easy to have twice the same shortcut
	in a window ...

	If you want to make a translation in your language, and succeed in this
	task .... PLEASE .... Send it to marust@users.sourceforge.net !!!
    ... Thanks ...

    As MUIBuilder is now Open Source it would be nice to get the
    *.ct files.
