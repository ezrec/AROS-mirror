/*
 * @(#) $Header$
 *
 * BGUI library
 * spacingclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.2  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.1  2000/05/15 19:27:02  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.0  2000/05/09 22:10:15  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:09  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:13:11  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:48  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

/*
 * This object cannot be hit.
 */
METHOD(SpacingClassHitTest, Msg, msg)
{
   return 0;
}
METHOD_END

/*
 * This object cannot be helped.
 */
METHOD(SpacingClassHelp, Msg, msg)
{
   return BMHELP_NOT_ME;
}
METHOD_END

/*
 * Simple dimensions request.
 */
METHOD(SpacingClassDimensions, struct grmDimensions *, dim)
{
   *(dim->grmd_MinSize.Width)  = 0;
   *(dim->grmd_MinSize.Height) = 0;
   return 1;
}
METHOD_END

/*
 * Left extention request.
 */
METHOD(SpacingClassLeftExt, struct bmLeftExt *, le)
{
   *(le->bmle_Extention) = 0;
   return 1;
}
METHOD_END

/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   GM_HITTEST,       (FUNCPTR)SpacingClassHitTest,
   GRM_DIMENSIONS,   (FUNCPTR)SpacingClassDimensions,
   BASE_LEFTEXT,     (FUNCPTR)SpacingClassLeftExt,
   BASE_SHOWHELP,    (FUNCPTR)SpacingClassHelp,
   DF_END,           NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitSpacingClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassID, GadgetClass,
                         CLASS_DFTable,      ClassFunc,
                         TAG_DONE);
}

