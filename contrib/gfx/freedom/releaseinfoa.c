/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Freedom function ReleaseInfoA()
    Lang: English
*/

#include "freedom_intern.h"
#include <proto/exec.h>
#include <proto/utility.h>

#define DEBUG 0
#   include <aros/debug.h>

/*****************************************************************************

    NAME */

	AROS_LH2(ULONG, ReleaseInfoA,

/*  SYNOPSIS */
    	AROS_LHA(struct GlyphEngine *, glyphEngine, A0),
	AROS_LHA(struct TagItem *, tagList, A1),
	
/*  LOCATION */
	struct Library *, FreedomBase, 9, Freedom)

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
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,FreedomBase)

    struct TagItem  *tstate,*ti;
    struct GlyphMap *gm;
    APTR    	    data;
    
    tstate = tagList;
    D(bug("\n*** ReleaseInfoA ***\n"));
    
    while ((ti = NextTagItem((const struct TagItem **)&tstate)))
    {
        data = (APTR) ti->ti_Data;

        if (data)
        {
            switch(ti->ti_Tag)
            {
                case OT_WidthList:
                    D(bug("  OT_WidthList 0x%x\n", data));
                    if (data) FreeVec(data);
                    break;
                    
                case OT_GlyphMap:
                    D(bug("  OT_GlyphMap 0x%x\n", data));
                    if (data)
                    {
                        gm = (struct GlyphMap *)data;
                        if (gm->glm_BitMap && (gm->glm_BitMap != emptybitmap))
                        {
                            FreeVec(gm->glm_BitMap);
                        }
                        FreeVec(gm);
                    }
                    break;

               default:
                    D(bug("  OT_??? tag = 0x%x data = %d (0x%x) \n", ti->ti_Tag, data, data));
                    break;
                                        
            } /* switch(ti->ti_Tag) */

        } /* if (data) */

    } /* while ((ti=NextTagItem(&tstate))) */

    D(bug("\n#### ReleaseInfoA done ###\n"));

    return 0;
    
    AROS_LIBFUNC_EXIT

} /* ReleaseInfoA */
