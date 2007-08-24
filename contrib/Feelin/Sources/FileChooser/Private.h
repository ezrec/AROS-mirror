/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/feelin.h>

#include <libraries/feelin.h>

/****************************************************************************
*** Attributes, Methods, Values, Flags **************************************
****************************************************************************/

enum    {

        FA_FileChooser_Path,
        FA_FileChooser_File,
        FA_FileChooser_FilterIcons,
        FA_FileChooser_FilterFiles,
        FA_FileChooser_Pattern,
        FA_FileChooser_Choosed

        };

enum    {

        FM_FileChooser_Update,
        FM_FileChooser_Activate

        };

struct  FS_FileChooser_Update                   { uint32 From; uint32 Value; };

enum    {

        FV_FROM_PATH = 1,
        FV_FROM_FILE,
        FV_FROM_LIST_PATH,
        FV_FROM_LIST_FILE

        };
        
/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct ListEntryRaw
{
    uint32                          Type;
    APTR                            Data;
};

struct ListEntry
{
    uint32                          Type;
    STRPTR                          Name;
    BPTR                            Lock;

    /* size is a percentage for volumes */

    uint32                          Size;

    STRPTR                          DevName;
};

enum    {

        FV_ENTRY_VOLUME = 1,
        FV_ENTRY_VOLUME_SEPARATOR,
        FV_ENTRY_ASSIGN,
        FV_ENTRY_DIRECTORY,
        FV_ENTRY_DIRECTORY_SEPARATOR,
        FV_ENTRY_FILE

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FObject                         Path;
    FObject                         File;

    FObject                         list;
    FObject                         volumes;
    FObject                         parent;
    FObject                         balance;
    FObject                         match;
    FObject                         pattern;
    FObject                         icons;
    
    bits32                          Flags;
};

#define FF_FILECHOOSER_FILTERICONS              (1 << 0)
#define FF_FILECHOOSER_FILTERFILES              (1 << 1)
