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
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <guigfx/guigfx.h>
#include <proto/guigfx.h>

#include "Scalos.h"
#include "BitmapClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "SubRoutines.h"
#include "Debug.h"

/****** Bitmap.scalos/--background ******************************************
*
*   Bitmap class is the lowlevel class for blitting graphics data of
*   several formats to a RastPort.
*   It is derived from ScalosV2 Rootclass.
*   One object of this class represents one abstract bitmap, not several,
*   and can do some actions with it.
*   The methods of this class require guigfx.library version 8.3.
*
*   A valid calling sequence for the methods is for example:
*
*   PreThinkWindow with SCCV_Area_Action_OpenClose
*       Draw
*       Draw
*       ...
*       PreThinkWindow with SCCV_Area_Action_Resize
*           Draw
*           ...
*           PreThinkWindow with SCCV_Area_Action_Resize
*               Draw
*               ...
*           PostThinkWindow with SCCV_Area_Action_Resize
*       PostThinkWindow with SCCV_Area_Action_Resize
*   PostThinkWindow with SCCV_Area_Action_OpenClose
*
*****************************************************************************
*/
// /
/****** Bitmap.scalos/SCCA_Bitmap_Bitmap ************************************
*
*  NAME
*   SCCA_Bitmap_Bitmap -- (V40) I.G (struct BitMap *)
*
*  FUNCTION
*   This attribute is a pointer to the graphics data for the object.
*   Only one of SCCA_Bitmap_Bitmap and SCCA_Bitmap_RawData may be
*   specified for a bitmap object and one of them needs to be specified.
*   Specifying a zero has the same effect as if you don`t specify the tag
*   at all.
*
*   The bitmap structure may be up to 8 bit deep.
*   It may be scaleable if you provide a palette for it.
*   Refer to SCCA_Bitmap_RawData, SCCA_Bitmap_BltMask for more info about
*   scaleability.
*
*  SEE ALSO
*   SCCA_Bitmap_Width, SCCA_Bitmap_Height
*
*****************************************************************************
*/                                                              
// /
/****** Bitmap.scalos/SCCA_Bitmap_RawData ***********************************
*
*  NAME
*   SCCA_Bitmap_RawData -- (V40) I.G (APTR)
*
*  FUNCTION
*   This attribute is a pointer to the graphics data for the object.
*   Only one of SCCA_Bitmap_Bitmap and SCCA_Bitmap_RawData may be
*   specified for a bitmap object and one of them needs to be specified.
*   Specifying a zero has the same effect as if you don`t specify the tag
*   at all.
*   
*   Chunky data may be 8 bit or 24 bit deep and may be scaleable with or
*   without a palette for it.
*   Refer to SCCA_Bitmap_Bitmap, SCCA_Bitmap_BltMask for more info about
*   scaleability.
*
*  SEE ALSO
*   SCCA_Bitmap_RawType, SCCA_Bitmap_Width, SCCA_Bitmap_Height
*
*****************************************************************************
*/                                                              
// /
/****** Bitmap.scalos/SCCA_Bitmap_RawType ***********************************
*
*  NAME
*   SCCA_Bitmap_RawType -- (V40) I.G (ULONG)
*
*  FUNCTION
*   This attribute is the pixel format of the raw data (SCCA_Bitmap_RawData).
*   It is one of PIXFMT_CHUNKY_CLUT and PIXFMT_0RGB_32 for raw data 8bit
*   chunky with/without palette or raw data 24bit chunky.
*
*  SEE ALSO
*   SCCA_Bitmap_RawData
*
*****************************************************************************
*/                                                              
// /
/****** Bitmap.scalos/SCCA_Bitmap_Palette ***********************************
*
*  NAME
*   SCCA_Bitmap_Palette -- (V40) I.. (APTR)
*
*  FUNCTION
*   This attribute is a pointer to the palette data for the object.
*   If you specify this attribute you have to specify
*   SCCA_Bitmap_PaletteFormat and SCCA_Bitmap_NumColors too.
*   Specifying a zero has the same effect as if you don`t specify the tag
*   at all.
*   
*  SEE ALSO
*   SCCA_Bitmap_PaletteFormat, SCCA_Bitmap_NumColors
*
*****************************************************************************
*/                                                              
// /
/****** Bitmap.scalos/SCCA_Bitmap_NumColors *********************************
*
*  NAME
*   SCCA_Bitmap_NumColors -- (V40) I.. (ULONG)
*
*  FUNCTION
*   This attribute is the number of colors of SCCA_Bitmap_Palette.
*
*   Specifying a zero has the same effect as if you don`t specify the tag
*   at all.
*
*  SEE ALSO
*   SCCA_Bitmap_Palette, SCCA_Bitmap_PaletteFormat
*
*****************************************************************************
*/                                                              
// /
/****** Bitmap.scalos/SCCA_Bitmap_PaletteFormat *****************************
*
*  NAME
*   SCCA_Bitmap_PaletteFormat -- (V40) I.. (ULONG)
*
*  FUNCTION
*   This attribute is PALFMT_RGB8 for ULONGS of this kind 0x00rrggbb
*   and               PALFMT_RGB32 for ULONGS of this kind ULONG red,
*                                  ULONG green, ULONG blue
*
*   Refer to <render/render.h> of render.library for the #defines
*   
*   Specifying a zero has the same effect as if you don`t specify the tag
*   at all.
*
*  SEE ALSO
*   SCCA_Bitmap_Palette, SCCA_Bitmap_NumColors
*
*****************************************************************************
*/                                                              
// /
/****** Bitmap.scalos/SCCA_Bitmap_BltMask ***********************************
*
*  NAME
*   SCCA_Bitmap_BltMask -- (V40) I.. (PLANEPTR)
*
*  FUNCTION
*   This attribute is the blit mask for the graphics data.
*   If the blit mask is not in chip ram it will be copied to chip ram.
*   If you set this attribute the graphics data is not scaleable.
*   The modulos will be corrected to match the output graphics data format.
*   The size of the blit mask must match the size of the graphics data.
*
*****************************************************************************
*/                                                              
// /
/****** Bitmap.scalos/SCCA_Bitmap_CopyOriginal ******************************
*
*  NAME
*   SCCA_Bitmap_CopyOriginal -- (V40) I.. (BOOL)
*
*  FUNCTION
*   If this attribute is set to TRUE, the bitmap object will make a copy of
*   the given data. This means that RawData or Bitmap will be copied in the
*   init routine of the object.
*
*****************************************************************************
*/
// /

/** Bitmap Init
 * our object exists before we are called, so our instance data pointer is valid
 */
static ULONG Bitmap_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct BitmapInst *inst)
{
        DEBUG("Got Init\n");

        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {

                // we need to get these attributes so that the user won`t forget to set them for each bitmap object
                // the user can only set them for the object initialization method, so once per object
                
                // one of them must be set...

                inst->copyorig = GetTagData(SCCA_Bitmap_CopyOriginal,0,msg->ops_AttrList);

                if (inst->SrcBitmap = (struct BitMap *) GetTagData(SCCA_Bitmap_Bitmap,0,msg->ops_AttrList))
                {
                        struct BitMap *bm;

                        if (inst->copyorig)
                        {
                                if (bm = AllocBitMap(inst->SrcBitmap->BytesPerRow * 8,inst->SrcBitmap->Rows,inst->SrcBitmap->Depth,0,NULL))
                                {
                                        BltBitMap(inst->SrcBitmap,0,0,bm,0,0,inst->SrcBitmap->BytesPerRow * 8,inst->SrcBitmap->Rows,ABNC | ABC, -1, NULL);
                                        inst->SrcBitmap = bm;
                                        WaitBlit();
                                }
                                else
                                {
                                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Exit);
                                        return FALSE;
                                }
                        }
                }

                inst->RawData = (APTR) GetTagData(SCCA_Bitmap_RawData,0,msg->ops_AttrList);
                
                if(!((inst->SrcBitmap) || inst->RawData))       // ... and only one of them may be set
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Exit);
                        return FALSE;
                }
                        
                if((inst->SrcBitmap) && (inst->RawData))
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Exit);
                        return FALSE;
                }

                if((inst->RawData) &&
                        (!(inst->RawType = GetTagData(SCCA_Bitmap_RawType,0,msg->ops_AttrList))))
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Exit);
                        return FALSE;
                }

                if(!(FindTagItem(SCCA_Graphic_Height,msg->ops_AttrList)))
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Exit);
                        return FALSE;
                }

                if(!(FindTagItem(SCCA_Graphic_Width,msg->ops_AttrList)))
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Exit);
                        return FALSE;
                }

                inst->DestHeight = ((struct SC_GraphicObject *) obj)->bounds.Height; // default scale value is graphics data value
                inst->DestWidth = ((struct SC_GraphicObject *) obj)->bounds.Width; // default scale value is graphics data value

                // if a special palette for the gfx data is passed we need to get the other tags that belong to it too

                if(inst->Palette                = (APTR)       GetTagData(SCCA_Bitmap_Palette,0,msg->ops_AttrList))
                {
                        if(!(inst->NumColors            = GetTagData(SCCA_Bitmap_NumColors,0,msg->ops_AttrList)))
                        {
                                SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Exit);
                                return FALSE;
                        }

                        if(!(inst->PaletteFormat        =  GetTagData(SCCA_Bitmap_PaletteFormat,0,msg->ops_AttrList)))
                        {
                                SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Exit);
                                return FALSE;
                        }
                        
                }
                
                inst->BltMask                   = (PLANEPTR)    GetTagData(SCCA_Bitmap_BltMask,0,msg->ops_AttrList);

                        
                // hier frame attrib checken, setzbar vor prethinkwindow
        }
        return(TRUE);
}
// /

/** Bitmap_PreThinkScreen
 * Make all actions that can be done when having the screen data without the window data
 *
 */

static ULONG Bitmap_PreThinkScreen(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_PreThinkScreen *msg, struct BitmapInst *inst)
{
        DEBUG("Got PreThinkScreen\n");

        if (!(SC_DoSuperMethodA(cl,obj,(Msg) msg)))
                return(FALSE);

        inst->Screen = (struct Screen *) get(msg->Screenobj,SCCA_Screen_Screen);
        
        if(inst->SrcBitmap && inst->Palette)
        {

                /*
                 * bitmap struct plus palette, lower or equal to 8bit deep planar bitmap
                 */

                if(!(inst->Picture = MakePicture(inst->SrcBitmap,                               // source
                                                                                 ((struct SC_GraphicObject *)obj)->bounds.Width, // source width [pixels]
                                                                                 ((struct SC_GraphicObject *)obj)->bounds.Height,// source height [rows]
                                                                                 GGFX_PixelFormat, PIXFMT_BITMAP_CLUT,          // source type
                                                                                 GGFX_Palette, inst->Palette,                   // pointer to a color table
                                                                                 GGFX_NumColors, inst->NumColors,               // number of colors in the color table
                                                                                 GGFX_PaletteFormat, inst->PaletteFormat,       // palette type
                                                                                 TAG_DONE)))
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkScreen);
                        
                        DEBUG("PreThinkScreen failed\n");

                        return FALSE;
                }

        }
                                
        /*
         * chunky raw data 8bit or 24bit
         */

        if((inst->RawData) && ((inst->RawType == PIXFMT_CHUNKY_CLUT) || (inst->RawType == PIXFMT_0RGB_32))) // chunky 8 or 32 bit with/without palette
        {
                /*
                 * chunky raw data 8/24bit plus palette
                 */

                if(inst->Palette)
                {
                        if(!(inst->Picture = MakePicture(inst->RawData,
                                                                                         ((struct SC_GraphicObject *)obj)->bounds.Width,
                                                                                         ((struct SC_GraphicObject *)obj)->bounds.Height,
                                                                                         GGFX_PixelFormat, inst->RawType,
                                                                                         GGFX_Palette, inst->Palette,
                                                                                         GGFX_NumColors, inst->NumColors,
                                                                                         GGFX_PaletteFormat, inst->PaletteFormat,
                                                                                         GGFX_Independent, TRUE,
                                                                                         TAG_DONE)))
                        {
                                SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkScreen);
                                
                                DEBUG("PreThinkScreen failed\n");
                                
                                return FALSE;
                        }
                }


                /*
                 * chunky raw data 8/24bit without palette
                 */

                if(!(inst->Palette))
                {
                        if(!(inst->Picture = MakePicture(inst->RawData,
                                                                                         ((struct SC_GraphicObject *)obj)->bounds.Width, // source width [pixels]
                                                                                         ((struct SC_GraphicObject *)obj)->bounds.Height, // source height [rows]
                                                                                         GGFX_PixelFormat, inst->RawType,
                                                                                         GGFX_Independent, TRUE,                        // copy pixel array to a seperate buffer
                                                                                         TAG_DONE)))
                        {
                                SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkScreen);
                                
                                DEBUG("PreThinkScreen failed\n");
                                
                                return FALSE;
                        }
                }
        }
                                                 

        if( !(inst->SrcBitmap && !(inst->Palette)) ) // if guigfx case
        {

                if(!(inst->Psm = CreatePenShareMap(TAG_DONE)))
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkScreen);
                        
                        DEBUG("PreThinkScreen failed\n");
                        
                        return FALSE;
                }

                if(!(AddPicture(inst->Psm,
                                                inst->Picture,
                                                TAG_DONE)))
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkScreen);
                        
                        DEBUG("PreThinkScreen failed\n");
                        
                        return FALSE;
                }
        }
                                

// sinn: modulo werte von friendbitmap und der maskbitmap gleich zu haben und die maskbitmap planar zu haben, 1 bit tief

// wenn maske angegeben dann keine frames

// wenn masken attribut gesetzt dann transparent blitten

// breite/8*hoehe( format der maske) (bytes pro bitmap), ausmasse vom attribut, bltbitmap
// planare bitmap 1 bit tief anlegen mit flags 0(-> maskenbitmap)
// breite, hoehe und flags holen aus friendbitmap -> AloocBitMap(), wenn interleaved nicht gesetzt in friend bitmap, dann bitmap mit breite und hoehe( von friendbitmap mit getattr()) , tiefe 1, flags 0, like 0
// bitmap structure anlegen für übergebene maske, per hand, rows ist hoehe, depth 1, flags 0, breite auf /16 teilbar runden und durch 8 dividieren
// wenn interleaved gesetzt, dann breite*tiefe der friendbitmap als breite für maskenbitmap, tiefe 1, flags 0, like 0
// rein blitten der planaren plane in angelegte maskbitmap.
// wenn maske nicht im chipram dann nach chipram copy
// BltMaskBitMapRastPort ERsten bitplane ptr der maskenbitmap als maske nehmen, typeofmem()

// atrib ist ptr auf plane mit evtl. falschen modulos

        DEBUG("PreThinkScreen succeeded\n");

        return TRUE;
        
}
// /

/** Bitmap_PreThinkWindow
 *
 * Do what can be done when having the window data and the screen data
 * the given Window RastPort and Screen should always belong together
 * so the Window should only appear on that Screen
 *
 * This method does only do the time consuming actions if it needs to.
 */

static ULONG Bitmap_PreThinkWindow(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_PreThinkWindow *msg, struct BitmapInst *inst)
{
        ULONG FriendBitmapWidth, FriendBitmapHeight;
        
        UBYTE FriendBitmapDepth;
        
        ULONG FriendBitmapWidthRounded;
        
        DEBUG("Got PreThinkWindow\n");
        
        if (!(SC_DoSuperMethodA(cl,obj,(Msg) msg)))
                return(FALSE);

        // use new scale values as set by OM_SET at SCCM_Graphic_PreThinkWindow method entry only

        if(!(inst->BltMask)) // blit mask XOR scaleable graphics data not both
        {
                inst->DestHeight = ((struct SC_GraphicObject *) obj)->bounds.Height; // default scale value is graphics data value
                inst->DestWidth = ((struct SC_GraphicObject *) obj)->bounds.Width; // default scale value is graphics data value
        }

        if(inst->SrcBitmap && !(inst->Palette))
        {
                /*
                 * bitmap struct without palette, lower or equal to 8bit deep planar bitmap
                 *
                 */

                if(msg->action == SCCV_Area_Action_OpenClose) // window has been opened
                {
                        // copy the source bitmap for this object to our friendbitmap which is the input area for the draw method
                        // use screen palette, we don`t have a special palette for our gfx data

                        // this is the temporary bitmap that is used to hold the picture and optionally some border graphics around it
                        // we draw its contents in the draw method so the friendbitmap is as big as the blit in the draw method

                        // we make an empty optimized bitmap ...

                        if(!(inst->FriendBitmap = AllocBitMap(((struct SC_GraphicObject *)obj)->bounds.Width,
                                                                                                  ((struct SC_GraphicObject *)obj)->bounds.Height,
                                                                                                  GetBitMapAttr(inst->Screen->RastPort.BitMap,BMA_DEPTH),
                                                                                                  BMF_DISPLAYABLE|BMF_INTERLEAVED|BMF_MINPLANES,    // BMF_MINPLANES for cgx, so that FriendBitmap may also be chunky
                                                                                                  inst->Screen->RastPort.BitMap)))                  // btw. looking at the notation you see: Screen includes a complete RastPort struct
                                                                                                                                                                                                        // if not, it won`t be possible to access BitMap (whatever it is, a ptr or not)
                                                                                                                                                                                                        // without following a pointer
                                                                                                                                                                                                        // and with a dot notation we actually don`t follow a ptr (e.g. RastPort.BitMap)
                        {
                                SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkWindow);
                                
                                DEBUG("PreThinkWindow failed\n");
                                
                                return FALSE;
                        }

                        InitRastPort(&(inst->FriendRastport));
                        
                        inst->FriendRastport.BitMap = inst->FriendBitmap;
                
                        // ... and blit the source data to it. the resulting bitmap is blitable to the window rastport with maximum performance

                        BltBitMap(inst->SrcBitmap,      // source
                                          0,                    // source left edge
                                          0,                    // source top edge
                                          inst->FriendBitmap,   // destination
                                          0,                    // destination left edge
                                          0,                    // destination top edge
                                          ((struct SC_GraphicObject *)obj)->bounds.Width, // rectangle to blit, width
                                          ((struct SC_GraphicObject *)obj)->bounds.Height, // rectangle to blit, height
                                          ABNC|ABC,             // minterm
                                          ~0,                   // mask
                                          NULL);                // memory
                                  
                }

                DEBUG("PreThinkWindow succeeded\n");
                
                return TRUE;
        }

        /*
         * chunky raw data 8bit or 24bit or bitmap plus palette
         * guigfx
         */

        /*
         * window has been opened
         * guigfx
         */
        
        else if(msg->action == SCCV_Area_Action_OpenClose)
        {
                // ObtainDrawHandle() needs to be done only once per window RastPort

                if(!(inst->Drawhandle = ObtainDrawHandle(inst->Psm,
                                                                                                 ((struct SC_GraphicObject *)obj)->rastport, // optimize for window rastport
                                                                                                 inst->Screen->ViewPort.ColorMap,       // to allocate pens
                                                                                                 TAG_DONE)))                            // more tags available
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkWindow);
                        
                        DEBUG("PreThinkWindow failed\n");
                        
                        return FALSE;
                }

                if(inst->FriendBitmap = CreatePictureBitMap(inst->Drawhandle,
                                                                                                          inst->Picture,
                                                                                                          GGFX_SourceX, 0,                  // left edge inside the picture where to fetch the pixels
                                                                                                          GGFX_SourceY, 0,                  // top edge inside the picture where to fetch the pixels
                                                                                                          GGFX_DestWidth, inst->DestWidth,  // scale to this width
                                                                                                          GGFX_DestHeight, inst->DestHeight,// scale to this height
                                                                                                          TAG_DONE))                        // more tags available !!
                {
                        // if we got a blit mask and scaling width/height values do:
                        // ignore scale stuff

                        // oldDestWidth, oldDestHeight are set once and will never change so no scaling
                
                        inst->oldDestWidth = inst->DestWidth;
                        inst->oldDestHeight = inst->DestHeight;
                }
                else
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkWindow);
                        
                        DEBUG("PreThinkWindow failed\n");
                        
                        return FALSE;
                }
        }
        
        /*
         * window has been resized
         * guigfx
         */

        else if(msg->action == SCCV_Area_Action_Resize)
        {
                // check for need to scale the graphics data to new values
                // we just need to scale if the scaling dimensions have been changed

                if( (inst->DestWidth != inst->oldDestWidth) ||
                        (inst->DestHeight != inst->oldDestHeight) )
                {
                        // free the old bitmap that was left behind
                        // allocated by the last invocation of this method
                        // (with SCCV_Area_Action_OpenClose or SCCV_Area_Action_Resize as action)

                        if(inst->FriendBitmap)
                        {
                                FreeBitMap(inst->FriendBitmap);
                                inst->FriendBitmap = 0;

                                // we update these values so that we don`t scale again for the same scale values

                                inst->oldDestWidth = inst->DestWidth;
                                inst->oldDestHeight = inst->DestHeight;
                        }
                
                        // create new bitmap, blit to friend bitmap to pos (0,0)
                        // scale picture dimensions, optionally use a clip area inside the picture

                        if(!(inst->FriendBitmap = CreatePictureBitMap(inst->Drawhandle,
                                                                                                                  inst->Picture,
                                                                                                                  GGFX_SourceX, 0,                  // left edge inside the picture where to fetch the pixels
                                                                                                                  GGFX_SourceY, 0,                  // top edge inside the picture where to fetch the pixels
                                                                                                                  GGFX_DestWidth, inst->DestWidth,  // scale to this width
                                                                                                                  GGFX_DestHeight, inst->DestHeight,// scale to this height
                                                                                                                  TAG_DONE)))                       // more tags available !!
                        {
                                SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkWindow);
                                
                                DEBUG("PreThinkWindow failed\n");
                                
                                return FALSE;
                        }
                }
        }
        
        /*
         * create the mask bitmap
         * we don`t scale graphics that has a blit mask associated to it
         * a mask bitmap can be used with any supported graphics format
         */

/*
        if(
                ( (inst->BltMask) && (msg->action == SCCV_Area_Action_OpenClose) ) ||
                ( (inst->BltMask) && (msg->action == SCCV_Area_Action_Resize) &&
                  (
                        (inst->DestWidth != inst->oldDestWidth) ||
                        (inst->DestHeight != inst->oldDestHeight)
                  )
                )
          )
*/
        if( (inst->BltMask) && (msg->action == SCCV_Area_Action_OpenClose) ) // if window has been opened and a blit mask exists
        {
                APTR BltMaskOk = 0;
        
                DEBUG("Mask attribute\n");

                FriendBitmapWidth = GetBitMapAttr(inst->FriendBitmap, BMA_WIDTH); // already rounded ?
                FriendBitmapHeight = GetBitMapAttr(inst->FriendBitmap, BMA_HEIGHT);
                FriendBitmapDepth = (UBYTE) GetBitMapAttr(inst->FriendBitmap, BMA_DEPTH);
        
                FriendBitmapWidthRounded = ((FriendBitmapWidth+15)&0xFFFFFFF0); // divideable by 16 
        
                if(GetBitMapAttr(inst->FriendBitmap, BMA_FLAGS) & BMF_INTERLEAVED)
                {
                        if(!(inst->MaskBitmap = AllocBitMap(FriendBitmapWidthRounded*FriendBitmapDepth, // width
                                                                                                FriendBitmapHeight,                         // height
                                                                                                1,                                          // depth
                                                                                                0,                                          // flags
                                                                                                NULL                                        // like_bitmap
                                                                                           )))
                        {
                                SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkWindow);
                                
                                DEBUG("PreThinkWindow failed\n");
                                
                                return FALSE;
                        }
                }
                else // non interleaved
                {
                        if(!(inst->MaskBitmap = AllocBitMap(FriendBitmapWidthRounded,   // width
                                                                                                FriendBitmapHeight,         // height
                                                                                                1,                          // depth
                                                                                                0,                          // flags
                                                                                                NULL                        // like_bitmap
                                                                                           )))
                        {
                                SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkWindow);
                                
                                DEBUG("PreThinkWindow failed\n");
                                
                                return FALSE;
                        }
                }

                inst->MaskBitmapNotOk.BytesPerRow = FriendBitmapWidthRounded/8;
                inst->MaskBitmapNotOk.Rows = FriendBitmapHeight;
                inst->MaskBitmapNotOk.Flags = 0;
                inst->MaskBitmapNotOk.Depth = 1;
                inst->MaskBitmapNotOk.pad = 0;
                inst->MaskBitmapNotOk.Planes[1] = 0;
                inst->MaskBitmapNotOk.Planes[2] = 0;
                inst->MaskBitmapNotOk.Planes[3] = 0;
                inst->MaskBitmapNotOk.Planes[4] = 0;
                inst->MaskBitmapNotOk.Planes[5] = 0;
                inst->MaskBitmapNotOk.Planes[6] = 0;
                inst->MaskBitmapNotOk.Planes[7] = 0;
        
                if( !( TypeOfMem(inst->BltMask) & MEMF_CHIP ) ) // if SCCA_Bitmap_BltMask data is not in ChipRam
                {
                        // BltMaskOk is longword aligned

                        if(!(BltMaskOk = (APTR) AllocVec((FriendBitmapWidthRounded/8*FriendBitmapHeight+31) & 0xFFFFFFFC, MEMF_CHIP)))
                        {
                                SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Graphic_PostThinkWindow);
                                
                                DEBUG("PreThinkWindow failed\n");
                                
                                return FALSE;
                        }
                
                        if(((ULONG) inst->BltMask) & 0x3) // not longword aligned
                        {
                
                                CopyMem(inst->BltMask,                                  // source
                                                BltMaskOk,                                      // destination
                                                FriendBitmapWidthRounded/8*FriendBitmapHeight   // size
                                           );
                        }
                        else
                        {
                                CopyMemQuick(inst->BltMask,                                                     // source
                                                         BltMaskOk,                                                         // destination
                                                         (FriendBitmapWidthRounded/8*FriendBitmapHeight+31) & 0xFFFFFFFC    // size
                                                        );
                        }
                        inst->MaskBitmapNotOk.Planes[0] = BltMaskOk;
                }
                else
                        inst->MaskBitmapNotOk.Planes[0] = inst->BltMask;

        
                // correct the modulos of the mask bitmap for BltMaskBitMapRastPort()
        
                BltBitMap(&inst->MaskBitmapNotOk,   // srcbitmap
                                  0,                        // srcx
                                  0,                        // srcy
                                  inst->MaskBitmap,         // dstbitmap
                                  0,                        // dstx
                                  0,                        // dsty
                                  FriendBitmapWidth,        // width
                                  FriendBitmapHeight,       // height
                                  ABNC | ABC,               // minterm
                                  ~0,                       // mask
                                  0                         // mem
                                 );

                if(BltMaskOk) FreeVec(BltMaskOk);

        }

        // now we have a ready-to-use mask bitmap (inst->MaskBitmap) for BltMaskBitMapRastPort()

        DEBUG("PreThinkWindow succeeded\n");

        return TRUE;
}
// /

/** Bitmap_Draw
 *
 */

static void Bitmap_Draw(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_Draw *msg, struct BitmapInst *inst)
{
        int x = msg->posX;
        int y = msg->posY;

        DEBUG2("Draw Bitmap Orig %ld,%ld\n",x,y);

        if (msg->drawflags == SCCV_Graphic_DrawFlags_RelPos)
        {
                x += ((struct SC_GraphicObject *)obj)->bounds.Left;         // direct access allowed, because we are access our own
                y += ((struct SC_GraphicObject *)obj)->bounds.Top;
        }

        DEBUG2("Draw Bitmap %ld,%ld\n",x,y);
        DEBUG2("BitmapPos %ld,%ld\n",((struct SC_GraphicObject *)obj)->bounds.Left,((struct SC_GraphicObject *)obj)->bounds.Top);

        SC_DoSuperMethodA(cl,obj,(Msg) msg);

        // blit the perfect blitable friendbitmap (0,0) to the rastport (posX,posY)

        if(inst->BltMask)
                BltMaskBitMapRastPort(inst->FriendBitmap,                                   // bitmap
                                          0,                                                            // source x
                                          0,                                                            // source y
                                          ((struct SC_GraphicObject *)obj)->rastport,                   // destination RastPort
                                          x,                                                            // destination x
                                          y,                                                            // destination y
                                          inst->DestWidth,                                              // width of the area to blit
                                          inst->DestHeight,                                             // height of the area to blit
                                          ABNC|ABC,                                                     // minterm
                                          inst->MaskBitmap->Planes[0]);                                 // bltmask
                
        else
                BltBitMapRastPort(inst->FriendBitmap,                                       // bitmap
                                          0,                                                            // source x
                                          0,                                                            // source y
                                          ((struct SC_GraphicObject *)obj)->rastport,                   // destination RastPort
                                          x,                                                            // destination x
                                          y,                                                            // destination y
                                          inst->DestWidth,                                              // width of the area to blit
                                          inst->DestHeight,                                             // height of the area to blit
                                          ABNC|ABC);                                                    // minterm



        DEBUG("Draw finished\n");
}
// /

/****** Bitmap.scalos/SCCM_Bitmap_Fill **************************************
*
*  NAME
*   SCCM_Bitmap_Fill (V40)
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Bitmap_Fill,LONG MinX, LONG MinY, LONG MaxX, LONG MaxY, LONG OffsetX, LONG OffsetY, ULONG BlitType );
*
*  FUNCTION
*   This method fills a rectangular area with the bitmap. If the size is
*   bigger than bitmap or the offset causes it, the bitmap will be tiled.
*   Masks will be ignored.
*
*  INPUTS
*   MinX,MinY,MaxX,MaxY - the rectangle to fill
*   OffsetX, OffsetY - bitmap offset, can be bigger that the width of the
*                      bitmap
*   BlitType - SCCV_Bitmap_Fill_BlitType_Clipped - this should be used if
*                                                  you like to fill area
*                                                  within windows.
*              SCCV_Bitmap_Fill_BlitType_IgnoreClipping - should only be used
*                                                         for LayerHooks.
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*
* Blit sequence :
*   +---+-------+---------+
*   | 1 |   2   |         |
*   +---+-------+         |
*   |   |       |         |
*   | 3 |   4   |         |
*   |   |       |         |
*   +---+-------+         |
*   |                     |
*   |                     |
*   |                     |
*   +---------------------+
*
* After this first step the bitmap on screen will be double in X direction and
* the in Y direction until the area is completly filled.
*
*/                                                              


static void Bitmap_Fill(struct SC_Class *cl, Object *obj, struct SCCP_Bitmap_Fill *msg, struct BitmapInst *inst)
{
        LONG    xoffset = msg->OffsetX % inst->DestWidth;
        LONG    yoffset = msg->OffsetY % inst->DestHeight;                              // real inner offset
        ULONG   rectwidth = msg->MaxX - msg->MinX + 1;
        ULONG   rectheight = msg->MaxY - msg->MinY + 1;                                 // rectangle size
        ULONG   blitwidth, blitheight;
        ULONG   blitwidth2 = 0;
        ULONG   blitheight2 = 0;
        LONG    destoffset;

// set width and height for the rectangle

        DEBUG("Got Fill\n");

        if ((blitwidth = inst->DestWidth - xoffset) > rectwidth)
                blitwidth = rectwidth;
        if ((blitheight = inst->DestHeight - yoffset) > rectheight)
                blitheight = rectheight;

        if (msg->BlitType == SCCV_Bitmap_Fill_BlitType_Clipped)
        {
                LONG destoffsetx = msg->MinX;
                LONG destoffsety = msg->MinY;
                LONG xoffset2;

                blitheight2 = blitheight;

                while (destoffsety <= msg->MaxY)
                {
                        destoffset = destoffsetx;
                        blitwidth2 = blitwidth;
                        xoffset2 = xoffset;

                        while (destoffset <= msg->MaxX)
                        {
                                BltBitMapRastPort(inst->FriendBitmap,                                   // bitmap
                                                                  xoffset2,                                             // source x
                                                                  yoffset,                                              // source y
                                                                  ((struct SC_GraphicObject *)obj)->rastport,              // destination RastPort
                                                                  destoffset,                                           // destination x
                                                                  destoffsety,                                          // destination y
                                                                  blitwidth2,                                           // width of the area to blit
                                                                  blitheight2,                                          // height of the area to blit
                                                                  ABNC|ABC);                                            // minterm

                                xoffset2 = 0;
                                destoffset += blitwidth2;
                                if (((blitwidth2 = inst->DestWidth) + destoffset) > msg->MaxX)
                                        blitwidth2 = msg->MaxX - destoffset + 1;
                        }

                        yoffset = 0;
                        destoffsety += blitheight2;
                        if (((blitheight2 = inst->DestHeight) + destoffsety) > msg->MaxY)
                                blitheight2 = msg->MaxY - destoffsety + 1;
                }
        }
        else
        {

// 1. the first rectangle to blit is the maximum width and height depending on
//    the offset
                BltBitMapRastPort(inst->FriendBitmap,                                           // bitmap
                                                  xoffset,                                                      // source x
                                                  yoffset,                                                      // source y
                                                  ((struct SC_GraphicObject *)obj)->rastport,                      // destination RastPort
                                                  msg->MinX,                                                    // destination x
                                                  msg->MinY,                                                    // destination y
                                                  blitwidth,                                                    // width of the area to blit
                                                  blitheight,                                                   // height of the area to blit
                                                  ABNC|ABC);                                                    // minterm

// 2. blit one rectangle right of the first one if possible
                if ((blitwidth != inst->DestWidth) && (blitwidth < rectwidth))
                {
                        if ((blitwidth2 = rectwidth - blitwidth) > inst->DestWidth)
                                blitwidth2 = inst->DestWidth;

                        BltBitMapRastPort(inst->FriendBitmap,
                                                          0,                                                        // source x
                                                          yoffset,                                                  // source y
                                                          ((struct SC_GraphicObject *)obj)->rastport,               // destination RastPort
                                                          msg->MinX + blitwidth,                                    // destination x
                                                          msg->MinY,                                                // destination y
                                                          blitwidth2,                                               // width of the area to blit
                                                          blitheight,                                               // height of the area to blit
                                                          ABNC|ABC);                                                // minterm
                }

// 3. blit one rectangle below the first one if possible
                if ((blitheight != inst->DestHeight) && (blitheight < rectheight))
                {
                        if ((blitheight2 = rectheight - blitheight) > inst->DestHeight)
                                blitheight2 = inst->DestHeight;

                        BltBitMapRastPort(inst->FriendBitmap,
                                                          xoffset,                                                  // source x
                                                          0,                                                        // source y
                                                          ((struct SC_GraphicObject *)obj)->rastport,               // destination RastPort
                                                          msg->MinX,                                                // destination x
                                                          msg->MinY + blitheight,                                   // destination y
                                                          blitwidth,                                                // width of the area to blit
                                                          blitheight2,                                              // height of the area to blit
                                                          ABNC|ABC);                                                // minterm


// 4. blit one rectangle right below to complete a big rectangle which will be
//    doubled in X and Y direction
                        if ((blitwidth != inst->DestWidth) && (blitwidth < rectwidth))
                        {

                                BltBitMapRastPort(inst->FriendBitmap,
                                                                  0,                                                    // source x
                                                                  0,                                                    // source y
                                                                  ((struct SC_GraphicObject *)obj)->rastport,           // destination RastPort
                                                                  msg->MinX + blitwidth,                                // destination x
                                                                  msg->MinY + blitheight,                               // destination y
                                                                  blitwidth2,                                           // width of the area to blit
                                                                  blitheight2,                                          // height of the area to blit
                                                                  ABNC|ABC);                                            // minterm
                        }
                }


                xoffset = msg->MinX;
                if (blitwidth != inst->DestWidth)
                        xoffset += blitwidth;

                yoffset = msg->MinY;
                if (blitheight != inst->DestHeight)
                        yoffset += blitheight;

                destoffset = xoffset;
                if (!(blitwidth2))
                        blitwidth2 = inst->DestWidth;

                while ((destoffset += blitwidth2) <= msg->MaxX)
                {
                        if (((blitwidth2 *= 2) + destoffset) > msg->MaxX)
                                blitwidth2 = msg->MaxX - destoffset + 1;

                        BltBitMapRastPort(((struct SC_GraphicObject *)obj)->rastport->BitMap,
                                                          xoffset,                                              // source x
                                                          msg->MinY,                                            // source y
                                                          ((struct SC_GraphicObject *)obj)->rastport,           // destination RastPort
                                                          destoffset,                                           // destination x
                                                          msg->MinY,                                            // destination y
                                                          blitwidth2,                                           // width of the area to blit
                                                          blitheight + blitheight2,                             // height of the area to blit
                                                          ABNC|ABC);                                            // minterm
                }

                destoffset = yoffset;
                if (!(blitheight2))
                        blitheight2 = inst->DestHeight;

                while ((destoffset += blitheight2) <= msg->MaxY)
                {
                        if (((blitheight2<<1) + destoffset) > msg->MaxY)
                                blitheight2 = msg->MaxY - destoffset + 1;

                        BltBitMapRastPort(((struct SC_GraphicObject *)obj)->rastport->BitMap,
                                                          msg->MinX,                                            // source x
                                                          yoffset,                                              // source y
                                                          ((struct SC_GraphicObject *)obj)->rastport, // destination RastPort
                                                          msg->MinX,                                            // destination x
                                                          destoffset,                                           // destination y
                                                          rectwidth,                                            // width of the area to blit
                                                          blitheight2,                                          // height of the area to blit
                                                          ABNC|ABC);                                            // minterm
                }

        }
}
// /

/** Bitmap_PostThinkWindow
 * Dealloc Bitmap_PreThinkWindow allocations
 */
static void Bitmap_PostThinkWindow(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_PostThinkWindow *msg, struct BitmapInst *inst)
{
        DEBUG("Got PostThinkWindow\n");
        
        SC_DoSuperMethodA(cl,obj,(Msg) msg);

        // this is a new method, not a derived method, so there is no need to call DoSuperMethodA()

        if( (inst->BltMask) && (msg->action == SCCV_Area_Action_OpenClose) ) // if window will be closed and a blit mask exists
        {
                if(inst->MaskBitmap)
                {
                        FreeBitMap(inst->MaskBitmap);
                        inst->MaskBitmap = 0;
                }
        }

        /*
         * bitmap struct without palette, lower or equal to 8bit deep planar bitmap
         */

        if(inst->SrcBitmap && !(inst->Palette))
        {
                if(msg->action == SCCV_Area_Action_OpenClose) // window will be closed
                {
                        if(inst->FriendBitmap)
                        {
                                FreeBitMap(inst->FriendBitmap);
                                inst->FriendBitmap = 0;
                        }
                }

                DEBUG("PostThinkWindow finished\n");

                return;
        }

        else // guigfx
        {

                /*
                 * chunky raw data 8bit or 24bit or bitmap plus palette
                 * guigfx
                 */

                if(msg->action == SCCV_Area_Action_OpenClose) // if window will be closed
                {
                        /*
                         * there will always be one bitmap left allocated
                         * by PreThinkWindow with SCCV_Area_Action_OpenClose or SCCV_Area_Action_Resize
                         * if any was successfull.
                         * this one will be removed here
                         */

                        if(inst->FriendBitmap)
                        {
                                FreeBitMap(inst->FriendBitmap);
                                inst->FriendBitmap = 0;
                        }

                        if(inst->Drawhandle)
                        {
                                ReleaseDrawHandle(inst->Drawhandle);
                        }

                        DEBUG("PostThinkWindow finished\n");
                        
                        return;
                }

                // if window was resized we can do nothing because we
                // can`t predict if the new size will be different to the current
                // size so we can`t dealloc the bitmap because we don`t want
                // to alloc a new bitmap in PreThinkWindow *always*
        }
}
// /

/** Bitmap_PostThinkScreen
 * Dealloc Bitmap_PreThinkScreen allocations
 */

static void Bitmap_PostThinkScreen(struct SC_Class *cl, Object *obj, Msg *msg, struct BitmapInst *inst)
{
        DEBUG("Got PostThinkScreen\n");
        
        SC_DoSuperMethodA(cl,obj,(Msg) msg);

        // this is a new method, not a derived method, so there is no need to call DoSuperMethodA()

        if(inst->Psm)
        {
                DeletePenShareMap(inst->Psm);
        }
        
        if(inst->Picture)
        {
                DeletePicture(inst->Picture);
        }
}   
// /

/** Get one attribute of all of the getable attributes
 */

static ULONG Bitmap_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct BitmapInst *inst )
{
        
        DEBUG("Got Get\n");
 
        /*
         * in order to make bobs of the graphics data ...
         * ... you need to copy it
         */

        if (msg->opg_AttrID == SCCA_Bitmap_Bitmap)
        {
                *(msg->opg_Storage) = (ULONG) inst->SrcBitmap;
                
                DEBUG("Get succeeded\n");
                
                return( TRUE );
        }

        if (msg->opg_AttrID == SCCA_Bitmap_RawData)
        {
                *(msg->opg_Storage) = (ULONG) inst->RawData;
                
                DEBUG("Get succeeded\n");
                
                return( TRUE );
        }

        if (msg->opg_AttrID == SCCA_Bitmap_RawType)
        {
                *(msg->opg_Storage) = (ULONG) inst->RawType;
                
                DEBUG("Get succeeded\n");
                
                return( TRUE );
        }

        DEBUG("Get failed\n");

        return( SC_DoSuperMethodA(cl, obj, (Msg) msg) );
}
// /

/** Bitmap Exit
 */
static ULONG Bitmap_Exit(struct SC_Class *cl, Object *obj, struct SCCP_Exit *msg, struct BitmapInst *inst)
{
        if (inst->copyorig)
        {
           if (inst->SrcBitmap)
           {
                   FreeBitMap(inst->SrcBitmap);
           }
        }

        return(SC_DoSuperMethodA(cl, obj, (Msg) msg));
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData BitmapMethods[] =
{
        { OM_GET,(ULONG)                        Bitmap_Get, 0, 0, NULL },               // derived from RootClass
        { SCCM_Init,(ULONG)                     Bitmap_Init, 0, 0, NULL },              // derived from RootClass
        { SCCM_Graphic_PreThinkScreen,(ULONG)   Bitmap_PreThinkScreen, 0, 0, NULL },    // derived from GraphicClass
        { SCCM_Graphic_PreThinkWindow,(ULONG)   Bitmap_PreThinkWindow, 0, 0, NULL },    // derived from GraphicClass
        { SCCM_Graphic_Draw,(ULONG)             Bitmap_Draw, 0, 0, NULL },              // derived from GraphicClass
        { SCCM_Bitmap_Fill,(ULONG)              Bitmap_Fill, sizeof(struct SCCP_Bitmap_Fill), SCMDF_DIRECTMETHOD, NULL },               // new method
        { SCCM_Graphic_PostThinkWindow,(ULONG)  Bitmap_PostThinkWindow, 0, 0, NULL },   // derived from GraphicClass
        { SCCM_Graphic_PostThinkScreen,(ULONG)  Bitmap_PostThinkScreen, 0, 0, NULL },   // derived from GraphicClass
        { SCCM_Exit,(ULONG)                     Bitmap_Exit, 0, 0, NULL }, // derived from RootClass
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

