/******************************************************************************

    MODUL
	menucom.c

    DESCRIPTION
	[X]DME command interface for menubase.c

    NOTES
	experimental status

	in zukunft koennte es moeglich sein,
	einen einzigen namensparameter zu akzeptieren,
	der dann ueber split_menu_names in seine
	komponenten aufgespalten wuerde

    BUGS
	IMPORTANT
	    we cannot actually react on ERRORs !!

    TODO
	include some error()-calls
	support checks

    EXAMPLES

    description of the user-commands:

	the commands below might get other names;
	if so, please update their descriptions according
	to the names they are used in command.c

*!***************************************************************************
*!
*!  MENUES:
*!  ******
*!
*!  the following commands allow access to [X]DME's menustructures
*!  that way it is possible for each user to create an individual
*!  GUI for "his" [X]DME, for "his" programming language and for
*!  his wishes.
*!
*! >MENUDELHDR menuname
*!
*!	delete a menuheader and all associated items (and subitems)
*!
*!	menuheaders are created by using their names in item or
*!	subitem definitions
*!
*! >MENUDEL    menuname itemname
*! >MENUDELBAR menuname 			    (obsolete)
*!
*!	delete a menuitem-entry
*!	DELBAR deletes the first itembar in a menu's itemlist
*!	DEL deletes the menuitem with the name itemname
*!	    if itemname is a subitem-carrier, all subs are deleted, too
*!
*!	You can delete all items of a menu, if You call MENUDELHDR
*!
*! >MENUDELSUB	  menuname itemname subname	    (obsolete)
*! >MENUDELSUBBAR menuname itemname		    (obsolete)
*!
*!	delete a submenu-entry
*!	DELSUBBAR deletes the first submenubar in a menuitem's subitemlist
*!	DELSUB deletes the submenu with the name subname
*!
*!	You can delete all an item's subitem if You MENUDEL the carrying item
*!
*! >MENUADD   menuname itemname subname command
*! >MENUBAR   menuname itemname 		    (obsolete)
*! >MENUCHECK menuname itemname subname command     (obsolete)
*!
*!	create a menuitem-entry
*!	BAR simply appends an itembar to the item-list
*!	ADD creates a full menuitem-entry, if not already one
*!	    of the same name exists
*!	CHECK also sets the menutoggle flag to an entry created
*!	    like ADD
*!
*!	You cannot add subitems to items that were created with
*!	    MENUADD or MENUCHECK as these have commands added
*!	    while we expect subitem carrier to have no command added
*!	You cannot redefine the type of an menuitem once created
*!	    normal items can't be redefined to checkitems
*!	    or subitem carriers and vice versa
*!
*!	Since 22 Jun 1994 MENUADD incorporates MENU*BAR, MENU*CHECK, MENU*ADD
*!	    so MENUSUB(ADD|BAR|CHECK) are not really necessary any more
*!	    SUBnames	   are splitted at ^S,
*!	    AMIGAshortcuts are splitted at ^A,
*!	    CHECKitems	   are leaded	by ^C (at subs the last/sub name),
*!	    BARitems	   are called	   ^B (at subs the last/sub name)
*!
*!
*! >MENUSUBADD	 menuname itemname subname command  (obsolete)
*! >MENUSUBBAR	 menuname itemname		    (obsolete)
*! >MENUSUBCHECK menuname itemname subname command  (obsolete)
*!
*!	create a submenu-entry
*!	SUBBAR simply appends an itembar to the subitem-list
*!	SUBADD creates a full subitem-entry, if not already one
*!	    of the same name exists
*!	SUBCHECK also sets the menutoggle flag to an entry created
*!	    like SUBADD
*!
*!	You cannot add subitems to items that were created with
*!	    MENUADD or MENUCHECK as these are managed in a quite
*!	    different way, so You create subitem-carrier simply by
*!	    using their names in a subitem definition
*!	You cannot redefine the type of an subitem once created
*!	    normal subs can't be redefined to subcheck and vice versa
*!	Since 22 Jun 1994 MENU*ADD incorporates MENU*BAR, MENU*CHECK, MENU*ADD
*!	    so MENU*(BAR|CHECK) are not really necessary any more
*!	    since MENUADD also handles subs, MENUSUBADD is obsolete, too
*!
*! >MENUSETITEM menuname itemname	  status
*! >MENUSETSUB	menuname itemname subname status	(obsolete)
*! >MENUCHKITEM menuname itemname	  variablename
*! >MENUCHKSUB	menuname itemname subname variablename	(obsolete)
*!
*!	these commands set or check the satus of the (/sub)item-checked
*!	flags for (sub)items created with the MENU(SUB)CHECK commands
*!	status is one of 0|1/on|off the value set into variable is also 0|1
*!	    (again, please note, "toggle" is not yet supported)
*!	Since 22 Jun 1994 MENU???ITEM is able to split its itemname at
*!	    SUBBREAK=^S, so MENU???SUB are not really neccessary any more.
*!
*! >MENUCLEAR
*!
*!	clean up the menustrip, i.e. delete all of its
*!	menuheaders
*!
*! >MENUSAVE filename
*!
*!	save the menustrip as a file that can be
*!	read by MENULOAD
*!
*! >MENULOAD filename
*!
*!	clear the menustrip and instead build a new
*!	contents out of the file filename
*!
*! >MENUON
*! >MENUOFF
*!
*!	show or hide the whole menustrip
*!	MENUOFF is stackable, i.e You need as many MENUON calls
*!	to show the menustrip as previously MENUOFF calls
*!
*!
*!  MENUSTRIPS:
*!  **********
*!
*!  the following commands do only make sense, if
*!  You are using multiple menustrips
*!  at [X]DME's start a menustrip called "default" is created;
*!  for "normal" usage, that single strip may suffer.
*!
*!  Please note that the above MENU... commands apply to the
*!  CURRENT menustrip (and only to it)
*!
*! >USEMENUSTRIP name
*!
*!	search for a certain menustrip and use it as the current one
*!
*! >REMMENUSTRIP
*!
*!	delete the current menustrip, if it is not the only one
*!
*! >NEWMENUSTRIP name
*!
*!	if there is already a menustrip called name,
*!	    simply call USEMENUSTRIP name
*!	else create a new menustrip called name and use it
*!
*! NOTES about Menus
*! -----------------
*!
*! CHANGE at 22 Jun 1994
*! some commands become obsolete, but stay accessible for some patchlevels
*! (for compability reasons)
*! MENUSUBADD  replaces MENUSUBCHECK, MENUSUBBAR by ^C, ^B
*! MENUADD     replaces MENUCHECK,    MENUBAR	 by ^C, ^B
*! MENUDELSUB  replaces MENUDELSUBBAR		 by	^B
*! MENUDEL     replaces MENUDELSUB		 by	^B
*! MENUADD     replaces MENUSUBADD		 by	   ^S
*! MENUDEL     replaces MENUDELSUB		 by	   ^S
*! MENUSETITEM replaces MENUSETSUB		 by	   ^S
*! MENUCHKITEM replaces MENUCHKSUB		 by	   ^S
*! so MENUADD, MENUDEL, MENUSETITEM, MENUCHKITEM should be enough
*! the other commands might stay usable for a while, however, but they
*! are not supported any more.
*! please note, that we are introducing another Specialkey ^A to identify
*! Amiga-Shortcuts
*! please note, that MENUCHECK will not support ^A
*! also note, that ^A, ^C are not really part of an Item's name, so
*! menuitems must be deleted with ^C/^A stripped off.
*!
*! Finally I have to mention, that the variable access to menuitems
*! does currently not use the same break-symbols as menuadd does;
*! in var-access we still use "-" to separate itemnames from
*! menunames and from subnames (for compability reasons). (if that
*! functionality changes, we'll have to note it)
*!
*! as soon, as these obsolete declared commands are dropped
*! we will support a set of macros in order to emulate them
*! with the remaining commands, which offer even more functionality
*!

    SEE ALSO
	menustrips.c menu_dme.c menucontrol.c command.c


    INDEX
	$Header : $

    HISTORY
	18 Dec 1992 b_null created
	20 Dec 1992 b_null rewritten & documented
	23 Jan 1993 b_null added menutomacro
	25 Jan 1993 b_null added do_delmenu, do_itemcheck, do_subcheck
	22 Jun 1994 null added support of check_symbol, short_cut
	05-08-94    null added STATIC Command Interface
	$Date$ last update

******************************************************************************/

/**************************************
		Includes
**************************************/
#define  MENU_INTERNAL
#include "defs.h"
#include "menubase.h"
#ifdef PATCH_NULL
#include "COM.h"
#endif


/**************************************
	    Globale Variable
**************************************/
/* menu(item)management */
Prototype void do_delmenu   (void);
Prototype void do_delitem   (void);
Prototype void do_delitembar(void);
Prototype void do_delsub    (void);
Prototype void do_delsubbar (void);
Prototype void do_itembar   (void);
Prototype void do_itemcheck (void);
Prototype void do_itemadd   (void);
Prototype void do_subadd    (void);
Prototype void do_subcheck  (void);
Prototype void do_subbar    (void);
Prototype void do_menuoff   (void);
Prototype void do_menuon    (void);
Prototype void do_menuload  (void);
Prototype void do_menusave  (void);
Prototype char*menutomacro  (char*);
Prototype void do_menuclear (void);

/* modification on checks */
Prototype void do_getcheckitem (void);
Prototype void do_getchecksub  (void);
Prototype void do_setcheckitem (void);
Prototype void do_setchecksub  (void);

/* management for menustrips */
Prototype void do_new_menustrip (void);
Prototype void do_del_menustrip (void);
Prototype void do_use_menustrip (void);


/**************************************
      Interne Defines & Strukturen
**************************************/

#define    SPLIT_CHECK_NAME(pn)     ((**pn==checkmark_Symbol) && ((*pn)++))
#define SPLIT_SHORTCUT_NAME(pn,ps)  ((*ps=strchr(*pn,scutbreak_Symbol)) && ((**ps=0)||(*ps)++))
#define  SPLIT_SUBITEM_NAME(pn,ps)  ((*ps=strchr(*pn, subbreak_Symbol)) && ((**ps=0)||(*ps)++))
#define 	IS_BAR_NAME(pn)     ((strcmp (*pn, barlabel_Symbol)==0)&&(*pn=BAR))


/**************************************
	    Interne Variable
**************************************/

/* fuer diese werte koennte ich sogar prefs-commandos basteln */
char checkmark_Symbol = '\003'; /* in breakout: ^C for _C_heck    */
char  subbreak_Symbol = '\023'; /* in breakout: ^S for _S_ub      */
char scutbreak_Symbol = '\001'; /* in breakout: ^A for _A_miga    */
char *barlabel_Symbol = "\002"; /* in breakout: ^B for _B_arlabel */

/**************************************
	   Interne Prototypes
**************************************/



/*****************************************************************************

    NAME
	do_delmenu

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	25 Jan 1993 b_noll created

******************************************************************************/

void do_delmenu (void)
{
    menuoff (currentmenu(), currentwindow());
    menudel (currentmenu(), GetArg(1));
    menuon  (currentmenu(), currentwindow());
} /* do_delmenu */



/*****************************************************************************

    NAME
	do_itemadd

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES
	major change due to suggestion of Karl ...

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created
	22 Jun 1994 b_noll enhanced functionality - is now also subadd, check, bar,...

******************************************************************************/

void do_itemadd (void)
{
    char *iname, *sname, *scut;
    int   chk;

    menuoff (currentmenu(), currentwindow());

    iname = GetArg(2);
    SPLIT_SHORTCUT_NAME(&iname, &scut);

    if (SPLIT_SUBITEM_NAME(&iname, &sname)) {
	chk = SPLIT_CHECK_NAME(&sname);
	if (IS_BAR_NAME(&sname)) {
	    chk = 0;
	    scut = NULL;
	} /* if */
	subadd (currentmenu(), GetArg(1), iname, sname, GetArg(3), NULL, chk, scut);
    } else {
	chk = SPLIT_CHECK_NAME(&iname);
	if (IS_BAR_NAME(&iname)) {
	    chk = 0;
	    scut = NULL;
	} /* if */
	itemadd (currentmenu(), GetArg(1), iname,       GetArg(3), NULL, chk, scut);
    } /* if */

    menuon  (currentmenu(), currentwindow());
} /* do_itemadd */



/*****************************************************************************

    NAME
	do_itemcheck

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	25 Jan 1993 b_noll created

******************************************************************************/

void do_itemcheck (void)
{
    char *iname, *sname;

    menuoff (currentmenu(), currentwindow());

    iname = GetArg(2);
    if (SPLIT_SUBITEM_NAME(&iname, &sname)) {
	if (IS_BAR_NAME(&sname));
	subadd	(currentmenu(), GetArg(1), iname, sname, GetArg(3), NULL, 1, NULL);
    } else {
	if (IS_BAR_NAME(&iname));
	itemadd (currentmenu(), GetArg(1), iname,        GetArg(3), NULL, 1, NULL);
    } /* if */
    menuon  (currentmenu(), currentwindow());
} /* do_itemcheck */



/*****************************************************************************

    NAME
	do_itembar

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created

******************************************************************************/

void do_itembar (void)
{
    menuoff (currentmenu(), currentwindow());
    itemadd (currentmenu(), GetArg(1), BAR, NULL, NULL, 0, NULL);
    menuon  (currentmenu(), currentwindow());
} /* do_itembar */



/*****************************************************************************

    NAME
	do_subadd

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created
	22 Jun 1994 b_noll enhanced functionality

******************************************************************************/

void do_subadd (void)
{
    int chk;
    char *sname, *scut;
    menuoff (currentmenu(), currentwindow());

    sname = GetArg(3);
       SPLIT_SHORTCUT_NAME(&sname, &scut);
    chk = SPLIT_CHECK_NAME(&sname);
    if (IS_BAR_NAME(&sname)) {
	chk = 0;
	scut = NULL;
    } /* if */
    subadd  (currentmenu(), GetArg(1), GetArg(2), sname, GetArg(4), NULL, chk, scut);
    menuon  (currentmenu(), currentwindow());
} /* do_subadd */



/*****************************************************************************

    NAME
	do_subcheck

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	25 Jan 1993 b_noll created

******************************************************************************/

void do_subcheck (void)
{
    menuoff (currentmenu(), currentwindow());
    subadd  (currentmenu(), GetArg(1), GetArg(2), GetArg(3), GetArg(4), NULL, 1, NULL);
    menuon  (currentmenu(), currentwindow());
} /* do_subcheck */



/*****************************************************************************

    NAME
	do_subbar

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created

******************************************************************************/

void do_subbar (void)
{
    menuoff (currentmenu(), currentwindow());
    subadd  (currentmenu(), GetArg(1), GetArg(2), BAR, NULL, NULL, 0, NULL);
    menuon  (currentmenu(), currentwindow());
} /* do_subbar */



/*****************************************************************************

    NAME
	do_delitem

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created
	22 Jun 1994 b_noll enhanced functionality

******************************************************************************/

void do_delitem (void)
{
    char *iname, *sname;
    menuoff (currentmenu(), currentwindow());
    iname = GetArg(2);

    if (SPLIT_SUBITEM_NAME(&iname, &sname)) {
	if (IS_BAR_NAME(&sname));
	subdel	(currentmenu(), GetArg(1), iname, sname);
    } else {
	if (IS_BAR_NAME(&iname));
	itemdel (currentmenu(), GetArg(1), iname);
    } /* if */
    menuon  (currentmenu(), currentwindow());
} /* do_delitem */



/*****************************************************************************

    NAME
	do_delitembar

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	22 Dec 1992 b_noll created

******************************************************************************/

void do_delitembar (void)
{
    menuoff (currentmenu(), currentwindow());
    itemdel (currentmenu(), GetArg(1), BAR);
    menuon  (currentmenu(), currentwindow());
} /* do_delitembar */



/*****************************************************************************

    NAME
	do_delsub

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created
	22 Jun 1994 b_noll enhanced functionality

******************************************************************************/

void do_delsub (void)
{
    char *sname;
    menuoff (currentmenu(), currentwindow());
    sname = GetArg(3);
    if (IS_BAR_NAME(&sname));
    subdel  (currentmenu(), GetArg(1), GetArg(2), sname);
    menuon  (currentmenu(), currentwindow());
} /* do_del */



/*****************************************************************************

    NAME
	do_delsubbar

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	22 Dec 1992 b_noll created

******************************************************************************/

void do_delsubbar (void)
{
    menuoff (currentmenu(), currentwindow());
    subdel  (currentmenu(), GetArg(1), GetArg(2), BAR);
    menuon  (currentmenu(), currentwindow());
} /* do_delsubbar */



/*****************************************************************************

    NAME
	do_menuclear

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created

******************************************************************************/

void do_menuclear (void)
{
    menuoff   (currentmenu(), currentwindow());
    menuclear (currentmenu());
    menuon    (currentmenu(), currentwindow());
} /* do_menuclear */



/*****************************************************************************

    NAME
	do_menuon

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created

******************************************************************************/

void do_menuon (void)
{
    menuon (currentmenu(), currentwindow());
} /* do_menuon */



/*****************************************************************************

    NAME
	do_menuoff

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created

******************************************************************************/

void do_menuoff (void)
{
    menuoff (currentmenu(), currentwindow());
} /* do_menuoff */



/*****************************************************************************

    NAME
	do_menusave

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created

******************************************************************************/

void do_menusave (void)
{
    menuoff   (currentmenu(), currentwindow());
    menusave  (currentmenu(), GetArg(1));
    menuon    (currentmenu(), currentwindow());
} /* do_menusave */



/*****************************************************************************

    NAME
	do_menuload

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME command interface for

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	20 Dec 1992 b_noll created

******************************************************************************/

void do_menuload (void)
{
    menuoff   (currentmenu(), currentwindow());
    menuload  (currentmenu(), GetArg(1));
    menuon    (currentmenu(), currentwindow());
} /* do_menuload */



/*****************************************************************************

    NAME
	do_menutomacro

    PARAMETER
	char * str

    RESULT
	the command that is attached to the menu referred with str

    RETURN
	char *

    DESCRIPTION
	[X]DME vars interface for menu2macro

    NOTES
	that function was added 'cause in vars.c we use a
	table of all search-functions, and all the others
	do have only 1 argument

    BUGS
	never tested

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	22 Jan 1993 b_noll created

******************************************************************************/

char * menutomacro (char * str)
{
    return (menu2macro (currentmenu(), str));
} /* menutomacro */



/*****************************************************************************

    NAME
	do_getcheckitem

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME vars interface for

    NOTES

    BUGS
	never tested

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	25 Jan 1993 b_noll created
	22 Jun 1994 b_noll enhanced functionality

******************************************************************************/

void do_getcheckitem (void)
{
    int  v;
    char vv[2] = "0";
    char *iname, *sname;

    iname = GetArg(2);
    if (SPLIT_SUBITEM_NAME(&iname, &sname)) {
	v = chksubcheck  (currentmenu(), GetArg(1), iname, sname);
    } else {
	v = chkitemcheck (currentmenu(), GetArg(1), iname);
    } /* if */

    if (v >= 0) {
	vv[0] = v+48;
	SetTypedVar (GetArg(3), vv, VAR_GV);
    } else {
	error ("%s:\n no check item of that name:\n %s-%s", CommandName(), GetArg(1), GetArg(2));
    } /* if */
} /* do_getcheckitem */



/*****************************************************************************

    NAME
	do_getchecksub

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME vars interface for

    NOTES

    BUGS
	never tested

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	25 Jan 1993 b_noll created

******************************************************************************/

void do_getchecksub (void)
{
    int  v;
    char vv[2] = "0";

    v = chksubcheck (currentmenu(), GetArg(1), GetArg(2), GetArg(3));

    if (v >= 0) {
	vv[0] = v+48;
	SetTypedVar (GetArg(4), vv, VAR_GV);
    } else {
	error ("%s:\n no check sub item of that name:\n %s-%s-%s", CommandName(), GetArg(1), GetArg(2), GetArg(3));
    } /* if */
} /* do_getchecksub */



/*****************************************************************************

    NAME
	do_setcheckitem

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME vars interface for

    NOTES

    BUGS
	never tested
	toggle wont work

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	25 Jan 1993 b_noll created
	22 Jun 1994 b_noll enhanced functionality

******************************************************************************/

void do_setcheckitem (void)
{
    int v;

    char *iname, *sname;
    iname = GetArg(2);
    if (SPLIT_SUBITEM_NAME(&iname, &sname))
	v = setsubcheck (currentmenu(),  GetArg(1), iname, sname, test_arg(GetArg(3),1));
    else
	v = setitemcheck (currentmenu(), GetArg(1), iname,        test_arg(GetArg(3),1));

    if (v != RET_SUCC) {
	error ("%s:\n no check item of that name:\n %s-%s", CommandName(), GetArg(1), GetArg(2));
    } else {
	menu_strip (currentmenu(), Ep->win);
    } /* if */
} /* do_setcheckitem */



/*****************************************************************************

    NAME
	do_setchecksub

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	[X]DME vars interface for

    NOTES

    BUGS
	never tested
	toggle wont work

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	25 Jan 1993 b_noll created

******************************************************************************/

void do_setchecksub (void)
{
    int v;
    v = setsubcheck (currentmenu(), GetArg(1), GetArg(2), GetArg(3), test_arg(GetArg(4),1));
    if (v == RET_FAIL) {
	error ("%s:\n no check sub item of that name:\n %s-%s-%s", CommandName(), GetArg(1), GetArg(2), GetArg(3));
    } else {
	menu_strip (currentmenu(), Ep->win);
    } /* if */
} /* do_setchecksub */


/*****************************************************************************

    NAME
	do_new_menustrip

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyaddes/qualifier

    NOTES
	<never tested>

    BUGS
	<none known>

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	28 Jan 1993  b_null created

******************************************************************************/

void do_new_menustrip (void)
{
    MENUSTRIP * ms = new_menustrip (GetArg(1), 1);

    if (ms) {
	/* menuoff (currentmenu(), Ep->win); */
	Ep->menustrip = ms;
	menu_strip (currentmenu(), Ep->win);
	/* menuon  (currentmenu(), Ep->win); */
    } /* if */
} /* do_new_menustrip */



/*****************************************************************************

    NAME
	do_del_menustrip

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyaddes/qualifier

    NOTES
	<never tested>
	that function is highly dangerous!


    BUGS
	<none known>

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	28 Jan 1993  b_null created

******************************************************************************/

void do_del_menustrip (void)
{
    MENUSTRIP * ms = currentmenu ();
    MENUSTRIP * ns;
    ED	      * ep;

    menuoff (ms, Ep->win);          /* remove all strips */

    delete_menustrip (ms, 0);
    ns = get_menustrip (NULL);

    for (ep = GetHead (&DBase); ep; ep = GetSucc (ep)) {
	if (ep->menustrip == ms) {
	    ep->menustrip = NULL; /* ns */
	} /* if */
    } /* for */

    menuoff (ns, Ep->win);          /* this is def_too != 0 since */
    menuon  (ns, Ep->win);          /* ns == get_menustrip(NULL)  */
} /* do_del_menustrip */



/*****************************************************************************

    NAME
	do_use_menustrip

    PARAMETER
	void

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	command interface for keyaddes/qualifier

    NOTES
	<never tested>

    BUGS
	<none known>

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	28 Jan 1993  b_null created

******************************************************************************/

void do_use_menustrip (void)
{
    MENUSTRIP * ms = get_menustrip (GetArg (1));

    if (ms) {
	Ep->menustrip = ms;
	menu_strip (ms, Ep->win);
    } /* if */
} /* do_use_menustrip */





/*****************************************************************************

    NAME
	menufind
	menugethelp
	menusethelp
	menugetcommand
	menusetcommand
	menucall

    PARAMETER

    RESULT

    RETURN

    DESCRIPTION
	interfaces to support handles used by the
	STATIC Command Interface
	in order to enable use of COMTREE menu
	as well as VARTREE menucomm/menuhelp

    NOTES
	<never tested>

    BUGS
	<none known>

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	05-08-94 null created

******************************************************************************/

#ifdef STATIC_COM


Prototype APTR menufind (char *name);
APTR menufind (char *name) {
    return findmenu(currentmenu(), name);
} /* menufind */

Prototype char *menugethelp(APTR handle);
char *menugethelp(APTR handle) {
    return ((XITEM *)handle)->help;
} /* menugethelp */

Prototype char *menugetcommand(APTR handle);
char *menugetcommand(APTR handle) {
    return ((XITEM *)handle)->com;
} /* menugetcommand */

Prototype char *menusetcommand(APTR handle, char *val);
char *menusetcommand(APTR handle, char *val) {
    char *ptr;
    if (ptr = DupFunc(val)) {
	DeallocFunc(((XITEM *)handle)->com);
	((XITEM *)handle)->com = ptr;
	return TRUE;
    } /* if */
    nomemory();
    return FALSE;
} /* menusetcommand */

Prototype char *menusethelp (APTR handle, char *val);
char *menusethelp (APTR handle, char *val) {
    char *ptr;
    if (ptr = DupFunc(val)) {
	DeallocFunc(((XITEM *)handle)->help);
	((XITEM *)handle)->help = ptr;
	return TRUE;
    } /* if */
    nomemory();
    return FALSE;
} /* menusethelp */

Prototype int menucall (APTR handle);
int menucall (APTR handle) {
    char *ptr;
    if (ptr = strdup(menugetcommand(handle))) {
	retval = do_command(ptr);
	free (ptr);
	return OK;
    } /* if */
    nomemory();
    return FALSE;
} /* menucall */

#endif /* STATIC_COM */

#ifdef PATCH_NULL

DEFUSERCMD("menuadd", 3, CF_VWM|CF_ICO, void, do_itemadd, (void),;)
DEFUSERCMD("menubar", 1, CF_VWM|CF_ICO, void, do_itembar, (void),;)
DEFUSERCMD("menucheck", 3, CF_VWM|CF_ICO, void, do_itemcheck, (void),;)
DEFUSERCMD("menuclear", 0, CF_VWM|CF_ICO, void, do_menuclear, (void),;)
DEFUSERCMD("menudel", 2, CF_VWM|CF_ICO, void, do_delitem, (void),;)
DEFUSERCMD("menudelbar", 1, CF_VWM|CF_ICO, void, do_delitembar, (void),;)
DEFUSERCMD("menudelhdr", 1, CF_VWM|CF_ICO, void, do_delmenu, (void),;)
DEFUSERCMD("menudelsub", 3, CF_VWM|CF_ICO, void, do_delsub, (void),;)
DEFUSERCMD("menudelsubar", 2, CF_VWM|CF_ICO, void, do_delsubbar, (void),;)
DEFUSERCMD("menugetcheck", 3, CF_VWM|CF_ICO, void, do_getcheckitem, (void),;)
DEFUSERCMD("menugetscheck", 4, CF_VWM|CF_ICO, void, do_getchecksub, (void),;)
DEFUSERCMD("menuload", 1, CF_VWM|CF_ICO, void, do_menuload, (void),;)
DEFUSERCMD("menuoff", 0, CF_VWM|CF_ICO, void, do_menuoff, (void),;)
DEFUSERCMD("menuon", 0, CF_VWM|CF_ICO, void, do_menuon, (void),;)
DEFUSERCMD("menusave", 1, CF_VWM|CF_ICO, void, do_menusave, (void),;)
DEFUSERCMD("menusetcheck", 3, CF_VWM|CF_ICO, void, do_setcheckitem, (void),;)
DEFUSERCMD("menusetscheck", 4, CF_VWM|CF_ICO, void, do_setchecksub, (void),;)
DEFUSERCMD("menusubadd", 4, CF_VWM|CF_ICO, void, do_subadd, (void),;)
DEFUSERCMD("menusubbar", 2, CF_VWM|CF_ICO, void, do_subbar, (void),;)
DEFUSERCMD("menusubcheck", 4, CF_VWM|CF_ICO, void, do_subcheck, (void),;)
DEFUSERCMD("newmenustrip", 1, CF_VWM|CF_ICO|CF_COK, void, do_new_menustrip, (void),;)
DEFUSERCMD("usemenustrip", 1, CF_VWM|CF_ICO|CF_COK, void, do_use_menustrip, (void),;)

/* **************************************************
	STATIC XDME Command Interface
************************************************** //

//	    Name      #Args Function	      Flags
:FX:COMMAND menuadd	  3 do_itemadd
:FX:COMMAND menubar	  1 do_itembar
:FX:COMMAND menucheck	  3 do_itemcheck
:FX:COMMAND menuclear	  0 do_menuclear
:FX:COMMAND menudel	  2 do_delitem
:FX:COMMAND menudelbar	  1 do_delitembar
:FX:COMMAND menudelhdr	  1 do_delmenu
:FX:COMMAND menudelsub	  3 do_delsub
:FX:COMMAND menudelsubar  2 do_delsubbar
:FX:COMMAND menugetcheck  3 do_getcheckitem
:FX:COMMAND menugetscheck 4 do_getchecksub
:FX:COMMAND menuload	  1 do_menuload
:FX:COMMAND menuoff	  0 do_menuoff
:FX:COMMAND menuon	  0 do_menuon
:FX:COMMAND menusave	  1 do_menusave
:FX:COMMAND menusetcheck  3 do_setcheckitem
:FX:COMMAND menusetscheck 4 do_setchecksub
:FX:COMMAND menusubadd	  4 do_subadd
:FX:COMMAND menusubbar	  2 do_subbar
:FX:COMMAND menusubcheck  4 do_subcheck
:FX:COMMAND newmenustrip  1 do_new_menustrip
:FX:COMMAND usemenustrip  1 do_use_menustrip
//X:COMMAND remmenustrip  1 do_del_menustrip	// that function is not stable yet


//	    Name   Pri Lock	    Unlock   Call	    NArgs
:FX:COMTREE menu     1 menufind     NULL     Call=menucall  NULL

//	    Name   Pri Lock	    Unlock   Get		Set		    Unset
:FX:VARTREE menu     1 menufind     NULL     Get=menugetcommand Set=menusetcommand  Unset=NULL
:FX:VARTREE menuhelp 1 menufind     NULL     Get=menugethelp	Set=menusethelp     Unset=NULL

// **************************************************
	DYNAMIC XDME Command Interface
************************************************** */

static const
struct CommandNode MENU_Commands[] = {
    COMMAND("menuadd",        3,        CF_VWM|CF_ICO, (FPTR)do_itemadd  )
    COMMAND("menubar",        1,        CF_VWM|CF_ICO, (FPTR)do_itembar  )
    COMMAND("menucheck",      3,        CF_VWM|CF_ICO, (FPTR)do_itemcheck    )
    COMMAND("menuclear",      0,        CF_VWM|CF_ICO, (FPTR)do_menuclear    )
    COMMAND("menudel",        2,        CF_VWM|CF_ICO, (FPTR)do_delitem  )
    COMMAND("menudelbar",     1,        CF_VWM|CF_ICO, (FPTR)do_delitembar   )
    COMMAND("menudelhdr",     1,        CF_VWM|CF_ICO, (FPTR)do_delmenu  )
    COMMAND("menudelsub",     3,        CF_VWM|CF_ICO, (FPTR)do_delsub   )
    COMMAND("menudelsubar",   2,        CF_VWM|CF_ICO, (FPTR)do_delsubbar    )
    COMMAND("menugetcheck",   3,        CF_VWM|CF_ICO, (FPTR)do_getcheckitem )
    COMMAND("menugetscheck",  4,        CF_VWM|CF_ICO, (FPTR)do_getchecksub  )
    COMMAND("menuload",       1,        CF_VWM|CF_ICO, (FPTR)do_menuload ) /* tester */
    COMMAND("menuoff",        0,        CF_VWM|CF_ICO, (FPTR)do_menuoff  )
    COMMAND("menuon",         0,        CF_VWM|CF_ICO, (FPTR)do_menuon   )
    COMMAND("menusave",       1,        CF_VWM|CF_ICO, (FPTR)do_menusave ) /* tester */
    COMMAND("menusetcheck",   3,        CF_VWM|CF_ICO, (FPTR)do_setcheckitem )
    COMMAND("menusetscheck",  4,        CF_VWM|CF_ICO, (FPTR)do_setchecksub  )
    COMMAND("menusubadd",     4,        CF_VWM|CF_ICO, (FPTR)do_subadd   )
    COMMAND("menusubbar",     2,        CF_VWM|CF_ICO, (FPTR)do_subbar   )
    COMMAND("menusubcheck",   4,        CF_VWM|CF_ICO, (FPTR)do_subcheck )
    COMMAND("newmenustrip",   1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_new_menustrip    )
 /* COMMAND("remmenustrip",   1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_del_menustrip    ) */ /* that function is not stable yet */
    COMMAND("usemenustrip",   1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_use_menustrip    )
};


MK_AUTOINIT( MENU_init )
{
    int i;

    for (i = sizeof (MENU_Commands)/sizeof (struct CommandNode) - 1;i >= 0; --i)
	COM_Add (&MENU_Commands[i]);

 /* COM_AddClass()   ->menutomacro */
 /* COM_AddVarClass()->menutomacro */
} /* MENU_init */


MK_AUTOEXIT( MENU_Exit )
{
    int  i;
    APTR lock;

    for (i = sizeof (MENU_Commands)/sizeof (struct CommandNode) - 1; (i >= 0); DEC(i))
	if ((lock = COM_Lock (MENU_Commands[i].cn_Name)))
	    COM_Remove (lock);
} /* MENU_Exit */
#endif



/******************************************************************************
*****  ENDE menucom.c
******************************************************************************/
