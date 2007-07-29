
/*
** $Id: popplaceholder_mcc.h,v 1.6 2000/04/04 17:58:20 carlos Exp $
**
** © 1999 Marcin Orlowski <carlos@amiga.com.pl>
*/


/*** Include stuff ***/


#ifndef POPPLACEHOLDER_MCC_H
#define POPPLACEHOLDER_MCC_H

#ifndef LIBRARIES_MUI_H
#include "libraries/mui.h"
#endif


/*** MUI Defines ***/

#define MUIC_Popplaceholder  "Popplaceholder.mcc"
#define MUIC_Popplaceholderp "Popplaceholder.mcp"
#define PopplaceholderObject MUI_NewObject(MUIC_Popplaceholder

#define MUIC_Popph  "Popplaceholder.mcc"
#define MUIC_Popphp "Popplaceholder.mcp"
#define PopphObject MUI_NewObject(MUIC_Popph

#ifndef CARLOS_MUI
#define MUISERIALNR_CARLOS 2447
#define TAGBASE_CARLOS (TAG_USER | ( MUISERIALNR_CARLOS << 16))
#define CARLOS_MUI
#endif

#define TBPPH TAGBASE_CARLOS


/*** Methods ***/

#define MUIM_Popph_OpenAsl     (TBPPH + 0x0000)       /* PRIVATE */
#define MUIM_Popph_DoCut       (TBPPH + 0x0001)
#define MUIM_Popph_DoCopy      (TBPPH + 0x0002)
#define MUIM_Popph_DoPaste     (TBPPH + 0x0003)
#define MUIM_Popph_DoClear     (TBPPH + 0x0004)

/*** Method structs ***/


/*** Special method values ***/


/*** Special method flags ***/


/*** Attributes ***/


#define MUIA_Popph_Array         (TBPPH + 0x0010)    /* v14 {IS.} APTR    */
#define MUIA_Popph_Separator     (TBPPH + 0x0011)    /* v14 {ISG} CHAR default '|' */
#define MUIA_Popph_Contents      (TBPPH + 0x0012)    /* v14 {ISG} STRPTR  */
#define MUIA_Popph_StringKey     (TBPPH + 0x0013)    /* v14 {IS.} CHAR    */
#define MUIA_Popph_PopbuttonKey  (TBPPH + 0x0014)    /* v14 {IS.} CHAR    */
#define MUIA_Popph_StringMaxLen  (TBPPH + 0x0015)    /* v14 {I..} ULONG   */
#define MUIA_Popph_CopyEntries   (TBPPH + 0x0016)    /* v14 {ISG} BOOL    */
#define MUIA_Popph_PopAsl        (TBPPH + 0x0017)    /* v15 {I..} BOOL    */
#define MUIA_Popph_AslActive     (TBPPH + 0x0018)    /* v15 {..G} ULONG   */
#define MUIA_Popph_AslType       (TBPPH + 0x0019)    /* v15 {I..} ULONG   */
#define MUIA_Popph_Avoid         (TBPPH + 0x001a)    /* v15 {I..} ULONG   */
#define MUIA_Popph_StringType    (TBPPH + 0x001b)    /* v15 {..G} ULONG   */
#define MUIA_Popph_ReplaceMode   (TBPPH + 0x001c)    /* v15 {ISG} ULONG   */
#define MUIA_Popph_StringObject  (TBPPH + 0x001d)    /* v15 {..G} APTR    */
#define MUIA_Popph_ListObject    (TBPPH + 0x001e)    /* v15 {..G} APTR    */
#define MUIA_Popph_DropObject    (TBPPH + 0x001f)    /* v15 {IS.} PRIVATE */
#define MUIA_Popph_BufferPos     (TBPPH + 0x0020)    /* v14 {ISG} PRIVATE */
#define MUIA_Popph_MaxLen        (TBPPH + 0x0021)    /* v14 {I.G} PRIVATE */
#define MUIA_Popph_ContextMenu   (TBPPH + 0x0022)    /* v15 {ISG} BOOL    */
#define MUIA_Popph_PopCycleChain (TBPPH + 0x0023)    /* v15 (ISG) BOOL    */
#define MUIA_Popph_Title         (TBPPH + 0x0024)    /* v15 (IS.) STRPTR  */
#define MUIA_Popph_SingleColumn  (TBPPH + 0x0025)    /* V15 (ISG) BOOL    */
#define MUIA_Popph_AutoClose     (TBPPH + 0x0026)    /* V15 (ISG) BOOL    */


/*** Special attribute values ***/

#define MUIV_Popph_StringType_String        0
#define MUIV_Popph_StringType_Betterstring  1
#define MUIV_Popph_StringType_Textinput     2

#define MUIV_Popph_Avoid_Betterstring  1<<0
#define MUIV_Popph_Avoid_Textinput     1<<1
//#define MUIV_Popph_Avoid_Nlist         1<<2      /* not supported yet! */

#define MUIV_Popph_InsertMode_DD_Default    1<<0
#define MUIV_Popph_InsertMode_DD_CursorPos  1<<1
#define MUIV_Popph_InsertMode_DD_Apend      1<<2
#define MUIV_Popph_InsertMode_DD_Prepend    1<<3
#define MUIV_Popph_InsertMode_DC_Default    1<<4
#define MUIV_Popph_InsertMode_DC_CursorPos  1<<5
#define MUIV_Popph_InsertMode_DC_Apend      1<<6
#define MUIV_Popph_InsertMode_DC_Prepend    1<<7

#define MUIV_Popph_InsertMode_Default       MUIV_Popph_InsertMode_DD_Default   | MUIV_Popph_InsertMode_DC_Default
#define MUIV_Popph_InsertMode_CursorPos     MUIV_Popph_InsertMode_DD_CursorPos | MUIV_Popph_InsertMode_DC_CursorPos
#define MUIV_Popph_InsertMode_Apend         MUIV_Popph_InsertMode_DD_Apend     | MUIV_Popph_InsertMode_DC_Apend
#define MUIV_Popph_InsertMode_Prepend       MUIV_Popph_InsertMode_DD_Prepend   | MUIV_Popph_InsertMode_DC_Prepend

/*** Structures, Flags & Values ***/


/*** Configs ***/



/*** Other things ***/

#define POPPH_MAX_KEY_LEN       50       /* touch this and die! */
#define POPPH_MAX_STRING_LEN   128       /* touch this and die! */


#endif /* POPPLACEHOLDER_MCC_H */

