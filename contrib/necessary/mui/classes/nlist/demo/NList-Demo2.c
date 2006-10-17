
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>

#ifdef __GNUC__
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <clib/muimaster_protos.h>
#endif

#ifdef __SASC
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/utility_protos.h>
#include <clib/asl_protos.h>
#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds
#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/asl_pragmas.h>
#include <pragmas/muimaster_pragmas.h>
extern struct Library *SysBase,*IntuitionBase,*UtilityBase,*GfxBase,*DOSBase,*IconBase;
#endif

extern struct Library *MUIMasterBase;

#ifdef __AROS__
#include <aros/asmcall.h>
#include <MUI/NList_mcc.h>
#endif

#include <MUI/NListview_mcc.h>
#include <MUI/NFloattext_mcc.h>

#include "NList-Demo2.h"


#ifdef __SASC
#include <proto/muimaster.h>
#endif


/* *********************************************** */

struct LITD {
  LONG num;
  char str1[4];
  char *str2;
};

/* *********************************************** */

#ifdef __SASC
SAVEDS ASM APTR ConstructLI_TextFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct NList_ConstructMessage *ncm)
{
#endif

#ifdef __GNUC__
#ifdef __AROS__
AROS_UFH3S(APTR, ConstructLI_TextFunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(struct NList_ConstructMessage *, ncm, A1))
{
  AROS_USERFUNC_INIT
#else
static APTR ConstructLI_TextFunc(void)
{ register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct NList_ConstructMessage *a1 __asm("a1"); struct NList_ConstructMessage *ncm = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif
#endif

  struct LITD *new_entry = (struct LITD *) AllocVec(sizeof(struct LITD),0);
  if (new_entry)
  { int i = 0, j = 0;
    new_entry->num = -1;
    new_entry->str2 = (char *) ncm->entry;
    while ((j < 3) && new_entry->str2[i])
    { if ((new_entry->str2[i] > 'A') && (new_entry->str2[i] < 'z'))
        new_entry->str1[j++] = new_entry->str2[i];
      if (new_entry->str2[i] == '\033')
        i++;
      i++;
    }
    new_entry->str1[j] = '\0';

    return (new_entry);
  }
  return (NULL);
#ifdef __AROS__
  AROS_USERFUNC_EXIT
#endif
}

const struct Hook ConstructLI_TextHook = { { NULL,NULL },(VOID *)ConstructLI_TextFunc, NULL,NULL };

/* *********************************************** */


#ifdef __SASC
SAVEDS ASM void DestructLI_TextFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct NList_DestructMessage *ndm)
{
#endif
#ifdef __GNUC__
#ifdef __AROS__
AROS_UFH3S(void, DestructLI_TextFunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(struct NList_DestructMessage *, ndm, A1))
{
  AROS_USERFUNC_INIT
#else
static void DestructLI_TextFunc(void)
{ register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct NList_DestructMessage *a1 __asm("a1"); struct NList_DestructMessage *ndm = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif
#endif
  if (ndm->entry)
    FreeVec((void *) ndm->entry);
#ifdef __AROS__
  AROS_USERFUNC_EXIT
#endif
}

const struct Hook DestructLI_TextHook = { { NULL,NULL },(VOID *)DestructLI_TextFunc, NULL,NULL };

/* *********************************************** */


static char buf[20];

#ifdef __SASC
SAVEDS ASM void DisplayLI_TextFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct NList_DisplayMessage *ndm)
{
#endif
#ifdef __GNUC__
#ifdef __AROS__
AROS_UFH3S(void, DisplayLI_TextFunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(struct NList_DisplayMessage *, ndm, A1))
{
  AROS_USERFUNC_INIT
#else
static void DisplayLI_TextFunc(void)
{ register Object *a2 __asm("a2");                      Object *obj = a2;
  register struct NList_DisplayMessage *a1 __asm("a1"); struct NList_DisplayMessage *ndm = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif
#endif

  struct LITD *entry = (struct LITD *) ndm->entry;

  if (entry)
  { if (entry->num < 0)
      entry->num = ndm->entry_pos;

    ndm->preparses[0]  = "\033c";
    ndm->preparses[1]  = "\033c";

    if      (entry->num % 20 == 3)
      ndm->strings[0] = "\033o[0]";
    else if (entry->num % 20 == 13)
      ndm->strings[0] = "\033o[1]";
    else
    { sprintf(buf,"%ld",entry->num);
      ndm->strings[0]  = buf;
    }

    ndm->strings[1]  = (char *) entry->str1;
    ndm->strings[2]  = (char *) entry->str2;
  }
  else
  {
    ndm->preparses[0]  = "\033c";
    ndm->preparses[1]  = "\033c";
    ndm->preparses[2]  = "\033c";
    ndm->strings[0]  = "Num";
    ndm->strings[1]  = "Short";
    ndm->strings[2]  = "This is the list title !\033n\033b   :-)";
  }

#ifdef __AROS__
  AROS_USERFUNC_EXIT
#endif
}


const struct Hook DisplayLI_TextHook = { { NULL,NULL },(VOID *)DisplayLI_TextFunc, NULL,NULL };


/* *********************************************** */

#ifdef __SASC
SAVEDS ASM LONG CompareLI_TextFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct NList_CompareMessage *ncm)
{
#endif
#ifdef __GNUC__
#ifdef __AROS__
AROS_UFH3S(LONG, CompareLI_TextFunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(struct NList_CompareMessage *, ncm, A1))
{
    AROS_USERFUNC_INIT
#else
static LONG CompareLI_TextFunc(void)
{ register Object *a2 __asm("a2");                      Object *obj = a2;
  register struct NList_CompareMessage *a1 __asm("a1"); struct NList_CompareMessage *ncm = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif
#endif

  struct LITD *entry1 = (struct LITD *) ncm->entry1;
  struct LITD *entry2 = (struct LITD *) ncm->entry2;
  LONG col1 = ncm->sort_type & MUIV_NList_TitleMark_ColMask;
  LONG col2 = ncm->sort_type2 & MUIV_NList_TitleMark2_ColMask;
  LONG result = 0;

/*
  LONG st = ncm->sort_type & MUIV_NList_TitleMark_TypeMask;
kprintf("%lx|Compare() %lx / %lx / %lx\n",obj,ncm->sort_type,st,ncm->sort_type2);
*/

  if (ncm->sort_type == MUIV_NList_SortType_None)
    return (0);

  if      (col1 == 0)
  { if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
      result = entry2->num - entry1->num;
    else
      result = entry1->num - entry2->num;
  }
  else if (col1 == 1)
  { if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
      result = (LONG) stricmp(entry2->str1,entry1->str1);
    else
      result = (LONG) stricmp(entry1->str1,entry2->str1);
  }
  else if (col1 == 2)
  { if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
      result = (LONG) stricmp(entry2->str2,entry1->str2);
    else
      result = (LONG) stricmp(entry1->str2,entry2->str2);
  }

  if ((result != 0) || (col1 == col2))
    return (result);

  if      (col2 == 0)
  { if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
      result = entry2->num - entry1->num;
    else
      result = entry1->num - entry2->num;
  }
  else if (col2 == 1)
  { if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
      result = (LONG) stricmp(entry2->str1,entry1->str1);
    else
      result = (LONG) stricmp(entry1->str1,entry2->str1);
  }
  else if (col2 == 2)
  { if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
      result = (LONG) stricmp(entry2->str2,entry1->str2);
    else
      result = (LONG) stricmp(entry1->str2,entry2->str2);
  }

  return (result);

#ifdef __AROS__
  AROS_USERFUNC_EXIT
#endif
}

const struct Hook CompareLI_TextHook = { { NULL,NULL },(VOID *)CompareLI_TextFunc, NULL,NULL };

