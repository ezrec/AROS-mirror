#ifndef _DRIVE_H_
#define _DRIVE_H_

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

enum DriveType
{
    DT_FILESYSTEM,
    DT_HARDFILE
};

enum DriveMode
{
    DM_READWRITE,
    DM_READONLY
};

struct Drive
{
    STRPTR         d_Path;      /* path to file or directory */
    enum DriveType d_Type;      /* type of drive, see above */
    enum DriveMode d_Mode;      /* access mode, see above */
    
    union
    {
        struct
        {
            ULONG Sectors;
            ULONG Heads;
            ULONG Reserved;
        } HF;
        
        struct
        {
            STRPTR VolumeName;  /* volume name */
        } FS;
    } d_Parameters;
};

/*** Prototypes *************************************************************/
struct Drive *Drive_Create();
VOID          Drive_Destroy(struct Drive *drive);

#endif /* _DRIVE_H_ */
