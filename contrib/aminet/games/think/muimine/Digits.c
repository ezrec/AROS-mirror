/*
    MUI custom class for X-Mines 3-digit display
*/

#include "MUIMine.h"
#include "Digits.h"


/*
    Instance data
*/
struct DigitsData
{
    int     Width, Height;          // width and height of a digit
    int     Number;                 // number displayed
    BYTE    Digits[4];              // image index and flags for digits
    ULONG   Flags;                  // flags (see defines below)
    STRPTR  ImageFile;              // name of imagery picture file
    struct Screen * RenderScreen;   // screen bitmap was rendered to
    Object * ImageDTObject;         // picture data type object for imagery
    struct BitMap * ImageBM;        // bitmap containing face imagery
};

/*
    defines for DigitData.Flags
*/
#define DDF_NEWIMAGE        0x00000001L // image file has been changed

/*
    Digits flags
*/
#define DF_REDRAW       0x80    // digit need redrawing


/*
    defines for defaults 
*/
#define DEFAULT_IMAGEFILE (STRPTR)"def_DigitsImage"


/*
    defines for image indexes
*/
#define IMAGEIDX_BLANK      0
#define IMAGEIDX_DIGIT(n)   ((n) + 1)
#define IMAGEIDX_DASH       11

#define IMAGE_COUNT         12


/*
    other defines
*/



/*
    private function prototypes
*/
static void DeleteImageFileName(struct DigitsData * data);
static void SetImageFileName(struct DigitsData * data, STRPTR newname);
BOOL InitDigitsImageBM(struct DigitsData * data, struct Screen * rscreen);
void FreeDigitsImageBM(struct DigitsData * data);

BOOL SetNumber(struct DigitsData * data, int newNumber);


/*
    function :    OM_NEW method handler for Digits class
*/
static ULONG mNew(struct IClass *cl, Object *obj, struct opSet *msg)
{
    int i;
    struct DigitsData *data;
    STRPTR fname;

    if (!(obj = (Object *)DoSuperMethodA(cl, obj, (APTR)msg)))
    {
        return 0;
    }

    data = INST_DATA(cl, obj);

    data->Height = data->Width = 0;
    for (i = 0; i < 4; i++) data->Digits[i] = 0;
    data->Flags = 0;
    data->RenderScreen = NULL;
    data->ImageFile = NULL;
    data->ImageDTObject = NULL;
    data->ImageBM = NULL;
    SetNumber(data, 0);

    fname = (STRPTR)GetTagData(MUIA_Digits_ImageFile, NULL,
                               msg->ops_AttrList);
    SetImageFileName(data, fname);

    return (ULONG)obj;
}


/*
    function :    OM_DELETE method handler for Digits class
*/
static ULONG mDispose(struct IClass *cl, Object *obj, Msg msg)
{
    struct DigitsData *data = INST_DATA(cl, obj);

    FreeDigitsImageBM(data);
    DeleteImageFileName(data);

    return DoSuperMethodA(cl, obj, msg);
}


/*
    function :    MUIM_Setup method handler for Digits class
*/
static ULONG mSetup(struct IClass *cl, Object *obj, struct MUIP_Setup * msg)
{
    struct DigitsData *data = INST_DATA(cl, obj);

    if (!(DoSuperMethodA(cl, obj, (APTR)msg)))
    {
        return FALSE;
    }

    if (!InitDigitsImageBM(data, msg->RenderInfo->mri_Screen))
    {
        CoerceMethod(cl, obj, MUIM_Cleanup);
        return FALSE;
    }

    return TRUE;
}


/*
    function :    MUIM_Cleanup method handler for Digits class
*/
//static ULONG mCleanup(struct IClass *cl, Object *obj, Msg msg)
//{
//    struct DigitsData *data = INST_DATA(cl, obj);

//    return(DoSuperMethodA(cl, obj, msg));
//}

/*
    function :    MUIM_AskMinMax method handler for Digits class
*/
static ULONG mAskMinMax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
    int x, y;
    struct DigitsData *data = INST_DATA(cl, obj);

    /*
    ** let our superclass first fill in what it thinks about sizes.
    ** this will e.g. add the size of frame and inner spacing.
    */

    DoSuperMethodA(cl, obj, (APTR)msg);

    /*
    ** now add the values specific to our object. note that we
    ** indeed need to *add* these values, not just set them!
    */

    x = data->Width * 3;
    y = data->Height;

    msg->MinMaxInfo->MinWidth  += x;
    msg->MinMaxInfo->DefWidth  += x;
    msg->MinMaxInfo->MaxWidth  += x;

    msg->MinMaxInfo->MinHeight += y;
    msg->MinMaxInfo->DefHeight += y;
    msg->MinMaxInfo->MaxHeight += y;

    return 0;
}


/*
    function :    OM_SET method handler for Digits class
*/
static ULONG mSet(struct IClass *cl, Object *obj, struct opSet * msg)
{
    struct DigitsData *data = INST_DATA(cl,obj);
    struct TagItem *tags, *tag;

    for (tags = msg->ops_AttrList; tag = NextTagItem(&tags); )
    {
        switch (tag->ti_Tag)
        {
            case MUIA_Digits_ImageFile:
                SetImageFileName(data, (STRPTR)tag->ti_Data);
                break;

            case MUIA_Digits_Number:
                if (data->Number != (int)tag->ti_Data)
                {
                    if (SetNumber(data, (int)tag->ti_Data))
                    {
                        MUI_Redraw(obj, MADF_DRAWUPDATE);
                    }
                }
                break;
 
        }
    }

    return DoSuperMethodA(cl, obj, (APTR)msg);
}


/*
    function :    MUIM_Draw method handler for Digits class
*/
static ULONG mDraw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    WORD bx;
    int i, l, t, d, w, h;
    struct DigitsData *data = INST_DATA(cl, obj);

    w = data->Width;
    h = data->Height;
    l = (_mright(obj) + _mleft(obj) + 1 - 3 * w) / 2;
    t = (_mbottom(obj) + _mtop(obj) + 1 - h) / 2;

    /*
    ** let our superclass draw itself first, area class would
    ** e.g. draw the frame and clear the whole region. What
    ** it does exactly depends on msg->flags.
    */

    DoSuperMethodA(cl,obj,(APTR)msg);

    /*
    ** only re-draw if MADF_DRAWOBJECT is set
    */

    if (msg->flags & MADF_DRAWOBJECT)
    {
        for (i = 0; i < 3; i++)
        {
            d = data->Digits[i] &= ~DF_REDRAW;
            /*
                determine the co-ordinates of the image in the bit map
            */
            bx = (WORD)((d >= 0  && d <= IMAGE_COUNT) ? w * d : 0);

            /*
                blit the image from the bitmap to the rastport
            */
            BltBitMapRastPort(data->ImageBM, bx, 0, _rp(obj),
                              (WORD)(l + w * i), (WORD)t,
                              (WORD)w, (WORD)h,
                              (UBYTE)0xC0);
        }
    }
    else if (msg->flags & MADF_DRAWUPDATE)
    {
        for (i = 0; i < 3; i++)
        {
            if ((data->Digits[i] & DF_REDRAW) != 0)
            {
                d = data->Digits[i] &= ~DF_REDRAW;
                /*
                    determine the co-ordinates of the image in the bit map
                */
                bx = (WORD)((d >= 0  && d <= IMAGE_COUNT) ? w * d : 0);

                /*
                    blit the image from the bitmap to the rastport
                */
                BltBitMapRastPort(data->ImageBM, bx, 0, _rp(obj),
                                  (WORD)(l + w * i), (WORD)t,
                                  (WORD)w, (WORD)h,
                                  (UBYTE)0xC0);
            }
        }
    }
    return 0;
}


#ifndef __AROS__
SAVEDS ASM ULONG DigitsDispatcher(
        REG(a0) struct IClass *cl,
        REG(a2) Object *obj,
        REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, DigitsDispatcher,
 AROS_UFHA(struct IClass *, cl , A0),
 AROS_UFHA(Object *       , obj, A2),
 AROS_UFHA(Msg            , msg, A1))
#endif
{
    AROS_USERFUNC_INIT

    switch (msg->MethodID)
    {
        case OM_NEW          : return        mNew(cl, obj, (APTR)msg);
        case OM_DISPOSE      : return    mDispose(cl, obj, (APTR)msg);
        case OM_SET          : return        mSet(cl, obj, (APTR)msg);
        case MUIM_Setup      : return      mSetup(cl, obj, (APTR)msg);
//        case MUIM_Cleanup    : return    mCleanup(cl, obj, (APTR)msg);
        case MUIM_AskMinMax  : return  mAskMinMax(cl, obj, (APTR)msg);
        case MUIM_Draw       : return       mDraw(cl, obj, (APTR)msg);
    }

    return DoSuperMethodA(cl, obj, msg);

    AROS_USERFUNC_EXIT
}


/*
    function :    frees the memory used for the image file name

    parameters :  data = pointer to the DigitsData
*/
static void DeleteImageFileName(struct DigitsData * data)
{
    if (data->ImageFile != NULL)
    {
        FreeVec(data->ImageFile);
        data->ImageFile = NULL;
    }
}


/*
    function :    delete any existing image file name, sets the given new
                  image file name and sets the new image flag so the new
                  image file is used next time the window is opened

    parameters :  data = pointer to the DigitsData
                  newname = the new image file name or NULL to clear the
                            existing file name and use the default image

*/
static void SetImageFileName(struct DigitsData * data, STRPTR newname)
{
    DeleteImageFileName(data);

    if (newname)
    {
        int l = strlen(newname) + 1;
        data->ImageFile = (STRPTR)AllocVec(l, 0);
        if (data->ImageFile)
        {
            strcpy(data->ImageFile, newname);
        }
    }

    data->Flags |= DDF_NEWIMAGE;
}


/*
    function :    loads the bitmap used for rendering the button faces

    parameters :  data = pointer to the FaceButton data to initialize bitmap for
                  rscreen = pointer to the screen to render the bitmap to
                  fname = name of the image file

    return :      TRUE if bitmap initialized ok, FALSE if an error occured
*/
BOOL LoadDigitsImageBM(struct DigitsData * data, struct Screen * rscreen,
                           STRPTR fname)
{
    struct LoadBitMapData lbmdata;
    int rc;

    lbmdata.FileName = fname;
    lbmdata.Screen = rscreen;

    rc = LoadBitMap(&lbmdata);
    if (rc == LBMERR_NONE)
    {
        if ((lbmdata.BitMapHeader->bmh_Width % IMAGE_COUNT) == 0)
        {
            data->Width  = lbmdata.BitMapHeader->bmh_Width / IMAGE_COUNT;
            data->Height = lbmdata.BitMapHeader->bmh_Height;
            data->RenderScreen = rscreen;
            data->ImageDTObject = lbmdata.DTObject;
            data->ImageBM = lbmdata.BitMap;
            return TRUE;
        }
        else
        {
            DisposeDTObject(lbmdata.DTObject);
        }
    }
    return FALSE;
}

/*
    function :    initialize the bitmap used for rendering the digits

    parameters :  data = pointer to the FaceButton data to initialize bitmap for
                  rscreen = pointer to the screen to render the bitmap to

    return :      TRUE if bitmap initialized ok, FALSE if an error occured
*/
BOOL InitDigitsImageBM(struct DigitsData * data, struct Screen * rscreen)
{
    BOOL rc = TRUE;
    if (data->RenderScreen == NULL  ||  rscreen != data->RenderScreen  ||
        (data->Flags & DDF_NEWIMAGE) != 0)
    {
        data->Flags &= ~DDF_NEWIMAGE;
        FreeDigitsImageBM(data);
        rc = LoadDigitsImageBM(data, rscreen, (data->ImageFile)
                                                        ? data->ImageFile
                                                        : DEFAULT_IMAGEFILE);
        if (!rc  &&  data->ImageFile)
        {
            rc = LoadDigitsImageBM(data, rscreen, DEFAULT_IMAGEFILE);
        }
    }

    return rc;
}


/*
    function :    frees bitmap data allocated by InitDigitsImageBM()

    parameters :  data = pointer to the Digits data to free bitmap for

    return :      none
*/
void FreeDigitsImageBM(struct DigitsData * data)
{
    WaitBlit();
    if (data->ImageDTObject)
    {
        DisposeDTObject(data->ImageDTObject);
        data->ImageDTObject = NULL;
    }
    data->RenderScreen = NULL;
    data->ImageBM = NULL;
}


/*
    function :    sets a new number to display

    Parameters :  data = pointer to the Digits data
                  newNumber = new number to display

    return :      TRUE if the display needs redrawing
*/
BOOL SetNumber(struct DigitsData * data, int newNumber)
{
    BOOL ret = FALSE;
    BYTE dig[4];
    int i;

    /*
        update the current number to the new number
    */
    data->Number = newNumber;

    /*
        check for special inputs
    */
    if (newNumber == MUIV_Digits_Number_Blanked)
    {
        /*
            fill the digits with blanks
        */
        dig[0] = dig[1] = dig[2] = IMAGEIDX_BLANK;
    }
    else if (newNumber == MUIV_Digits_Number_Dashed)
    {
        /*
            fill the digits with dashes
        */
        dig[0] = dig[1] = dig[2] = IMAGEIDX_DASH;
    }
    else
    {
        /*
            treat as a number
        */
        BOOL negative;
        /*
            check for negative number
        */
        negative = (newNumber < 0);
        if (negative)
        {
            newNumber = -newNumber;
        }

        /*
            determine the digit images to use
        */
        dig[0] = dig[1] = IMAGEIDX_BLANK;           // leading blanks
        dig[2] = IMAGEIDX_DIGIT(newNumber % 10);    // right most digit (LSD)
        newNumber /= 10;
        if (newNumber)
        {
            dig[1] = IMAGEIDX_DIGIT(newNumber % 10);    // middle digit
            newNumber /= 10;
            if (newNumber)
            {
                dig[0] = IMAGEIDX_DIGIT(newNumber % 10);    // left most digit
            }
        }

        /*
            put in the 'minus' if the number was negative
        */
        if (negative)
        {
            if (dig[1] == 0)
            {
                dig[1] = IMAGEIDX_DASH;
            }
            else
            {
                dig[0] = IMAGEIDX_DASH;
            }
        }
    }

    /*
        compare new digits with old digits and update when different
    */
    for (i = 0; i < 3; i++)
    {
        if ((data->Digits[i] & ~DF_REDRAW) != dig[i])
        {
            data->Digits[i] = dig[i] | DF_REDRAW;
            ret = TRUE;
        }
    }

    return ret;
}


/*
    function :    creates the Digits MUI custom class

    return :      pointer to the created custom class or NULL
*/
struct MUI_CustomClass * CreateDigitsClass()
{
    return MUI_CreateCustomClass(NULL, MUIC_Area, NULL,
                                       sizeof(struct DigitsData),
                                       DigitsDispatcher);
}

/*
    function :    deletes of the Digits custom class

    parameters :  mcc = pointer to the Digits MUI_CustomClass to delete
*/
void DeleteDigitsClass(struct MUI_CustomClass * mcc)
{
    MUI_DeleteCustomClass(mcc);
}

