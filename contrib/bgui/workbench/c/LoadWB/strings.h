#ifndef STRINGS_H
#define STRINGS_H

/*
** When implementing localization in the future, the getString() macro
** will hopefully make it a bit easier. You just have to change the
** string #defines to string-ids and remake getString() as a function
** that calls GetLocaleStr().
*/

/*** Macros ******************************************************************/

#define getString(x) x

/*** Strings *****************************************************************/
/*                                                                           */
/* Note: 'SC' stands for 'ShortCut'                                          */

/** Errormessages **/

#define ERR_OPENLIBRARY     "Couldn't open %s version %d or higher"
#define ERR_MEMORY          "Not enough memory"

/** Menu: Workbench **/

#define WBM_WORKBENCH       "Workbench"

#define WBM_EXECUTE         "Execute command..."
#define WBM_EXECUTE_SC      "E"
#define WBM_ABOUT           "About..."
#define WBM_ABOUT_SC        "?"
#define WBM_QUIT            "Quit"
#define WBM_QUIT_SC         "Q"

/** Window: Execute command **/

#define EXC_WINDOWTITLE     "Execute command"

#define EXC_INSTRUCTIONS    ISEQ_C "Enter a command and its arguments."
#define EXC_COMMAND         "Command:"

#define EXC_EXECUTE         "_Execute"
#define EXC_CANCEL          "_Cancel"

/** Window: About **/

#define ABT_WINDOWTITLE     "About AROS"

#define ABT_TEXT            ISEQ_C "--=<( " ISEQ_B "AROS" ISEQ_N " )>=--\n\n" \
                            ISEQ_C "the Amiga Research OS\n\n"   \
                            ISEQ_C "Copyright © 1995-2000 the AROS Development Team.\n"
#define ABT_TEXT2A          ISEQ_C "All Rights Reserved. Read the"
#define ABT_TEXT2B          "license"
#define ABT_TEXT2C          "for more information."

#define ABT_DEV_LABEL       "The following people have contributed in the making of " ISEQ_B "AROS" ISEQ_N ":"

#define ABT_OK              "OK"

#endif /* STRINGS_H */
