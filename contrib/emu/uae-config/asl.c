/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <exec/memory.h>
#include <libraries/mui.h>
#include <libraries/asl.h>

#include <proto/dos.h>
#include <proto/muimaster.h>
#include <proto/exec.h>

#include <string.h>

#include "locale.h"
#include "asl.h"

/*** Functions **************************************************************/
STRPTR ASL_SelectFile(enum ASLMode mode)
{
    struct FileRequester *request  = NULL;
    STRPTR                filename = NULL;
    
    request = MUI_AllocAslRequestTags
    (
        ASL_FileRequest,
        
        mode == AM_SAVE       ?
                ASL_FuncFlags :
                TAG_IGNORE,     FILF_SAVE,
        
        TAG_DONE
    );
        
    if (request == NULL)
    {
        /* Allocation failed */
        /* FIXME: error dialog? */
        return NULL;
    }
     
    if
    (
        MUI_AslRequestTags
        (
            request,
            
            ASL_Hail, mode == AM_SAVE ?
                (IPTR) "Save state..." : 
                (IPTR) "Load state...",
            
            TAG_DONE
        )
    )
    {
        ULONG length = strlen(request->rf_Dir)  + 1  /* separating '/' */  
                     + strlen(request->rf_File) + 1; /* terminating '\0' */
        
        filename = AllocVec(length, MEMF_ANY);
        
        if (filename != NULL)
        {
            filename[0] = '\0';
            strlcat(filename, request->rf_Dir, length);
            if (!AddPart(filename, request->rf_File, length))
            {
                /* FIXME: Error dialog? */
            }
        }
        else
        {
            /* FIXME: Error dialog */
        }
    }
    
    MUI_FreeAslRequest(request);

    return filename;
}
