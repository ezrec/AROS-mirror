/*
 * tooltypes.h
 * ===========
 * Handles tooltypes.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */
#ifndef TOOLTYPES_H
#define TOOLTYPES_H

#include <exec/types.h>


/*
 * save_tooltypes
 * --------------
 * Saves the settings as tooltypes.
 *
 * Arguments:
 *  none
 * Returnvalue:
 *  none
 */
void
save_tooltypes (void);

/*
 * handle_startup_msg
 * ------------------
 * Handles arguments passed to the program at startup.
 *
 * Arguments:
 *  args    - Array of argument strings.
 *  from_wb - TRUE if the program was run from the Workbench,
 *            otherwise FALSE.
 * Returnvalue:
 *  none
 */
void
handle_startup_msg (
   char **args,
   BOOL   from_wb);

#endif /* TOOLTYPES_H */
