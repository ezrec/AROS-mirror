/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>

#include "drive.h"

struct Drive *Drive_Create()
{
    return AllocMem(sizeof(struct Drive), MEMF_ANY | MEMF_CLEAR);    
}

VOID Drive_Destroy(struct Drive *drive)
{
    if (drive != NULL)
    {
        if (drive->d_Path != NULL)
        {
            FreeVec(drive->d_Path);
        }
        
        if
        (
               drive->d_Type == DT_FILESYSTEM 
            && drive->d_Parameters.FS.VolumeName != NULL
        )
        {
            FreeVec(drive->d_Parameters.FS.VolumeName);
        }
        
        FreeMem(drive, sizeof(struct Drive));
    }
}
