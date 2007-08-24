/*
**    Private.h
**
**    feelin.library global header
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

$VER: 10.00 (2005/08/22)

    OLAV: Added mungwall support. define F_USE_MEMORY_WALL to use it.

    GROG: Beautify operation, add machine  dependent  macros,  modifiy  all
    existants  macros  to  be  compliant  with  new design. Morphos version
    macros.

    Removed all SAVEDS. There shall be NO global variables.  FeelinBase  is
    available  in the a6 register (or equivalent) of each library functions
    and *must* be passed to private functions.

    Added 'Flags' member to FClass type.

$VER 08.00 (2004/12/16)

    Original version from OLAV.

*/

#define F_LIB_STRUCT_AND_BASE                   struct in_FeelinBase *FeelinBase
//#define F_USE_MEMORY_WALL

/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <string.h>

#if defined(__MORPHOS__) || defined(__AROS__)
#include <proto/exec.h>
#else
#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>
#endif

#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/feelin.h>
#include <proto/feelinclass.h>

#include <exec/alerts.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <libraries/locale.h>
#include <libraries/feelin.h>

#ifndef __AROS__
#ifndef NDEBUG
#include <clib/debug_protos.h>
#endif
#endif

#define DOSBase                     FeelinBase -> Public.DOS
#define UtilityBase                 FeelinBase -> Public.Utility
#define LocaleBase                  FeelinBase -> Public.Locale
#define GfxBase                     FeelinBase -> Public.Graphics
#define IntuitionBase               FeelinBase -> Public.Intuition

/**** !!! DONT USE 'SAVEDS' !!! IN ANY PLACE *******************************/
/**** !!! DONT USE 'SAVEDS' !!! IN ANY PLACE *******************************/
/**** !!! DONT USE 'SAVEDS' !!! IN ANY PLACE *******************************/

/****************************************************************************
*** Machine dependent macros ************************************************
****************************************************************************/

#ifdef __MORPHOS__
#define F_METHOD(rt,name)                       _DEFFUNC5(name, rt, \
                                                    a2, FClass *, Class, \
                                                    a0, FObject, Obj, \
                                                    d0, uint32, Method, \
                                                    a1, APTR, Msg, \
                                                    a6, struct in_FeelinBase *, FeelinBase)

#define F_METHODM(rt,name,sn)                   _DEFFUNC5(name, rt, \
                                                    a2, FClass *, Class, \
                                                    a0, FObject, Obj, \
                                                    d0, uint32, Method, \
                                                    a1, struct sn *, Msg, \
                                                    a6, struct in_FeelinBase *, FeelinBase)

#else /* classic */

#undef  F_METHOD
#define F_METHOD(rc,name)                       ASM(rc) name(REG(a2,FClass *Class),REG(a0,FObject Obj),REG(d0,uint32 Method),REG(a1,APTR Msg),REG(a6,struct in_FeelinBase *FeelinBase))
#undef  F_METHODM
#define F_METHODM(rc,name,sn)                   ASM(rc) name(REG(a2,FClass *Class),REG(a0,FObject Obj),REG(d0,uint32 Method),REG(a1,struct sn *Msg),REG(a6,struct in_FeelinBase *FeelinBase))

#endif
  
/****************************************************************************
*** Flags *******************************************************************
****************************************************************************/

#define DEBUG_CODE

/*  DEBUG_CODE

    Enable debug codes e.g. safe invokation, public semaphores, or  various
    verbose.

*/

#define DB_LINK_SECURE
//#define DB_PUDDLE_DELETE

#define DB_DISPOSE_FILL

/*  DB_DISPOSE_FILL

    Disposed memory is filled with 0xABADF00D, useful for  developers.  You
    shouldn't undefined it, its very useful.

*/

/* F_USE_MEMORY_WALL

    IF F_USE_MEMORY_WALL is defined, a wall is added before and after  real
    allocated  memory.  If  the  wall  is trashed, which may happens if you
    write a bad program, the memory system will complain  about  this  when
    you dispose the memory.

*/


/* the following flags are adjustables by the user  through  the  LIB_DEBUG
system variable. Flags are updated each time the library is opened */

#define FF_DEBUG_INVOKATION                     (1 << 0)
#define FF_DEBUG_PUBLICSEMAPHORES               (1 << 1)
#define FF_VERBOSE_NEW                          (1 << 0)
#define FF_VERBOSE_DISPOSE                      (1 << 1)

/* GOFROMIEL: SAS/C ne connais pas les macros  à  arguments  variables.  Du
coup j'ai été obligé de mettre en commentaire tes appels. Désolé. */

#if !defined(NDEBUG) && defined(__MORPHOS__)
#define DEBUG_NULL(x, a...)       KPrintF(x,##a);
#define DEBUG_INIT(x, a...)       ({ KPutStr("LIB_Init: "); DEBUG_NULL(x,##a); })
#define DEBUG_OPEN(x, a...)       ({ KPutStr("LIB_Open: "); DEBUG_NULL(x,##a); })
#define DEBUG_CLOSE(x, a...)      ({ KPutStr("LIB_Close: "); DEBUG_NULL(x,##a); })
#define DEBUG_EXPUNGE(x, a...)    ({ KPutStr("LIB_Expunge: "); DEBUG_NULL(x,##a); })
#define DEBUG_FCT(x, a...)        ({ KPrintF("%s: ", __FUNCTION__); DEBUG_NULL(x,##a); })
#else
#define DEBUG_NULL(x)
#define DEBUG_INIT(x)
#define DEBUG_OPEN(x)
#define DEBUG_CLOSE(x)
#define DEBUG_EXPUNGE(x)
#define DEBUG_FCT(x)
#endif /* NDEBUG */

/****************************************************************************
*** Typedefs and structures *************************************************
****************************************************************************/


///Memory
#ifdef __AROS__
/* stegerg CHECKME/FIXME: library PosSize is UWORD but without this change it
   was 65800 bytes which do not fit into an UWORD */   
#define FV_MEMORY_HASH_SIZE                     0x7FF
#define FV_MEMORY_HASH_SHFT                     11
#else
#define FV_MEMORY_HASH_SIZE                     0xFFF
#define FV_MEMORY_HASH_SHFT                     12
#endif
#define F_MEMORY_HASH(mem)                      (((FV_MEMORY_HASH_SIZE << FV_MEMORY_HASH_SHFT) & (uint32)(mem)) >> FV_MEMORY_HASH_SHFT)

#ifdef F_USE_MEMORY_WALL
#define FV_DEBUG_MEMORY_WALL_PATTERN            0xABBAABBA
#endif

typedef struct FeelinMemChunk
{
    struct FeelinMemChunk          *next;
    uint32                          size; /* Complete size (Struct + Alloc) */
/* Memory area follows */
}
FMemChunk;

typedef struct FeelinPuddle
{
    struct FeelinPuddle            *next;
    struct FeelinPuddle            *prev;   /* needed since memory hashing */
    struct FeelinPuddle            *hash_next;
    struct FeelinMemChunk          *chunks;
    struct FeelinMemChunk          *middle;
    APTR                            lower;
    APTR                            upper;
    uint32                          size;   /* Real size of allocation */
    uint32                          free;   /* Memory area follows */
    struct FeelinPool              *pool;   /* needed since memory hashing */
}
FPuddle;

typedef struct FeelinPool
{
    struct FeelinPool              *next;
    struct FeelinPuddle            *puddles;

    struct SignalSemaphore          Semaphore;

    bits32                          flags;
    uint32                          puddle_size;
    uint32                          thresh_size;
#ifdef F_USE_MEMORY_WALL
    uint32                          wall_size;
#endif
}
FPool;
//+
///FeelinClass
struct in_FeelinClass
{
    struct FeelinClass              Public;

/** private **/

    struct Library                 *Module;
    struct FeelinPool              *ObjectsPool;

    FMethod                         Dispatcher;

    uint32                          DynamicID;
    bits32                          Flags;

    struct Catalog                 *Catalog;
};

#define FF_CLASS_INHERITED_POOL                 (1 << 0)

#define FV_Class_DynamicSpace                   64

/* FV_Class_DynamicSpace is the maximum number of methods or attributes per
class */

//+
///FeelinBase
struct in_FeelinBase
{
    struct FeelinBase               Public;

/** Private **/

    uint32                          SegList;
    struct SignalSemaphore          Henes;

    APTR                            DefaultPool;
    APTR                            HashPool;
    APTR                            ClassesPool;
    APTR                            DynamicPool;
    APTR                            NotifyPool;

    struct FeelinPool              *pools;
    struct FeelinPuddle            *hash_puddles[(FV_MEMORY_HASH_SIZE + 1) * sizeof (APTR)];

    FList                           Classes;
    FList                           Dynamic;
    struct FeelinListSemaphored     SharedList;

    FClass                         *ClassClass;

    FHashTable                     *HashClasses;

    // debug stuf

    bits32                          debug_flags;
    bits32                          verbose_flags;

    FClass                         *debug_classdo_class;
    uint32                          debug_classdo_method;
    FObject                         debug_classdo_object;

    uint8                           debug_log_level;
    uint8                           debug_memory_wall_size;

    uint32                          numpuddles;
};

//+

#if defined(__MORPHOS__)
#define FF_LIBRARY                              RTF_AUTOINIT | RTF_PPC | RTF_EXTENDED
#elif defined(__AROS__)
#define FF_LIBRARY                              RTF_AUTOINIT | RTF_EXTENDED
#else
#define FF_LIBRARY                              RTF_AUTOINIT
#endif

/****************************************************************************
*** Prototypes **************************************************************
****************************************************************************/

STRPTR            f_find_attribute_name         (uint32 id,struct in_FeelinBase *FeelinBase);
STRPTR            f_find_method_name            (uint32 id,struct in_FeelinBase *FeelinBase);
FClass *          f_dynamic_find_name           (STRPTR String,struct in_FeelinBase *FeelinBase);
void              f_dynamic_auto_add_class      (FClass *Class,struct in_FeelinBase *FeelinBase);

FClass *          fc_class_create               (struct in_FeelinBase *FeelinBase);
FClass *          fc_object_create              (struct in_FeelinBase *FeelinBase);

/****************************************************************************
*** Public Functions ********************************************************
****************************************************************************/

///Memory Management

#define F_LIB_POOL_CREATE                       LIB_DEFFUNC2(f_pool_create_a, FPool *, \
                                                    d0, uint32, ItemSize, \
                                                    a0, struct TagItem *, Tagl)
LIB_PROTO2(f_pool_create_a, FPool *,
    d0, uint32 ItemSize,
    a0, struct TagItem *Tagl);

#define F_LIB_POOL_DELETE                       LIB_DEFFUNC1(f_pool_delete, uint32, \
                                                    a0, FPool *,Pool)
LIB_PROTO1(f_pool_delete, uint32,
    a0, FPool * Pool);

#define F_LIB_NEW                               LIB_DEFFUNC1(f_new, APTR, \
                                                    d0, uint32, Size)
LIB_PROTO1(f_new, APTR,
    d0, uint32 Size);

#define F_LIB_NEWP                              LIB_DEFFUNC2(f_newp, APTR, \
                                                    a0, FPool *,Pool, \
                                                    d0, uint32, Size)
LIB_PROTO2(f_newp, APTR,
    a0, FPool * Pool,
    d0, uint32 Size);

#define F_LIB_DISPOSE                           LIB_DEFFUNC1(f_dispose, uint32, \
                                                    a1, APTR, Mem)
LIB_PROTO1(f_dispose, uint32,
    a1, APTR Mem);

#define F_LIB_DISPOSEP                          LIB_DEFFUNC2(f_disposep, uint32, \
                                                    a0, FPool *, Pool, \
                                                    a1, APTR, Mem)
LIB_PROTO2(f_disposep, uint32,
    a0, FPool * Pool,
    a1, APTR Mem);

#define F_LIB_OPOOL                             LIB_DEFFUNC1NR(f_opool, \
                                                    a0, FPool *, Pool)
LIB_PROTO1NR(f_opool,
    a0, FPool * Pool);

#define F_LIB_SPOOL                             LIB_DEFFUNC1NR(f_spool, \
                                                    a0, FPool *, Pool)
LIB_PROTO1NR(f_spool,
    a0, FPool * Pool);

#define F_LIB_RPOOL                             LIB_DEFFUNC1NR(f_rpool, \
                                                    a0, FPool *, Pool)
LIB_PROTO1NR(f_rpool,
    a0, FPool * Pool);

//+
///Linking
#define F_LIB_LINK_TAIL                         LIB_DEFFUNC2(f_link_tail, FNode *, \
                                                    a0, FList *, List, \
                                                    a1, FNode *, Node)
LIB_PROTO2(f_link_tail, FNode *,
    a0, FList * List,
    a1, FNode * Node);

#define F_LIB_LINK_HEAD                         LIB_DEFFUNC2(f_link_head, FNode *, \
                                                    a0, FList *, List, \
                                                    a1, FNode *, Node)
LIB_PROTO2(f_link_head, FNode *,
    a0, FList * List,
    a1, FNode * Node);

#define F_LIB_LINK_INSERT                       LIB_DEFFUNC3(f_link_insert, FNode *, \
                                                    a0, FList *, List, \
                                                    a1, FNode *, Node, \
                                                    a2, FNode *, Prev)
LIB_PROTO3(f_link_insert, FNode *,
    a0, FList * List,
    a1, FNode * Node,
    a2, FNode * Prev);

#define F_LIB_LINK_MOVE                         LIB_DEFFUNC3(f_link_move, FNode *, \
                                                    a0, FList *, List, \
                                                    a1, FNode *, Node, \
                                                    a2, FNode *, Prev)
LIB_PROTO3(f_link_move, FNode *,
    a0, FList * List,
    a1, FNode * Node,
    a2, FNode * Prev);

#define F_LIB_LINK_REMOVE                       LIB_DEFFUNC2(f_link_remove, FNode *, \
                                                    a0, FList *, List, \
                                                    a1, FNode *, Node)
LIB_PROTO2(f_link_remove, FNode *,
    a0, FList * List,
    a1, FNode * Node);

#define F_LIB_LINK_MEMBER                       LIB_DEFFUNC2(f_link_member, FNode *, \
                                                    a0, FList *, List, \
                                                    a1, FNode *, Node)
LIB_PROTO2(f_link_member, FNode *,
    a0, FList * List,
    a1, FNode * Node);

#define F_LIB_NEXT_NODE                         LIB_DEFFUNC1(f_next_node, FNode *, \
                                                    a0, FNode **, Nodeptr)
LIB_PROTO1(f_next_node, FNode *,
    a0, FNode ** Nodeptr);

//+
///Strings manipulation
#define F_LIB_STR_FMT                           LIB_DEFFUNC3(f_str_fmtA, APTR, \
                                                    a0, STRPTR, Buffer, \
                                                    a1, STRPTR, Fmt, \
                                                    a2, uint32 *,Params)
LIB_PROTO3(f_str_fmtA, APTR,
    a0, STRPTR Buffer,
    a1, STRPTR Fmt,
    a2, uint32 *Params);

#define F_LIB_STR_NEW                           LIB_DEFFUNC3(f_str_newA, STRPTR, \
                                                    a0, uint32 *, LenPtr, \
                                                    a1, STRPTR, Fmt, \
                                                    a2, uint32 *,Params)
LIB_PROTO3(f_str_newA, STRPTR,
    a0, uint32 *LenPtr,
    a1, STRPTR Fmt,
    a2, uint32 *Params);

#define F_LIB_STR_LEN                           LIB_DEFFUNC1(f_str_len, uint32, \
                                                    a0, STRPTR, Str)
LIB_PROTO1(f_str_len, uint32,
    a0, STRPTR Str);

#define F_LIB_STR_CMP                           LIB_DEFFUNC3(f_str_cmp, int32, \
                                                    a0, STRPTR, Str1, \
                                                    a1, STRPTR, Str2, \
                                                    d1, uint32, Length)
LIB_PROTO3(f_str_cmp, int32,
    a0, STRPTR Str1,
    a1, STRPTR Str2,
    d1, uint32 Length);
//+
///Alert
#define F_LIB_LOG                               LIB_DEFFUNC3NR(f_logA, \
                                                    d0, uint32, Level, \
                                                    a0, STRPTR, Fmt, \
                                                    a1, APTR, Msg)
LIB_PROTO3NR(f_logA,
    d0, uint32 Level,
    a0, STRPTR Fmt,
    a1, APTR Msg);

#define F_LIB_ALERT                             LIB_DEFFUNC3(f_alerta, int32, \
                                                    a0, STRPTR, Title, \
                                                    a1, STRPTR, Body, \
                                                    a2, APTR, Params)
LIB_PROTO3(f_alerta, int32,
    a0, STRPTR Title,
    a1, STRPTR Body,
    a2, APTR Params);

//+
///Hashing
#define F_LIB_HASH_CREATE                       LIB_DEFFUNC1(f_hash_create, FHashTable *, \
                                                    d0, uint32, Size)
LIB_PROTO1(f_hash_create, FHashTable *,
    d0, uint32 Size);

#define F_LIB_HASH_DELETE                       LIB_DEFFUNC1NR(f_hash_delete, \
                                                    a0, FHashTable *, Table)
LIB_PROTO1NR(f_hash_delete,
    a0, FHashTable * Table);

#define F_LIB_HASH_FIND                         LIB_DEFFUNC4(f_hash_find, FHashLink *, \
                                                    a0, FHashTable *, Table, \
                                                    a1, UBYTE *, Key, \
                                                    d1, uint32, KeyLength, \
                                                    a2, uint32 *, HashPtr)
LIB_PROTO4(f_hash_find, FHashLink *,
    a0, FHashTable * Table,
    a1, UBYTE *Key,
    d1, uint32 KeyLength,
    a2, uint32 *HashPtr);

#define F_LIB_HASH_ADD_LINK                     LIB_DEFFUNC2(f_hash_add_link, uint32, \
                                                    a0, FHashTable *, Table, \
                                                    a1, FHashLink *, Link)
LIB_PROTO2(f_hash_add_link, uint32,
    a0, FHashTable * Table,
    a1, FHashLink * Link);

#define F_LIB_HASH_REM_LINK                     LIB_DEFFUNC2(f_hash_rem_link, int32, \
                                                    a0, FHashTable *, Table, \
                                                    a1, FHashLink *, Link)
LIB_PROTO2(f_hash_rem_link, int32,
    a0, FHashTable * Table,
    a1, FHashLink * Link);

#define F_LIB_HASH_ADD                          LIB_DEFFUNC4(f_hash_add, FHashLink *, \
                                                    a0, FHashTable *, Table, \
                                                    a1, UBYTE *, Key, \
                                                    d0, uint32, KeyLength, \
                                                    a2, APTR, Data)
LIB_PROTO4(f_hash_add, FHashLink *,
    a0, FHashTable * Table,
    a1, UBYTE *Key,
    d0, uint32 KeyLength,
    a2, APTR Data);

#define F_LIB_HASH_REM                          LIB_DEFFUNC3(f_hash_rem, int32, \
                                                    a0, FHashTable *, Table, \
                                                    a1, UBYTE *, Key, \
                                                    d0, uint32, KeyLength)
LIB_PROTO3(f_hash_rem, int32,
    a0, FHashTable * Table,
    a1, UBYTE *Key,
    d0, uint32 KeyLength);
//+
///Dynamic System
#define F_LIB_DYNAMIC_CREATE                    LIB_DEFFUNC1(f_dynamic_create, int32, \
                                                    a0, struct in_FeelinClass *, Class)
LIB_PROTO1(f_dynamic_create, int32,
    a0, struct in_FeelinClass *Class);

#define F_LIB_DYNAMIC_DELETE                    LIB_DEFFUNC1NR(f_dynamic_delete, \
                                                    a0, struct FeelinClass *, Class)
LIB_PROTO1NR(f_dynamic_delete,
    a0, struct FeelinClass *Class);

#define F_LIB_DYNAMIC_FIND_ATTRIBUTE            LIB_DEFFUNC3(f_dynamic_find_attribute, FClassAttribute *, \
                                                    a0, STRPTR, Name, \
                                                    a1, FClass *, FromClass, \
                                                    a2, FClass **, RealClass)
LIB_PROTO3(f_dynamic_find_attribute, FClassAttribute *,
    a0, STRPTR Name,
    a1, FClass * FromClass,
    a2, FClass ** RealClass);

#define F_LIB_DYNAMIC_FIND_METHOD               LIB_DEFFUNC3(f_dynamic_find_method, FClassMethod *, \
                                                    a0, STRPTR, Name, \
                                                    a1, FClass *, FromClass, \
                                                    a2, FClass **, RealClass)
LIB_PROTO3(f_dynamic_find_method, FClassMethod *,
    a0, STRPTR Name,
    a1, FClass * FromClass,
    a2, FClass ** RealClass);

#define F_LIB_DYNAMIC_FIND_ID                   LIB_DEFFUNC1(f_dynamic_find_id, uint32, \
                                                    a0, STRPTR, Name)
LIB_PROTO1(f_dynamic_find_id, uint32,
    a0, STRPTR Name);

#define F_LIB_DYNAMIC_RESOLVE_TABLE             LIB_DEFFUNC1(f_dynamic_resolve_table, uint32, \
                                                    a0, FDynamicEntry *, Entries)
LIB_PROTO1(f_dynamic_resolve_table, uint32,
    a0, FDynamicEntry * Entries);

#define F_LIB_DYNAMIC_ADD_AUTO_TABLE            LIB_DEFFUNC1(f_dynamic_add_auto_table, int32, \
                                                    a0, FDynamicEntry *, Table)
LIB_PROTO1(f_dynamic_add_auto_table, int32,
    a0, FDynamicEntry * Table);

#define F_LIB_DYNAMIC_REM_AUTO_TABLE            LIB_DEFFUNC1NR(f_dynamic_rem_auto_table, \
                                                    a0, FDynamicEntry *, Table)
LIB_PROTO1NR(f_dynamic_rem_auto_table,
    a0, FDynamicEntry * Table);

#define F_LIB_DYNAMIC_NTI                       LIB_DEFFUNC3(f_dynamic_nti, struct TagItem *, \
                                                    a0, struct TagItem **, TLP, \
                                                    a1, struct TagItem *, item, \
                                                    a2, struct in_FeelinClass *, Class)
LIB_PROTO3(f_dynamic_nti, struct TagItem *,
    a0, struct TagItem **TLP,
    a1, struct TagItem *item,
    a2, struct in_FeelinClass *Class);

#define F_LIB_DYNAMIC_FTI                       LIB_DEFFUNC2(f_dynamic_fti, struct TagItem *, \
                                                    d0, uint32, Attribute, \
                                                    a0, struct TagItem *, Tags)
LIB_PROTO2(f_dynamic_fti, struct TagItem *,
    d0, uint32 Attribute,
    a0, struct TagItem *Tags);

#define F_LIB_DYNAMIC_GTD                       LIB_DEFFUNC3(f_dynamic_gtd, uint32, \
                                                    d0, uint32, Attribute, \
                                                    d1, uint32, Default, \
                                                    a0, struct TagItem *, Tags)
LIB_PROTO3(f_dynamic_gtd, uint32,
    d0, uint32 Attribute,
    d1, uint32 Default,
    a0, struct TagItem *Tags);
//+
///OOS
#define F_LIB_FIND_CLASS                        LIB_DEFFUNC1(f_find_class, FClass *, \
                                                    a0, STRPTR, Name)
LIB_PROTO1(f_find_class, FClass *,
    a0, STRPTR Name);

#define F_LIB_OPEN_CLASS                        LIB_DEFFUNC1(f_open_class, FClass *, \
                                                    a0, STRPTR, Name)
LIB_PROTO1(f_open_class, FClass *,
    a0, STRPTR Name);

#define F_LIB_CLOSE_CLASS                       LIB_DEFFUNC1(f_close_class, uint32, \
                                                    a0, struct in_FeelinClass *, Class)
LIB_PROTO1(f_close_class, uint32,
    a0, struct in_FeelinClass *Class);

#define F_LIB_CREATE_CLASS                      LIB_DEFFUNC2(f_create_classA, FClass *, \
                                                    a0, STRPTR, Name, \
                                                    a1, struct TagItem *, Tags)
LIB_PROTO2(f_create_classA, FClass *,
    a0, STRPTR Name,
    a1, struct TagItem *Tags);

#define F_LIB_DO                                LIB_DEFFUNC3(f_doa, uint32, \
                                                    a0, FObject, Obj, \
                                                    d0, uint32, Method, \
                                                    a1, APTR, Msg)
LIB_PROTO3(f_doa, uint32,
    a0, FObject Obj,
    d0, uint32 Method,
    a1, APTR Msg);

#define F_LIB_CLASS_DO                          LIB_DEFFUNC4(f_classdoa, uint32, \
                                                    a2, FClass *, Class, \
                                                    a0, FObject, Obj, \
                                                    d0, uint32, Method, \
                                                    a1, APTR, Msg)
LIB_PROTO4(f_classdoa, uint32,
    a2, FClass * Class,
    a0, FObject Obj,
    d0, uint32 Method,
    a1, APTR Msg);

#define F_LIB_SUPER_DO                          LIB_DEFFUNC4(f_superdoa, uint32, \
                                                    a2, FClass *, Class, \
                                                    a0, FObject, Obj, \
                                                    d0, uint32, Method, \
                                                    a1, APTR, Msg)
LIB_PROTO4(f_superdoa, uint32,
    a2, FClass * Class,
    a0, FObject Obj,
    d0, uint32 Method,
    a1, APTR Msg);

#define F_LIB_NEW_OBJ                           LIB_DEFFUNC2(f_new_obja, FObject, \
                                                    a0, STRPTR, Name, \
                                                    a1, struct TagItem *, Tags)
LIB_PROTO2(f_new_obja, FObject,
    a0, STRPTR Name,
    a1, struct TagItem *Tags);

#define F_LIB_MAKE_OBJ                          LIB_DEFFUNC2(f_make_obja, FObject, \
                                                    d0, uint32, Type, \
                                                    a0, uint32 *, Params)
LIB_PROTO2(f_make_obja, FObject,
    d0, uint32 Type,
    a0, uint32 *Params);

#define F_LIB_DISPOSE_OBJ                       LIB_DEFFUNC1(f_dispose_obj, uint32, \
                                                    a0, FObject, Obj)
LIB_PROTO1(f_dispose_obj, uint32,
    a0, FObject Obj);

#define F_LIB_GET                               LIB_DEFFUNC2(f_get, uint32, \
                                                    a0, FObject, Obj, \
                                                    d1, uint32, Tag)
LIB_PROTO2(f_get, uint32,
    a0, FObject Obj,
    d1, uint32 Tag);

#define F_LIB_SET                               LIB_DEFFUNC3NR(f_set, \
                                                    a0, FObject, Obj, \
                                                    d1, uint32, Tag, \
                                                    d2, uint32, Data)
LIB_PROTO3NR(f_set,
    a0, FObject Obj,
    d1, uint32 Tag,
    d2, uint32 Data);

#define F_LIB_DRAW                              LIB_DEFFUNC2NR(f_draw, \
                                                    a0, FObject, Obj, \
                                                    d0, uint32, Flags)
LIB_PROTO2NR(f_draw,
    a0, FObject Obj,
    d0, uint32 Flags);

/*OLAV: Attention, tu avais mis Flags en d1 */

#define F_LIB_LAYOUT                            LIB_DEFFUNC6NR(f_layout, \
                                                    a0, FObject, Obj, \
                                                    d0, WORD, x, \
                                                    d1, WORD, y, \
                                                    d2, UWORD, w, \
                                                    d3, UWORD, h, \
                                                    d4, uint32, Flags)
LIB_PROTO6NR(f_layout,
    a0, FObject Obj,
    d0, WORD x,
    d1, WORD y,
    d2, UWORD w,
    d3, UWORD h,
    d4, bits32 Flags);

#define F_LIB_ERASE                             LIB_DEFFUNC6NR(f_erase, \
                                                    a0, FObject, Obj, \
                                                    d0, WORD, x1, \
                                                    d1, WORD, y1, \
                                                    d2, WORD, x2, \
                                                    d3, WORD, y2, \
                                                    d4, bits32, Flags)
LIB_PROTO6NR(f_erase,
    a0, FObject Obj,
    d0, WORD x1,
    d1, WORD y1,
    d2, WORD x2,
    d3, WORD y2,
    d4, bits32 Flags);
//+
///Shared objects menagement
#define F_LIB_SHARED_FIND                       LIB_DEFFUNC1(f_shared_find, FObject, \
                                                    a0, STRPTR, Name)
LIB_PROTO1(f_shared_find, FObject,
    a0, STRPTR Name);

#define F_LIB_SHARED_CREATE                     LIB_DEFFUNC2(f_shared_create, FObject , \
                                                    a0, FObject, Object, \
                                                    a1, STRPTR, Name)
LIB_PROTO2(f_shared_create, FObject,
    a0, FObject Object,
    a1, STRPTR Name);

#define F_LIB_SHARED_DELETE                     LIB_DEFFUNC1NR(f_shared_delete, \
                                                    a0, FObject, Shared)
LIB_PROTO1NR(f_shared_delete,
    a0, FObject Shared);

#define F_LIB_SHARED_OPEN                       LIB_DEFFUNC1(f_shared_open, FObject, \
                                                    a0, STRPTR, Name)
LIB_PROTO1(f_shared_open, FObject,
    a0, STRPTR Name);

#define F_LIB_SHARED_CLOSE                      LIB_DEFFUNC1NR(f_shared_close, \
                                                    a0, FObject, Shared)
LIB_PROTO1NR(f_shared_close,
    a0, FObject Shared);
//+

