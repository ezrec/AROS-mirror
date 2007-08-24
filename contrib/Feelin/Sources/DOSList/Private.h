/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <stdarg.h>
 
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#define DOSBase                                 FeelinBase -> DOS
#define GfxBase                                 FeelinBase -> Graphics
#define UtilityBase                             FeelinBase -> Utility

extern struct ClassUserData                    *CUD;

/****************************************************************************
*** Attributes, Methods, Values, Flags **************************************
****************************************************************************/

enum    {

        FA_DOSList_Path,
        FA_DOSList_File,
        FA_DOSList_FilterIcons,
        FA_DOSList_FilterFiles,
        FA_DOSList_FilterHook,
        FA_DOSList_Pattern

        };
        
enum    {
        
        FM_DOSList_ChangePath
 
        };

struct  FS_DOSList_ChangePath                   { STRPTR Path; };

enum    {

        FV_DOSList_Path_Prog = -3,
        FV_DOSList_Path_Work,
        FV_DOSList_Path_Parent,
        FV_DOSList_Path_Volumes

        };

enum    {                                       // auto-resolve

        FA_List_Quiet,
        FA_List_Format,
        FM_List_InsertSingle,
        FM_List_Remove,
        FM_List_GetEntry

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

struct ClassUserData
{
    struct Hook                     construct_hook;
    struct Hook                     destruct_hook;
    struct Hook                     compare_hook;
    struct Hook                     display_hook;

    BYTE                            buf[64];
};

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    
    bits32                          Flags;
 
    STRPTR                          path;
    STRPTR                          file;
    STRPTR                          pattern;
    STRPTR                          tokens;
    
    struct Hook                    *filter_hook;
};

#define FF_DOSLIST_FILTERICONS                  (1 << 0)
#define FF_DOSLIST_FILTERFILES                  (1 << 1)
