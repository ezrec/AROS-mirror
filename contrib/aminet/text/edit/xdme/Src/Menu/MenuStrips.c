/******************************************************************************

    MODUL
	menustrips.c

    DESCRIPTION
	lowlevel commands to support programmable menues
	which are designed to invoke commands of a destinct
	macro language

    NOTES
	exerimental phase

	Eine wichtige Aenderung sollte zur Einbindung dieser Functionen
	in dem [X]DME vorgenommen werden:
	    in Zukunft hat immer nur maximal eins - das aktive Fenster -
	    eine MenueLeiste; fuer den Benutzer ist dies im
	    Allgemeinen nicht weiter von Bedeutung, da er
	    ohnehin nur das Menue des aktiven Fensters benutzen kann.
	    wenn er also waehrend eines laengeren Macros die fenster
	    wechselt, und in einem anderen Fenster eine Eingabe machen
	    will - die ohnehin erst nach Beendigung des Macros ausgefuehrt
	    wuerde, so wird ihm so gezeigt, dass er noch gar keine Eingabe
	    machen kann.

	Eine weitere Aenderung sollte stattfinden mit MACRO
	    kuenftig sollte ausschliesslich eine opaque typ Macro uebergeben
	    werden, die AUSSERHALB von menubase.c initialisiert wurde;
	    dies jedoch birgt enorme probleme bei fktionen wie menudel,
	    da diese nicht einfach die freiwerdenen macros zurueckgeben
	    koennen, sondern dann wiederum selbststaendig die macros loeschen
	    muessen
	    (es ist wahrscheinlich, dass die neue version der PL keine
	    Bindung zwischen funktionen und Help kennt, daher muessten dann
	    MACROS bestehen aus einer Node, die 2 Strings klammert:
	    den Funktionsaufruf und den Hilfstext)
	    (die entscheidung spielt aber auch in diesem Modul eine Rolle
	    da unter Umstaenden menuload/menusave an das neue Macrohandling
	    angepasst werden muessen; die betreffenden Stellen wurden
	    soweit sie erkannt wurden geklammert mit dem folgenden Kommentar:
	    --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED


    BUGS
	IMPORTANT
	    we cannot actually react on ERRORs !!

    TODO
	[SUB] - MX-CheckItems
	Help-Texts - dies ist ein thema, das m.E mehr fuer Macros.c gedacht ist - oder ?
	fix_items
	include some error()-calls

	wuenschenwert ist eine groessere abstraktion:
	    es wird vermutlich eine opaque type XITEM
	    mit den funktionen
		create,   delete,  getcheck, setcheck, settype,  gettype,
		setmacro, sethelp, addsub,   getname,  getmacro, gethelp
	    erzeugt werden. (in diesem augenblick muss eine wesentlich
	    striktere memory-kontrolle eingefuhrt werden)

	    es ist ueberlegenswert, ob doppelbenennungen nicht doch erlaubt werden sollen ????
	    ( xi = new_xitem (name, type); add_xitem (menuheader, xi); )

    EXAMPLES

    SEE ALSO

    INDEX
	$Header : $

    HISTORY
	18 Dec 1992 b_null created
	20-Jan 1993 b_null added checkitems
	27 Jan 1993 b_null added multiple menustrips (renamed menubase -> menustrips)
	$Date$ last update

******************************************************************************/

/**************************************
		Includes
**************************************/
#define  MENU_INTERNAL
#include "defs.h"
#include "menubase.h"
#ifdef PATCH_NULL
#include "AUTO.h"
#else
#define MK_AUTOINIT(x) void x (void)
#define MK_AUTOEXIT(x) void x (void)
#endif


/**************************************
	    Globale Variable
**************************************/

Prototype void		menuclear   (MENUSTRIP *ms);
Prototype int		menudel     (MENUSTRIP *ms, char *tname);
static	  struct Menu * getmenu     (MENUSTRIP *ms, char *tname);
static	  XITEM *	getitem     (MENUSTRIP *ms, char *tname, char *iname);
static	  XITEM *	getsub	    (MENUSTRIP *ms, char *tname, char *iname, char *sname);
Prototype int		itemdel     (MENUSTRIP *ms, char *tname, char *iname);
Prototype int		subdel	    (MENUSTRIP *ms, char *tname, char *iname, char *sname);
Prototype XITEM *	itemadd     (MENUSTRIP *ms, char *tname, char *iname,              char *macro, char *help, int check, char *scut);
Prototype XITEM *	subadd	    (MENUSTRIP *ms, char *tname, char *iname, char *sname, char *macro, char *help, int check, char *scut);
Prototype int		loadmenus   (MENUSTRIP *ms, FILE *fi,     int *lineno);
Prototype int		savemenus   (MENUSTRIP *ms, FILE *fo);
Prototype int		chkitemcheck(MENUSTRIP *ms, char *tname, char *iname);
Prototype int		chksubcheck (MENUSTRIP *ms, char *tname, char *iname, char *sname);
Prototype int		setitemcheck(MENUSTRIP *ms, char *tname, char *iname,              int status);
Prototype int		setsubcheck (MENUSTRIP *ms, char *tname, char *iname, char *sname, int status);
Prototype APTR		menu2macro  (MENUSTRIP *ms, char *str);

Prototype MENUSTRIP *get_menustrip    (char * name);
Prototype MENUSTRIP *new_menustrip    (char * name, int defaults);
Prototype void	     delete_menustrip (MENUSTRIP * kt, int force);

Prototype void	     exit_menustrips (void);

/**************************************
      Interne Defines & Strukturen
**************************************/
#define MENU_DELIMITER '-'  /* the char that divides menuename from itemname */
#define ITEM_DELIMITER '-'  /* the char that divides itemname from subitemname */

#define SCUT(xi)  ((xi)->item.Flags & COMMSEQ)
#define SCUTS(xi) ((SCUTS_dummy[0]=(xi)->item.Command)? SCUTS_dummy: SCUTS_dummy)


/**************************************
	    Interne Variable
**************************************/

#define cmpfunc(x,y) stricmp(x,y) /* if s.o wishes case-sensitivity redefine here */

static struct Image SepImage = {
    1,	/* left */
    1,	/* top	*/
    0,	/* wid. */
    2,	/* hig. */
    0,	/* dep. */
    NULL,/* data */
    0,	/* ppik */
    0,	/* poo	*/
    NULL/* next */
};

 struct MinList MenuStrips = { (struct MinNode *)&MenuStrips.mlh_Tail,
	NULL, (struct MinNode *)&MenuStrips.mlh_Head };
static char SCUTS_dummy[2] = { 0, 0 };

/**************************************
	   Interne Prototypes
**************************************/
static	XITEM * DeleteXItem	  (XITEM  *xi);
static	XITEM * CreateXItem	  (char   *name);
static	XITEM * get_xitem	  (XITEM  *start, char *name);
static	XITEM * mod_or_add_xitem  (XITEM **pr,    char *name, char *macro, char *help, int check, char *scut);
static	int	rem_xitem	  (XITEM **pr,    char *name);


/* !!! achtung irgendwo muessen noch menuoff/menuon eingebaut werden !!! */




/*****************************************************************************

    NAME
	DeleteXItem

    PARAMETER
	XITEM * xi

    RESULT
	xi->next

    RETURN
	XITEM *

    DESCRIPTION
	remove an xitem and all associated data (name, help, com, subs)

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS
	text ? loesche text
	loesche xitem

    HISTORY
	18 Dec 1992 b_null  created
	03 Feb 1993 b_null rewritten

******************************************************************************/

static
XITEM * DeleteXItem (XITEM * xi)
{
    if (xi) {
	XITEM * next = (XITEM *)xi->item.NextItem;

	if (xi->item.Flags & ITEMTEXT) {
	    struct IntuiText * it = &xi->fill.it;
	    XITEM * sub = (XITEM *)xi->item.SubItem;

	    while (sub)    { sub = DeleteXItem (sub); } /* while */
	    if (it->IText) { DeallocFunc (it->IText); } /* if */
	    if (xi->help)  { DeallocFunc (xi->help);  } /* if */
	    if (xi->com)   { DeallocFunc (xi->com);   } /* if */
	} /* if is text */

	FreeFunc (xi, sizeof (XITEM));
	return (next);
    } /* if */

    return (NULL);
} /* DeleteXItem */



/*****************************************************************************

    NAME
	get_xitem

    PARAMETER
	XITEM * start
	char  * name

    RESULT
	a item connected with name that is on the same level
	and later than start or start itself
	NULL if not found

    RETURN
	XITEM *

    DESCRIPTION
	this function can be used to search for entries in a certain level

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	19 Dec 1992 b_noll  created

******************************************************************************/

static
XITEM * get_xitem (XITEM * start, char * name)
{
    struct MenuItem * item;

    for (item = (struct MenuItem *)start; item; item = item->NextItem) {
	if ((item->Flags & ITEMTEXT) == 0) {
	    if (name == BAR) {
		return ((XITEM *)item);
	    } /* if */
	} else if (name != BAR) {
	    struct IntuiText * it = &((XITEM *)item)->fill.it;
	    if (strcmp (it->IText, name) == 0) {
		return ((XITEM *)item);
	    } /* if */
	} /* if */
    } /* for */

    return (NULL);
} /* get_xitem */



/*****************************************************************************

    NAME
	getmenu

    PARAMETER
	MENUSTRIP * ms
	char	  * tname

    RESULT
	the menuentry connected to tname
	NULL if not found

    RETURN
	struct Menu *

    DESCRIPTION

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	19 Dec 1992 b_noll  created

******************************************************************************/

static
struct Menu * getmenu (MENUSTRIP * ms, char * tname)
{
    struct Menu * menu;

    if (ms == NULL) {
	return (NULL);
    } /* if */

    for (menu = ms->data; menu; menu = menu->NextMenu) {
	if (strcmp (menu->MenuName, tname) == 0) {
	    return (menu);
	} /* if */
    } /* for */
    return (NULL);
} /* getmenu */



/*****************************************************************************

    NAME
	getitem

    PARAMETER
	MENUSTRIP * ms
	char	  * tname
	char	  * iname

    RESULT
	the itementry connected to tname/iname
	NULL if not found

    RETURN
	XITEM *

    DESCRIPTION

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	19 Dec 1992 b_noll  created

******************************************************************************/

static
XITEM * getitem (MENUSTRIP * ms, char * tname, char * iname)
{
    struct Menu * menu = getmenu (ms, tname);

    if (menu) {
	return (get_xitem ((XITEM *)menu->FirstItem, iname));
    } /* if */
    return (NULL);
} /* getitem */



/*****************************************************************************

    NAME
	getsub

    PARAMETER
	MENUSTRIP * ms
	char	  * tname
	char	  * iname
	char	  * sname

    RESULT
	the subentry connected to tname/iname/sname
	NULL if not found

    RETURN
	XITEM *

    DESCRIPTION

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	19 Dec 1992 b_noll  created

******************************************************************************/

static
XITEM * getsub (MENUSTRIP * ms, char * tname, char * iname, char * sname)
{
    struct MenuItem * parent = (struct MenuItem *)getitem (ms, tname, iname);

    if (parent) {
	return (get_xitem ((XITEM *)parent->SubItem, sname));
    } /* if */

    return (NULL);
} /* getsub */



/*****************************************************************************

    NAME
	menudel

    PARAMETER
	MENUSTRIP * ms
	char	  * tname

    RESULT
	success:
	    RET_FAIL - Menu was not found
	    RET_SUCC - Menu deleted

    RETURN
	int

    DESCRIPTION

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO
	menucom/do_delmenu

    INTERNALS

    HISTORY
	19 Dec 1992 b_noll created
	20 Jan 1993 b_noll changed return to int

******************************************************************************/

int menudel (MENUSTRIP * ms, char * tname)
{
    struct Menu *  menu;
    struct Menu ** pr;
    XITEM	*  xi;

    if (ms == NULL) {
	return (RET_FAIL);
    } /* if */
    pr = &ms->data;

    for (menu = *pr; menu; pr = &menu->NextMenu, menu = *pr) {
	if (strcmp (tname, menu->MenuName) == 0) {
	    break;
	} /* if menu exists */
    } /* for all headers */

    if (menu) {
	/* --- detach from list */
	*pr = menu->NextMenu;

	/* --- delete all items */
	xi = (XITEM *)menu->FirstItem;
	while (xi) {
	    xi = DeleteXItem (xi);
	} /* while */

	/* --- free data */
	DeallocFunc (menu->MenuName);
	FreeFunc    (menu, sizeof (struct Menu));

	return (RET_SUCC);
    } /* if */

    return (RET_FAIL);
} /* menudel */



/*****************************************************************************

    NAME
	rem_xitem

    PARAMETER
	XITEM ** pr
	char   * name

    RESULT
	success:
	    RET_FAIL - item name not found or wrong syntax
	    RET_SUCC - ok

    RETURN
	int

    DESCRIPTION

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	19 Dec 1992 b_noll  created

******************************************************************************/

static
int rem_xitem (XITEM ** pr, char * name)
{
    XITEM * xi;

    if (pr == NULL || name == NULL) {
	return (RET_FAIL);
    } else if (name == BAR) {
	for (xi = *pr; xi; pr = (XITEM **)&xi->item.NextItem, xi = *pr) {
	    if (!(xi->item.Flags & ITEMTEXT)) {
		break;
	    } /* if found */
	} /* for all entries */
    } else {
	for (xi = *pr; xi; pr = (XITEM **)&xi->item.NextItem, xi = *pr) {
	    if (xi->item.Flags & ITEMTEXT) {
		char * inter = (char *)((struct IntuiText *) xi->item.ItemFill)->IText;
		if (strcmp (inter, name) == 0) {
		    break;
		} /* if found */
	    } /* if no BAR */
	} /* for all items  */
    } /* if (no) bar/name */

    if (xi) {
	*pr = (XITEM *)DeleteXItem (xi);
	return (RET_SUCC);
    } /* if */

    return (RET_FAIL);
} /* rem_xitem */



/*****************************************************************************

    NAME
	itemdel

    PARAMETER
	MENUSTRIP * ms
	char	  * tname
	char	  * iname

    RESULT
	success:
	    RET_FAIL - item name not found or wrong syntax
	    RET_SUCC - ok

    RETURN
	int

    DESCRIPTION

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO
	menucom/do_delitem, menucom/do_delitembar

    INTERNALS

    HISTORY
	19 Dec 1992 b_noll created

******************************************************************************/

int itemdel (MENUSTRIP * ms, char * tname, char * iname)
{
    int succ = RET_FAIL;
    struct Menu * menu = getmenu (ms, tname);

    if (menu) {
	succ = rem_xitem ((XITEM **)&menu->FirstItem, iname);

	if (succ && !menu->FirstItem) {
	    menudel (ms, tname);
	} /* if */
    } /* if */
    return (succ);
} /* itemdel */



/*****************************************************************************

    NAME
	subdel

    PARAMETER
	MENUSTRIP * ms
	char	  * tname
	char	  * iname
	char	  * sname

    RESULT
	success:
	    RET_FAIL - item name not found or wrong syntax
	    RET_SUCC - ok

    RETURN
	int

    DESCRIPTION

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO
	menucom/do_delsub, menucom/do_delsubbar

    INTERNALS

    HISTORY
	19 Dec 1992 b_noll created

******************************************************************************/

int subdel (MENUSTRIP * ms, char * tname, char * iname, char * sname)
{
    int succ = RET_FAIL;
    struct MenuItem * item = (struct MenuItem *)getitem (ms, tname, iname);

    if (item) {
	succ = rem_xitem ((XITEM **)&item->SubItem, sname);
	if (succ && !item->SubItem) {
	    itemdel (ms, tname, iname);
	} /* if */
    } /* if */
    return (succ);
} /* subdel */



/*****************************************************************************

    NAME
	menuadd

    PARAMETER
	MENUSTRIP * ms
	char	  * tname

    RESULT
	success :
	    ==NULL = failure
	    !=NULL = ok (address of menu)

    RETURN
	struct Menu *

    DESCRIPTION
	we try to add a new menu-header to a menubar
	if it does not already exist; else we simply return it

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS
	falls das menu schon existiert,
	    gib es einfach zurueck

	erzeuge einen neuen body und
	ein namens-string,
	fuelle es
	und gib es zurueck

    HISTORY
	18 Dec 1992 b_null  created

******************************************************************************/

static
struct Menu * menuadd (MENUSTRIP * ms, char * tname)
{
    struct Menu *  menu;
    struct Menu ** pr;

    if (ms == NULL || tname == NULL) {
	return (NULL);
    } /* if */

    pr = &ms->data;

    for (menu = *pr; menu; pr = &menu->NextMenu, menu = *pr) {
	if (strcmp (tname, menu->MenuName) == 0) {
	    return (menu);
	} /* if menu exists */
    } /* for all headers */

    menu = AllocFunc (sizeof(struct Menu), MEMF_ANY);
    if (!menu) {
	return (NULL);
    } /* if no heap for menu */
    setmem (menu, sizeof(struct Menu), 0);

    menu->MenuName = DupFunc (tname, MEMF_ANY);
    if (!menu->MenuName) {
	FreeFunc (menu, sizeof (struct Menu));
	return (NULL);
    } /* if no heap for name */

    menu->NextMenu = *pr; /* == NULL */
    *pr = menu;
    menu->Flags = MENUENABLED;

    return (menu);
} /* menuadd */



/*****************************************************************************

    NAME
	CreateXItem

    PARAMETER
	char * name

    RESULT
	new XITEM

    RETURN
	XITEM *

    DESCRIPTION
	creates a new XITEM and all additional data
	does NOT link a macro

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS
	get heap fuer body
	fehler ? -> abbruch
	alles loeschen
	BAR ?
	ja-> data initialisieren
	nein-> get heap fuer string
	    fehler ? -> abbruch
	    data initailisieren
	ptr auf body zurueck

    HISTORY
	18 Dec 1992 b_null  created

******************************************************************************/

static
XITEM * CreateXItem (char * name)
{
    XITEM	    * xi;
    struct MenuItem * item;

    xi = AllocFunc (sizeof (XITEM), MEMF_ANY);
    if (!xi) {
	return (NULL);
    } /* if no heap for body */

    setmem (xi, sizeof(XITEM), 0);
    item	   = &xi->item;
    item->ItemFill = &xi->fill;
    item->SubItem  = NULL;	/* doppelt gemoppelt haelt besser */

    if (name == BAR) {
	struct Image * im = &xi->fill.im;

	memcpy (im, &SepImage, sizeof (struct Image));
	item->Flags = HIGHNONE; /* HIGHCOMP; */
    } else {
	struct IntuiText * it = &xi->fill.it;

	it->IText = DupFunc (name, MEMF_ANY);
	if (!it->IText) {
	    FreeFunc (xi, sizeof(XITEM));
	    return (NULL);
	} /* if */

#if 0
	it->BackPen    = 1;    /* These values are set in fixmenu - i hope */
	it->FrontPen   = 0;    /* These values are set in fixmenu - i hope */
	it->DrawMode   = JAM2; /* These values are set in fixmenu - i hope */
#endif

	item->Flags = ITEMTEXT|ITEMENABLED|HIGHCOMP;
    } /* if */

    return (xi);
} /* CreateXItem */



/*****************************************************************************

    NAME
	mod_or_add_xitem

    PARAMETER
	XITEM **root
	char   *name
	char   *macro
	char   *help
	int	check
       char    *scut

    RESULT
	found&modifed or created XITEM or NULL

    RETURN
	XITEM *

    DESCRIPTION
	try to find name in the list after pr
	if not found, try to create a new one
	return the result or the found
	(bars are always added)

    NOTES

    BUGS
	scut is not yet used

    EXAMPLES

    SEE ALSO

    INTERNALS
	ein BAR immer neu
	sonst suche nach name
	    eintrag gefunden -> modifiziere ihn & gib ihn zurueck
	erzeuge neuen eintrag
	und gib ihn zurueck

    HISTORY
	18 Dec 1992 b_null  created
	22 Jun 1994 b_null added scut

******************************************************************************/

static
XITEM * mod_or_add_xitem (XITEM **root, char *name, char *macro, char *help, int check, char *scut)
{
    XITEM *  xi;
    XITEM ** pr = root;

    if (root == NULL || name == NULL) {
	return (NULL);
    } else if (name == BAR) {
	for (xi = *pr; xi; pr = (XITEM **)&xi->item.NextItem, xi = *pr) {
	    /* do nothing - just go to the end of the list */
	} /* for all entries */
    } else {
	for (xi = *pr; xi; pr = (XITEM **)&xi->item.NextItem, xi = *pr) {
	    if (xi->item.Flags & ITEMTEXT) {
		char * inter = (char *)((struct IntuiText *) xi->item.ItemFill)->IText;
		if (strcmp (inter, name) == 0) {
		    break;
		} /* if found */
	    } /* if no BAR */
	} /* for all items */
    } /* if (no) bar/name */

    /* --- not found create a new one */
    if (!xi) {
	xi = CreateXItem (name);
	if (xi == NULL) {
	    return (NULL);
	} /* if fail */
    } /* if */

    *pr = (XITEM *)xi->item.NextItem; /* zunaechst aus der liste loesen */

    /*
    ** das sollte bei gelegenheit geaendert werden:
    ** so ist es nicht moegich, ein macro zu loeschen...
    */

    if (name != BAR) {
	/* dies bedeutet unnoetige fragmentation : wir sollten besser erst nachschauen, ob sich werte veraendert haben */
	DeallocFunc (xi->com);  xi->com  = NULL;
	DeallocFunc (xi->help); xi->help = NULL;

	if (macro) {
	    {	/* A MACRO-CARRIER MUST NOT BE A SUB-CARRIER */
		XITEM * sub = (XITEM *)xi->item.SubItem;
		while (sub) {
		    sub = DeleteXItem (sub);
		} /* while */
	    }

	    if (check) {
		xi->item.Flags |= CHECKIT|MENUTOGGLE;
	    } else {
		xi->item.Flags &= ~(CHECKIT|MENUTOGGLE);
	    } /* if */

	    if (!(xi->com = DupFunc (macro, MEMF_ANY))) {
		DeleteXItem (xi);
		return (NULL);
	    } /* if !replaced */

	    if (scut) {
		xi->item.Flags	|=  COMMSEQ;
		xi->item.Command = *scut;
	    } /* if */
	} /* if (macro) */

	if (help) {
	    if (!(xi->help = DupFunc (help, MEMF_ANY))) {
		DeleteXItem (xi);
		return (NULL);
	    } /* if !replaced */
	} /* if (help) */


    } /* if (!bar) */

    *pr = xi; /* (wieder) einhaengen erst wenn alles ok */
    return (xi);
} /* mod_or_add_xitem */



/*****************************************************************************

    NAME
	itemadd

    PARAMETER
	MENUSTRIP * ms
	char	  * tname
	char	  * iname
	char	  * macro
	char	  * help
	int	    check

    RESULT
	success :
	    ==NULL = failure
	    !=NULL = ok      (address of item)

    RETURN
	XITEM *

    DESCRIPTION
	find a menuitem-structure of the right names
	and create one at the end of the list,
	if there is none.
	(we try to add a item-structure to menubar)

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO
	menucom/do_itemadd, menucom/do_itembar

    INTERNALS
	finde das richtige menu
	falls der eintrag nicht existiert
	    erzeuge einen neuen
	revidiere das commando
	gib den eintrag zurueck

    HISTORY
	18 Dec 1992 b_null  created
	22 Jun 1994 b_null added scut

******************************************************************************/

XITEM * itemadd (MENUSTRIP *ms, char *tname, char *iname, char *macro, char *help, int check, char *scut)
{
    struct Menu  * menu;
    XITEM	 * item;

/* /*  */
 /* return NULL; */

    if (!(menu = menuadd (ms, tname))) {
	return (NULL);
    } /* if no header */

    /* menues have no commands -> no comcheck */

    item = mod_or_add_xitem ((XITEM **)&menu->FirstItem, iname, macro, help, check, scut);
    if (item == NULL && menu->FirstItem == NULL) {
	menudel (ms, tname);
	return (NULL);
    } /* if fail and only item */

    return (item);
} /* itemadd */



/*****************************************************************************

    NAME
	subadd

    PARAMETER
	MENUSTRIP * ms
	char	  * tname
	char	  * iname
	char	  * sname
	char	  * macro
	char	  * help
	int	    check
	char	   *scut

    RESULT
	success : ==NULL = failure !=NULL = ok

    RETURN
	XITEM *

    DESCRIPTION
	we try to add a subitem-structure to to menubar

    NOTES

    BUGS

    EXAMPLES
	menucom/do_subadd, menucom/do_subbar

    SEE ALSO

    INTERNALS
	falls es kein item mit menu/item mit d. richtigen namen gibt,
	    erzeuge eins
	    falls das fehlschug -> abbruch

	falls das item ein eigenes macro hat und daher keine
	    subs unterstuetzt -> abbruch

	falls es noch kein sub mit dem richtigen namen gibt,
	    erzeuge eins (bars werden immer addiert)

	falls das fehlschlug und extra ein item rzeugt worden war,
	    loesche das wieder -> abbruch


    HISTORY
	18 Dec 1992 b_null created
	22 Jun 1994 b_null added scut

******************************************************************************/

XITEM * subadd (MENUSTRIP *ms, char *tname, char *iname, char *sname, char *macro, char *help, int check, char *scut)
{
    XITEM	    * item;
    XITEM	    * sub;

    if (iname == BAR) {
	return (NULL);
    } /* if item=bar -	ein bar soll untermenues haben ??? */

    if (!(item = itemadd (ms, tname, iname, NULL, NULL, 0, NULL))) {
	return (NULL);
    } /* if no item */

    if (item->com != NULL) {
	return (NULL);
    } /* if no subs allowed */

    sub = mod_or_add_xitem ((XITEM **)&item->item.SubItem, sname, macro, help, check, scut);
    if (sub == NULL && item->item.SubItem == NULL) {
	itemdel (ms, tname, iname);
	return (NULL);
    } /* if fail and only sub */

    return (sub);
} /* subadd */



/*****************************************************************************

    NAME
	savemenus

    PARAMETER
	MENUSTRIP * ms
	FILE	  * fo

    RESULT
	success (senseless for the moment)

    RETURN
	int

    DESCRIPTION

    NOTES
	we have used an implementation-decision here, that should
	better bew invisible -
	macros must be accessible via their name, if that model is used
	else we have to rewrite parts of that function

    BUGS

    EXAMPLES

    SEE ALSO
	menucom/do_menusave

    INTERNALS

    HISTORY
	19 Dec 1992 b_noll  rewritten for subs/subbars

******************************************************************************/


int savemenus (MENUSTRIP * ms, FILE * fo)
{
    struct Menu     * menu;
    struct MenuItem * item;

    fprintf(fo, "MENUSTRIP START\n");
    for (menu= ms->data; menu; menu = menu->NextMenu) {
	fprintf(fo, "TITLE %s\n", menu->MenuName);
	for (item = menu->FirstItem; item; item = item->NextItem) {
	    if (item->Flags & ITEMTEXT) {
		char* ptr = (char *)((struct IntuiText *)item->ItemFill)->IText;
		if (item->Flags & CHECKIT) {
		    fprintf(fo, "\tITEMCHECK %s\n", ptr);
/* --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED */
			if (((XITEM *)item)->help)
			    fprintf (fo, "\t HELP %s\n", ((XITEM*)item)->help);
			if (SCUT((XITEM *)item))
			    fprintf (fo, "\t SHORT %s\n", SCUTS((XITEM *)item));
		    fprintf(fo, "\t COM  %s\n", ((XITEM*)item)->com);
/* --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED */
		} else {
		    fprintf(fo, "\tITEM %s\n", ptr);
		    if (item->SubItem) {
			struct MenuItem * sub;
			for (sub = item->SubItem; sub; sub = sub->NextItem) {
			    if (sub->Flags & ITEMTEXT) {
				char* ptr = (char *)((struct IntuiText *)sub->ItemFill)->IText;
				if (sub->Flags & CHECKIT) {
				    fprintf (fo, "\t\tSUBCHECK %s\n", ptr);
				} else {
				    fprintf (fo, "\t\tSUB %s\n", ptr);
				} /* if */
/* --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED */
				if (((XITEM *)item)->help)
				    fprintf (fo, "\t\t HELP %s\n", ((XITEM *)item)->help);
				if (SCUT((XITEM *)item))
				    fprintf (fo, "\t\t SHORT %s\n", SCUTS((XITEM *)item));
				fprintf (fo, "\t\t COM  %s\n", ((XITEM *)item)->com);
/* --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED */
			    } else {
				fprintf (fo, "\t\tSUBBAR");
			    } /* if (not) text */
			} /* for subs */
		    } else {
/* --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED */
			if (((XITEM *)item)->help)
			    fprintf (fo, "\t HELP %s\n", ((XITEM*)item)->help);
			if (SCUT((XITEM *)item))
			    fprintf (fo, "\t SHORT %s\n", SCUTS((XITEM *)item));
			fprintf(fo, "\t COM  %s\n", ((XITEM*)item)->com);
/* --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED */
		    } /* if (not) subs */
		} /* if (not) checkitem */
	    } else {
		fprintf(fo, "\tITEMBAR\n");
	    } /* if (not) text */
	} /* for items */
    } /* for headers */
    fprintf(fo, "MENUSTRIP END\n");

    return (RET_SUCC);
} /* savemenus */



/*****************************************************************************

    NAME
	menuclear

    PARAMETER
	MENUSTRIP * ms

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	deletes a full Menustrip
	and all connected data

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO
	menucom/do_menuclear

    INTERNALS

    HISTORY
	19 Dec 1992 b_noll  created

******************************************************************************/

void menuclear (MENUSTRIP * ms)
{
    struct Menu * menu;

    while ((menu = ms->data)) {
	menudel (ms, menu->MenuName);
    } /* while */
    ms->data = NULL;
} /* menuclear */



/*****************************************************************************

    NAME
	loadmenues

    PARAMETER
	MENUSTRIP * ms
	FILE	  * fi
	int	  * lineno

    RESULT
	success:
	    RET_FAIL - error
	    RET_SUCC - ok

    RETURN
	int

    DESCRIPTION

    NOTES
	we have used an implementation-decision here, that should
	better be invisible -
	macros must be accessible via their name, if that model is used
	else we have to rewrite parts of that function

    BUGS
	HELP may be redefined

    EXAMPLES

    SEE ALSO
	menucom/do_menuload

    INTERNALS

    HISTORY
	20 Jan 1993 b_null added check-handling

******************************************************************************/

#define UNEX_DEF   "Unexpected %s Definition"
#define EX_TITLE    1
#define EX_ITEM     2
#define EX_ITEMBAR  4
#define EX_SUB	    8
#define EX_SUBBAR  16
#define EX_COM	   32
#define EX_SCOM    64
#define EX_END	  128
#define EX_SHORT  256
#define EX_HELP   512

/* #define EX_HELP   (EX_COM|EX_SCOM) / * buggy! */


/* PLANNED METHOD

     "START", 5, EX_START, PUSH, EX_END|EX_ITEM,
     "END",   3, EX_END,   POP,  EX_END|EX_ITEM,
     "BAR",   3, EX_ITEM,  SUB,  0,
     "ITEM",  4, EX_ITEM,  SET,                 EX_START|EX_HELP|EX_SCUT|EX_COM,
     "SHORT", 5, EX_SHORT, SUB,                 EX_START|        EX_SCUT,
     "HELP",  4, EX_HELP,  SUB,                 EX_START|EX_HELP,
     "COM",   3, EX_COM,   SET,  EX_END|EX_ITEM,
*/

/* CURRENT METHOD
     "MENUSTRIP END",     , EX_END,   SET, 0,
     "TITLE ",           5, EX_TITLE, SET,                                EX_ITEM,
     "ITEM ",            4, EX_ITEM,  SET, EX_COM|EX_HELP|EX_SHORT|EX_SUB,
     "ITEMCHECK ",       9, EX_ITEM,  SET, EX_COM|EX_HELP|EX_SHORT,
     "ITEMBAR",          7, EX_ITEM,  SET,                                EX_ITEM|EX_TITLE,
     "HELP "              , EX_HELP,  SUB,        EX_HELP|         EX_SUB,
     "SHORT "             , EX_SHORT, SUB,                EX_SHORT|EX_SUB,
     "COM "               , EX_COM,   SET,                         EX_TTT|EX_ITEM|EX_TITLE,
     "SUB "               , EX_SUB,   SET, EX_COM|EX_HELP|EX_SHORT,
     "SUBCHECK "          , EX_SUB,   SET, EX_COM|EX_HELP|EX_SHORT,
     "SUBBAR"             , EX_SUB,   SET,                         EX_SUB|EX_ITEM|EX_TITLE,
*/


int loadmenus (MENUSTRIP * ms, FILE * fi, int * lineno)
{
    char * buf;
    char   ntitle [128];
    char   nitem  [128];
    char   nsub   [128];
    char   body   [LINE_LENGTH];
    char   help   [LINE_LENGTH];
    char   scut   [32];
    char *pscut = NULL;
/*  int    doloop = -1; */
    int     check =  0;
    long expected;

    buf = getnextcomline (fi, lineno);
    if (!buf) {
	/* error ("Unexpected EndOfFile"); */
	SETF_ABORTCOMMAND(Ep, 1);
	return (RET_FAIL);
    } /* if */
    if (strncmp (buf, "MENUSTRIP START", 15) != 0) {
	error	("No Menustart header");
	return	(RET_FAIL);
    } /* if */

    ntitle[0] = 0;
    nitem [0] = 0;
    nsub  [0] = 0;
    body  [0] = 0;
    help  [0] = 0;
    scut  [0] = 0;

    expected = EX_TITLE|EX_END;

    while (GETF_ABORTCOMMAND(Ep) == 0) {
	buf = getnextcomline (fi, lineno);
	if (!buf) {
	    SETF_ABORTCOMMAND(Ep, 1);
	    return (RET_FAIL);
	} /* if */

	if (strncmp(buf, "MENUSTRIP END", 13) == 0) {
	    if (!(expected & EX_END)) {
		error  (UNEX_DEF, "End Of Menu");
		return (RET_FAIL);
	    } /* if */
	    return (RET_SUCC);

	} else if (strncmp(buf, "TITLE ", 6) == 0) {
	    if (!(expected & EX_TITLE)) {
		error  (UNEX_DEF, "MenuTitle");
		return (RET_FAIL);
	    } /* if */
	    expected = EX_ITEM|EX_ITEMBAR;

	    buf += 6;
	    strncpy (ntitle, buf, 128);

	} else if (strncmp(buf, "ITEM ", 5) == 0) {
	    if (!(expected & EX_ITEM)) {
		error  (UNEX_DEF, "MenuItem");
		return (RET_FAIL);
	    } /* if */
	    expected = EX_SUB|EX_SUBBAR|EX_COM|EX_HELP|EX_SHORT;

	    buf += 5;
	    strncpy (nitem, buf, 128);

	} else if (strncmp(buf, "ITEMCHECK ", 10) == 0) {
	    if (!(expected & EX_ITEM)) {
		error  (UNEX_DEF, "MenuItem");
		return (RET_FAIL);
	    } /* if */
	    expected = EX_COM|EX_HELP|EX_SHORT;

	    buf += 10;
	    strncpy (nitem, buf, 128);
	    check = 1;

	} else if (strncmp(buf, "ITEMBAR", 7) == 0) {
	    if (!(expected & EX_ITEMBAR)) {
		error  (UNEX_DEF, "MenuItembar");
		return (RET_FAIL);
	    } /* if */
	    expected = EX_ITEM|EX_ITEMBAR|EX_TITLE|EX_END;

	    itemadd (ms, ntitle, BAR, NULL, NULL, 0, NULL);

	} else if (strncmp(buf, "SUB ", 4) == 0) {
	    if (!(expected & EX_SUB)) {
		error  (UNEX_DEF, "SubItem");
		return (RET_FAIL);
	    } /* if */
	    expected = EX_SCOM|EX_HELP|EX_SHORT;

	    buf += 4;
	    strncpy (nsub, buf, 128);

	} else if (strncmp(buf, "SUBCHECK ", 9) == 0) {
	    if (!(expected & EX_SUB)) {
		error  (UNEX_DEF, "SubItemCheck");
		return (RET_FAIL);
	    } /* if */
	    expected = EX_SCOM|EX_HELP|EX_SHORT;

	    buf += 9;
	    strncpy (nsub, buf, 128);
	    check = 1;

	} else if (strncmp(buf, "SUBBAR", 6) == 0) {
	    if (!(expected & EX_SUBBAR)) {
		error  (UNEX_DEF, "SubItembar");
		return (RET_FAIL);
	    } /* if */
	    expected = EX_SUB|EX_SUBBAR|EX_TITLE|EX_ITEM|EX_ITEMBAR|EX_END;

	    subadd (ms, ntitle, nitem, BAR, NULL, NULL, 0, NULL);

/* --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED */
	} else if (strncmp(buf, "HELP", 4) == 0 && (buf[4] < 33)) {
	    if (!(expected & (EX_HELP))) {
		error  (UNEX_DEF, "Help");
		return (RET_FAIL);
	    } /* if */

	    buf += 3; if (*buf) buf++;
	    strncpy (help, buf, LINE_LENGTH);
	    expected &= ~EX_HELP;

	} else if (strncmp(buf, "SHORT", 5) == 0 && (buf[5] < 33)) {
	    if (!(expected & (EX_SHORT))) {
		error  (UNEX_DEF, "Shortcut");
		return (RET_FAIL);
	    } /* if */

	    buf += 4; if (*buf) buf++;
	    strncpy (scut, buf, sizeof (scut));
	    expected &= ~EX_SHORT;
	    pscut = scut;

	} else if (strncmp(buf, "COM", 3) == 0 && (buf[3] < 33)) {
	    /* char * h2 = help[0] ? help : NULL; */

	    if (!(expected & (EX_COM|EX_SCOM))) {
		error  (UNEX_DEF, "Command");
		return (RET_FAIL);
	    } /* if */
	    /* expected = 0; */

	    buf += 3; if (*buf) buf++;
	    strncpy (body, buf, LINE_LENGTH);
	    if (expected & EX_SCOM) {
		expected = EX_SUB|EX_SUBBAR|EX_TITLE|EX_ITEM|EX_ITEMBAR|EX_END;
		subadd (ms, ntitle, nitem, nsub, body, help, check, pscut);
	    } else {
		expected = EX_TITLE|EX_ITEM|EX_ITEMBAR|EX_END;
		itemadd (ms, ntitle, nitem,      body, help, check, pscut);
	    } /* if */
	    check   = 0;
	    help[0] = 0;
	    scut[0] = 0;
	    pscut   = NULL;
/* --- THAT BLOCK MUST BE CHANGED IF MACROS ARE MODIFIED */
	} else {
	    error ("%s:\nunknown identifier '%s'", CommandName(), buf);
	} /* if types */
    } /* while not ready */
    return(RET_FAIL);
} /* loadmenus */



/*****************************************************************************

    NAME
	chkitemcheck

    PARAMETER
	MENUSTRIP * ms
	char	  * tname
	char	  * iname

    RESULT
	-1 error
	 0/1 status of check

    RETURN
	int

    DESCRIPTION
	check the status of a checkitem

    NOTES
	only works with items created as check-items

    BUGS
	none known

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Jan 1993 b_null created

******************************************************************************/

int chkitemcheck (MENUSTRIP * ms, char * tname, char * iname)
{
    struct MenuItem * item = (struct MenuItem *)getitem (ms, tname, iname);
    if ((!item) || !(item->Flags & (MENUTOGGLE|CHECKIT))) {
	return (-1);
    } /* if */

    if (item->Flags & CHECKED) {
	return (1);
    } /* if */
    return (0);
} /* chkitemcheck */



/*****************************************************************************

    NAME
	chksubcheck

    PARAMETER
	MENUSTRIP * ms
	char	  * tname
	char	  * iname
	char	  * sname

    RESULT
	-1 error
	 0/1 status of check

    RETURN
	int

    DESCRIPTION
	check the status of a check-subitem

    NOTES
	only works with subs created as check-subitems

    BUGS
	none known

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Jan 1993 b_null created

******************************************************************************/

int chksubcheck (MENUSTRIP * ms, char * tname, char * iname, char * sname)
{
    struct MenuItem * sub = (struct MenuItem *)getsub (ms, tname, iname, sname);

    if ((!sub) || !(sub->Flags & (MENUTOGGLE|CHECKIT))) {
	return (-1);
    } /* if */

    if (sub->Flags & CHECKED) {
	return (1);
    } /* if */
    return (0);
} /* chksubcheck */



/*****************************************************************************

    NAME
	setitemcheck

    PARAMETER
	MENUSTRIP * ms
	char	  * tname
	char	  * iname

    RESULT
	success :
	    RET_FAIL - not found or error
	    RET_SUCC - ok

    RETURN
	int

    DESCRIPTION
	set the status of a check-item

    NOTES
	only works with items created as check-items

    BUGS
	none known

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Jan 1993 b_null created

******************************************************************************/

int setitemcheck (MENUSTRIP * ms, char * tname, char * iname, int status)
{
    struct MenuItem * item = (struct MenuItem *)getitem (ms, tname, iname);

    if ((!item) || !(item->Flags & (MENUTOGGLE|CHECKIT))) {
	return (RET_FAIL);
    } /* if */

    if (status) {
	item->Flags |= CHECKED;
    } else {
	item->Flags &= ~CHECKED;
    } /* if */
    return (RET_SUCC);
} /* setitemcheck */



/*****************************************************************************

    NAME
	setsubcheck

    PARAMETER
	MENUSTRIP * ms
	char	  * tname
	char	  * iname
	char	  * sname

    RESULT
	success :
	    RET_FAIL - not found or error
	    RET_SUCC - ok

    RETURN
	int

    DESCRIPTION
	set the status of a check-subitem

    NOTES
	only works with subs created as check-subitems

    BUGS
	none known

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Jan 1993 b_null created

******************************************************************************/

int setsubcheck (MENUSTRIP * ms, char * tname, char * iname, char * sname, int status)
{
    struct MenuItem * sub = (struct MenuItem *)getsub (ms, tname, iname, sname);

    if ((!sub) || !(sub->Flags & (MENUTOGGLE|CHECKIT))) {
	return (RET_FAIL);
    } /* if */

    if (status) {
	sub->Flags |= CHECKED;
    } else {
	sub->Flags &= ~CHECKED;
    } /* if */
    return (RET_SUCC);
} /* setsubcheck */








/* old DATA */

/*
**  Die Folgenden functionen haben noch keine Header
**  und sind teilweise noch nicht fertig
*/


/*****************************************************************************

    NAME
	def_menus

    PARAMETER
	MENUSTRIP * ms

    RESULT

    RETURN

    DESCRIPTION

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	Jan 1993 b_null created

******************************************************************************/


int def_menus (MENUSTRIP * ms)
{

    ms->offCount = 1;
    menuclear (ms);

#   ifdef NOT_DEF /* PATCH_NULL [04 Feb 1993] : Hierher koennte noch ein Default-menue eintrag */
    {
	int i;
	int succ;
	for (i = 0, succ = RET_SUCC; (succ == RET_SUCC) && (def_menu[i].title != NULL); i++) {
	    if (def_menu[i].sub) {
		succ = subadd  (ms, def_menu[i].title, def_menu[i].item, def_menu[i].sub, def_menu[i].body, def_menu[i].help, def_menu[i].check, NULL);
	    } else {
		succ = itemadd (ms, def_menu[i].title, def_menu[i].item,                  def_menu[i].body, def_menu[i].help, def_menu[i].check, NULL);
	    } /* if sub */
	} /* for def_menu */
    }
#   endif
    ms->offCount = 0;

    return (RET_SUCC);
} /* def_menus */




/*****************************************************************************

    NAME
	split_menu_names

    PARAMETER
	char *	source
	char ** menu
	char ** item
	char ** sub

    RESULT
	success:
	    RET_SUCC == everythink ok
	    RET_FAIL == any failure

    RETURN
	int

    DESCRIPTION
	that function is to extract the menu-, item- and subitem-
	names out of a single string

	there is no test, if these extracted names are part of a menustrip

    NOTES
	the MENU_DELIMITER must not be part of a menuname
	the ITEM_DELIMITER must not be part of an itemname
	else we might get serious problems

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS
	suche nach dem MENU_DELIMITER,
	    kopiere den string bis dort nach menuname
	suche nach dem ITEM_DELIMITER
	    gefunden -> kopiere den string bis dort nach itemname,
			den rest nach subname
	    sonst    -> kopiere den string nach itemname

    HISTORY
	01 Feb 1993 b_null created

******************************************************************************/

int split_menu_names (char * source, char ** menu, char ** item, char ** sub)
{
    static char menuname[128];
    static char itemname[128];
    static char subname [128];
    int 	i;

    *menu = NULL;
    *item = NULL;
    *sub  = NULL;

    for (i = 0; source[i] && source[i] != MENU_DELIMITER; ++i);
    if (source[i] == MENU_DELIMITER) {
	/* Punkt 1 : hier bin ich noch nicht zufrieden: was passiert, wenn ein name ein '-' enthaelt */
	strncpy(menuname, source, i);
	menuname[i] = 0;
	*menu = menuname;

	source += (i+1);
	for (i = 0; source[i] && source[i] != ITEM_DELIMITER; ++i);
	strncpy(itemname, source, i);
	*item = itemname;
	itemname[i] = 0;

	if (source[i] == ITEM_DELIMITER) {
	    source += (i+1);
	    strcpy (subname, source);
	    *sub  = subname;
	} /* if contains subname */
	return (RET_SUCC);
    } /* if contains itemname */
    return (RET_FAIL);
} /* split_menu_names */



/*****************************************************************************

    NAME
	findmenu

    PARAMETER
	MENUSTRIP * ms
	char	  * name

    RESULT

    RETURN

    DESCRIPTION

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	29 Dec 1992 b_null created
	01 Feb 1993 b_null splitted

******************************************************************************/

struct MenuItem * findmenu (MENUSTRIP * ms, char * str)
{
    char	    * header;
    char	    * itembuf;
    char	    * subbuf;
    struct Menu     * menu;
    struct MenuItem * item;
    struct MenuItem * sub;

    split_menu_names (str, &header, &itembuf, &subbuf);

    if ((header) && (menu = getmenu (ms, header))) {
	if ((itembuf) && (item   = (struct MenuItem *)get_xitem ((XITEM *)menu->FirstItem, itembuf))) {
	    if ((subbuf) && (sub = (struct MenuItem *)get_xitem ((XITEM *)item->SubItem, subbuf))) {
		return (sub);
	    } else if (subbuf) {
		return (NULL);
	    } /* if subitem */
	    return (item);
	} /* if found item */
    } /* if found menu */
    return (NULL);
} /* findmenu */


/*****************************************************************************

    NAME
	menu2macro

    PARAMETER
	MENUSTRIP * ms
	char	  * name

    RESULT
	the macro used by the menu name

    RETURN
	APTR

    DESCRIPTION

    NOTES
	perhaps user has to call macroname, macrohelp or sthng like that
	to get the wanted information

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_null created

******************************************************************************/

APTR menu2macro (MENUSTRIP * ms, char * str)
{
    XITEM * xi;

    if ((xi = (XITEM *)findmenu (ms, str))) {
	if (xi->com) {
	    return (xi->com);
	} /* if */
    } /* if */
    return (NULL);
} /* menu2macro */





/*****************************************************************************

    BASIC FUNCTIONS ON MENUSTRIPS :

	get, new, delete

    all menustrips created with new_menustrip are deleted at program termination

*****************************************************************************/

/*****************************************************************************

    NAME
	get_menustrip

    PARAMETER
	char * name

    RESULT
	the menustrip with the name name, if it is already existing
	else NULL

    RETURN
	MENUSTRIP *

    DESCRIPTION
	search function on the hidden list of menustrips

    NOTES
	useful only, if we are using multiple menustrips

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	27 Jan 1993 b_null created

******************************************************************************/

MENUSTRIP * get_menustrip (char * name)
{
    MENUSTRIP * ms;

    if (name == NULL) {
	return (GetHead (&MenuStrips));
    } /* if wanted default */

    for (ms = GetHead (&MenuStrips); ms; ms = GetSucc(ms)) {
	if (strcmp (ms->node.ln_Name, name) == 0) {
	    return (ms);
	} /* if */
    } /* for */
    return (NULL);
} /* get_menustrip */



/*****************************************************************************

    NAME
	delete_menustrip

    PARAMETER
	MENUSTRIP * ms
	int	    force

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	delete a full menustrip
	that function does NOT delete the last menustrip,
	unless force is set to 1

    NOTES
	useful only, if we are using multiple menustrips

	that function does NOT take care of any references to a menustrip
	You must do that job on a higher abstraction-level
	    (e.g. in menucom.c - for all ED's if )

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS
	ausklinken aus der menustrip-liste
	loeschen der menuestruktur
	loeschen des namens
	loeschen des bodies

    HISTORY
	27 Jan 1993 b_null created

******************************************************************************/

void delete_menustrip (MENUSTRIP * ms, int force)
{
    if (ms) {
	Remove (&ms->node);
	if ((!force) && (!GetHead (&MenuStrips))) {
	    AddHead ((struct List *)&MenuStrips, &ms->node);
	    return;
	} /* if */

	menuclear (ms);
	DeallocFunc (ms->node.ln_Name);
	FreeFunc    (ms, sizeof (MENUSTRIP));
    } /* if */
} /* delete_menustrip */



/*****************************************************************************

    NAME
	new_menustrip

    PARAMETER
	char * name
	int    defaults

    RESULT
	either a new menustrip, if there is not already one with that name
	or the first menustrip, that matches that name
	if we have to create a new one, we can set defaults to 1 to
	fill in the default-menustrip

    RETURN
	MENUSTRIP *

    DESCRIPTION

    NOTES
	useful only, if we are using multiple menustrips

	if we can fill in only parts of the deafult-information,
	we do NOT fail

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	27 Jan 1993 b_null created

******************************************************************************/

MENUSTRIP * new_menustrip (char * name, int defaults)
{
    MENUSTRIP * ms;

    if ((ms = get_menustrip(name))) {
	/* goto newvalue; */ /* that way we do reset an existing menustrip, if we call new */
	return (ms);         /* that way we only use an existing menustrip, if we call new */
    } /* if */

    ms = AllocFunc (sizeof(MENUSTRIP), MEMF_ANY);
    if (ms == NULL) {
	return (NULL);
    } /* if */

    setmem (ms, sizeof(MENUSTRIP), 0);
    ms->node.ln_Name = DupFunc (name, MEMF_ANY);
    if (ms->node.ln_Name == NULL) {
	FreeFunc (ms, sizeof (MENUSTRIP));
	return (NULL);
    } /* if */

    AddTail ((struct List *)&MenuStrips, &ms->node);

/* newvalue: */

    if (defaults) {
	def_menus (ms);
    } /* if */

    return (ms);
} /* new_menustrip */



/*****************************************************************************

    NAME
	init_menustrips
	exit_menustrips

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	init and exit functions for menustrips

    NOTES
	useful only, if we are using multiple menustrips

	THESE are __AUTOEXIT/__AUTOEXIT - functions !!!!

    HISTORY
	27 Jan 1993 b_null created

******************************************************************************/


MK_AUTOINIT( init_menustrips )
{
    NewList	  ((struct List*)&MenuStrips);
    new_menustrip ("default",1);
} /* init_menustrips */


MK_AUTOEXIT( exit_menustrips )
{
    MENUSTRIP * ms;

/* PutStr ("menu_exiting\n"); */
    while ((ms = GetHead (&MenuStrips))) {
	delete_menustrip (ms, 1);
    } /* while */
/* PutStr ("menu_exited\n"); */
} /* exit_menustrips */




/******************************************************************************
*****  ENDE menustrips.c
******************************************************************************/
