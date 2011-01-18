/******************************************************************************

    MODUL
	menu_dme.c

    DESCRIPTION
	[X]DME enhancements to menubase.c

    NOTES
	that is only a fast hack derived from the original
	menu.c, that supports one menu on all Windows

	es ist beabsichtigt, in zukunft nur noch 1 fenster
	mit menues zuzulassen aber dafuer museen erst einmal alle
	windowswitches im [X]DME ueber ene zentrale fktion laufen,
	die dann menu_strip on/off waehlt

    BUGS

    TODO
	0 make the module compilable
	1 rewrite fixmenu for subs
	2 I intend to use a Menu only for the actual Window
	3 We must support multiple Menues
	4 we need another MenuToMacro without that damned MENUSTRIP for access from vars
	5 use menubase.fix_items (not ready yet)


    EXAMPLES

    SEE ALSO

    INDEX

******************************************************************************/

/**************************************
		Includes
**************************************/
#define  MENU_INTERNAL		    /* PATCH_NULL < 24-06-94 */
#include "defs.h"
#include "menubase.h"

#include <libraries/gadtools.h>
#include <proto/gadtools.h>

/**************************************
	    Globale Variable
**************************************/
Prototype void	menuon	     (MENUSTRIP * ms, struct Window * win);
Prototype void	menuoff      (MENUSTRIP * ms, struct Window * win);
Prototype void	menu_strip   (MENUSTRIP * ms, struct Window * Win);
Prototype void	menuload     (MENUSTRIP * ms,  char * fname);
Prototype void	menusave     (MENUSTRIP * ms,  char * fname);
Prototype void	fixmenu      (MENUSTRIP * ms, struct Window * win);

Prototype void* currentmenu  (void);
Prototype void* currentwindow(void);

/**************************************
      Interne Defines & Strukturen
**************************************/


/**************************************
	    Interne Variable
**************************************/


/**************************************
	   Interne Prototypes
**************************************/


/*****************************************************************************

    NAME
	menuon

    PARAMETER
	MENUSTRIP	* ms
	struct Window	* win

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	activates the menu definitions of a program; each
	call to menuon eliminates a PRIOR call to menuoff.

    NOTES
	Window is not dummy-parameter any more

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS
	Win war ein Dummy, der schon einmal eingebaut wurde,
	da ich beabsichtige, nur noch fuer das aktuelle Fenster
	den Menubar zuzulassen;
	das heisst unter anderem, dass ALLE fenster-manipulationen
	eine spezielle Function aufrufen muessen, die dafuer sogrt,
	dass von einem alten Fenster die Menuleiste abgetrennt wird
	(dafuer bietet sich menu_strip an)

	zur zeit verwenden wir als default-menustrip jedesmal get_menustip(NULL);
	dies ist zu beruecksichtigen

    HISTORY
	20 Dec 1992 b_null  created

******************************************************************************/

void menuon (MENUSTRIP * ms, struct Window * win)
{
    ED * ed;
    int  def_too;

    if (ms) {
	def_too = (get_menustrip(NULL) == ms);
	if (ms->data && ms->offCount == 1) {
	    fixmenu (ms, win); /* !!! we use win here !!! */

	    /* attach menues */
	    for (ed = (ED *)GetHead (&DBase); ed; ed = (ED *)GetSucc ((struct Node *)ed)) {
	     /* if (ed->Package == Package)    *//* <-PATCH_PACK */
		if ((ed->menustrip == ms) || (def_too && (ed->menustrip == NULL)))    /* <- PATCH_NEW */
		{
		    if (!GETF_ICONMODE(ed)) {
			SetMenuStrip (ed->win, ms->data);

			Forbid();
			ed->win->Flags &= ~WFLG_RMBTRAP;
			Permit();
		    } /* if */
		} /* if */
	    } /* for all texts */
	} /* if to be displayed */

	if (ms->offCount>0) {
	    ms->offCount --;
	} else {
	    ms->offCount = 0;
	} /* if menues are off */
    } /* if sensefully called */
} /* menuon */


/*****************************************************************************

    NAME
	menuoff

    PARAMETER
	MENUSTRIP	* ms
	struct Window	* win

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	inactivates the menudefinitions of a program
	menuoff is stackable that means
	calling menuoff twice needs also two calls to menon
	to be erased

    NOTES
	Window is not dummy-parameter any more

    BUGS

    EXAMPLES
	state a menuoff state b menuon	-> state a
	state a menuon	state a menuoff -> state a

    SEE ALSO

    INTERNALS
	Win war ein Dummy, der schon einmal eingebaut wurde,
	da ich beabsichtige, nur noch fuer das aktuelle Fenster
	den Menubar zuzulassen;
	das heisst unter anderem, dass ALLE fenster-manipulationen
	eine spezialle Function aufrufen muessen, die dafuer sogrt,
	dass von einem alten Fenster die Menuleiste abgetrennt wird
	(dafuer bietet sich menu_strip an)

	zur zeit verwenden wir als default-menustrip jedesmal get_menustip(NULL);
	dies ist zu beruecksichtigen

    HISTORY
	20 Dec 1992 b_null  created

******************************************************************************/

void menuoff (MENUSTRIP * ms, struct Window * win)
{
    ED * ed;
    int  def_too;

    if (ms) {
	def_too = (get_menustrip(NULL) == ms);
	if (ms->offCount == 0) {
	    /* detach menues */
	    for (ed = (ED *)GetHead(&DBase); ed; ed = (ED *)GetSucc((struct Node *)ed)) {
	     /* if (ed->Package == Package)    *//* <-PATCH_PACK */
		if ((ed->menustrip == ms) || (def_too && (ed->menustrip == NULL)))    /* <- PATCH_NEW */
		{
		    ClearMenuStrip (ed->win);
		    Forbid ();
		    ed->win->Flags |= WFLG_RMBTRAP;
		    Permit ();
		} /* if */
	    } /* for */
	} /* if first menuoff */
	++ms->offCount;
    } /* if */
} /* menuoff */


/*****************************************************************************

    NAME
	menu_strip

    PARAMETER
	MENUSTRIP	* ms
	struct Window	* Win

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	that function binds a menustrip to a window
	(or after menuoff, binds no menustrip to a window)

    NOTES
      ! in naher Zunkunft wird diese Funktion wohl einen
      ! weiteren Parameter erhalten : status = OFF|ON
	(iconmode ist schliesslich offiziell nicht sichtbar)

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_null  created

******************************************************************************/

void menu_strip (MENUSTRIP * ms, struct Window * Win)
{
    if (!ms->offCount && ms->data && !GETF_ICONMODE(Ep)) {
	SetMenuStrip (Win, ms->data);

	Forbid ();
	Win->Flags &= ~WFLG_RMBTRAP;
	Permit ();
    } else {
	Forbid ();
	Win->Flags |= WFLG_RMBTRAP;
	Permit ();
    } /* if */
} /* menu_strip */


/*
**  Die Folgenden functionen haben noch keine Header
**  und sind teilweise noch nicht fertig
*/


void menusave (MENUSTRIP * ms, char * name)
{
    FILE * fo;

    if ((fo = fopen (name, "w"))) {
	savemenus (ms, fo);
	fclose	  (fo);
    } else {
	error  ("%s:\nCan't open file %s for output", CommandName(), name);
    } /* if */
} /* menusave */



void menuload (MENUSTRIP * ms, char * name)
{
    FILE * fi;
    int    lineno = 0;

    if ((fi = fopen (name, "r"))) {
	menuoff   (ms, NULL);
	menuclear (ms);

	loadmenus (ms, fi, &lineno);
	menuon	  (ms, NULL);
	fclose	  (fi);
    } else {
	error ("%s:\nCan't open file %s for input", CommandName(), name);
    } /* if */
} /* menuload */


/*****************************************************************************

    NAME
	fixmenu

    PARAMETER
	MENUSTRIP     * ms
	struct Window * win

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	recalculate the sizes & positions of a menustrip

    NOTES
	that function uses GadTools.library

    BUGS

    EXAMPLES

    SEE ALSO
	GadTools/LayoutMenu

    INTERNALS
	(ursprueglich liessen wir den kram komplett berechnen, (spagetti)
	dann wurden separate functionen eingefuehrt, die die menuitems
	bzw die subitems rekursiv abarbeiteten)

	vorhin las ich jedoch, dass GadTools mit "normalen"
	intuition-menues arbeitet - also kann LayoutMenu() auch
	einen NON-GadTools menustrip verwenden!

	wenn aber nun GadToolsnormals IntuitionMenues verwendet, dann
	kann die verwendete VisualInfo nur als Informationsquelle
	dienen, also kann sie temporaer verwaltet werden

	(die Verwendung von GadTools spart > 1/2 kB  oder 130 LoC !)

	Aaron warum hast Du mir nict vorher dir RKM empfohlen ?!

    HISTORY
	03 Feb 1993 b_null rewritten for use of GadTools

******************************************************************************/

void fixmenu (MENUSTRIP * ms, struct Window * win)
{
    APTR	   vi;			    /* VisualInfo for GadTools - probably to get Info 'bout screensize a.s.o. */
    struct TagItem tags[1] = {{TAG_END,0}}; /* empty tags list for GadTools - varargs stuff is not visible with -mRR */

    vi = GetVisualInfoA (win->WScreen, tags);
    LayoutMenusA (ms->data, vi, tags);
    FreeVisualInfo (vi);
} /* fixmenu */




/*****************************************************************************

    NAME
	currentwindow

    PARAMETER
	void;

    RESULT
	the current window's menustrip

    RETURN
	void*

    DESCRIPTION
	An interface to the current ED's menustrip

    NOTES
	This function could easiliy be replaced by a macro

    HISTORY
	26-08-93    b_noll  created

******************************************************************************/

void * currentmenu (void)
{
    if ((Ep == NULL) || (Ep->menustrip == NULL)) {
	return (get_menustrip (NULL));
    } /* if */
    return (Ep->menustrip);
} /* currentmenu */


/*****************************************************************************

    NAME
	currentmenu

    PARAMETER
	void;

    RESULT
	the current window

    RETURN
	void*

    DESCRIPTION
	An interface to the current ED's window

    NOTES
	This function should better be replaced by a macro

    HISTORY
	26-08-93    b_noll  created

******************************************************************************/

void * currentwindow (void)
{
    return (Ep->win);
} /* currentwindow */




/******************************************************************************
*****  ENDE menu_dme.c
******************************************************************************/

