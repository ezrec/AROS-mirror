/*
    LevelData implementation for MUIMine
*/
#include "MUIMine.h"
#include "MFStrings.h"
#include "MFWindow.h"
#include "LevelData.h"



/*
    level data file header structure
*/
#define LEVEL_HEADER_ID_SIZE      8
#define LEVEL_HEADER_ID_STRING    "MFLEVELS"

struct LevelDataFileHeader
{
    char    IdString[LEVEL_HEADER_ID_SIZE];
    LONG    NumLevels;
    ULONG   CurrentLevel;
};

/*
    defines
*/

#define DEFAULT_LEVEL_FILE_NAME (STRPTR)"PROGDIR:MUIMineLevelData"


/*
    LevelDataList.Flags defines
*/
#define LDLF_STATIC_DATA        0x00000001



/*
    default LevelData
*/
#define NUM_DEF_LEVELS  3
struct LevelData defLevelData[NUM_DEF_LEVELS] =
{
    {MUIV_MFWindow_Level_Beginner,     MSG_LEVEL_BEGINNER,     NULL, "Beginner",     {{-1, ""}, {-1, ""}, {-1, ""}}},
    {MUIV_MFWindow_Level_Intermediate, MSG_LEVEL_INTERMEDIATE, NULL, "Intermediate", {{-1, ""}, {-1, ""}, {-1, ""}}},
    {MUIV_MFWindow_Level_Expert,       MSG_LEVEL_EXPERT,       NULL, "Expert",       {{-1, ""}, {-1, ""}, {-1, ""}}}
};


/*
    function :    loads the level data from a disk file, allocates and
                  initialize memory for the level array with data from
                  the disk file if it was successfully loaded or with
                  the default level data if the disk load failed

    parameters :  lList = pointer to a LevelDataList that is initialized
                          with the level data
                  fileName = pointer to name of level data file, if NULL
                             use the default level file name

    return :      TRUE if the LevelDataList was fully initialized
*/
BOOL LoadLevelData(struct LevelDataList *lList, STRPTR fileName)
{
    BOOL ret = FALSE;
    BPTR fileHandle;
    LONG size;
    int numLevels = 0;
    ULONG currLevel = 0;

    /*
        attempt to open existing level data file
    */
    fileHandle = Open(((fileName) ? fileName
                                  : DEFAULT_LEVEL_FILE_NAME),
                              MODE_OLDFILE);

    if (fileHandle)
    {
        struct LevelDataFileHeader header;
        /*
            determine if this is a valid level data file by reading
            the file header and checking the id string and comparing
            the file size with the size calculated for the number of
            levels given in the header
        */
        if (Read(fileHandle, (APTR)&header, sizeof(struct LevelDataFileHeader))
                    == sizeof(struct LevelDataFileHeader))
        {
            int i;
            char * p1, * p2;

            /*
                check the header id string
            */
            p1 = header.IdString;
            p2 = LEVEL_HEADER_ID_STRING;
            for (i = LEVEL_HEADER_ID_SIZE; i > 0; i--)
            {
                if (*p1++ != *p2++)
                {
                    break;
                }
            }
            if (i == 0)
            {
                /*
                    id string checked out ok, now get the file size
                */
                struct FileInfoBlock fib;
                if (ExamineFH(fileHandle, &fib))
                {
                    /*
                        check that the file size is the size of the file
                        header plus the size of LevelData structure times
                        the number of levels given in the header
                    */
                    size = fib.fib_Size - sizeof(struct LevelDataFileHeader);;
                    if (size  ==  header.NumLevels * sizeof(struct LevelData))
                    {
                        /*
                            set the number of levels to allocate for
                        */
                        numLevels = header.NumLevels;
                        currLevel = header.CurrentLevel;
                    }
                }
            }
        }
    }

    /*
        initialize the level data list for the number of levels
        in the data file if it exists otherwise use the number
        of default levels
    */
    if (InitLevelDataList(lList, (numLevels) ? numLevels : NUM_DEF_LEVELS))
    {
        /*
            buffer was allocated now read the level data from the file
            if it was opened and contains level data otherwise copy the
            default level data
        */
        if (numLevels)
        {
            /*
                buffer was allocated now read the data from the file
            */
            if (Read(fileHandle, (APTR)lList->LevelList, size) == size)
            {
                int i;
                struct LevelData * pLevel;
                /*
                    the level data was read from the file to the allocated
                    buffer, initialize the LevelDataList with the loaded data
                */
                lList->CurrentLevel = currLevel;
                /*
                    clear the menu data members of the LevelData's as the
                    data that was last saved is obsolete and needs to be
                    initialized
                */
                for (i = lList->NumLevels, pLevel = lList->LevelList;
                     i > 0;
                     i--, pLevel++)
                {
                    pLevel->MenuId = 0;
                    pLevel->MenuItem = NULL;
                }
                /*
                    set the return flag to success
                */
                ret = TRUE;
            }
            else
            {
                /*
                    failed to read the file, delete the allocate buffer
                    and use the default level data
                */
                FreeVec((APTR)lList->LevelList);
                lList->NumLevels = 0;
                lList->LevelList = NULL;
            }
        }
        else
        {
            /*
                level data file was not opened, copy the default level
                data to the allocated buffer
            */
            int i;
            STRPTR p;
            struct LevelData *p1, *p2;

            for (i = 0, p1 = lList->LevelList, p2 = defLevelData;
                 i < NUM_DEF_LEVELS;
                 i++, p1++, p2++)
            {
                *p1 = *p2;
                p = GetStr((LONG)defLevelData[i].MenuId);
                if (p)
                {
                    strcpy(p1->Name, p);
                }
            }

            /*
                set the level data in the LevelDataList
            */
            lList->CurrentLevel = lList->LevelList->Level;

            ret = TRUE; // set return code to success
        }
    }

    /*
        close the level data file if it was opened
    */
    if (fileHandle)
    {
        Close(fileHandle);
    }

    /*
        if the level data was not loaded from the file then use the
        default level data list
    */
    if (lList->LevelList == NULL)
    {
        lList->NumLevels = NUM_DEF_LEVELS;
        lList->Flags |= LDLF_STATIC_DATA;
        lList->LevelList = defLevelData;
        lList->CurrentLevel = lList->LevelList->Level;
        ret = TRUE;
    }

    /*
        return the success flag
    */
    return ret;
}

/*
    function :    saves the level data list to a disk file

    parameters :  lList = pointer to the level data list to save
                  fileName = pointer to name of file to save to, if NULL
                             then save to the default file

    return :      TRUE if data was rwitten successfully, FALSE otherwise
*/
BOOL SaveLevelData(struct LevelDataList * lList, STRPTR fileName)
{
    BOOL ret = FALSE;

    /*
        only save data if there is something to save
    */
    if (lList  &&  lList->LevelList  &&  lList->NumLevels != 0)
    {
        /*
            attempt to open the file as a new file
        */
        BPTR fileHandle = Open(((fileName) ? fileName : DEFAULT_LEVEL_FILE_NAME),
                                MODE_NEWFILE);
        if (fileHandle)
        {
            struct LevelDataFileHeader header;
            int i;
            char *p1, *p2;

            /*
                fill in the file header, copy the id string, set the
                number of levels and the current level
            */
            p1 = header.IdString;
            p2 = LEVEL_HEADER_ID_STRING;
            for (i = LEVEL_HEADER_ID_SIZE; i > 0; i--)
            {
                *p1++ = *p2++;
            }
            header.NumLevels = lList->NumLevels;
            header.CurrentLevel = lList->CurrentLevel;

            /*
                write the header to the file
            */
            if (Write(fileHandle, (APTR)&header, sizeof(struct LevelDataFileHeader))
                        ==  sizeof(struct LevelDataFileHeader))
            {
                /*
                    calculate the size of the level data to write to the
                    file and write the level data to the file setting the
                    return code to the success of the write
                */
                LONG size = lList->NumLevels * sizeof(struct LevelData);
                ret = (Write(fileHandle, (APTR)lList->LevelList, size)
                            == size);
            }

            /*
                close the file
            */
            Close(fileHandle);
        }
    }

    return ret;     // return success code
}

/*
    function :    initializes a level data list, by allocating a level
                  data array and initializing the list's members

    parameters:   lList = pointer to the level data list to initialize
                  numLevels = number of levels to allocate

    return :      TRUE if level data was allocated, FALSE otherwise
*/
BOOL InitLevelDataList(struct LevelDataList * lList, int numLevels)
{
    BOOL ret = FALSE;

    /*
        initialize members
    */
    lList->NumLevels = 0;
    lList->Flags = 0;
    lList->CurrentLevel = 0;
    lList->LevelList = NULL;

    /*
        check number of levels parameter
    */
    if (numLevels > 0)
    {
        /*
            attempt to allocate the level data array
        */
        lList->LevelList = (struct LevelData *)AllocVec(
                                        numLevels * sizeof(struct LevelData),
                                        MEMF_CLEAR);
        if (lList->LevelList)
        {
            /*
                level data array allocated, set the NumLevels member
                to the number of allocated levels and set the return
                code to TRUE to indicate that level data was allocated
            */
            lList->NumLevels = numLevels;
            ret = TRUE;
        }
    }

    return ret;
}

/*
    function :    dispose of allocated level data

    parameters :  lList = pointer to the level data list to dispose of

    return :      none
*/
void DisposeLevelData(struct LevelDataList * lList)
{
    if (lList)
    {
        if ((lList->Flags & LDLF_STATIC_DATA) == 0  &&
            lList->LevelList)
        {
            FreeVec((APTR)lList->LevelList);
        }
        lList->NumLevels = 0;
        lList->Flags = 0;
        lList->LevelList = NULL;
    }
}


/*
    function :    searches a LevelDataList for a LevelData for the given level

    parameters :  lList = pointer to the LevelDataList to search
                  level = level to search for

    return :      pointer to the LevelData if found, NULL otherwise
*/
struct LevelData * FindLevel(struct LevelDataList * lList, ULONG level)
{
    struct LevelData * ret = NULL;

    /*
        check for list and non-NULL level
    */
    if (lList  &&  level)
    {
        int i;
        struct LevelData * pLevel;

        /*
            step through list comparing each level's Level member
            to the level being searched for
        */
        for (i = lList->NumLevels, pLevel = lList->LevelList;
             i > 0;
             i--, pLevel++)
        {
            if (pLevel->Level == level)
            {
                /*
                    the level was found, set the return vale to the found
                    level and exit the search loop
                */
                ret = pLevel;
                break;
            }
        }
    }

    return ret;
}


/*
    function :    searches a LevelDataList for a LevelData with a given menu id

    parameters :  lList = pointer to the LevelDataList to search
                  menuId = menu id to search for

    return :      pointer to the LevelData if found, NULL otherwise
*/
struct LevelData * FindMenuId(struct LevelDataList * lList, ULONG menuId)
{
    struct LevelData * ret = NULL;

    /*
        check for list and non-NULL menu id
    */
    if (lList  &&  menuId)
    {
        int i;
        struct LevelData * pLevel;

        /*
            step through the list comparing each level's MenuId member
            to the menu id being searched for
        */
        for (i = lList->NumLevels, pLevel = lList->LevelList;
             i > 0;
             i--, pLevel++)
        {
            if (pLevel->MenuId == menuId)
            {
                /*
                    the menu id was found, set the return value to this
                    level and exit the search loop
                */
                ret = pLevel;
                break;
            }
        }
    }

    return ret;
}

/*
    function :    searches a LevelDataList for the current LevelData

    parameters :  lList = pointer to the LevelDataList to search

    return :      pointer to the LevelData if found, NULL otherwise
*/
struct LevelData * FindCurrentLevel(struct LevelDataList * lList)
{
    return FindLevel(lList, lList->CurrentLevel);
}

