/*
    Copyright © 1995-2000, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: english
*/
#define AROS_TAGRETURNTYPE Object *
#include <utility/tagitem.h>
#include <proto/alib.h>
#include <proto/exec.h>

/*****************************************************************************

    NAME */
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/bgui.h>
extern struct Library * BGUIBase;

	Object * BGUI_NewObject (

/*  SYNOPSIS */
	ULONG classID,
	Tag tag1, 
	...)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
#if defined(__AROS__) && defined(NO_LINEAR_VARARGS)
/*
 * Var-args stub for BGUI_NewObjectA().
 *
 * This is a little complicated on NO_LINEAR_VARARGS architectures,
 * since BGUI puts ((GROUP_Member,obj) ... (TAG_END,0)) sequences
 * in its arguments to BGUI_NewObject, so we can't use the
 * standard AROS_SLOWSTACKTAGS_* family.
 */

   va_list va, vc;
   int gm_depth = 0, len = 0;
   Tag tag;
   IPTR data;
   Object *rc = NULL;
   struct TagItem *tags;

   va_start(va, tag1);
   va_copy(vc, va);

   /* Determine the va list length, skipping GROUP_Member tagsets */
   for (tag = tag1; gm_depth >= 0; tag = va_arg(va, IPTR) ) {
       data = va_arg(va, IPTR);
       len++;

       switch (tag) {
       case TAG_END:
       	   gm_depth--;
       	   break;
       case TAG_MORE:
       	   if (gm_depth == 0)
       	       gm_depth = -1;
       	   break;
       case GROUP_Member:
       	   gm_depth++;
       	   break;
       }
   }

   tags = AllocMem(sizeof(tags[0])*len, MEMF_ANY);
   if (tags != NULL) {
       int i;
       tag = tag1;
       for (i = 0; i < len; i++) {
       	   tags[i].ti_Tag = tag;
       	   tags[i].ti_Data = va_arg(vc, IPTR);
       	   if (i != (len-1))
       	   	tag = va_arg(vc, IPTR);
       }

       rc = BGUI_NewObjectA(classID, tags);
       FreeMem(tags, sizeof(tags[0])*len);
   }
   va_end(vc);
   va_end(va);

   return rc;
#else
    return BGUI_NewObjectA(num, &tag1);
#endif
} /* BGUI_NewObject */
