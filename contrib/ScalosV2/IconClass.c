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
#include <proto/exec.h>
#include <proto/graphics.h>
#include <string.h>
#include <exec/memory.h>
#include <workbench/workbench.h>
#include <graphics/gfx.h>
#include <graphics/rpattr.h>

#include "Scalos.h"
#include "IconClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"
#include "CompilerSupport.h"

/****** Icon.scalos/--background ********************************************
*
*   Icon class is the super class of all icons. It will remember all
*   information of an icon including images and tooltypes. A object of this
*   class will represent not only a file on disk. It should be handled as
*   which knows how to react itself. Some methods of this class are im-
*   plemented only in subclasses.
*   Icons are nut just tiny images. These icons are little dos object. They
*   handle anything, including reading or write from/to a file. Icons are the
*   interface to the lower level files or dirs.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_Selected **************************************
*
*  NAME
*   SCCA_Icon_Selected -- (V40) ISG (BOOL)
*
*  FUNCTION
*   This attribute selectes the image which should be displayed. To show the
*   changes SCCM_Graphic_Draw have to be called.
*
*****************************************************************************
*/                                                              
// /
/****** Icon.scalos/SCCA_Icon_GraphicNormal *********************************
*
*  NAME
*   SCCA_Icon_GraphicNormal -- (V40) IS. (Object *)
*
*  FUNCTION
*   Using this attribute the normal image will be set.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_GraphicSelected *******************************
*
*  NAME
*   SCCA_Icon_GraphicSelected -- (V40) IS. (Object *)
*
*  FUNCTION
*   Using this attribute the selected image will be set.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_IsList ****************************************
*
*  NAME
*   SCCA_Icon_IsList -- (V40) ..G (BOOL)
*
*  FUNCTION
*   If this object contains a list this attribute is true. This list can be
*   parsed using the SCCM_Icon_Entry method. You have to implement this
*   attribute into your own class only if you like to set it to TRUE. It
*   default set to FALSE.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_Table *****************************************
*
*  NAME
*   SCCA_Icon_Table -- (V40) ..G (Object *)
*
*  FUNCTION
*   To find out which special info an icon has, you'll need to get a Table
*   object. This object has all infos which are needed to identify the
*   special info, e.g. id, name, datatype.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_Name ******************************************
*
*  NAME
*   SCCA_Icon_Name -- (V40) I.G (char *)
*
*  FUNCTION
*   Every icon object has a name.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_NoPostion *************************************
*
*  NAME
*   SCCA_Icon_NoPosition -- (V40) ISG (BOOL)
*
*  FUNCTION
*   The icon has no position. A new position have to be set before drawing
*   it.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_Type ******************************************
*
*  NAME
*   SCCA_Icon_Type -- (V40) ISG (UBYTE)
*
*  FUNCTION
*   This attribute gives information about the icon type (not filetype!).
*   Available values are:
*       WBDISK - icon of a disk
*       WBDRAWER - icon of a directory
*       WBTOOL - icon of a executable program
*       WBPROJECT - icon of a file which should be argument from the default-
*                   tool
*       WBGARBAGE - garbage icon
*       WBDEVICE - icon of a device (normally not used)
*       WBKICK - icon of of a non-DOS disk
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_StackSize *************************************
*
*  NAME
*   SCCA_Icon_StackSize -- (V40) ISG (ULONG)
*
*  FUNCTION
*   This one is normaly only defined if the icon type is a WBTOOL. The
*   executable program should be started with this amount of stack.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_WindowLeft ************************************
*
*  NAME
*   SCCA_Icon_WindowLeft -- (V40) ISG (WORD)
*
*  FUNCTION
*   Drawer and disk icons can have information about the of that directory.
*   If a window should be opened from the directory which this icon
*   represents, the values of this attribute should be used as window
*   parameters.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_WindowTop *************************************
*
*  NAME
*   SCCA_Icon_WindowTop -- (V40) ISG (WORD)
*
*  FUNCTION
*   Drawer and disk icons can have information about the of that directory.
*   If a window should be opened from the directory which this icon
*   represents, the values of this attribute should be used as window
*   parameters.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_WindowWidth ***********************************
*
*  NAME
*   SCCA_Icon_WindowWidth -- (V40) ISG (UWORD)
*
*  FUNCTION
*   Drawer and disk icons can have information about the of that directory.
*   If a window should be opened from the directory which this icon
*   represents, the values of this attribute should be used as window
*   parameters.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_WindowHeight **********************************
*
*  NAME
*   SCCA_Icon_WindowHeight -- (V40) ISG (UWORD)
*
*  FUNCTION
*   Drawer and disk icons can have information about the of that directory.
*   If a window should be opened from the directory which this icon
*   represents, the values of this attribute should be used as window
*   parameters.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_WindowOffsetX *********************************
*
*  NAME
*   SCCA_Icon_WindowOffsetX -- (V40) ISG (LONG)
*
*  FUNCTION
*   Drawer and disk icons can have information about the of that directory.
*   If a window should be opened from the directory which this icon
*   represents, the values of this attribute should be used as parameter for
*   scroller positions.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_WindowOffsetY *********************************
*
*  NAME
*   SCCA_Icon_WindowOffsetY -- (V40) ISG (LONG)
*
*  FUNCTION
*   Drawer and disk icons can have information about the of that directory.
*   If a window should be opened from the directory which this icon
*   represents, the values of this attribute should be used as parameter for
*   scroller positions.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_WindowAllFiles ********************************
*
*  NAME
*   SCCA_Icon_WindowAllFiles -- (V40) ISG (BOOL)
*
*  FUNCTION
*   Drawer and disk icons can have information about the of that directory.
*   If a window should be opened from the directory which this icon
*   represents, the values of this attribute indicates if all files should
*   be visible or only icons.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_RawIconType ***********************************
*
*  NAME
*   SCCA_Icon_RawIconType -- (V40) ISG (ULONG)
*
*  FUNCTION
*   This attribute gives information about the icon format of the original
*   disk object. (e.g. standard icon or newicon) It's needed to know which
*   format should be written back to disk on SCCM_Icon_PutIcon. It's very
*   simple to change the disk format of an icon by setting this attribute and
*   putting it back to disk.
*   The ULONG is a id such as : 'ICON' or 'NEWI'
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_ToolTypes *************************************
*
*  NAME
*   SCCA_Icon_ToolTypes -- (V40) ISG (Object *)
*
*  FUNCTION
*   The tooltypes have some special information about the icon. It's not
*   stored in an array like the old Amiga icon.library diskobjects. All
*   tooltypes are in a list object, which can be manipulated very easy.
*
*****************************************************************************
*/
// /
/****** Icon.scalos/SCCA_Icon_Path ******************************************
*
*  NAME
*   SCCA_Icon_Path -- (V40) I.. (char *)
*
*  FUNCTION
*   This attribute can be used to open an icon with a specific path. The
*   icon object will extract the name from the path and set this as name.
*
*****************************************************************************
*/
// /


/****** Icon.scalos/SCCM_Icon_GetObject *************************************
*
*  NAME
*   SCCM_Icon_GetObject
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Icon_GetObject, struct TagItem *taglist);
*
*  FUNCTION
*   This method will return a child of itself. For example you have a
*   directory icon and you want to have an object of a specific entry, you
*   can use this method. To specify which entry you like to have, additional
*   information have to be given. In most cases the name is enough, but you
*   should give as much information as you have.
*   If you give no arguments the icon will return an object from the current
*   selected entry. This is the best way to generate a child object.
*
*  INPUTS
*   A list of tags to specify the child object. All available tags can be
*   found in a table object of the parent.
*
*  RESULT
*   An object or NULL if an error occured
*
*  NOTES
*   This method is implemented by a sub class of the icon class.
*
*  SEE ALSO
*
*****************************************************************************
*/
// /

/****** Icon.scalos/SCCM_Icon_Entry *****************************************
*
*  NAME
*   SCCM_Icon_Entry
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Icon_Entry, ULONG Pos);
*
*  FUNCTION
*   This method selectes a current entry. Attributes of this entry can be got
*   using the standard SC_GetAttr() function. Maybe selecting an entry will
*   cause the list to be locked. You should parse quickly through the list
*   and then select entry "None". Probably some classes doesn't have all
*   values implemented. For detailed information of the implemented values
*   check out the documentation of the class.
*
*  INPUTS
*   Pos - one of these values
*           SCCV_Icon_Entry_First - select first entry
*           SCCV_Icon_Entry_Last - select last entry
*           SCCV_Icon_Entry_Next - select next entry
*           SCCV_Icon_Entry_Previous - select previous entry
*           SCCV_Icon_Entry_None - release the lock on the list
*
*  RESULT
*   TRUE if the new entry was successfully selected. FALSE if the selection
*   fails. In this case "None" will automatically selected.
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
// /

/****** Icon.scalos/SCCM_Icon_Open ******************************************
*
*  NAME
*   SCCM_Icon_Open
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Icon_Open,ULONG type);
*
*  FUNCTION
*   This method generates a so called open handle. It's needed to read from
*   file. If the object is a directory a NULL will be returned.
*
*  INPUTS
*   type - AmigaDOS open type (OLD or NEW)
*
*  RESULT
*   An open handle or NULL if an error occur.
*
*  NOTES
*   This method is implemented by a sub class of the icon class.
*   The returned handle is not an AmigaDOS filehandle!
*
*  SEE ALSO
*   SCCM_Icon_Read, SCCM_Icon_Close
*
*****************************************************************************
*/
// /

/****** Icon.scalos/SCCM_Icon_Close *****************************************
*
*  NAME
*   SCCM_Icon_Close
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Icon_Close, ULONG handle);
*
*  FUNCTION
*   This method is used to free a open handle returned by SCCM_Icon_Open.
*
*  INPUTS
*   handle - open handle, the result of SCCM_Icon_Open
*
*  RESULT
*
*  NOTES
*   This method is implemented by a sub class of the icon class.
*
*  SEE ALSO
*   SCCM_Icon_Read, SCCM_Icon_Open
*
*****************************************************************************
*/
// /

/****** Icon.scalos/SCCM_Icon_Read ******************************************
*
*  NAME
*   SCCM_Icon_Read
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Icon_Read, ULONG handle, APTR memory, ULONG size);
*
*  FUNCTION
*   This method reads a number of bytes from the file.
*
*  INPUTS
*   handle - open handle, the result of SCCM_Icon_Open
*   memory - memory area where to place the data
*   size - number of bytes to read
*
*  RESULT
*   number of bytes actually read
*
*  NOTES
*   This method is implemented by a sub class of the icon class.
*
*  SEE ALSO
*   SCCM_Icon_Open, SCCM_Icon_Close
*
*****************************************************************************
*/
// /

/****** Icon.scalos/SCCM_Icon_Write *****************************************
*
*  NAME
*   SCCM_Icon_Write
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Icon_Write, ULONG handle, APTR memory, ULONG size);
*
*  FUNCTION
*   This method writes a number of bytes to the file.
*
*  INPUTS
*   handle - open handle, the result of SCCM_Icon_Open
*   memory - memory area where the data is placed
*   size - number of bytes to write
*
*  RESULT
*   number of bytes actually write
*
*  NOTES
*   This method is implemented by a sub class of the icon class.
*
*  SEE ALSO
*   SCCM_Icon_Open, SCCM_Icon_Close
*
*****************************************************************************
*/
// /

/****** Icon.scalos/SCCM_Icon_OpenIcon **************************************
*
*  NAME
*   SCCM_Icon_OpenIcon
*
*  SYNOPSIS
*   APTR SC_DoMethod(obj,SCCM_Icon_OpenIcon, char *ext);
*
*  FUNCTION
*   Open an icon with the given extention. Normaly this is name + ext, but
*   some classes may change the name. (e.g. DOSDeviceIcon will open disk+ext)
*   The returned handle will be needed to read from the file.
*   Application should not use this method, but it must be implemented in
*   sub classes of icon class.
*
*  INPUTS
*   ext - the filename extention which should be use to load the file
*
*  RESULT
*   Handle to read from file. The returned structure depends on the icon sub
*   class. The result NULL means the file doesn't exists or any other error
*   occured.
*
*  NOTES
*   This method is implemented by a sub class of the icon class.
*
*  SEE ALSO
*
*****************************************************************************
*/
// /

/****** Icon.scalos/SCCM_Icon_ReadIcon **************************************
*
*  NAME
*   SCCM_Icon_ReadIcon
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Icon_ReadIcon, APTR handle, APTR mem, ULONG size);
*
*  FUNCTION
*   The method will read a number of bytes from an icon which was opened with
*   SCCM_Icon_OpenIcon.
*
*  INPUTS
*   handle - return value from SCCM_Icon_OpenIcon
*   mem - pointer to the memory area where to load the data
*   size - number of bytes to read
*
*  RESULT
*   Number of bytes read.
*
*  NOTES
*   This method is implemented by a sub class of the icon class.
*
*  SEE ALSO
*
*****************************************************************************
*/
// /

/****** Icon.scalos/SCCM_Icon_CloseIcon *************************************
*
*  NAME
*   SCCM_Icon_CloseIcon
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Icon_CloseIcon, APTR handle);
*
*  FUNCTION
*   Close the icon opened with SCCM_Icon_OpenIcon.
*
*  INPUTS
*   handle - return value from SCCM_Icon_OpenIcon
*
*  RESULT
*
*  NOTES
*   This method is implemented by a sub class of the icon class.
*
*  SEE ALSO
*
*****************************************************************************
*/
// /

/****** Icon.scalos/SCCM_Icon_GetString *************************************
*
*  NAME
*   SCCM_Icon_GetString
*
*  SYNOPSIS
*   char *SC_DoMethod(obj,SCCM_Icon_GetString, ULONG Attr);
*
*  FUNCTION
*   Using this method it's possible to get the value of an attribute and the
*   result will always a string. Again, getting a attribute which normaly is
*   a ULONG (e.g. SCCA_DOSIcon_Size) you'll got the number as a string.
*
*  INPUTS
*   Attr - attribute which value should be returned
*
*  RESULT
*   The value of the attribute as string.
*
*  NOTES
*   This method is implemented by a sub class of the icon class.
*
*  SEE ALSO
*
*****************************************************************************
*/
// /

/****** Icon.scalos/SCCM_Icon_GetIcon ***************************************
*
*  NAME
*   SCCM_Icon_GetIcon
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Icon_GetIcon, ULONG type);
*
*  FUNCTION
*   After creating an icon object you normaly need an image to show this
*   icon on screen. This method will read the images and setup anything you
*   need.
*
*  INPUTS
*   type - which icon should be loaded :
*           SCCV_Icon_GetIcon_Disk - load the icon from disk
*           SCCV_Icon_GetIcon_Default - use a default icon depending on the
*                                       filetype
*
*  RESULT
*   TRUE if the icon was successfully got. FALSE if something went wrong.
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static ULONG Icon_GetIcon(struct SC_Class *cl, Object *obj, struct SCCP_Icon_GetIcon *msg, struct IconInst *inst)
{
        if (msg->type == SCCV_Icon_GetIcon_Disk)
        {
                ULONG handle;
                UBYTE *icon = NULL;
                int memsize = 8*512;
                int iconsize = 0;

                if ((handle = SC_DoMethod(obj,SCCM_Icon_OpenIcon,".info")))
                {
                        BOOL loading = TRUE;
                        APTR oldmem = NULL;


                                // read the icon in 4kb pieces
                        for (memsize = 8*512; loading; memsize += 8*512)
                        {
                                if ((icon = AllocVec(memsize,MEMF_ANY)))
                                {
                                        if (oldmem)
                                        {
                                                memcpy(icon,oldmem,memsize - 8*512);
                                                FreeVec(oldmem);
                                        }

                                        if ((iconsize += SC_DoMethod(obj,SCCM_Icon_ReadIcon,handle,icon,8*512)) != 8*512)
                                        {
                                                loading = FALSE;
                                        }
                                        else
                                        {
                                                oldmem = icon;
                                        }
                                }
                                else
                                {
                                        loading = FALSE;
                                        FreeVec(icon);
                                        icon = NULL;
                                }
                        }

                        SC_DoMethod(obj,SCCM_Icon_CloseIcon,handle);
                }

                if (icon)
                {
                        struct DiskObject *icondo = (struct DiskObject *) icon;

                        if ((icondo->do_Magic == WB_DISKMAGIC) && (icondo->do_Version == WB_DISKVERSION))
                        {
                                struct Image *iconimg;
                                int imagesize, imagewidth, i;
                                int offset = sizeof(struct DiskObject);
                                struct BitMap bm;
                                ULONG planeptr;
                                Object *bmobj;
                                BOOL nopos = FALSE;

                                if ((icondo->do_CurrentX == NO_ICON_POSITION) || (icondo->do_CurrentY == NO_ICON_POSITION))
                                {
                                        nopos = TRUE;
                                        icondo->do_CurrentX = icondo->do_CurrentY = 0;
                                }

                                SC_SetAttrs(obj,SCCA_Icon_Type,icondo->do_Type,SCCA_Graphic_Left,icondo->do_CurrentX,
                                                        SCCA_Graphic_Top,icondo->do_CurrentY,SCCA_Icon_StackSize,icondo->do_StackSize,
                                                        SCCA_Icon_RawIconType,MAKE_ID('I','C','O','N'),SCCA_Icon_NoPosition,nopos,
                                                        TAG_DONE);

                                if (((struct DiskObject *) icon)->do_DrawerData)
                                {
                                        struct DrawerData *icondd = (struct DrawerData *) &icon[offset];
                                        offset += sizeof(struct OldDrawerData);

                                        SC_SetAttrs(obj,SCCA_Icon_WindowLeft,icondd->dd_NewWindow.LeftEdge,
                                                                SCCA_Icon_WindowTop,icondd->dd_NewWindow.TopEdge,
                                                                SCCA_Icon_WindowWidth,icondd->dd_NewWindow.Width,
                                                                SCCA_Icon_WindowHeight,icondd->dd_NewWindow.Height,
                                                                SCCA_Icon_WindowOffsetX,icondd->dd_CurrentX,
                                                                SCCA_Icon_WindowOffsetY,icondd->dd_CurrentY,
                                                                TAG_DONE);
                                }

                                iconimg = (struct Image *) &icon[offset];
                                imagewidth = (iconimg->Width + 15)/8 & 0xfffffffe; // width in bytes divideable by 2
                                imagesize = imagewidth * iconimg->Height * iconimg->Depth;      // width * height * depth is the size of the image

                                        // init the bitmap
                                        // the image data is currently in fast. This must be changed in future!

                                bm.BytesPerRow = imagewidth;
                                bm.Rows = iconimg->Height;
                                bm.Flags = BMF_STANDARD;
                                bm.Depth = iconimg->Depth;
                                bm.pad = 0;
                                planeptr = (ULONG) &icon[offset + sizeof(struct Image)];

                                for (i = 0; i < 8; i++)
                                {
                                        if (iconimg->PlanePick & (1<<i))
                                        {
                                                bm.Planes[i] = (APTR) planeptr;
                                                planeptr += imagewidth * iconimg->Height;
                                        }
                                        else
                                                bm.Planes[i] = NULL;
                                }

                                if ((bmobj = SC_NewObject(NULL,SCC_BITMAP_NAME,SCCA_Bitmap_Bitmap,&bm,
                                                                                 SCCA_Graphic_Width,iconimg->Width,
                                                                                 SCCA_Graphic_Height,iconimg->Height,
                                                                                 SCCA_Bitmap_CopyOriginal,TRUE,
                                                                                 TAG_DONE)))
                                {
                                        SC_SetAttrs(obj,SCCA_Icon_GraphicNormal,bmobj,TAG_DONE);
                                }
                                offset += sizeof(struct Image) + imagesize;


                                if (icondo->do_Gadget.Flags & GFLG_GADGHIMAGE)
                                {
                                        iconimg = (struct Image *) &icon[offset];
                                        planeptr = (ULONG) &icon[offset + sizeof(struct Image)];

                                        for (i = 0; i < 8; i++)
                                        {
                                                if (iconimg->PlanePick & (1<<i))
                                                {
                                                        bm.Planes[i] = (APTR) planeptr;
                                                        planeptr += imagewidth * iconimg->Height;
                                                }
                                                else
                                                        bm.Planes[i] = NULL;
                                        }

                                        if ((bmobj = SC_NewObject(NULL,SCC_BITMAP_NAME,SCCA_Bitmap_Bitmap,&bm,
                                                                                         SCCA_Graphic_Width,iconimg->Width,
                                                                                         SCCA_Graphic_Height,iconimg->Height,
                                                                                         SCCA_Bitmap_CopyOriginal,TRUE,
                                                                                         TAG_DONE)))
                                        {
                                                SC_SetAttrs(obj,SCCA_Icon_GraphicSelected,bmobj,TAG_DONE);
                                        }
                                        offset += sizeof(struct Image) + imagesize;
                                }
                                else
                                {
                                        struct RastPort rp;

                                        InitRastPort(&rp);

                                        if ((rp.BitMap = AllocBitMap(iconimg->Width,iconimg->Height,iconimg->Depth,0,NULL)))
                                        {
                                                BltBitMapRastPort(&bm,0,0,&rp,0,0,iconimg->Width,iconimg->Height, ABNC | ABC);

                                                if (icondo->do_Gadget.Flags & GFLG_GADGBACKFILL)
                                                {
                                                        struct BitMap maskbm;
                                                        struct BitMap *oldbm;
                                                        struct TmpRas tmpras;
                                                        APTR tmpmem;
                                                        int maskwidth = (imagewidth + 4)/2;

                                                        maskbm.BytesPerRow = imagewidth + 4;
                                                        maskbm.Rows = (iconimg->Height + 2);
                                                        maskbm.Flags = 0;
                                                        maskbm.Depth = 2;
                                                        maskbm.pad = 0;

                                                                // the mask has a clean area around to fill it
                                                        if ((maskbm.Planes[0] = AllocVec(maskbm.BytesPerRow * maskbm.Rows * 2, MEMF_ANY | MEMF_CHIP | MEMF_CLEAR)))
                                                        {
                                                                if ((tmpmem = AllocVec(maskbm.BytesPerRow * maskbm.Rows, MEMF_ANY | MEMF_CHIP | MEMF_CLEAR)))
                                                                {
                                                                        int i,j,k;

                                                                        InitTmpRas(&tmpras,tmpmem, maskbm.BytesPerRow * maskbm.Rows);
                                                                        rp.TmpRas = &tmpras;

                                                                                // generate a two bitplane bitmap
                                                                        maskbm.Planes[1] = maskbm.Planes[0] + (maskbm.BytesPerRow * maskbm.Rows);

                                                                                // or all planes of the image to one for the mask


                                                                        for (k = 0; k < iconimg->Depth; k++)
                                                                        {
                                                                                for (j = 1; j < maskbm.Rows - 1; j++)
                                                                                {
                                                                                        for (i = 1; i < maskwidth - 1; i++)
                                                                                        {
                                                                                                ((UWORD *) maskbm.Planes[0])[i + (j * maskwidth)] |= ((UWORD *) bm.Planes[0])[(i - 1) + (j - 1) * imagewidth/2 + k * imagewidth/2 * iconimg->Height];
                                                                                        }
                                                                                }
                                                                        }


                                                                        oldbm = rp.BitMap;
                                                                        rp.BitMap = &maskbm;

                                                                        SetAPen(&rp,2); // set color 2 to effect only bitplane 2
                                                                        Flood(&rp,1,0,0);
                                                                        WaitBlit();

                                                                        rp.BitMap = oldbm;

                                                                        for (j = 0; j < iconimg->Height; j++)
                                                                        {
                                                                                for (i = 0; i < maskwidth - 2; i++)
                                                                                {
                                                                                        ((UWORD *) maskbm.Planes[1])[i + (j * imagewidth/2)] = 0xffff ^ (((UWORD *) maskbm.Planes[1])[(i + 1) + ((j + 1) * maskwidth)]);
                                                                                }
                                                                        }

                                                                        SetDrMd(&rp,COMPLEMENT);
                                                                        SetWriteMask(&rp,iconimg->PlanePick);
                                                                        BltPattern(&rp,maskbm.Planes[1],0,0,iconimg->Width - 1,iconimg->Height - 1,imagewidth);
                                                                        WaitBlit();

                                                                        FreeVec(tmpras.RasPtr);
                                                                }
                                                                FreeVec(maskbm.Planes[0]);
                                                        }
                                                }
                                                else

                                                {
                                                                        SetDrMd(&rp,COMPLEMENT);
                                                                        SetWriteMask(&rp,iconimg->PlanePick);
                                                        BltPattern(&rp,NULL,0,0,iconimg->Width - 1,iconimg->Height - 1,0);
                                                }

                                                if ((bmobj = SC_NewObject(NULL,SCC_BITMAP_NAME,SCCA_Bitmap_Bitmap,rp.BitMap,
                                                                                                 SCCA_Graphic_Width,iconimg->Width,
                                                                                                 SCCA_Graphic_Height,iconimg->Height,
                                                                                                 SCCA_Bitmap_CopyOriginal,TRUE,
                                                                                                 TAG_DONE)))
                                                {
                                                        SC_SetAttrs(obj,SCCA_Icon_GraphicSelected,bmobj,TAG_DONE);
                                                }

                                                FreeBitMap(rp.BitMap);
                                        }
                                }

                                if (icondo->do_DefaultTool && (offset <= iconsize))
                                {
                                        SC_SetAttrs(obj,SCCA_Icon_DefaultTool,&icon[offset]);
                                        offset += strlen((char *) &icon[offset]) + 1;               // strlen doesn't count the 0 byte
                                }

                                if (icondo->do_ToolTypes && (offset <= iconsize))
                                {
                                        ULONG numtt = (icon[offset]<<24 | icon[offset+1]<<16 | icon[offset+2]<<8 | icon[offset+3]);
                                        Object *list;

                                        if ((numtt <= 8) && (offset < iconsize)
                                                && (list = SC_NewObject(NULL,SCC_NODELIST_NAME,SCCA_NodeList_StringList,TRUE,TAG_DONE)))
                                        {
                                                offset += sizeof(ULONG);

                                                for (i = 0; (i < (numtt/4 - 1)) && (offset < iconsize); i++)
                                                {
                                                        SC_DoMethod(list,SCCM_NodeList_Insert,&icon[offset + sizeof(ULONG)],SCCV_NodeList_Insert_After);
                                                        offset += (icon[offset]<<24 | icon[offset+1]<<16 | icon[offset+2]<<8 | icon[offset+3]) + sizeof(ULONG);
                                                }

                                                SC_SetAttrs(obj,SCCA_Icon_ToolTypes,list,TAG_DONE);
                                        }
                                }

                                if (icondo->do_DrawerData && ((offset + 6) <= iconsize))        // 6 bytes extention
                                {
                                        BOOL allfiles;
                                        UWORD viewmode = icon[offset+4]<<8 | icon[offset+5];

                                        allfiles = (icon[offset+3] & 1) ? TRUE : FALSE;               // bit 1 - ShowAllFiles

                                        SC_SetAttrs(obj,SCCA_Icon_WindowAllFiles,allfiles,SCCA_Icon_WindowViewMode,viewmode,TAG_DONE);
                                }

                        }
                        FreeVec(icon);

                        return(TRUE);
                }

        }
        else
        {
                return(FALSE);
        }

        return(FALSE);

}
// /

/****** Icon.scalos/SCCM_Icon_PopupMenu *************************************
*
*  NAME
*   SCCM_Icon_PopupMenu
*
*  SYNOPSIS
*   Object *SC_DoMethod(obj,SCCM_Icon_PopupMenu, ULONG type);
*
*  FUNCTION
*   This method will cause the icon to generate a popup menu. The icon class
*   will make the object and add some entries. The sub classes will probably
*   add some entries itself.
*
*  INPUTS
*   type - which type of popup menu should be generated
*           SCCV_Icon_PopupMenu_ClickMenu - the menu without any action
*                                           before
*           SCCV_Icon_PopupMenu_DropMenu - the menu after droping the icon
*
*  RESULT
*   Object from PopupMenu class or NULL if the method fails.
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static Object *Icon_PopupMenu(struct SC_Class *cl, Object *obj, struct SCCP_Icon_PopupMenu *msg, struct IconInst *inst)
{
        Object *ret = NULL;

        switch(msg->type)
        {
          case SCCV_Icon_PopupMenu_ClickMenu :
                ret = SC_NewObject(NULL,SCC_POPUPMENU_NAME,
                                                   SCCA_PopupMenu_Item,(ULONG) SC_NewObject(NULL,SCC_MENUITEM_NAME,
                                                                                                                                        SCCA_MenuItem_Name,"Open",TAG_DONE),
                                                   TAG_DONE);
                break;

          case SCCV_Icon_PopupMenu_DropMenu :
                break;
        }

        return(ret);
}
// /

/** PreThinkScreen
*/                                                              

static ULONG Icon_PreThinkScreen(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_PreThinkScreen *msg, struct IconInst *inst)
{
                // forward the prethink to our images
        if ((inst->normimage) && SC_DoMethodA(inst->normimage,(Msg) msg))
        {
                if ((inst->selimage) && SC_DoMethodA(inst->selimage,(Msg) msg))
                {
                        return(TRUE);
                }
                else
                        SC_DoMethod(inst->normimage,SCCM_Graphic_PostThinkScreen);
        }
        return(FALSE);
}
// /

/** PreThinkWindow
*/

static ULONG Icon_PreThinkWindow(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_PreThinkWindow *msg, struct IconInst *inst)
{
                // forward the prethink to our images
        if ((inst->normimage) && SC_DoMethodA(inst->normimage,(Msg) msg))
        {
                if ((inst->selimage) && SC_DoMethodA(inst->selimage,(Msg) msg))
                {
                        return(TRUE);
                }
                else
                        SC_DoMethod(inst->normimage,SCCM_Graphic_PostThinkWindow);
        }
        return(FALSE);
}
// /

/** Draw
*/

static void Icon_Draw(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_Draw *msg, struct IconInst *inst)
{
        if (!(inst->flags & ICONF_Selected) && inst->normimage)
                SC_DoMethodA(inst->normimage,(Msg) msg);
        if ((inst->flags & ICONF_Selected) && inst->selimage)
                SC_DoMethodA(inst->selimage,(Msg) msg);
}
// /

/** PostThinkWindow
*/

static void Icon_PostThinkWindow(struct SC_Class *cl, Object *obj, struct SCCP_Icon_PostThinkWindow *msg, struct IconInst *inst)
{
        if (inst->normimage)
                SC_DoMethodA(inst->normimage,(Msg) msg);
        if (inst->selimage)
                SC_DoMethodA(inst->selimage,(Msg) msg);
}
// /

/** PostThinkScreen
*/

static void Icon_PostThinkScreen(struct SC_Class *cl, Object *obj, Msg *msg, struct IconInst *inst)
{
        if (inst->normimage)
                SC_DoMethodA(inst->normimage,(Msg) msg);
        if (inst->selimage)
                SC_DoMethodA(inst->selimage,(Msg) msg);
}   
// /

/** SetTags routine
*/
static void SetTags(Object *obj, struct IconInst *inst, struct TagItem *taglist)
{
        struct TagItem *tag;
        struct TagItem **tags = &taglist;
        char *name;

        while ((tag = NextTagItem(tags)))
        {
                switch (tag->ti_Tag)
                {

                  case SCCA_Icon_GraphicNormal:
                        if (inst->normimage)
                                SC_DisposeObject(inst->normimage);
                        inst->normimage = (Object *) tag->ti_Data;
                        if (inst->normimage)
                        {
                                SC_SetAttrs(obj,SCCA_Graphic_Width,get(inst->normimage,SCCA_Graphic_Width),
                                                        SCCA_Graphic_Height,get(inst->normimage,SCCA_Graphic_Height));
                                SC_SetAttrs(inst->normimage,SCCA_Graphic_Left,((struct SC_GraphicObject *) obj)->bounds.Left,
                                                        SCCA_Graphic_Top,((struct SC_GraphicObject *) obj)->bounds.Top);
                        }
                        break;

                  case SCCA_Icon_GraphicSelected:
                        if (inst->selimage)
                                SC_DisposeObject(inst->selimage);
                        inst->selimage = (Object *) tag->ti_Data;
                        if (!(inst->normimage))
                        {
                                SC_SetAttrs(obj,SCCA_Graphic_Width,get(inst->selimage,SCCA_Graphic_Width),
                                                        SCCA_Graphic_Height,get(inst->selimage,SCCA_Graphic_Height));
                        }
                        if (inst->selimage)
                        {
                                SC_SetAttrs(inst->selimage,SCCA_Graphic_Left,((struct SC_GraphicObject *) obj)->bounds.Left,
                                                        SCCA_Graphic_Top,((struct SC_GraphicObject *) obj)->bounds.Top);
                        }
                        break;

                  case SCCA_Icon_Selected:
                        inst->flags &= !ICONF_Selected;         // clear the bit
                        if (tag->ti_Data)
                                inst->flags |= ICONF_Selected;
                        break;

                  case SCCA_Icon_NoPosition:
                        inst->flags &= !ICONF_NoPosition;         // clear the bit
                        if (tag->ti_Data)
                                inst->flags |= ICONF_NoPosition;
                        break;

                  case SCCA_Icon_WindowAllFiles:
                        inst->flags &= !ICONF_WindowAllFiles;         // clear the bit
                        if (tag->ti_Data)
                                inst->flags |= ICONF_WindowAllFiles;
                        break;

                  case SCCA_Icon_Type:
                        inst->type = tag->ti_Data;
                        break;

                  case SCCA_Icon_Name:
                        if ((name = AllocCopyString((char *) tag->ti_Data)))
                        {
                                if (inst->name)
                                        FreeVec(inst->name);
                                inst->name = name;
                        }
                        break;

                  case SCCA_Icon_DefaultTool:
                        if ((name = AllocCopyString((char *) tag->ti_Data)))
                        {
                                if (inst->defaulttool)
                                        FreeVec(inst->defaulttool);
                                inst->defaulttool = name;
                        }
                        break;

                  case SCCA_Icon_StackSize:
                        inst->stacksize = tag->ti_Data;
                        break;

                  case SCCA_Icon_WindowLeft:
                        inst->windowleft = tag->ti_Data;
                        break;

                  case SCCA_Icon_WindowTop:
                        inst->windowtop = tag->ti_Data;
                        break;

                  case SCCA_Icon_WindowWidth:
                        inst->windowwidth = tag->ti_Data;
                        break;

                  case SCCA_Icon_WindowHeight:
                        inst->windowheight = tag->ti_Data;
                        break;

                  case SCCA_Icon_WindowOffsetX:
                        inst->windowoffsetx = tag->ti_Data;
                        break;

                  case SCCA_Icon_WindowOffsetY:
                        inst->windowoffsety = tag->ti_Data;
                        break;

                  case SCCA_Icon_WindowViewMode:
                        inst->windowviewmode = tag->ti_Data;
                        break;

                  case SCCA_Icon_RawIconType:
                        inst->rawtype = tag->ti_Data;
                        break;

                  case SCCA_Icon_ToolTypes:
                        if (inst->tooltypes)
                                SC_DisposeObject(inst->tooltypes);
                        inst->tooltypes = (Object *) tag->ti_Data;
                        break;

                  case SCCA_Graphic_Left:
                        if (inst->normimage)
                                SC_SetAttrs(inst->normimage,SCCA_Graphic_Left,tag->ti_Data,TAG_DONE);
                        if (inst->selimage)
                                SC_SetAttrs(inst->selimage,SCCA_Graphic_Left,tag->ti_Data,TAG_DONE);
                        break;

                  case SCCA_Graphic_Top:
                        if (inst->normimage)
                                SC_SetAttrs(inst->normimage,SCCA_Graphic_Top,tag->ti_Data,TAG_DONE);
                        if (inst->selimage)
                                SC_SetAttrs(inst->selimage,SCCA_Graphic_Top,tag->ti_Data,TAG_DONE);
                        break;
                }
        }
}
// /

/** Init a icon object
*/

static ULONG Icon_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct IconInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {
                SetTags(obj,inst,msg->ops_AttrList);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Set one or more setable attribute(s)
 */
static void Icon_Set( struct SC_Class *cl, Object *obj, struct opSet *msg, struct IconInst *inst )
{
        SetTags(obj,inst,msg->ops_AttrList);

        SC_DoSuperMethodA(cl,obj,(Msg) msg);
}
// /

/** Get one attribute of all of the getable attributes
*/
static ULONG Icon_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct IconInst *inst )
{
        if (msg->opg_AttrID == SCCA_Icon_Selected)
        {
                *(msg->opg_Storage) = (inst->flags & ICONF_Selected) ? TRUE : FALSE;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_NoPosition)
        {
                *(msg->opg_Storage) = (inst->flags & ICONF_NoPosition) ? TRUE : FALSE;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_WindowAllFiles)
        {
                *(msg->opg_Storage) = (inst->flags & ICONF_WindowAllFiles) ? TRUE : FALSE;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_IsList)
        {
                *(msg->opg_Storage) = FALSE;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_Name)
        {
                *(msg->opg_Storage) = (ULONG) inst->name;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_Type)
        {
                *(msg->opg_Storage) = inst->type;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_DefaultTool)
        {
                *(msg->opg_Storage) = (ULONG) inst->defaulttool;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_StackSize)
        {
                *(msg->opg_Storage) = inst->stacksize;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_WindowLeft)
        {
                *(msg->opg_Storage) = inst->windowleft;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_WindowTop)
        {
                *(msg->opg_Storage) = inst->windowtop;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_WindowWidth)
        {
                *(msg->opg_Storage) = inst->windowwidth;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_WindowHeight)
        {
                *(msg->opg_Storage) = inst->windowheight;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_WindowOffsetX)
        {
                *(msg->opg_Storage) = inst->windowoffsetx;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_WindowOffsetY)
        {
                *(msg->opg_Storage) = inst->windowoffsety;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_RawIconType)
        {
                *(msg->opg_Storage) = inst->rawtype;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_ToolTypes)
        {
                *(msg->opg_Storage) = (ULONG) inst->tooltypes;
                return(TRUE);
        }

        return(SC_DoSuperMethodA(cl, obj, (Msg) msg));
}
// /

/** exit routine
*/
static void Icon_Exit( struct SC_Class *cl, Object *obj, Msg msg, struct IconInst *inst )
{
        if (inst->normimage)
                SC_DisposeObject(inst->normimage);
        if (inst->selimage)
                SC_DisposeObject(inst->selimage);

        if (inst->tooltypes)
                SC_DisposeObject(inst->tooltypes);

        if (inst->name)
                FreeVec(inst->name);

        SC_DoSuperMethodA(cl,obj,msg);
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData IconMethods[] =
{
        { OM_SET,(ULONG)                        Icon_Set, 0, 0, NULL },               // derived from RootClass
        { OM_GET,(ULONG)                        Icon_Get, 0, 0, NULL },               // derived from RootClass
        { SCCM_Init,(ULONG)                     Icon_Init, 0, 0, NULL },              // derived from RootClass
        { SCCM_Graphic_PreThinkScreen,(ULONG)   Icon_PreThinkScreen, 0, 0, NULL },  // a new method
        { SCCM_Graphic_PreThinkWindow,(ULONG)   Icon_PreThinkWindow, 0, 0, NULL },  // a new method
        { SCCM_Graphic_Draw,(ULONG)             Icon_Draw, 0, 0, NULL },               // derived from AreaClass
        { SCCM_Graphic_PostThinkWindow,(ULONG)  Icon_PostThinkWindow, 0, 0, NULL },    // a new method
        { SCCM_Graphic_PostThinkScreen,(ULONG)  Icon_PostThinkScreen, 0, 0, NULL },   // a new method
        { SCCM_Exit,(ULONG)                     Icon_Exit, 0, 0, NULL }, // derived from RootClass

        { SCCM_Icon_Open, NULL, sizeof(struct SCCP_Icon_Open), 0, NULL },
        { SCCM_Icon_Close, NULL, sizeof(struct SCCP_Icon_Close), 0, NULL },
        { SCCM_Icon_Read, NULL, sizeof(struct SCCP_Icon_Read), 0, NULL },
        { SCCM_Icon_Write, NULL, sizeof(struct SCCP_Icon_Write), 0, NULL },
        { SCCM_Icon_OpenIcon, NULL, sizeof(struct SCCP_Icon_OpenIcon), 0, NULL },
        { SCCM_Icon_ReadIcon, NULL, sizeof(struct SCCP_Icon_ReadIcon), 0, NULL },
        { SCCM_Icon_CloseIcon, NULL, sizeof(struct SCCP_Icon_CloseIcon), 0, NULL },
        { SCCM_Icon_Entry, NULL, sizeof(struct SCCP_Icon_Entry), 0, NULL },
        { SCCM_Icon_GetObject, NULL, sizeof(struct SCCP_Icon_GetObject), 0, NULL },
        { SCCM_Icon_GetString, NULL, sizeof(struct SCCP_Icon_GetString), 0, NULL },
        { SCCM_Icon_GetIcon, (ULONG) Icon_GetIcon, sizeof(struct SCCP_Icon_GetIcon), 0, NULL },
        { SCCM_Icon_PopupMenu, (ULONG) Icon_PopupMenu, sizeof(struct SCCP_Icon_PopupMenu), 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

