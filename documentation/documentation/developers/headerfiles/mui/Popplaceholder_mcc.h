#ifndef POPPLACEHOLDER_MCC_H
#define POPPLACEHOLDER_MCC_H

/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <libraries/mui.h>

/*** Name *******************************************************************/
#define MUIC_Popplaceholder      "Popplaceholder.mcc"
#define MUIC_Popph               "Popplaceholder.mcc"

/*** Identifier base ********************************************************/
#define MUIB_Popplaceholder      (TAG_USER | (2447 << 16))

/*** Attributes *************************************************************/
#define MUIA_Popph_Array         (MUIB_Popplaceholder | 0x0010)    /* v14 {IS.} APTR    */
#define MUIA_Popph_Separator     (MUIB_Popplaceholder | 0x0011)    /* v14 {ISG} CHAR default '|' */
#define MUIA_Popph_Contents      (MUIB_Popplaceholder | 0x0012)    /* v14 {ISG} STRPTR  */
#define MUIA_Popph_StringKey     (MUIB_Popplaceholder | 0x0013)    /* v14 {IS.} CHAR    */
#define MUIA_Popph_PopbuttonKey  (MUIB_Popplaceholder | 0x0014)    /* v14 {IS.} CHAR    */
#define MUIA_Popph_StringMaxLen  (MUIB_Popplaceholder | 0x0015)    /* v14 {I..} ULONG   */
#define MUIA_Popph_CopyEntries   (MUIB_Popplaceholder | 0x0016)    /* v14 {ISG} BOOL    */
#define MUIA_Popph_PopAsl        (MUIB_Popplaceholder | 0x0017)    /* v15 {I..} BOOL    */
#define MUIA_Popph_AslActive     (MUIB_Popplaceholder | 0x0018)    /* v15 {..G} ULONG   */
#define MUIA_Popph_AslType       (MUIB_Popplaceholder | 0x0019)    /* v15 {I..} ULONG   */
#define MUIA_Popph_Avoid         (MUIB_Popplaceholder | 0x001a)    /* v15 {I..} ULONG   */
#define MUIA_Popph_StringType    (MUIB_Popplaceholder | 0x001b)    /* v15 {..G} ULONG   */
#define MUIA_Popph_ReplaceMode   (MUIB_Popplaceholder | 0x001c)    /* v15 {ISG} ULONG   */
#define MUIA_Popph_StringObject  (MUIB_Popplaceholder | 0x001d)    /* v15 {..G} APTR    */
#define MUIA_Popph_ListObject    (MUIB_Popplaceholder | 0x001e)    /* v15 {..G} APTR    */
#define MUIA_Popph_DropObject    (MUIB_Popplaceholder | 0x001f)    /* v15 {IS.} PRIVATE */
#define MUIA_Popph_BufferPos     (MUIB_Popplaceholder | 0x0020)    /* v14 {ISG} PRIVATE */
#define MUIA_Popph_MaxLen        (MUIB_Popplaceholder | 0x0021)    /* v14 {I.G} PRIVATE */
#define MUIA_Popph_ContextMenu   (MUIB_Popplaceholder | 0x0022)    /* v15 {ISG} BOOL    */
#define MUIA_Popph_PopCycleChain (MUIB_Popplaceholder | 0x0023)    /* v15 (ISG) BOOL    */
#define MUIA_Popph_Title         (MUIB_Popplaceholder | 0x0024)    /* v15 (IS.) STRPTR  */
#define MUIA_Popph_SingleColumn  (MUIB_Popplaceholder | 0x0025)    /* V15 (ISG) BOOL    */
#define MUIA_Popph_AutoClose     (MUIB_Popplaceholder | 0x0026)    /* V15 (ISG) BOOL    */

/*** Methods ***/
#define MUIM_Popph_OpenAsl     (MUIB_Popplaceholder + 0x0000)       /* PRIVATE */
#define MUIM_Popph_DoCut       (MUIB_Popplaceholder + 0x0001)
#define MUIM_Popph_DoCopy      (MUIB_Popplaceholder + 0x0002)
#define MUIM_Popph_DoPaste     (MUIB_Popplaceholder + 0x0003)
#define MUIM_Popph_DoClear     (MUIB_Popplaceholder + 0x0004)

/*** Values ***/
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

#define POPPH_MAX_KEY_LEN       50
#define POPPH_MAX_STRING_LEN   128

/*** Macros *****************************************************************/
#define PopplaceholderObject MUIOBJMACRO_START(MUIC_Popplaceholder)
#define PopphObject MUIOBJMACRO_START(MUIC_Popph)

#endif /* POPPLACEHOLDER_MCC_H */
