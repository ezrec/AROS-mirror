
/******************************************************************************

    MODULE
	AppIcon.c

    DESCRIPTION
	XDME Command Module

	to form a Workbench AppIcon for the XDME process

    NOTES
	the "COMMAND"-lines have a specail meaning (see STATIC_COM)
	the SPC_VAR-section has a special meaning (see there)

    BUGS
	none known

    TODO
	tell me

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	21-09-94 b_noll created

******************************************************************************/


/**************************************
	      Includes
**************************************/

#include "defs.h"
/* #include "COM.h" */
/* #include "AUTO.h" */

#include <workbench/icon.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>

#include <proto/icon.h>
#include <proto/workbench.h>


/**************************************
	    Global Variables
**************************************/

Prototype UBYTE 	    *APIC_IconTitle;
Prototype UBYTE 	    *APIC_IconFile;
Prototype UBYTE 	    *APIC_ActionDblClick;
Prototype UBYTE 	    *APIC_ActionDrop;
//Prototype UBYTE	       APIC_Active;
Prototype ULONG 	     APIC_SigMask;

/**************************************
      Internal Defines & Structures
**************************************/

#define APIC_OK 0
#define APIC_NOICON	    -1
#define APIC_NODISKOBJECT   -2
#define APIC_NOPORT	    -3
#define APIC_max

#define APIC_BufferSize 500

/**************************************
	   Internal Variables
**************************************/

struct DiskObject *APIC_DiskObject = NULL;
APTR		   APIC_Icon	   = NULL;
struct MsgPort	  *APIC_Port	   = NULL;

UBYTE		  *APIC_IconTitle	= NULL;
UBYTE		  *APIC_IconFile	= NULL;
UBYTE		  *APIC_ActionDblClick	= NULL;
UBYTE		  *APIC_ActionDrop	= NULL;

//UBYTE 	     APIC_Active      = 0;
ULONG		   APIC_SigMask     = 0;

/**************************************
	   Internal Prototypes
**************************************/


/**************************************
		 Macros
**************************************/

#define APIC_DefTitle	    "XDME"
#define APIC_DefIconFile    "XDME"

#define APIC_DefDblClick    "newwindow arpload"
#define APIC_DefDrop	    "newwindow newfile `%s'"

/**************************************
	     Implementation
**************************************/



int APIC_Hide (void) {
    if (!APIC_Icon)
	return APIC_NOICON;

    if (APIC_Icon)
	RemoveAppIcon(APIC_Icon);
    APIC_Icon = NULL;

    if (APIC_DiskObject)
	FreeDiskObject(APIC_DiskObject);
    APIC_DiskObject = NULL;

    APIC_Active = 0;
    return APIC_OK;
} /* APIC_Hide */


int APIC_Show (const UBYTE * iconfile, const UBYTE *icontitle) {
    if (!APIC_Port)
	return APIC_NOPORT;

    if ((APIC_DiskObject = GetDiskObject (APIC_IconFile)) == NULL) {
	if (!getappname(tmp_buffer, sizeof(tmp_buffer)) ||
	   ((APIC_DiskObject = GetDiskObject (tmp_buffer)) == NULL)) {
	    if ((APIC_DiskObject = GetDefDiskObject (WBTOOL)) == NULL) {
		APIC_Hide();
		return APIC_NODISKOBJECT;
	    } /* if */
	} /* if */
    } /* if */

    APIC_Icon = AddAppIconA (NULL, NULL, APIC_IconTitle, APIC_Port, NULL, APIC_DiskObject, NULL);

    if (!APIC_Icon ) {
	APIC_Hide();
	return APIC_NOICON;
    } /* if */

    APIC_Active = 1;
    return APIC_OK;
} /* APIC_Show */






/*************************************************
	Entry & Exit Code
*************************************************/

Prototype void APIC_Terminate (void);
void APIC_Terminate (void) {

    /* ---- hide the icon */
    APIC_Hide();

    { /* ---- drop all waiting messages */
	struct Message *msg;
	while (msg = GetMsg(APIC_Port)) {
	    ReplyMsg (msg);
	} /* while */
    }

    /* ---- close the port */
    if (APIC_Port)
	DeleteMsgPort(APIC_Port);
    APIC_Port = NULL;

} /* APIC_Terminate */


Prototype void APIC_Initialize (void);
void APIC_Initialize (void) {
    APIC_Port		= CreateMsgPort();
    APIC_IconTitle	= strdup (APIC_DefTitle);
    APIC_IconFile	= strdup (APIC_DefIconFile);
    APIC_ActionDrop	= strdup (APIC_DefDrop);
    APIC_ActionDblClick = strdup (APIC_DefDblClick);

    if (!APIC_Port || !APIC_IconTitle || !APIC_IconFile || !APIC_ActionDrop || !APIC_ActionDblClick ) {
	APIC_Terminate();
	return;
    } /* if */

    APIC_SigMask = 1 << APIC_Port->mp_SigBit;

} /* APIC_Initialize */


/*************************************************
	Application Interface
*************************************************/


/*
** Explain an APIC related error
*/
int APIC_Error (int code, const UBYTE *command, ...) {
    switch (code) {
    case APIC_OK:
	return RET_SUCC;
    case APIC_NOICON:
	error ("%s:\nAppIcon not shown!", command);
	break;
    case APIC_NODISKOBJECT:
	error ("%s:\nIcon not created!", command);
	break;
    case APIC_NOPORT:
	error ("%s:\nAppIcon Module not active!", command);
	break;
    default:
	error ("%s:\nAppIcon Internal Error!", command);
    } /* switch */
    return RET_FAIL;
} /* APIC_Error */


/*
**  Get a message from the AppIcon
*/
Prototype void APIC_Control (void);
void APIC_Control (void) {
    struct AppMessage *msg;

    while (msg = (struct AppMessage *)GetMsg (APIC_Port)) {
	if (!msg->am_NumArgs) {
	    char *buffer;
	    buffer = strdup (APIC_ActionDblClick);
	    if (buffer) {
		do_command(buffer);
		free(buffer);
	    } else {
		nomemory();
	    } /* if */
	} else {
	    char *buffer = malloc (APIC_BufferSize);
	    if (buffer) {
		int i;
		for (i = 0; i < msg->am_NumArgs; ++i) {
		    /* ---- getpathto() is found in XDME/Src/Subs.c */
		    if (getpathto(msg->am_ArgList[i].wa_Lock, msg->am_ArgList[i].wa_Name, tmp_buffer)) {
			sprintf (buffer, APIC_ActionDrop, tmp_buffer, tmp_buffer, tmp_buffer, tmp_buffer);
			do_command(buffer);
		    } else {
			/* ---- file not found - und nun? */
		    } /* if */
		} /* for */
	    } else {
		nomemory();
	    } /* if */

	} /* if */

	ReplyMsg ((struct Message *)msg);
    } /* while */
} /* APIC_Control */


/*************************************************
	Fragments of other modules
*************************************************/

#ifdef MAIN_C
/* line 272 */
    /* Init AppIcon */ /* PATCH_NULL [21 Sep 1994] : added */
    APIC_Initialize();
    Mask |= APIC_SigMask;

/* line 504 */
	/* else */if (wait_ret & APIC_SigMask)   /* Message from AppIcon */ /* PATCH_NULL [21 Sep 1994] : added */
	{
	   text_cursor (0);
	   show_cursor ++;

	   APIC_Control();
	}

/* line 1116 */
    APIC_Terminate();
#endif


/***************************************************
		COMMAND INTERFACE
***************************************************/


Prototype int do_apicshow (void);
int do_apicshow (void) {
    return APIC_Error( APIC_Show(APIC_IconFile, APIC_IconTitle), av[0]);
} /* do_apicshow */

Prototype int do_apichide (void);
int do_apichide (void) {
    return APIC_Error(APIC_Hide(), av[0]);
} /* do_apichide */



/*DEFLONG #long APPICON

That Package allows use of a Workbench AppIcon.
Currently the whole package is conrolled via some special
variables, not via commands;
the following five variables are used:

    $appicon - (BOOL) the status of the appicon:
	 setting it to "1" makes the AppIcon appear,
	 setting it to "0" makes the AppIcon disappear.

    $appiconname - (FILE) the icon to be used for the AppIcon Image;
	plase note, that modification of that variable currently
	only has effect after the NEXT appearance of the AppIcon,
	the visible Image is not changed.
	Defaults to "XDME".

    $appicontitle - (STRING) the titlestring to be used in connection
	with the AppIcon (the same limit as for $appiconname)
	Defaults to "XDME".

    $appicondropaction - (COMMAND) the command to be executed,
	whenever another icon is dropped onto the AppIcon; any
	"%s" in that variable are expanded to the full name of
	the dropped icon (w/ sprintf).
	Defaults to "newwindow newfile `%s'".

    $appiconclickaction - (COMMAND) the command to be executed,
	whenever user doubleclicks on the AppIcon.
	Defaults to "newwindow arpload".

*/




#undef STATIC_COM
/***********************************************************
    The STATIC_COM section has currently _2_ purposes:
      - first of all it is designed to allow modules
	that need no references from the main application
	and so can be used just by adding them to the
	source tree of a tool
      - to achieve that goal we have 2 possibilities:
	- we can use the "COMMAND(...)" lines to generate
	  a List of all commands of a certain application,
	  (static solution)
	- we can add AUTOINIT and AUTOEXIT functions
	  which add those COMMAND lines to a global
	  database themselfes (dynamic solution)
	  that way is very interesting in case of using
	  a split application, cluttered in many small
	  modules, which can be activated or removed
	  by user request

    Since Dynamic method means too much overhead to be of
    any use for the XDME, we will probably probably never
    use the DYNAMIC solution, and for that reason it might
    be of more use to put those command lines to the
    functions, which implement those commands.
***********************************************************/
#ifdef STATIC_COM
#define COMMAND(n,a,f,p) /* these lines cannot be translated here! */

    /* COMMAND("", , , ) */

    DEFFLAG( 94-09-19, APIC_Active, 0 )


#endif


#undef SPC_VAR
/***********************************************************
    The SPC_VAR section has only one purpose:
      - it contains descriptions for all special
	variables of the module, which are of interest
	for the user - to change them or to ask their
	values via variable interface.
    that section shall _never_ be considered C-Source
    (for that reason the "#undef" is put just before)
    but it is scanned by an external parser in order
    to produce a list of all Special variables and
    their access functions in a seperate File.
***********************************************************/
#ifdef SPC_VAR

str appicontitle	= %{ reference = APIC_IconTitle;      set = $(REFERENCE) = strrep ($(REFERENCE), value); %};
str appiconname 	= %{ reference = APIC_IconFile;       set = $(REFERENCE) = strrep ($(REFERENCE), value); %};
str appicondropaction	= %{ reference = APIC_ActionDrop;     set = $(REFERENCE) = strrep ($(REFERENCE), value); %};
str appiconclickaction	= %{ reference = APIC_ActionDblClick; set = $(REFERENCE) = strrep ($(REFERENCE), value); %};

bit appicon		= %{ reference = APIC_Active; set = %{ { char inter; inter = test_arg(value, $(REFERENCE)); if (inter != $(REFERENCE)) if (inter) do_apicshow(); else do_apichide(); } %}; %};


#endif /* SPC_VAR */

/******************************************************************************
*****  END AppIcon.c
******************************************************************************/

