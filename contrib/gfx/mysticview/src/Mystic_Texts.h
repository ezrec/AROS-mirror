/*********************************************************************
----------------------------------------------------------------------

	MysticView
	texts

----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_TEXTS_H
#define	MYSTIC_TEXTS_H 1


#include	"Mystic_Global.h"


#define	MVTEXT_SAVELISTAS				"Save List as..."

//#define	MVTEXT_SELECTNEXTPICTORESUME	"select 'Next Picture' to resume"
#define	MVTEXT_SELECTNEXTPICTORESUME	"end of list"
#define	MVTEXT_OPENLISTFILE				"Open List File..."
#define	MVTEXT_APPENDLISTFILE			"Add List File..."
#define	MVTEXT_SELECTSCREENMODE			"Select Screenmode..."
#define	MVTEXT_SELECTSTARTPIC			"Select Startup Picture..."
#define	MVTEXT_SAVEPRESETAS				"Save Preset as..."
#define	MVTEXT_LOADPRESET				"Load Preset..."
#define	MVTEXT_SELECTPICTURES			"Open Picture(s)..."
#define	MVTEXT_ADDPICTURES				"Add Picture(s)..."

#define	MVTEXT_STAT_NOPIX					"no pictures"
#define	MVTEXT_STAT_NORMAL					"%s  "
#define	MVTEXT_STAT_NORMALLOADING			"%s *"
#define	MVTEXT_STAT_NORMAL_DIR				"<%s>  "
#define	MVTEXT_STAT_NORMALLOADING_DIR		"<%s> *"
#define	MVTEXT_STAT_SCANNING				"(%ld pictures)  "
#define	MVTEXT_STAT_SCANNINGLOADING			"(%ld pictures) *"
#define	MVTEXT_STAT_MULTI					"(%d of %d) %s  "
#define	MVTEXT_STAT_MULTILOADING			"(%d of %d) %s *"
#define	MVTEXT_STAT_MULTI_DIR				"(%d of %d) <%s> "
#define	MVTEXT_STAT_MULTILOADING_DIR		"(%d of %d) <%s> *"
#define	MVTEXT_STAT_SLIDESHOW				"slideshow - (%d of %d) %s  "
#define	MVTEXT_STAT_SLIDESHOWLOADING		"slideshow - (%d of %d) %s *"
#define	MVTEXT_STAT_SLIDESHOW_DIR			"slideshow - (%d of %d) <%s>  "
#define	MVTEXT_STAT_SLIDESHOWLOADING_DIR	"slideshow - (%d of %d) <%s> *"
#define	MVTEXT_STAT_LOADING_ONE			"loading"
#define	MVTEXT_PALETTE							"8"
#define	MVTEXT_TRUECOLOR						"24"

#define	MVTEXT_ERROR_LOADING			"error loading %s"
#define	MVTEXT_ERROR_LOADING2			"error loading"
#define MVTEXT_STAT_ERROR			"%s - %s"

#define	MVTEXT_SELECTPRESET				"Select Preset..."
#define	MVTEXT_SELECTDESTINATION		"Select Destination..."
#define	MVTEXT_SAVEPICTUREAS			"Save Picture as..."
#define	MVTEXT_RENAMEAS					"Rename Picture as..."

#define	MVTEXT_REQUEST					PROGNAME " Request"
#define	MVTEXT_PROBLEM					PROGNAME " Problem"
#define	MVTEXT_LIMITATION				PROGNAME " Limitation"
#define	MVTEXT_ABOUT					"About " PROGNAME
#define	MVTEXT_NOTE						PROGNAME " Note"

#define	MVREQ_OVERWRITE					"the file %s\n"	\
										"already exists. are you sure\n"	\
										"you want to overwrite it?"

#define	MVREQ_ERROR_COPYING				"error copying\n"	\
										"%s to\n"	\
										"%s."

#define	MVREQ_ERROR_DELETING			"error deleting\n"	\
										"%s."

#define	MVREQ_ERROR_RENAMING			"error renaming\n"	\
										"%s."

#define	MVREQ_ERROR_COPYSAMEFILE		"Cannot copy a file over itself."

#define	MVREQ_ERROR_RENAMESAMEFILE		"Cannot rename a file over itself."

#define	MVREQ_ERROR_CREATING_THUMBNAIL	"Could not create thumbnail.\n\n"	\
										"This feature requires newicon.library."

#define	MVREQ_ERROR_NO_NEWICON_LIB		"This feature requires newicon.library."

#define	MVREQ_ERROR_SAVING				"An error occured.\n" \
										"The image was not saved."

#define	MVREQ_NOPICTURE					"No picture loaded."

#define	MVREQ_ERROR_CREATING_BUTTONS	"MysticView failed to\nsetup the button bar."
#define	MVREQ_ERROR_CREATING_VIEW		"MysticView failed to initialize\nthe viewing and layout engine."
#define	MVREQ_ERROR_CREATING_PICHANDLER	"MysticView failed to create\nits picture management tasks."


#define	MVREQ_ABOUT_UNREGISTERED		PROGNAME PROGVERSION "\n\n"	\
										"written by bifat\n" \
										"©1997-2001 TEK neoscientists\n\n" \
										"this program is freeware. please\n"	\
										"send a donation to the author if\n" \
										"you use it frequently.\n\n" \
										"visit our homepage:\n" \
										"www.neoscientists.org"

#define	MVREQ_ABOUT_REGISTERED			PROGNAME PROGVERSION "\n\n"	\
										"written by bifat\n" \
										"©1997-2001 TEK neoscientists\n\n" \
										"registered to:\n" \
										"%s\n"	\
										"thank you for your support.\n\n" \
										"visit our homepage:\n" \
										"www.neoscientists.org"


#define	MVREQ_OKAY						"Okay"
#define	MVREQ_RETRYCANCEL				"Retry|Cancel"
#define	MVREQ_DELETECANCEL				"Delete|Cancel"

#define	MVREQ_RETRYCHANGECANCEL			"Retry|Change...|Cancel"
#define	MVREQ_OVERWRITECHANGECANCEL		"Overwrite|Change...|Cancel"

#define	MVREQ_SURE_DELETING				"Are you sure you want to delete\n"	\
										"%s?"

#define	MVREQ_ERROR_NONEURALLIB			"neuralnet.library not found.\n"	\
										"artificial intelligence is disabled."

#define MVREQ_ERROR_SAVINGLIST			"error saving list.\n"


#endif
