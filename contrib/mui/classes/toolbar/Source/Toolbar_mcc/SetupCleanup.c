/***************************************************************************

 Toolbar MCC - MUI Custom Class for Toolbar handling

 Copyright (C) 1997-2004 by Benny Kjær Nielsen <floyd@amiga.dk>
                            Darius Brewka <d.brewka@freenet.de>
                            Jens Langner <Jens.Langner@light-speed.de>

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 $Id$

***************************************************************************/

// ANSI C
#include <string.h> // strlen/strcpy

// Protos
#include <clib/alib_protos.h>
#include <proto/datatypes.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>

#ifdef USE_IMAGEPOOL
#include <proto/imagepool.h>
#include <libraries/imagepool.h>
#endif

#include <libraries/mui.h>

#define TB_OBSOLETE

// Other
#include "Toolbar_mcc.h"
#include "InstanceData.h"
#include "ConfigValues.h"
#include "Draw.h" // TB_

#ifdef USE_IMAGEPOOL
extern struct Library *ImagePoolBase;
#endif

#if 0
VOID DumpKeyValues(struct Toolbar_Data *data)
{
  KPrintF("--------------------\n");
  KPrintF("Key Values\n");
  KPrintF("--------------------\n");
  KPrintF("Group Space: %ld\n", data->GroupSpace);
  KPrintF("Tool Space: %ld\n", data->ToolSpace);
  KPrintF("InnerSpace: %ld\n", data->InnerSpace);
  KPrintF("Group Spaces: %ld\n", data->GroupSpaces);
  KPrintF("Tool Spaces: %ld\n", data->ToolSpaces);
  KPrintF("Buttons: %ld\n", data->Buttons);
  KPrintF("Image Width: %ld\n", data->ImageWidth);
  KPrintF("Image Height: %ld\n", data->ImageHeight);
  KPrintF("Icon Width: %ld\n", data->IconWidth);
  KPrintF("Icon Height: %ld\n", data->IconHeight);
  KPrintF("Button Width: %ld\n", data->ButtonWidth);
  KPrintF("Button Height: %ld\n", data->ButtonHeight);
  KPrintF("--------------------\n");
}
#endif

ULONG GetConfigItem(Object *obj, ULONG configitem, ULONG defaultsetting)
{
  ULONG value;
  return( DoMethod(obj, MUIM_GetConfigItem, configitem, &value) ? *(ULONG *)value : defaultsetting);
}

ULONG MakeDark(ULONG color)
{
  return((color > 0x1a1a1a1a) ? color -= 0x1a1a1a1a : 0x000000);
}

ULONG MakeBright(ULONG color)
{
  return((color < 0xffffffff-0x1a1a1a1a) ? color += 0x1a1a1a1a : 0xffffffff);
}

LONG AllocShineShadowPen(struct ColorMap *cm, ULONG *color, BOOL darker)
{
  ULONG r, g, b;

  r = darker ? MakeDark(color[0]) : MakeBright(color[0]);
  g = darker ? MakeDark(color[1]) : MakeBright(color[1]);
  b = darker ? MakeDark(color[2]) : MakeBright(color[2]);

  return(ObtainBestPenA(cm, r, g, b, NULL));
}

BOOL ReportSizeError(char *cause, Object *app)
{
  return((BOOL)!MUI_Request (app, NULL, 0, NULL, "_Continue|_Fail",
        "The width and/or height of the %s image\n"
        "does not correspond to the normal image.\n"
        "If you continue, the toolbar will be visually corrupted,\n"
        "but it will not crash!\n"
        "\nRead the Toolbar.mcc documentation for more information.", cause));
}

BOOL ProcessImageFile(Object *obj, struct Toolbar_Data *data, APTR src_data, struct BitMap **dest_bm, PLANEPTR *dest_mask,
                      UNUSED struct IP_Image **ip_image, Object **dt_object, UWORD *width, UWORD *height, LONG *trans)
{
//  KPrintF("start : ProcessImageFile\n");
  BOOL result = FALSE;
  Object *o = NULL;

  /* To block any dos-requesters */
/*  struct Process *myproc = (struct Process *)FindTask(NULL);
  APTR oldwindowptr = myproc->pr_WindowPtr;
  myproc->pr_WindowPtr = (APTR)-1;
*/

  switch(data->ImageType)
  {
    case MUIV_Toolbar_ImageType_File:
    // no break;
    case MUIV_Toolbar_ImageType_Memory:
    {
      ULONG sourcetype;
      STRPTR buffer = NULL;
      if (data->ImageType==MUIV_Toolbar_ImageType_File)
      {
        sourcetype=DTST_FILE;
        if(data->Path)
        {
          ULONG max_length = strlen(data->Path)+strlen((STRPTR)src_data)+2; // 1 til '\0' og 1 til evt. '/'
          if((buffer = AllocVec(max_length*sizeof(char), MEMF_CLEAR)))
          {
            strcpy(buffer, data->Path);
            if(AddPart(buffer, (STRPTR)src_data, max_length))
              src_data = buffer;
          }
        }
      }
      else
        sourcetype=DTST_RAM;

#ifdef USE_IMAGEPOOL
      if(ImagePoolBase)
      {
        if((*ip_image = IP_RequestImageTags((STRPTR)src_data, ImagePool_Screen, _screen(obj), ImagePool_Precision, data->Precision, TAG_DONE )) != NULL)
        {
          struct IP_Image *ipm = *ip_image;
          *width = ipm->Width;
          *height = ipm->Height;
          *dest_bm = ipm->BMP;

          // Getting the transparency colour
          if(ipm->Flags & IPF_ValidTransparentColor)
            *trans = ipm->TransparentColor;
          //else?

          result = TRUE;
        }
      }
#endif

      if(!result)
      { 
    //    KPrintF("try to get NewDTObject DataTypesBase = %ld\n", (int) DataTypesBase);
        *dt_object = o = NewDTObject(src_data,  DTA_SourceType, sourcetype,
                              DTA_GroupID,        GID_PICTURE,
                              OBP_Precision,        data->Precision,
                              PDTA_DestMode,        PMODE_V43,
                              TAG_DONE);  
    //    KPrintF("NewDTObject got\n");     
      }
      FreeVec(buffer);
    }
    break;
    case MUIV_Toolbar_ImageType_Object:
      o = (Object *)src_data;
    break;

  }

  if(o)
  {

    struct BitMapHeader *bmhd;
    get(o, PDTA_BitMapHeader, &bmhd);
    if(bmhd)
    {
      ULONG *cr;
      struct FrameInfo fri;

      memset(&fri, 0, sizeof(struct FrameInfo));

      if(get(o ,PDTA_CRegs, &cr))
      {
        ULONG color = bmhd->bmh_Transparent*3;
        *trans = ObtainBestPenA(_screen(obj)->ViewPort.ColorMap, cr[color], cr[color+1], cr[color+2], NULL);
      }
      set(o, PDTA_Screen,           _screen(obj));
      set(o, PDTA_FreeSourceBitMap, TRUE);
      set(o, PDTA_UseFriendBitMap,  TRUE);
      DoMethod(o, DTM_FRAMEBOX, NULL, &fri, &fri, sizeof(struct FrameInfo), 0);

      if (fri.fri_Dimensions.Depth>0)
      {
        if (DoMethod(o, DTM_PROCLAYOUT, NULL, 1L))
        {
          GetDTAttrs(o, PDTA_DestBitMap, dest_bm, TAG_DONE);
          if(!*dest_bm)
            GetDTAttrs(o, PDTA_BitMap, dest_bm, TAG_DONE);
          if(*dest_bm)
          {
            *width = bmhd->bmh_Width;
            *height = bmhd->bmh_Height;
            get(o, PDTA_MaskPlane, dest_mask);
            result = TRUE;
          }
        }
      }
    }
  }

  return(result);
}

ULONG Toolbar_Setup(struct IClass *cl, Object *obj, Msg msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);
  struct RastPort rp;
  struct MUI_PenSpec *tmp;
  ULONG bgcol[3];
  struct ColorMap *cmap;
  struct MUIP_Toolbar_Description *desc;
  ULONG image_icons, entries;
  BYTE image_offset[32];
  ULONG number;
  BYTE *perm;
  BYTE n;
  UWORD max_width;
  BOOL adjust;
  struct MinNode *node;

  if (!(DoSuperMethodA(cl, obj, msg)))
    return(FALSE);
  // Setup an event-handler
  data->EHNode.ehn_Object = obj;
  data->EHNode.ehn_Class  = cl;
  data->EHNode.ehn_Events = IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY;
   DoMethod(_win(obj),MUIM_Window_AddEventHandler,&data->EHNode);
  // Should we show the tooltext? ...the images?
  data->ToolImages = data->ToolTexts = TRUE; // Default
  switch(GetConfigItem(obj, MUICFG_Toolbar_ToolbarLook, LOOK_IMAGE_TEXT))
  {
    case LOOK_IMAGE: data->ToolTexts  = FALSE; break;
    case LOOK_TEXT:  data->ToolImages = FALSE; break;
  }
  // Default background text-colours
  data->SelectTrans = data->NormalTrans = _pens(obj)[MPEN_HALFSHINE];
  data->GhostTrans  = _pens(obj)[MPEN_BACKGROUND];
  // Get the ghosting-effect - even if there are no images!
  data->GhostEffect = GetConfigItem(obj, MUICFG_Toolbar_GhostEffect, DEFAULT_GHOSTEFFECT);
  // *********** Images - Begin ***********
  if(data->ToolImages)
  {
    data->Precision = GetConfigItem(obj, MUICFG_Toolbar_Precision, DEFAULT_PRECISION);
    data->Precision = (data->Precision == 0 ? (ULONG)-1 : (data->Precision-1)*16);

    /* Normal image */
    if(!ProcessImageFile(obj, data, data->NormalData, &data->NormalBM, &data->NormalMask, &data->NormalIP, &data->NormalDTO, &data->ImageWidth, &data->ImageHeight, &data->NormalTrans))
    {
      data->NormalBM = NULL;
      data->ToolTexts = TRUE; // Forced because of no images.
      data->ToolImages = FALSE;
    }
    data->SelectBM = data->GhostBM = data->NormalBM; // Default images
    data->SelectMask = data->GhostMask = data->NormalMask; // Default images

    if(data->ToolImages)
    {
      data->SelectTrans = data->GhostTrans = data->NormalTrans;
    
      if(GetConfigItem(obj, MUICFG_Toolbar_UseImages, DEFAULT_USEIMAGES))
      {
        UWORD tempWidth, tempHeight; // Used to validate the sizes

        /* Selected image */
        if(data->SelectData)
        {
          if(ProcessImageFile(obj, data, data->SelectData, &data->SelectBM, &data->SelectMask, &data->SelectIP, &data->SelectDTO, &tempWidth, &tempHeight, &data->SelectTrans))
          {
            if(data->ImageWidth!=tempWidth || data->ImageHeight!=tempHeight)
            {
                {
                CoerceMethod(cl, obj, MUIM_Cleanup);
                return(FALSE);
                }
            }
          }
        }

        /* Ghosted image */
        if(data->GhostData)
        {
          if(ProcessImageFile(obj, data, data->GhostData, &data->GhostBM, &data->GhostMask, &data->GhostIP, &data->GhostDTO, &tempWidth, &tempHeight, &data->GhostTrans))
          {
            data->GhostEffect = GHOST_BITMAP;
            if(data->ImageWidth!=tempWidth || data->ImageHeight!=tempHeight)
            {
              if(ReportSizeError("ghosted", _app(obj)))
                {
                CoerceMethod(cl, obj, MUIM_Cleanup);
                return(FALSE);
                }
            }
          }
        }
      }
    }
  }
  // *********** Images - End ***********

  // Get BorderType
  data->BorderType = GetConfigItem(obj, MUICFG_Toolbar_BorderType, DEFAULT_BORDERTYPE);

  // Get SelectionType
  data->SelectionMode = GetConfigItem(obj, MUICFG_Toolbar_SelectionMode, DEFAULT_SELECTIONMODE);

  // Get AutoActive
  data->AutoActive = (BOOL) GetConfigItem(obj, MUICFG_Toolbar_AutoActive, FALSE);

  // Get the space-size between groups/tools
  data->GroupSpace = GetConfigItem(obj, MUICFG_Toolbar_GroupSpace, DEFAULT_GROUPSPACE);
  data->ToolSpace  = GetConfigItem(obj, MUICFG_Toolbar_ToolSpace,  DEFAULT_TOOLSPACE);

  // FixSize/NeverShrink
  data->FixSize = data->GroupSpace != 50;
//  data->NeverShrink = GetConfigItem(obj, MUICFG_Toolbar_NeverShrink,  DEFAULT_NEVERSHRINK);
  data->NeverShrink = TRUE;
  set(obj, MUIA_Weight, data->NeverShrink ? 100 : 5000);

  // Get textfont
  if(data->ToolTexts)
  {
    char *value;
    
    if(DoMethod(obj, MUIM_GetConfigItem, MUICFG_Toolbar_ToolFont, &value) &&
       value != NULL && *value != '\0')
    {
      struct TextAttr ta = {NULL, 0, 0, 0};
      char fontnamespace[30], *fontstr, *p;
      
      ta.ta_Name = p = &fontnamespace[0];

      for(fontstr=value; *fontstr && *fontstr != '/';)
      {
        *p++ = *fontstr++;
      }
      
      strcpy(p, ".font");

      if(fontstr[0])
      {
        LONG ysiz;
        StrToLong(&fontstr[1], &ysiz);
        ta.ta_YSize = (UWORD)ysiz;
      }

      // KPrintF("Font: %s, Size: %ld .. %s\n", ta.ta_Name, ta.ta_YSize);

      if((data->ToolFont = (struct TextFont *)OpenDiskFont(&ta)) != NULL)
        data->CloseToolFont = TRUE;
      else
        data->ToolFont = _font(obj);
    }
    else
      data->ToolFont = _font(obj);

    // Get halfshine/shadow/textpen colours
    tmp = (struct MUI_PenSpec *)(DoMethod(obj, MUIM_GetConfigItem, MUICFG_Toolbar_ToolPen, &value) ? value : DEFAULT_TEXTPEN);
    data->TextPen = MUI_ObtainPen(muiRenderInfo(obj), tmp, 0);
    cmap = _screen(obj)->ViewPort.ColorMap;
    GetRGB32(cmap, data->GhostTrans, 1, bgcol);
    data->HalfShine  = AllocShineShadowPen(cmap, bgcol, FALSE);
    data->HalfShadow = AllocShineShadowPen(cmap, bgcol, TRUE);

    // Build a dummy rastport to be able to use TextLength
    InitRastPort(&rp);
    SetFont(&rp, data->ToolFont);
    data->UnderscoreSize = TextLength(&rp, "_", 1);
  }

  // Let us find the number of tools in the structure/image
  desc = data->Desc;
  image_icons = 0;
  entries = 0;
  number = 0;

  while (desc->Type != TDT_END)
  {
    if((desc->Type == TDT_BUTTON) || (desc->Type == TDT_IGNORE))
    {
      image_offset[number] = image_icons;
      image_icons++;
    }

    if(desc->Type != TDT_IGNORE)
      entries++;

    number++;
    desc++;
  }

  // Handle the permutation
  if(!data->Permutation)
  {
    // Default behaviour - create the structure...
    BYTE *perm;
    BYTE number = 0;

    if(!(perm = data->Permutation = AllocVec((entries+1)*sizeof(BYTE), MEMF_CLEAR)))
    {
      CoerceMethod(cl, obj, MUIM_Cleanup);
      return(FALSE);
    }

    data->PermutationCreated = TRUE;
    desc = data->Desc;
    while (desc->Type != TDT_END)
    {
      if(desc->Type != TDT_IGNORE)
        *perm++ = number;

      desc++;
      number++;
    }
    *perm = -1;

  }

  NewList((struct List *)&data->ToolList);
  desc = data->Desc;

  perm = data->Permutation;
//  KPrintF("--------\n");
  while((n = *perm++) != -1)
  {
    if((n == TP_SPACE) || (desc[n].Type == TDT_SPACE))
    {
      struct TB_Element *space;
      if((space = AllocVec(sizeof(struct TB_Element), MEMF_CLEAR)))
      {
        space->Type = TDT_SPACE;
        space->Size = data->GroupSpace;
        AddTail((struct List *)&data->ToolList, (struct Node *)space);
      }
      else
      {
        CoerceMethod(cl, obj, MUIM_Cleanup);
        return(FALSE);
      }
    }
    else if(desc[n].Type == TDT_BUTTON)
    {
      struct TB_Element *tool;
      if((tool = AllocVec(sizeof(struct TB_Element), MEMF_CLEAR)))
      {
        tool->Type = TDT_BUTTON;
        tool->Number = n;
        AddTail((struct List *)&data->ToolList, (struct Node *)tool);
      }
      else
      {
        CoerceMethod(cl, obj, MUIM_Cleanup);
        return(FALSE);
      }
    }
  }

  // Calculate the maximum width of the texts
  // and count the number of toolspaces/spaces.

  max_width = 0;
  adjust = FALSE;
  data->Buttons = 0, data->GroupSpaces = 0, data->ToolSpaces = 0;

  for(node = data->ToolList.mlh_Head; node->mln_Succ; node = *(struct MinNode **)node)
  {
    struct TB_Element *tool = (struct TB_Element *)node;
    if(tool->Type == TDT_BUTTON)
    {
      struct MUIP_Toolbar_Description *tdesc = &desc[tool->Number];

      if(adjust)
        data->ToolSpaces++;
      else
        adjust = TRUE;

      if(tdesc->ToolText)
      {
        tool->ParseUnderscore = FALSE; // Important since new doesn't nullify.
        if(data->ParseUnderscore) // Always parse because of hotkeys
        {
          STRPTR text = tdesc->ToolText;
          while(*text != '\0')
          {
            if(*text == '_')
            {
              text++;
//KPrintF("I set button number %ld to TRUE\n", tool->Number);
              tool->ParseUnderscore = TRUE;
//              if(tdesc.Key == NULL)
              tdesc->Key = ToLower(*text);
              break;
            }
            text++;
          }
        }

        if(data->ToolTexts)
        {
          WORD width = TextLength(&rp, tdesc->ToolText, strlen(tdesc->ToolText));

          if(tool->ParseUnderscore)
            width -= data->UnderscoreSize;

          if(width > max_width)
            max_width = width;
        }
      }
      data->Buttons++;
    }
    else // TDT_SPACE
    {
      data->GroupSpaces++;
      adjust = FALSE;
    }
  }

  // What to do if no bitmap is available (and it should be)
  if(!data->NormalBM)
  {
    if(max_width>0)
    {
      data->ImageWidth = max_width*image_icons + image_icons - 1;
      /* Can ToolImages be TRUE at this point???? - I think NOT */
//      data->ImageHeight = data->ToolImages ? (max_width - data->ToolFont->tf_YSize) : 0;
      data->ImageHeight = 0;
    }
    else
    {
      data->ImageWidth = 15*image_icons + image_icons - 1;
      /* Can ToolImages be TRUE at this point???? - I think NOT */
//      data->ImageHeight = data->ToolImages ? 15 : 5;
      data->ImageHeight = 5;
    }
  }

  // Calculate icon-width/height
  data->IconWidth = (data->ImageWidth - image_icons + 1)/image_icons; // Notice subtraction of black lines
  data->IconHeight = data->ImageHeight;

  // Does everything add up
  if(data->IconWidth*image_icons+image_icons-1 != data->ImageWidth)
  {
    if(!MUI_Request (_app(obj), NULL, 0, NULL, "_Continue|_Fail",
      "The width of your toolbar-image is not correct. Remember\n"
      "that all icons MUST have the same width.\n"
      "If you continue, the toolbar will be visually corrupted,\n"
      "but it will not crash!\n"
      "\nRead the Toolbar.mcc documentation for more information."
#ifdef __AROS__
#warning "FIXME: this sucks!"
      , 0
#endif      
      ))
      {
      CoerceMethod(cl, obj, MUIM_Cleanup);
      return(FALSE);
      }
  }

  // Update source-coordinates
  for(node = data->ToolList.mlh_Head; node->mln_Succ; node = *(struct MinNode **)node)
  {
    struct TB_Element *tool = (struct TB_Element *)node;
    if(tool->Type == TDT_BUTTON)
    {
      tool->SrcOffset = image_offset[tool->Number] * (data->IconWidth + 1);
    }
  }

  // Calculate button size and inner-spacing.
  if(data->ToolTexts && (max_width>0))
  {
    data->InnerSpace = GetConfigItem(obj, MUICFG_Toolbar_InnerSpace_Text, DEFAULT_INNERSPACE_TEXT);
    data->ButtonWidth = (data->IconWidth < max_width
                  ? max_width + 4 + 2*data->InnerSpace
                  : data->IconWidth + 4 + 2*data->InnerSpace);
    data->ButtonHeight = data->IconHeight + 4 + 2*data->InnerSpace + 2 + data->ToolFont->tf_YSize;
  }
  else
  {
    data->ToolTexts = FALSE;
    data->InnerSpace = GetConfigItem(obj, MUICFG_Toolbar_InnerSpace_NoText, DEFAULT_INNERSPACE_NOTEXT);
    data->ButtonWidth = data->IconWidth + 4 + 2*data->InnerSpace;
    data->ButtonHeight = data->IconHeight + 4 + 2*data->InnerSpace;
  }

  data->ButtonSize = data->Horizontal ? data->ButtonWidth : data->ButtonHeight;

  data->Setup = TRUE;
  // KPrintF("->NO Errors in the Setup section\n");
  return(TRUE);
}

ULONG Toolbar_Cleanup(struct IClass *cl, Object *obj, Msg msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);

  data->Setup = FALSE;

#ifdef USE_IMAGEPOOL
  if(ImagePoolBase)
  {
    IP_FreeImage(data->NormalIP);
    IP_FreeImage(data->SelectIP);
    IP_FreeImage(data->GhostIP);
  }
#endif

  DisposeDTObject(data->NormalDTO);
  DisposeDTObject(data->SelectDTO);
  DisposeDTObject(data->GhostDTO);

  data->NormalDTO = NULL; data->SelectDTO = NULL; data->GhostDTO = NULL;
  data->NormalIP = NULL; data->SelectIP = NULL; data->GhostIP = NULL;
  data->NormalBM = NULL; data->SelectBM = NULL; data->GhostBM = NULL;

  while(!IsListEmpty((struct List *)&data->ToolList))
  {
    struct TB_Element *elem = (struct TB_Element *)RemHead((struct List *)&data->ToolList);
    if(elem)
      FreeVec(elem);
  }

  if(data->ToolTexts)
  {
    // Passing -1 will result in nothing.
    struct ColorMap *cmap = _screen(obj)->ViewPort.ColorMap;
    ReleasePen(cmap, data->HalfShine);
    ReleasePen(cmap, data->HalfShadow);
    MUI_ReleasePen(muiRenderInfo(obj), data->TextPen);
  }

  if(data->CloseToolFont)
  {
    CloseFont(data->ToolFont);
    data->CloseToolFont = FALSE;
  }

  // Remove the event-handler.
  DoMethod(_win(obj), MUIM_Window_RemEventHandler,&data->EHNode);

  return(DoSuperMethodA(cl, obj, msg));
}
