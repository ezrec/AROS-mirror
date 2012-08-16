/*------------------------------------------*/
/* Code generated with ChocolateCastle 0.4  */
/* written by Grzegorz "Krashan" Kraszewski */
/* <krashan@teleinfo.pb.edu.pl>             */
/*------------------------------------------*/

#ifndef CHOCOLATE_CASTLE_EDITOR_H
#define CHOCOLATE_CASTLE_EDITOR_H

/* EditorClass header. */

#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>

#if !defined __AROS__ && !defined __MORPHOS__ && !defined __amigaos4__
 #ifndef __amigaos3__
  #define __amigaos3__
 #endif
#endif

extern struct MUI_CustomClass *EditorClass;

struct MUI_CustomClass *CreateEditorClass(void);
void DeleteEditorClass(void);

/* Attributes */

#define EDLA_List          0x6EDA8A90  // [I..] the list object
#define EDLA_WindowID      0x6EDA8A91  // [..G] editor window ID (for subclasses)
#define EDLA_WindowTitle   0x6EDA8A92  // [..G] editor window title (for subclasses)
#define EDLA_Window        0x6EDA8A93  // [..G] Editor window object (for subclasses)
#define EDLA_Generator     0x6EDA8A94  // [I..] parent generator (handled in subclasses)

/* Methods */

/* All these methods are implemented in subclasses. */

#define EDLM_ListToWindow  0x6EDA3F57  // put list entry into edit gadgets
#define EDLM_WindowToList  0x6EDA3F58  // put edit gads into a list entry
#define EDLM_BuildEditor   0x6EDA3F59  // create edit gadgets group
#define EDLM_Action        0x6EDAD459  // in subclass: prepare editor window
#define EDLM_Clear         0x6EDAD45A  // in subclass: clears editor for a new method

#define EDLV_Action_Add    0x6EDA9FE3
#define EDLV_Action_Edit   0x6EDA7A4D

struct EDLP_Action
{
	ULONG  MethodID;
	IPTR   ActionType;
};

struct EDLP_ListToWindow
{
	ULONG   MethodID;
	IPTR    Position;
};

struct EDLP_WindowToList
{
	ULONG   MethodID;
	IPTR    Position;
};

#endif /* CHOCOLATE_CASTLE_EDITOR_H */

