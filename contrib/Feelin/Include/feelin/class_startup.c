/*
**
**  $VER: class_startup.c 1.0 (2005/08/09)
**
**  Feelin class library startup code.
**
*****************************************************************************

$VER: 01.00 (2005/08/09)

    Rewrote class startup. It's no longer a linked object, but  a  file  to
    include in the "Project.c" file of the class.

    The following macros must be defined by the compiler when creating  the
    object :

    F_CLASS_NAME: The name of the class e.g. "Area"
    F_CLASS_VERSION_STIRNG: The version string e.g. "09.12 (2005/07/25)"
    F_CLASS_VERSION: The version number of the class e.g. "09"
    F_CLASS_REVISION: The revision number of the class e.g. "12"
    F_CLASS_AUTHOR: The author of the class e.g. "Olivier LAVIALE"

*/

#define F_LIB_STRUCT_AND_BASE                   struct in_ClassBase *ClassBase

#include <proto/exec.h>

#include <exec/types.h>
#include <exec/tasks.h>
#include <exec/ports.h>
#include <exec/memory.h>
#include <exec/lists.h>
#include <exec/semaphores.h>
#include <exec/execbase.h>
#include <exec/alerts.h>
#include <exec/libraries.h>
#include <exec/interrupts.h>
#include <exec/resident.h>
#include <dos/dos.h>

#include <feelin/types.h>
#include <feelin/machine.h>

///NDEBUG

#ifndef NDEBUG
#ifndef __AROS__
#include <clib/debug_protos.h>
#endif

#define DEBUG_INIT(x)       x
#define DEBUG_OPEN(x)       x
#define DEBUG_CLOSE(x)      x
#define DEBUG_EXPUNGE(x)    x
#define DEBUG_NULL(x)       x

#else

#define DEBUG_INIT(x)
#define DEBUG_OPEN(x)
#define DEBUG_CLOSE(x)
#define DEBUG_EXPUNGE(x)
#define DEBUG_NULL(x)

#endif

//+

#ifndef F_CLASS_AUTHOR
#define F_CLASS_AUTHOR                          "Olivier LAVIALE (gofromiel@gofromiel.com)"
#endif
#ifndef F_CLASS_EXTENSION
#define F_CLASS_EXTENSION                       ".fc"
#endif

/// Structures & declarations
struct FeelinLID
{
   uint8 i_Type,o_Type,d_Type,p_Type;
   uint16 i_Name,o_Name; STRPTR d_Name;
   uint8 i_Flags,o_Flags,d_Flags,p_Flags;
   uint8 i_Version,o_Version; uint16 d_Version;
   uint8 i_Revision,o_Revision; uint16 d_Revision;
   uint16 i_IdString,o_IdString; STRPTR d_IdString;
   uint32 endmark;
};

struct in_ClassBase
{
    struct Library                  lib;
    BPTR                            seglist;
};

extern const uint32                 lib_init_table[4];
extern const char                   feelin_auto_class_id[];
extern const char                   feelin_auto_class_name[];
extern struct TagItem *             feelin_auto_class_query(uint32 Which);
//+

#ifdef __MORPHOS__
uint32 __abox__ = 1;
#endif

struct ExecBase                    *SysBase;
struct FeelinBase                  *FeelinBase;

///lib_return
int32 lib_return(void)
{
    return -1;
}
//+

STATIC struct Resident ROMTag
#if (defined(__GNUC__) && defined(__amigaos4__)) || defined(__AROS__)
__attribute__((used))
#endif
= {
    RTC_MATCHWORD,
    &ROMTag,
    &ROMTag + 1,

#if defined(__MORPHOS__)
    RTF_PPC | RTF_EXTENDED | RTF_AUTOINIT,
#elif defined(__AROS__)
    RTF_EXTENDED | RTF_AUTOINIT,
#else
    RTF_AUTOINIT,
#endif
    F_CLASS_VERSION,
    NT_LIBRARY,
    0,
    (STRPTR) &feelin_auto_class_name,
    (STRPTR) &feelin_auto_class_id[6],
    &lib_init_table
#if defined(__MORPHOS__) || defined(__AROS__)
    /* New Fields */
    ,F_CLASS_REVISION,
    NULL
#endif
};

const char feelin_auto_class_name[] = F_CLASS_NAME F_CLASS_EXTENSION;
const char feelin_auto_class_id[] = "$VER: " F_CLASS_NAME " " F_CLASS_VERSION_STRING " by " F_CLASS_AUTHOR;

/// lib_init
SAVEDS STATIC F_LIB_INIT
{
    F_LIB_INIT_ARGS
/*
    DEBUG_INIT(KPrintF("LibInit: in_ClassBase 0x%p SegList 0x%lx SysBase 0x%p\n",
                       ClassBase, SegList, SYS));
*/
    ClassBase->seglist = (BPTR)SegList;

    SysBase = SYS;

    return &ClassBase->lib;
}
//+
/// lib_expunge
SAVEDS STATIC F_LIB_EXPUNGE
{
    F_LIB_EXPUNGE_ARGS

    uint32 seglist = (uint32) ClassBase -> seglist;
/*
    DEBUG_EXPUNGE(KPrintF("LIB_Expunge: LibBase 0x%p <%s> OpenCount %ld\n",
                          ClassBase, ClassBase->lib.lib_Node.ln_Name, ClassBase->lib.lib_OpenCnt));
*/
    if (ClassBase -> lib.lib_OpenCnt)
    {
/*
        DEBUG_EXPUNGE(KPrintF("LIB_Expunge: set LIBF_DELEXP\n"));
*/
        ClassBase -> lib.lib_Flags |= LIBF_DELEXP;

        return 0;
    }

    /* We don't need a forbid() because Expunge and Close are called with a
    pending forbid. But let's do it for safety if somebody does it by hand.
    */

    Forbid();
/*
    DEBUG_EXPUNGE(KPrintF("LIB_Expunge: remove the library\n"));
*/
    Remove(&ClassBase->lib.lib_Node);

    Permit();
/*
    DEBUG_EXPUNGE(KPrintF("LIB_Expunge: free the library\n"));
*/
    FreeMem((APTR) ((uint32) (ClassBase) - (uint32) (ClassBase->lib.lib_NegSize)),
            ClassBase->lib.lib_NegSize + ClassBase->lib.lib_PosSize);
/*
    DEBUG_EXPUNGE(KPrintF("LIB_Expunge: return Segment 0x%lx to ramlib\n", MySegment));
*/
    return seglist;
}
//+
/// lib_open
STATIC F_LIB_OPEN
{
    F_LIB_OPEN_ARGS

/*
    DEBUG_OPEN(KPrintF("LIB_Open: 0x%p <%s> OpenCount %ld\n",
                       MyLibBase, MyLibBase->Lib.lib_Node.ln_Name, MyLibBase->Lib.lib_OpenCnt));
*/

    ClassBase->lib.lib_Flags &= ~LIBF_DELEXP;
    ClassBase->lib.lib_OpenCnt++;

    return &ClassBase->lib;
}
//+
///lib_close
STATIC F_LIB_CLOSE
{
    F_LIB_CLOSE_ARGS
/*
    DEBUG_CLOSE(KPrintF("LIB_Close: 0x%p <%s> OpenCount %ld\n",
                        MyLibBase, MyLibBase->Lib.lib_Node.ln_Name, MyLibBase->Lib.lib_OpenCnt));
*/
    /* This call is protected by a Forbid() */

    if (ClassBase->lib.lib_OpenCnt > 0)
        ClassBase->lib.lib_OpenCnt--;

    if (ClassBase->lib.lib_OpenCnt == 0)
    {
        if (ClassBase->lib.lib_Flags & LIBF_DELEXP)
        {
/*
            DEBUG_CLOSE(KPrintF("LIB_Close: LIBF_DELEXP set\n"));
*/
#ifdef __MORPHOS__
            REG_A6 = (uint32) ClassBase;

            return lib_expunge();
#else
            return lib_expunge(ClassBase);
#endif

        }
    }
/*
    else
    {
        DEBUG_CLOSE(KPrintF("LIB_Close: done\n"));
    }
*/
    return 0;
}
//+
///lib_reserved
STATIC uint32 lib_reserved(void)
{
    uint32 a;

    /* These useless lines are used to hide symbols and shutup the compiler
    */

    a = (uint32)(&ROMTag) - (uint32)(&ROMTag);

    return a;
}
//+

#include <proto/feelin.h>

///lib_query
SAVEDS STATIC F_LIB_QUERY
{
    F_LIB_QUERY_ARGS

    FeelinBase = Feelin;

    return feelin_auto_class_query(Which);
}
//+

STATIC const APTR lib_func_table[] =
{
#ifdef __MORPHOS__
    (APTR) FUNCARRAY_BEGIN,
    (APTR) FUNCARRAY_32BIT_NATIVE,
#endif

    lib_open,
    lib_close,
    lib_expunge,
    lib_reserved,

    lib_query,

    (APTR) -1,

#ifdef __MORPHOS__
    (APTR) FUNCARRAY_END
#endif
};

#if defined(__MORPHOS__) || defined(__AROS__)
#define lib_init_data                           NULL
#else
STATIC const struct FeelinLID lib_init_data[] =
{
    0xA0,  8, NT_LIBRARY, 0,
    0x80, 10, (STRPTR) &feelin_auto_class_name,
    0xA0, 14, LIBF_SUMUSED|LIBF_CHANGED, 0,
    0x90, 20, F_CLASS_VERSION,
    0x90, 22, F_CLASS_REVISION,
    0x80, 24, (STRPTR) &feelin_auto_class_id,

    0
};
#endif

const uint32 lib_init_table[4] =
{
   (uint32) sizeof (struct in_ClassBase),
   (uint32) lib_func_table,
   (uint32) lib_init_data,
   (uint32) lib_init
};

