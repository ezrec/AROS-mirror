// :ts=4 (Tabsize)

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
#include <proto/utility.h>
#include <proto/alib.h>
#include <proto/graphics.h>
#include <proto/exec.h>

#include <string.h>

#include "Scalos.h"
#include "TextClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"

/****** Text.scalos/--background ********************************************
*
*   Text class is a class of graphic object to display strings on screen.
*
*****************************************************************************
*/
// /
/****** Text.scalos/SCCA_Text_String ****************************************
*
*  NAME
*   SCCA_Text_String -- (V40) ISG (char *)
*
*  FUNCTION
*   This attribute sets the string which should be displayed.
*
*  NOTE
*   This attribute will be ignored between PreThinkWindow and
*   PostThinkWindow. To changing it PostThinkWindow has to be called.
*
*****************************************************************************
*/                                                              
// /
/****** Text.scalos/SCCA_Text_Type ******************************************
*
*  NAME
*   SCCA_Text_Type -- (V40) IS. (ULONG)
*
*  FUNCTION
*   Using this attribute the type of text can be selected.
*   These types are available:
*       SCCV_Text_Type_Normal - standard text
*       SCCV_Text_Type_Outline - outline text
*       SCCV_Text_Type_Shadow - shadowed text
*
*****************************************************************************
*/
// /
/****** Text.scalos/SCCA_Text_Style *****************************************
*
*  NAME
*   SCCA_Text_Style -- (V40) IS. (UBYTE)
*
*  FUNCTION
*   Set the style of the text. A struct TextAttr Style can be used.
*
*****************************************************************************
*/
// /
/****** Text.scalos/SCCA_Text_TextPen ***************************************
*
*  NAME
*   SCCA_Text_TextPen -- (V40) IS. (ULONG)
*
*  FUNCTION
*   This attribute sets the color pen of the text.
*   Default is the drawinfo textpen.
*
*****************************************************************************
*/
// /
/****** Text.scalos/SCCA_Text_BackPen ***************************************
*
*  NAME
*   SCCA_Text_BackPen -- (V40) IS. (ULONG)
*
*  FUNCTION
*   This attribute sets the background pen of the text. Special value:
*       SCCV_Text_BackPen_Transparent - the text background is transparent
*   Default is the drawinfo backgroundpen.
*
*****************************************************************************
*/
// /
/****** Text.scalos/SCCA_Text_Font ******************************************
*
*  NAME
*   SCCA_Text_Font -- (V40) IS. (struct TextAttr *)
*
*  FUNCTION
*   Select a font for the text. Default is the rastports font.
*
*****************************************************************************
*/
// /


/** PreThinkWindow
*/
static ULONG Text_PreThinkWindow(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_PreThinkWindow *msg, struct TextInst *inst)
{
        UWORD width;
        struct DrawInfo *dri;

        if (SC_DoSuperMethodA(cl,obj,(Msg) msg))
        {
                if ((dri = (struct DrawInfo *) get(((struct SC_GraphicObject *) obj)->screenobj,SCCA_Screen_DrawInfo)))
                {
                        width = TextLength(msg->rastport,inst->string,inst->numchars);
                        if (inst->type == SCCV_Text_Type_Outline)
                                width += 2;
                        else if (inst->type == SCCV_Text_Type_Shadow)
                                width += 1;

                        ((struct SC_GraphicObject *) obj)->bounds.Width = width;
                        ((struct SC_GraphicObject *) obj)->bounds.Height = msg->rastport->TxHeight;

                        if (inst->textpen == -1)
                                inst->apen = dri->dri_Pens[TEXTPEN];
                        else
                                inst->apen = inst->textpen;

                        if (inst->backpen == -1)
                                inst->bpen = dri->dri_Pens[BACKGROUNDPEN];
                        else
                                inst->bpen = inst->backpen;

                        if (inst->outlinepen == -1)
                                inst->open = dri->dri_Pens[SHADOWPEN];
                        else
                                inst->open = inst->outlinepen;

                        return(TRUE);
                }
                else
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkWindow);
                }
        }
        return(FALSE);
}
// /

/** Draw
*/
static void Text_Draw(struct SC_Class *cl, SC_GraphicObject *obj, struct SCCP_Graphic_Draw *msg, struct TextInst *inst)
{
        int x,y;

        if (msg->drawflags == SCCV_Graphic_DrawFlags_AbsPos)
        {
                x = msg->posX;
                y = msg->posY + obj->rastport->TxBaseline;
        }
        else
        {
                x = msg->posX + obj->bounds.Left;
                y = msg->posY + obj->bounds.Top + obj->rastport->TxBaseline;
        }

        if (inst->backpen == SCCV_Text_BackPen_Transparent)
                SetABPenDrMd(obj->rastport,inst->apen,0,JAM1);
        else
                SetABPenDrMd(obj->rastport,inst->apen,inst->bpen,JAM2);

        switch (inst->type)
        {
          case SCCV_Text_Type_Outline :
                SetAPen(obj->rastport,inst->open);
                Move(obj->rastport,x,y+1);
                Text(obj->rastport,inst->string,inst->numchars);
                SetDrMd(obj->rastport,JAM1);
                Move(obj->rastport,x+1,y);
                Text(obj->rastport,inst->string,inst->numchars);
                Move(obj->rastport,x+1,y+2);
                Text(obj->rastport,inst->string,inst->numchars);
                Move(obj->rastport,x+2,y+1);
                Text(obj->rastport,inst->string,inst->numchars);
                SetAPen(obj->rastport,inst->apen);
                Move(obj->rastport,x+1,y+1);
                Text(obj->rastport,inst->string,inst->numchars);
                break;

          case SCCV_Text_Type_Shadow :
                SetAPen(obj->rastport,inst->open);
                Move(obj->rastport,x+1,y+1);
                Text(obj->rastport,inst->string,inst->numchars);
                SetABPenDrMd(obj->rastport,inst->apen,0,JAM1);

          case SCCV_Text_Type_Normal :
                Move(obj->rastport,x,y);
                Text(obj->rastport,inst->string,inst->numchars);
                break;
        }

}
// /

/** PostThinkWindow
*/
// /

/** Init a text object
*/
static ULONG Text_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct TextInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {
                struct TagItem *tag;

                if ((tag = FindTagItem(SCCA_Text_String,msg->ops_AttrList)))
                {
                        if ((inst->string = AllocCopyString((char *)tag->ti_Data)))
                                inst->numchars = strlen(inst->string);
                }

                if ((tag = FindTagItem(SCCA_Text_Font,msg->ops_AttrList)))
                {
                        struct TextAttr *font = (struct TextAttr *) tag->ti_Data;

                        if (font)
                        {
                                inst->font = *font;
                        }
                }

                inst->type = GetTagData(SCCA_Text_Type,SCCV_Text_Type_Normal,msg->ops_AttrList);
                inst->style = GetTagData(SCCA_Text_Style,0,msg->ops_AttrList);
                inst->textpen = GetTagData(SCCA_Text_TextPen,-1,msg->ops_AttrList);
                inst->outlinepen = GetTagData(SCCA_Text_OutlinePen,-1,msg->ops_AttrList);
                inst->backpen = GetTagData(SCCA_Text_BackPen,-1,msg->ops_AttrList);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Set one or more setable attribute(s)
 */
static void Text_Set( struct SC_Class *cl, Object *obj, struct opSet *msg, struct TextInst *inst )
{
        struct TagItem *tag;

        if ((tag = FindTagItem(SCCA_Text_String,msg->ops_AttrList)))
        {
                if (inst->string)
                        FreeVec(inst->string);

                inst->string = AllocCopyString((char *) tag->ti_Data);
                inst->numchars = strlen(inst->string);
        }

        inst->type = GetTagData(SCCA_Text_Type,inst->type,msg->ops_AttrList);
        inst->style = GetTagData(SCCA_Text_Style,inst->style,msg->ops_AttrList);
        inst->textpen = GetTagData(SCCA_Text_TextPen,inst->textpen,msg->ops_AttrList);
        inst->outlinepen = GetTagData(SCCA_Text_OutlinePen,inst->outlinepen,msg->ops_AttrList);
        inst->backpen = GetTagData(SCCA_Text_BackPen,inst->backpen,msg->ops_AttrList);

        SC_DoSuperMethodA(cl,obj,(Msg) msg);
}
// /

/** Get one attribute of all of the getable attributes
*/
static ULONG Text_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct TextInst *inst )
{
        if (msg->opg_AttrID == SCCA_Text_String)
        {
                *(msg->opg_Storage) = (LONG) inst->string;
                return(TRUE);
        }

        return(SC_DoSuperMethodA(cl, obj, (Msg) msg));
}
// /

/** exit routine
*/
static void Text_Exit( struct SC_Class *cl, Object *obj, Msg msg, struct TextInst *inst )
{
        if (inst->string)
                FreeVec(inst->string);
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData TextMethods[] =
{
        { OM_SET,(ULONG)                        Text_Set, 0, 0, NULL },               // derived from RootClass
        { OM_GET,(ULONG)                        Text_Get, 0, 0, NULL },               // derived from RootClass
        { SCCM_Init,(ULONG)                     Text_Init, 0, 0, NULL },              // derived from RootClass
        { SCCM_Graphic_PreThinkWindow,(ULONG)   Text_PreThinkWindow, 0, 0, NULL },    // a new method
        { SCCM_Graphic_Draw,(ULONG)             Text_Draw, 0, 0, NULL },              // derived from AreaClass
        { SCCM_Exit,(ULONG)                     Text_Exit, 0, 0, NULL }, // derived from RootClass
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

