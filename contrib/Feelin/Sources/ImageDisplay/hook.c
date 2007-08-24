#include "Private.h"

typedef struct Hook *                           FImage;

///id_hook_create
STATIC F_CODE_CREATE_XML(id_hook_create)
{
    struct Hook *src=NULL;

    for ( ; Attribute ; Attribute = Attribute -> Next)
    {
        switch (Attribute -> Name -> ID)
        {
            case FV_XML_ID_SRC:
            {
                src = (struct Hook *) F_Do(CUD->XMLDocument,F_ID(CUD->IDs, FM_Document_Resolve), Attribute -> Data, FV_TYPE_HEXADECIMAL, NULL, NULL);
            }
            break;
        }
    }
                                    
    if (TypeOfMem(src))
    {
        return src;
    }
 
    F_Log(0,"Suspicious hook 0x%08lx",src);
    
    return NULL;
}
//+
///id_hook_draw
STATIC F_CODE_DRAW(id_hook_draw)
{
    FRect srect;
    uint32 handle;
    
    struct FS_ImageDisplay_HookDraw hd_msg;
    
    hd_msg.Render = Msg -> Render;
    hd_msg.Rect   = Msg -> Rect;
    hd_msg.Flags  = Msg -> Flags;
    hd_msg.Region = &srect;
    
    if (Msg->Origin)
    {
        srect.x1 = Msg->Origin->x;
        srect.y1 = Msg->Origin->y;
        srect.x2 = Msg->Origin->x + Msg->Origin->w - 1;
        srect.y2 = Msg->Origin->y + Msg->Origin->h - 1;
    }
    else
    {
        struct Window *win = (struct Window *)(F_Get(Msg -> Render -> Window,FA_Window));

        if (win &&
            win -> RPort == Msg -> Render -> RPort)
        {
            srect.x1 = 0 ; srect.x2 = win -> Width - 1;
            srect.y1 = 0 ; srect.y2 = win -> Height - 1;
        }
        else
        {
            srect.x1 = 0 ; srect.x2 = GetBitMapAttr(Msg -> Render -> RPort -> BitMap,BMA_WIDTH) - 1;
            srect.y1 = 0 ; srect.y2 = GetBitMapAttr(Msg -> Render -> RPort -> BitMap,BMA_HEIGHT) - 1;
        }
    }

    handle = F_Do(Msg -> Render, FM_Render_AddClip, Msg -> Rect);

    CallHookPkt((struct Hook *) image,NULL,&hd_msg);

    F_Do(Msg -> Render, FM_Render_RemClip, handle);
    
    return TRUE;
}
//+

struct in_CodeTable id_hook_table =
{
    (in_Code_Create *)  &id_hook_create,
    NULL,
    NULL,
    NULL,
    NULL,
    (in_Code_Draw *)    &id_hook_draw
};

