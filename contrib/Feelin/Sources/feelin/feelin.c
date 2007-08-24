/*
**    feelin.c
**
**    THE library :-)
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

$VER 10.00 (2005/08/08)

    GROG: MorphOS adaptation. Operation MrProper! Using direct calls to lib
    functions than thus in inline/ (or ppcinline/).
   
    [Gofromiel]
   
    Added macros to define library functions and arguments, hiding  machine
    depent code further more.
   
    Moved  machine  relative  code   macros   to   <feelin/machine.h>   and
    standardize them. Same for the API basics.

$VER: 09.00 (2005/05/12)
 
   Version bump because of the new preference items.
   
   Added the PUBSEMAPHORES option to the LIB_DEBUG system variable. If this
   option  is  defined,  all semaphores (pools and FC_Semaphore object) are
   made public.
   
   To help portability,  all  libraries  functions  are  now  defined  with
   macros.
 
$VER: 08.10 (2005/01/14)

   The library is finaly completely written in C.

   The variable Feelin/LIB_ERRORLEVEL is now obsolete. The log level, debug
   flags and verbose flags are read together from the same file. ReadArgs()
   is used to parse the parameters. They are updated each time the function
   is opened.

*/

#include "Private.h"

#include <exec/initializers.h>
#include <exec/resident.h>

#define LIBNAME "feelin.library"
#define LIBVERS "Feelin 10.00 (2005/08/08) © Olivier LAVIALE (gofromiel@gofromiel.com)"

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

extern const uint32 lib_init_table[4];
extern const uint8 name[];
extern const uint8 vers[];

/*

GOFROMIEL: Yomgui des bois, JE NE VEUX PAS utiliser de variables  globales.
Tout  est  contenu dans FeelinBase alors pas besoin de se prendre la tête à
faire des #undef partout.  Je  vois  pas  l'intérêt  de  d'initialiser  des
variables  qui  ne  seront  JAMAIS utilisée. Du coup, j'ai tout viré :-) Et
oui, je suis comme ça :-P


YOMGUI: j'm'en fout! j'en met quand même, na! :-P (je suis obligé pour ceux
là) Mais comme je suis vachement sympas je le fait que pour Morphos ;-)

*/

#ifdef __MORPHOS__
uint32 __abox__ = 1;
#endif

#if defined(__MORPHOS__) || defined(__AROS__)
#undef SysBase
#undef DOSBase
#undef LocaleBase

struct FeelinBase   *FeelinBase;
struct ExecBase     *SysBase;
struct DosLibrary   *DOSBase;
#ifdef __AROS__
struct LocaleBase   *LocaleBase;
#else
struct Library      *LocaleBase;
#endif

static inline void initBases(APTR sysbase, APTR dosbase, APTR localebase, APTR feelinbase)
{
    SysBase = sysbase;
    DOSBase = dosbase;
    LocaleBase = localebase;
    FeelinBase = feelinbase;
}

#endif /* __MORPHOS__ */
 
/****************************************************************************
*** Magic *******************************************************************
****************************************************************************/

///lib_return
int32 lib_return(void)
{
   return -1;
}
//+
  
const struct Resident ROMTag =
{
    RTC_MATCHWORD,
    (struct Resident *) &ROMTag,
    (APTR) &lib_init_table,
    FF_LIBRARY,
    FV_FEELIN_VERSION,
    NT_LIBRARY,
    0,
    (STRPTR) name,
    (STRPTR) vers,
    (APTR) &lib_init_table
#if defined(__MORPHOS__) || defined(__AROS__)
    ,FV_FEELIN_REVISION,
    NULL
#endif /* __MORPHOS__ */
};

const uint8 name[] = LIBNAME;
const uint8 vers[] = LIBVERS;

/****************************************************************************
*** Library Interface *******************************************************
****************************************************************************/

///lib_null
STATIC uint32 lib_null(void)
{
   return 0;
}
//+
///lib_exit
STATIC uint32 lib_exit
(
   struct in_FeelinBase *FeelinBase
)
{
    struct FeelinClass *Class;

    //DEBUG_FCT("FeelinBase 0x%p <%s> OpenCount %ld\n",FeelinBase, FeelinBase->Public.Libnode.lib_Node.ln_Name, FeelinBase->Public.Libnode.lib_OpenCnt);

/*** remove remaining classes **********************************************/

    //DEBUG_NULL("remove remaining classes\n");
    
    Class = (FClass *)(FeelinBase -> Classes.Tail);

    while (Class)
    {
       if (Class -> UserCount == 0)
       {
          F_DeleteClass(Class);

          Class = (FClass *)(FeelinBase -> Classes.Tail);
       }
       else
       {
          Class = (FClass *)(Class -> Prev);
       }
    }

    Class = (FClass *)(FeelinBase -> Classes.Tail);

    while (Class)
    {
        if (((struct in_FeelinClass *)(Class)) -> Module)
        {
            F_Log(FV_LOG_USER,"feelin.expunge() - Class '%s' - UserCount %ld - Super '%s' - Module %s{%08lx}",Class -> Name,Class -> UserCount,(Class -> Super) ? Class -> Super -> Name : (STRPTR)("NONE"),((struct in_FeelinClass *)(Class)) -> Module -> lib_Node.ln_Name,((struct in_FeelinClass *)(Class)) -> Module);

            F_DeleteClass(Class);

            Class = (FClass *)(FeelinBase -> Classes.Tail);
        }
        else
        {
            Class = (FClass *)(Class -> Prev);
        }
    }

/*** free resources allocated during lib_init() ****************************/
    
    //DEBUG_NULL("free resources allocated during lib_init()\n");

    F_HashDelete(FeelinBase -> HashClasses); FeelinBase -> HashClasses = NULL;

    while (FeelinBase -> pools)
    {
        F_DeletePool(FeelinBase -> pools);
    }

    if (FeelinBase -> Public.Console)
    {
        Close(FeelinBase -> Public.Console);
        FeelinBase -> Public.Console = NULL;
    }

/*** close libraries *******************************************************/

    //DEBUG_NULL("close libraries\n");

    if (FeelinBase -> Public.DOS)
    {
        CloseLibrary((struct Library *)(FeelinBase -> Public.DOS));
        FeelinBase -> Public.DOS = NULL;
    }

    if (FeelinBase -> Public.Graphics)
    {
        CloseLibrary((struct Library *)(FeelinBase -> Public.Graphics));
        FeelinBase -> Public.Graphics = NULL;
    }

    if (FeelinBase -> Public.Intuition)
    {
        CloseLibrary((struct Library *)(FeelinBase -> Public.Intuition));
        FeelinBase -> Public.Intuition = NULL;
    }

    if (FeelinBase -> Public.Utility)
    {
        CloseLibrary((struct Library *)(FeelinBase -> Public.Utility));
        FeelinBase -> Public.Utility = NULL;
    }

    if (FeelinBase -> Public.Layers)
    {
        CloseLibrary((struct Library *)(FeelinBase -> Public.Layers));
        FeelinBase -> Public.Layers = NULL;
    }

    if (FeelinBase -> Public.Locale)
    {
        CloseLibrary((struct Library *)(FeelinBase -> Public.Locale));
        FeelinBase -> Public.Locale = NULL;
    }
   
    //DEBUG_FCT("bye\n");
   
    return 0;
}
//+

///lib_init
STATIC F_LIB_INIT
{
    F_LIB_INIT_ARGS

    //DEBUG_INIT("FeelinBase 0x%p SegList 0x%lx SysBase 0x%p\n", FeelinBase, SegList, SysBase);
    
#ifdef __AROS__
    /* stegerg CHECKME: Doesn't MorphOS need this, too? */
    SysBase = SYS;
#endif
    
    FeelinBase -> SegList = SegList;
    FeelinBase -> Public.SYS = SYS;
    FeelinBase -> debug_log_level = FV_LOG_CORE;
    FeelinBase -> debug_flags = FF_DEBUG_INVOKATION;

/****************************************************************************
*** Init Semaphores *********************************************************
****************************************************************************/

    InitSemaphore(&FeelinBase -> Henes);
    InitSemaphore(&FeelinBase -> SharedList.Semaphore);

/****************************************************************************
*** Open System Libraries ***************************************************
****************************************************************************/

    if (!(FeelinBase -> Public.DOS = (APTR) OpenLibrary("dos.library",39)))
    {
       Alert(AG_OpenLib | AO_DOSLib); goto __error;
    }

    if (!(FeelinBase -> Public.Graphics = (APTR) OpenLibrary("graphics.library",39)))
    {
       Alert(AG_OpenLib | AO_GraphicsLib); goto __error;
    }

    if (!(FeelinBase -> Public.Intuition = (APTR) OpenLibrary("intuition.library",39)))
    {
       Alert(AG_OpenLib | AO_Intuition); goto __error;
    }

    if (!(FeelinBase -> Public.Utility = (APTR) OpenLibrary("utility.library",39)))
    {
       Alert(AG_OpenLib | AO_UtilityLib); goto __error;
    }

    if (!(FeelinBase -> Public.Layers = (APTR) OpenLibrary("layers.library",39)))
    {
       Alert(AG_OpenLib | AO_LayersLib); goto __error;
    }

/*GOFROMIEL: Coquin !! Tu avais déplacé l'ouverture de locale.library  ici.
Ce  qui  n'est  pas  bien  du tout. Si Henes savait ça il t'aurait cassé un
doigt ! */

    #if defined(__MORPHOS__) || defined(__AROS__)
    
/*
    
YOMGUI: obligatoire et ne peut être mis ailleur
    
GOFROMIEL: bon d'accord :-)
    
*/
    
    initBases(  FeelinBase->Public.SYS,
                FeelinBase->Public.DOS,
                FeelinBase->Public.Locale,
                FeelinBase);
    #endif /* __MORPHOS__ */

/****************************************************************************
*** Create Memory Pools *****************************************************
****************************************************************************/

    /*** This is the default memory pool, used by F_New() ******************/

    FeelinBase -> DefaultPool = F_CreatePool(8192,

        FA_Pool_Items, 1,
        FA_Pool_Name, "feelin-default",

        TAG_DONE);

    if (!FeelinBase -> DefaultPool)
    {
       goto __error;
    }

    /*** This pool is used to create hash links ****************************/

    FeelinBase -> HashPool = F_CreatePool(sizeof (FHashLink),
        
        FA_Pool_Attributes, 0,
        FA_Pool_Items, 128,
        FA_Pool_Name, "feelin-hash",

        TAG_DONE);

    if (!FeelinBase -> HashPool)
    {
       goto __error;
    }

    /*** This pool is used by FC_Class to create instances *****************/

    FeelinBase -> ClassesPool = F_CreatePool(sizeof (struct in_FeelinClass) + 4,

        FA_Pool_Name, "feelin-classes",

        TAG_DONE);

    if (!FeelinBase -> ClassesPool)
    {
       goto __error;
    }

    /*** This pool is used by the Dynamic system ***************************/

    FeelinBase -> DynamicPool = F_CreatePool(1024,

        FA_Pool_Items, 1,
        FA_Pool_Name, "feelin-dynamic",

        TAG_DONE);

    if (!FeelinBase -> DynamicPool)
    {
       goto __error;
    }

    /*** This pool is used by FC_Object for notifycation handlers **********/

    FeelinBase -> NotifyPool = F_CreatePool(1024,

        FA_Pool_Items, 1,
        FA_Pool_Name, "object-notify",

        TAG_DONE);

    if (!FeelinBase -> NotifyPool)
    {
       goto __error;
    }

/****************************************************************************
*** Create Hash Tables ******************************************************
****************************************************************************/

   if (!(FeelinBase -> HashClasses = F_HashCreate(FV_HASH_NORMAL)))
   {
      goto __error;
   }

/****************************************************************************
*** Create Classes **********************************************************
****************************************************************************/

   if (!(fc_class_create(FeelinBase))) goto __error;
   if (!(fc_object_create(FeelinBase))) goto __error;

   return (struct Library *)(FeelinBase);

__error:

   lib_exit(FeelinBase);

   return NULL;
}
//+
///lib_expunge
STATIC F_LIB_EXPUNGE
{
    F_LIB_EXPUNGE_ARGS

    //DEBUG_EXPUNGE("FeelinBase 0x%p <%s> OpenCount %ld\n",FeelinBase, FeelinBase->Public.Libnode.lib_Node.ln_Name, FeelinBase->Public.Libnode.lib_OpenCnt);

    if (!(FeelinBase -> Public.Libnode.lib_OpenCnt))
    {
        uint32 seglist = FeelinBase -> SegList;

        lib_exit(FeelinBase);

        Remove((struct Node *)(FeelinBase));
        
        FreeMem((APTR)((uint32)(FeelinBase) - FeelinBase -> Public.Libnode.lib_NegSize),
            FeelinBase -> Public.Libnode.lib_NegSize +
            FeelinBase -> Public.Libnode.lib_PosSize);
        
        //DEBUG_EXPUNGE("bye!\n");

        return seglist;
    }
    else
    {
       FeelinBase -> Public.Libnode.lib_Flags |= LIBF_DELEXP;
    }
    return 0;
}
//+
///lib_close
STATIC F_LIB_CLOSE
{
    F_LIB_CLOSE_ARGS

    //DEBUG_CLOSE("FeelinBase 0x%p <%s> OpenCount %ld\n",FeelinBase, FeelinBase->Public.Libnode.lib_Node.ln_Name, FeelinBase->Public.Libnode.lib_OpenCnt);

    if (FeelinBase -> Public.Libnode.lib_OpenCnt > 0)
    {
       FeelinBase -> Public.Libnode.lib_OpenCnt--;
    }
 
    if (FeelinBase -> Public.Libnode.lib_OpenCnt == 0)
    {
       if (LIBF_DELEXP & FeelinBase -> Public.Libnode.lib_Flags)
       {
#ifdef __MORPHOS__
            REG_A6 = (uint32) FeelinBase;
         
            return lib_expunge();
#else
            return lib_expunge(FeelinBase);
#endif
        }
    }
    return 0;
}
//+
///lib_open
#define TEMPLATE                    "LEVEL=L/K,MEMWALLSIZE/N,FASTINVOKATION=FI/S,VERBOSE_NEW=VBNEW/S,VERBOSE_DISPOSE=VBDISPOSE/S,PUBSEMAPHORES=PUBSEM/S"
#define CONSOLE                     "CON:0/30//130/Feelin Debug/INACTIVE/AUTO"

enum  {

      OPT_LEVEL,
      OPT_MEMORYWALL,
      OPT_FASTINVOKATION,
      OPT_VERBOSE_NEW,
      OPT_VERBOSE_DISPOSE,
      OPT_PUBSEMAPHORES,

      OPT_COUNT

      };

STATIC F_LIB_OPEN
{
    F_LIB_OPEN_ARGS
   
    STRPTR buf;
    int32 ar[OPT_COUNT];

    //DEBUG_OPEN("FeelinBase 0x%p <%s> OpenCount %ld\n",FeelinBase, FeelinBase->Public.Libnode.lib_Node.ln_Name, FeelinBase->Public.Libnode.lib_OpenCnt);
    ObtainSemaphore(&FeelinBase -> Henes);

    FeelinBase -> Public.Libnode.lib_OpenCnt++;
    FeelinBase -> Public.Libnode.lib_Flags &= ~LIBF_DELEXP;
    
    if (!FeelinBase -> Public.Locale)
    {
        FeelinBase -> Public.Locale = (struct Library *) OpenLibrary("locale.library",39);
    }
    
    ar[OPT_LEVEL]           = (int32)("USER");
    ar[OPT_MEMORYWALL]      = 0;
    ar[OPT_FASTINVOKATION]  = FALSE;
    ar[OPT_VERBOSE_NEW]     = FALSE;
    ar[OPT_VERBOSE_DISPOSE] = FALSE;
    ar[OPT_PUBSEMAPHORES]   = FALSE;

    if ((buf = AllocMem(1024,MEMF_CLEAR)) != NULL)
    {
        BPTR record = Open("T:feelin.log",MODE_READWRITE);
        uint32 len;

        if (record)
        {
            struct DateTime dt;
            char buf_date[32];
            char buf_time[16];

            DateStamp(&dt.dat_Stamp);

            dt.dat_Format  = FORMAT_INT;
            dt.dat_Flags   = 0;
            dt.dat_StrDay  = NULL;
            dt.dat_StrDate = (STRPTR) buf_date;
            dt.dat_StrTime = (STRPTR) buf_time;

            DateToStr(&dt);

            Seek(record,0,OFFSET_END);

            FPuts(record, "\n\n\n");
            FPuts(record, buf_date);
            FPutC(record, ' ');
            FPuts(record, buf_date);
            FPuts(record, " NEW SESSION ***\n");

            Close(record);
        }
       
        /* update debug level, debug flags... */

        GetVar("Feelin/LIB_DEBUG",buf,1023,0);
                
        len = F_StrLen(buf);
        
        if (len)
        {
            struct RDArgs *rda = (struct RDArgs *) AllocDosObject(DOS_RDARGS,NULL);
                
            if (rda)
            {
                rda -> RDA_Source.CS_Buffer = buf;
                rda -> RDA_Source.CS_Length = len;

                if (ReadArgs(TEMPLATE,ar,rda))
                {
                    FreeArgs(rda);
                }

                FreeDosObject(DOS_RDARGS,rda);
            }
        }
        
        /* open console */

        if (!FeelinBase -> Public.Console)
        {
            CopyMem(CONSOLE,buf,F_StrLen(CONSOLE));
            
            GetVar("Feelin/LIB_CONSOLE",buf,1023,0);

            FeelinBase -> Public.Console = Open(buf,MODE_NEWFILE);
        }

        FreeMem(buf,1024);
    }

    if (ar[OPT_LEVEL])
    {
       if (Stricmp((STRPTR)(ar[OPT_LEVEL]),"USER") == 0)        FeelinBase -> debug_log_level = FV_LOG_USER;
       else if (Stricmp((STRPTR)(ar[OPT_LEVEL]),"DEV") == 0)    FeelinBase -> debug_log_level = FV_LOG_DEV;
       else if (Stricmp((STRPTR)(ar[OPT_LEVEL]),"CLASS") == 0)  FeelinBase -> debug_log_level = FV_LOG_CLASS;
       else if (Stricmp((STRPTR)(ar[OPT_LEVEL]),"CORE") == 0)   FeelinBase -> debug_log_level = FV_LOG_CORE;
    }

    if (ar[OPT_FASTINVOKATION])   FeelinBase -> debug_flags   &= ~FF_DEBUG_INVOKATION;
    else                          FeelinBase -> debug_flags   |=  FF_DEBUG_INVOKATION;
    if (ar[OPT_PUBSEMAPHORES])    FeelinBase -> debug_flags   |=  FF_DEBUG_PUBLICSEMAPHORES;
    else                          FeelinBase -> debug_flags   &= ~FF_DEBUG_PUBLICSEMAPHORES;
    if (ar[OPT_VERBOSE_NEW])      FeelinBase -> verbose_flags |=  FF_VERBOSE_NEW;
    else                          FeelinBase -> verbose_flags &= ~FF_VERBOSE_NEW;
    if (ar[OPT_VERBOSE_DISPOSE])  FeelinBase -> verbose_flags |=  FF_VERBOSE_DISPOSE;
    else                          FeelinBase -> verbose_flags &= ~FF_VERBOSE_DISPOSE;
    
    #ifdef F_USE_MEMORY_WALL
    FeelinBase->debug_memory_wall_size = 32;
    //FeelinBase->debug_memory_wall_size = *((uint32 *)(ar[OPT_MEMORYWALL]));
    #endif
    
    ReleaseSemaphore(&FeelinBase -> Henes);
    
    return (struct Library *)(FeelinBase);
}
//+

/****************************************************************************
*** Functions Table *********************************************************
****************************************************************************/

STATIC const APTR lib_func_table[] =
{
#ifdef __MORPHOS__
   (APTR) FUNCARRAY_BEGIN,
   (APTR) FUNCARRAY_32BIT_NATIVE,
#endif /* __MORPHOS__ */
   
   /*** Library interface ***/

   lib_open,
   lib_close,
   lib_expunge,
   lib_null,

   /*** Memory related functions ***/

   f_pool_create_a,
   f_pool_delete,
   lib_null,
   lib_null,
   lib_null,
   lib_null,
   lib_null,

   f_new,
   f_newp,
   f_dispose,
   f_disposep, // deprecated

   f_opool,
   f_spool,
   f_rpool,

   lib_null,

   /*** List related functions ***/

   f_link_tail,
   f_link_head,
   f_link_insert,
   f_link_move,
   f_link_remove,
   f_link_member,
   f_next_node,

   /*** String functions ***/

   f_str_fmtA,
   f_str_newA,
   f_str_len,
   f_str_cmp,

   /*** Bug report functions ***/

   f_logA,
   f_alerta,

   /*** Hash functions ***/
         
   f_hash_create,
   f_hash_delete,
   f_hash_find,
   f_hash_add_link,
   f_hash_rem_link,
   f_hash_add,
   f_hash_rem,

   /*** Dynamic IDs allocation system ***/

   f_dynamic_create,
   f_dynamic_delete,
   f_dynamic_find_attribute,
   f_dynamic_find_method,
   f_dynamic_find_id,
   f_dynamic_resolve_table,
   f_dynamic_add_auto_table,
   f_dynamic_rem_auto_table,
   f_dynamic_nti,
   f_dynamic_fti,
   f_dynamic_gtd,

   /*** Object-oriented system functions ***/

   f_find_class,
   f_open_class,
   f_close_class,
   f_create_classA,
   f_dispose_obj,

   f_doa,
   f_classdoa,
   f_superdoa,

   f_new_obja,
   f_make_obja,
   f_dispose_obj,

   f_get,
   f_set,

   /*** FC_Area support ***/

   f_draw,
   f_layout,
   f_erase,

   /*** Shared objects functions ***/

   f_shared_find,
   f_shared_create,
   f_shared_delete,
   f_shared_open,
   f_shared_close,

   (APTR) -1
#ifdef __MORPHOS__
   (APTR)  FUNCARRAY_END
#endif /* __MORPHOS__ */
};

#if defined(__MORPHOS__) || defined(__AROS__)
#define lib_init_data                           NULL
#else
STATIC const struct FeelinLID lib_init_data[] =
{
   0xA0,  8, NT_LIBRARY, 0,
   0x80, 10, LIBNAME,
   0xA0, 14, LIBF_SUMUSED|LIBF_CHANGED, 0,
   0x90, 20, FV_FEELIN_VERSION,
   0x90, 22, FV_FEELIN_REVISION,
   0x80, 24, LIBVERS,

   F_ARRAY_END
};
#endif

const uint32 lib_init_table[4] =
{
   (uint32) sizeof (struct in_FeelinBase),
   (uint32) lib_func_table,
   (uint32) lib_init_data,
   (uint32) lib_init
};

