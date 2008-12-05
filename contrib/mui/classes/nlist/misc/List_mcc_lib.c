/******************************************************************************/
/*                                                                            */
/* includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include <exec/types.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <dos/dosextens.h>
#include <libraries/mui.h>
#include <graphics/text.h>
#include <clib/muimaster_protos.h>
#include <exec/libraries.h>
#include <proto/exec.h>
#include <proto/graphics.h>

/* define that if you want the mcc class to be expunge at last close */
/* which can be very nice for tests (can avoid lot of avail flush) ! */
/*#define EXPUNGE_AT_LAST_CLOSE*/

/******************************************************************************/
/*                                                                            */
/* MCC/MCP name and version                                                   */
/*                                                                            */
/* ATTENTION:  The FIRST LETTER of NAME MUST be UPPERCASE                     */
/*                                                                            */
/******************************************************************************/

#include "List_priv_mcc.h"

#include "List_mcc.rev.h"

#include <MUI/NList_mcc.h>

#define SUPERCLASS          MUIC_NList
#define CLASS               MCCC_List
#define CLASS_STRUCT_DATA   struct LData
#define CLASS_DISPATCHER    L_Dispatcher

/* make only one of these: */
#define IS_MCC
/*#define IS_MCP*/


const BYTE LibName[] =     CLASS;
const BYTE LibIdString[] = "$VER:" CLASS " " LIB_REV_STRING " (" LIB_DATE ") " LIB_COPYRIGHT;

const UWORD LibVersion = LIB_VERSION;
const UWORD LibRevision = LIB_REVISION;


/*extern ULONG CLASS_DISPATCHER (void);*/
extern ULONG CLASS_DISPATCHER (LONG pad);

/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

#include "mcc_header.c"

#include <proto/muimaster.h>

/******************************************************************************/
/*                                                                            */
/* global declarations (only libs/classes global pointers)                    */
/*                                                                            */
/******************************************************************************/

struct DosLibrary *DOSBase = NULL;
struct Library *UtilityBase = NULL;
struct GfxBase *GfxBase = NULL;
struct Library *IntuitionBase = NULL;

struct MUI_CustomClass *ThisClass = NULL;

/* these one are needed copies for libnix.a */
struct DosLibrary *__DOSBase = NULL;
struct Library *__UtilityBase = NULL;

/* libnix.a can need __LocaleBase, __MathieeeDoubBasBase, __MathieeeSingBase, */
/* and __MathieeeDoubTransBase if you use some functions of it. */

/******************************************************************************/
/*                                                                            */
/* user mui initialization                                                    */
/*                                                                            */
/* !!! CAUTION: This function may run in a forbidden state !!!                */
/*                                                                            */
/* MUIMasterBase, LayersBase and SysBase are already handled by mcc_header.c  */
/******************************************************************************/

int MUI_Mcc_Init(void)
{
  if (ThisClass = MUI_CreateCustomClass(myLibPtr,SUPERCLASS,NULL,
                                        sizeof(CLASS_STRUCT_DATA),(APTR) CLASS_DISPATCHER))
  {
    UtilityBase   = (struct Library *) ThisClass->mcc_UtilityBase;
    DOSBase       = (struct DosLibrary *) ThisClass->mcc_DOSBase;
    GfxBase       = (struct GfxBase *) ThisClass->mcc_GfxBase;
    IntuitionBase = (struct Library *) ThisClass->mcc_IntuitionBase;
    __DOSBase     = DOSBase;
    __UtilityBase = UtilityBase;
    return (TRUE);
  }

  ThisClass = NULL;
  DOSBase = NULL;
  UtilityBase = NULL;
  GfxBase = NULL;
  IntuitionBase = NULL;
  __DOSBase = NULL;
  __UtilityBase = NULL;
  return (FALSE);
}

/******************************************************************************/
/*                                                                            */
/* user mui cleanup                                                           */
/*                                                                            */
/* !!! CAUTION: This function runs in a forbidden state !!!                   */
/*                                                                            */
/* MUIMasterBase, LayersBase and SysBase are already handled by mcc_header.c  */
/******************************************************************************/

void MUI_Mcc_Cleanup(void)
{
  if (ThisClass)
    MUI_DeleteCustomClass( ThisClass );
  ThisClass = NULL;
  DOSBase = NULL;
  UtilityBase = NULL;
  GfxBase = NULL;
  IntuitionBase = NULL;
  __DOSBase = NULL;
  __UtilityBase = NULL;
}

/******************************************************************************/
/*                                                                            */
/* query function                                                             */
/*                                                                            */
/******************************************************************************/

ULONG MUI_Mcc_Query(LONG which)
{
  switch (which)
  {
    case 0:
#ifdef IS_MCC
      return ((ULONG)ThisClass);
#else
      return (NULL);
#endif

    case 1:
#ifdef IS_MCP
      return ((ULONG)ThisClass);
#else
      return (NULL);
#endif

    case 2:
    {
#ifdef EXPUNGE_AT_LAST_CLOSE
      return (NULL);
#else
#ifdef PREFSIMAGEOBJECT
      Object *obj = PREFSIMAGEOBJECT;
      return((ULONG)obj);
#else
      return (NULL);
#endif
#endif
    }

#ifdef IS_MCP
    case 3:
    {
#ifdef ONLYGLOBAL
      return(TRUE);
#else
      return (FALSE);
#endif
    }
#endif
  }
  return((ULONG)NULL);
}

