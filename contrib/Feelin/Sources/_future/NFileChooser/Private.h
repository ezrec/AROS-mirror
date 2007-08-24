/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <stdarg.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#define DOSBase                     FeelinBase -> DOS
#define UtilityBase                 FeelinBase -> Utility

extern struct ClassUserData                    *CUD;

/****************************************************************************
*** Attributes & Methods ****************************************************
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
struct  FS_FileChooser_Activate                 { uint32 From; uint32 Active; };

enum    {

        FV_FILECHOOSER_LIST_VOLUMES = 1,
        FV_FILECHOOSER_LIST_FILES

        };

enum    {

        FV_FROM_PATH = 1,
        FV_FROM_FILE,
        FV_FROM_LIST_PATH,
        FV_FROM_LIST_FILE

        };

enum    { // auto-resolve

        FA_List_Quiet,
        FA_List_Format,
        FA_List_Activation,
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

struct ClassUserData
{
    struct Hook                     hook_destruct;
    struct Hook                     hook_compare;
    struct Hook                     hook_volumes_construct;
    struct Hook                     hook_volumes_display;

    BYTE                            buf[64];
};

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    bits32                          flags;

    FObject                         list_volumes;
    FObject                         list_files;
/*
    FObject                         path;
    FObject                         file;
*/
/*
    FObject                         parent;
    FObject                         balance;
    FObject                         match;
    FObject                         pattern;
    FObject                         icons;
*/
};

#define FF_FILECHOOSER_FILTERICONS              (1 << 0)
#define FF_FILECHOOSER_FILTERFILES              (1 << 1)
#define FF_FILECHOOSER_PATH_NOTIFIED            (1 << 2)

/* FF_FILECHOOSER_PATH_NOTIFIED


    This flag is set when path is modified because of a list  change,  thus
    we don't have to adjust active list entry.

*/

#define DEF_FILECHOOSER_IMAGE_HOME              "<image type='picture' src='Feelin:Images/FileChooser/home.png' />"
#define DEF_FILECHOOSER_IMAGE_BACK              "<image type='picture' src='Feelin:Images/FileChooser/back.png' />"
#define DEF_FILECHOOSER_IMAGE_FORWARD           "<image type='picture' src='Feelin:Images/FileChooser/forward.png' />"
