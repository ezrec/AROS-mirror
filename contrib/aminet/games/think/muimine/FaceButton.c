/*
    MUI custom class for X-Mines Face Button
*/

#include "MUIMine.h"
#include "FaceButton.h"


/*
    Instance data
*/
struct FaceButtonData
{
    int     Width, Height;          // width and height of button image
    int     ImageIdx;               // index of image to show
    int     SelSaveImageIdx;        // saves the origional index during select
    ULONG   Flags;                  // Flags (see defines below)
    STRPTR  ImageFile;              // name of imagery picture file
    struct Screen * RenderScreen;   // screen bitmap was rendered to
    Object * ImageDTObject;         // picture data type object for imagery
    struct BitMap * ImageBM;        // bitmap containing face imagery
};

/*
    defines for FaceButtonData.Flags
*/
#define FBDF_NEWIMAGE       0x00000001L // the image file name has changed

/*
    defines for defaults 
*/
#define DEFAULT_IMAGEFILE (STRPTR)"def_FaceButtonImage"
#define DEFAULT_IMAGEDIR  (STRPTR)"Images"


/*
    defines for image indexes
*/
#define IMAGEIDX_NORMAL     MUIV_FaceButton_ImageIdx_Normal
#define IMAGEIDX_SELECTED   MUIV_FaceButton_ImageIdx_Selected
#define IMAGEIDX_OH         MUIV_FaceButton_ImageIdx_Oh
#define IMAGEIDX_GOOD       MUIV_FaceButton_ImageIdx_Good
#define IMAGEIDX_BAD        MUIV_FaceButton_ImageIdx_Bad

#define IMAGE_COUNT         5


/*
    other defines
*/



/*
    private function prototypes
*/
static void DeleteImageFileName(struct FaceButtonData * data);
static void SetImageFileName(struct FaceButtonData * data, STRPTR newname);
BOOL InitFaceButtonImageBM(struct FaceButtonData * data, struct Screen * rscreen);
void FreeFaceButtonImageBM(struct FaceButtonData * data);



/*
    function :    OM_NEW method handler for FaceButton class
*/
static ULONG mNew(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct FaceButtonData *data;
    STRPTR fname;

    if (!(obj = (Object *)DoSuperMethodA(cl, obj, (APTR)msg)))
    {
        return 0;
    }

    data = INST_DATA(cl, obj);

    data->Height = data->Width = 0;
    data->ImageIdx = data->SelSaveImageIdx = 0;
    data->Flags = 0;
    data->RenderScreen = NULL;
    data->ImageFile = NULL;
    data->ImageDTObject = NULL;
    data->ImageBM = NULL;

    fname = (STRPTR)GetTagData(MUIA_FaceButton_ImageFile, NULL,
                               msg->ops_AttrList);
    SetImageFileName(data, fname);

    return (ULONG)obj;
}


/*
    function :    OM_DELETE method handler for FaceButton class
*/
static ULONG mDispose(struct IClass *cl, Object *obj, Msg msg)
{
    struct FaceButtonData *data = INST_DATA(cl, obj);

    FreeFaceButtonImageBM(data);
    DeleteImageFileName(data);

    return DoSuperMethodA(cl, obj, msg);
}


/*
    function :    MUIM_Setup method handler for FaceButton class
*/
static ULONG mSetup(struct IClass *cl, Object *obj, struct MUIP_Setup * msg)
{
    struct FaceButtonData *data = INST_DATA(cl, obj);

    if (!(DoSuperMethodA(cl, obj, (APTR)msg)))
    {
        return FALSE;
    }

    if (!InitFaceButtonImageBM(data, msg->RenderInfo->mri_Screen))
    {
        CoerceMethod(cl, obj, MUIM_Cleanup);
        return FALSE;
    }

    MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS);

    return TRUE;
}


/*
    function :    MUIM_Cleanup method handler for FaceButton class
*/
static ULONG mCleanup(struct IClass *cl, Object *obj, Msg msg)
{
//    struct FaceButtonData *data = INST_DATA(cl, obj);

    MUI_RejectIDCMP(obj, IDCMP_MOUSEBUTTONS);

    return(DoSuperMethodA(cl, obj, msg));
}

/*
    function :    MUIM_AskMinMax method handler for FaceButton class
*/
static ULONG mAskMinMax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
    struct FaceButtonData *data = INST_DATA(cl, obj);

    /*
    ** let our superclass first fill in what it thinks about sizes.
    ** this will e.g. add the size of frame and inner spacing.
    */

    DoSuperMethodA(cl, obj, (APTR)msg);

    /*
    ** now add the values specific to our object. note that we
    ** indeed need to *add* these values, not just set them!
    */

    msg->MinMaxInfo->MinWidth  += data->Width;
    msg->MinMaxInfo->DefWidth  += data->Width;
    msg->MinMaxInfo->MaxWidth  += data->Width;

    msg->MinMaxInfo->MinHeight += data->Height;
    msg->MinMaxInfo->DefHeight += data->Height;
    msg->MinMaxInfo->MaxHeight += data->Height;

    return 0;
}


/*
    function :    OM_SET method handler for FaceButton class
*/
static ULONG mSet(struct IClass *cl, Object *obj, struct opSet * msg)
{
    struct FaceButtonData *data = INST_DATA(cl,obj);
    struct TagItem *tags, *tag;

    for (tags = msg->ops_AttrList; tag = NextTagItem(&tags); )
    {
        switch (tag->ti_Tag)
        {
            case MUIA_FaceButton_ImageFile:
                SetImageFileName(data, (STRPTR)tag->ti_Data);
                break;

            case MUIA_FaceButton_ImageIdx:
                if (data->ImageIdx != (int)tag->ti_Data)
                {
                    data->ImageIdx = (int)tag->ti_Data;
                    MUI_Redraw(obj, MADF_DRAWOBJECT);
                }
                break;
 
            case MUIA_Selected:
                if (tag->ti_Data)
                {
                    data->SelSaveImageIdx = data->ImageIdx;
                    data->ImageIdx = IMAGEIDX_SELECTED;
                }
                else
                {
                    data->ImageIdx = data->SelSaveImageIdx;
                }
                MUI_Redraw(obj, MADF_DRAWOBJECT);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (APTR)msg);
}


/*
    function :    MUIM_Draw method handler for FaceButton class
*/
static ULONG mDraw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct FaceButtonData *data = INST_DATA(cl, obj);

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
        WORD bx;

        /*
            determine the co-ordinates of the image in the bit map
        */
        bx = (WORD)((data->ImageIdx >= 0  && data->ImageIdx <= IMAGE_COUNT)
                            ? data->Width * data->ImageIdx : 0);

        /*
            blit the image from the bitmap to the rastport
        */
        BltBitMapRastPort(data->ImageBM, bx, 0,
                          _rp(obj), (WORD)(_mleft(obj)), (WORD)(_top(obj)),
                          (WORD)data->Width, (WORD)data->Height,
                          (UBYTE)0xC0);
    }

    return 0;
}


#ifndef __AROS__
SAVEDS ASM ULONG FaceButtonDispatcher(
        REG(a0) struct IClass *cl,
        REG(a2) Object *obj,
        REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, FaceButtonDispatcher,
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
        case MUIM_Cleanup    : return    mCleanup(cl, obj, (APTR)msg);
        case MUIM_AskMinMax  : return  mAskMinMax(cl, obj, (APTR)msg);
        case MUIM_Draw       : return       mDraw(cl, obj, (APTR)msg);
    }

    return DoSuperMethodA(cl, obj, msg);

    AROS_USERFUNC_EXIT
}



/*
    function :    frees the memory used for the image file name

    parameters :  data = pointer to the FaceButtonData
*/
static void DeleteImageFileName(struct FaceButtonData * data)
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

    parameters :  data = pointer to the FaceButtonData
                  newname = the new image file name or NULL to clear the
                            existing file name and use the default image

*/
static void SetImageFileName(struct FaceButtonData * data, STRPTR newname)
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

    data->Flags |= FBDF_NEWIMAGE;
}


/*
    function :    loads the bitmap used for rendering the button faces

    parameters :  data = pointer to the FaceButton data to initialize bitmap for
                  rscreen = pointer to the screen to render the bitmap to
                  fname = name of the image file

    return :      TRUE if bitmap initialized ok, FALSE if an error occured
*/
BOOL LoadFaceButtonImageBM(struct FaceButtonData * data,
                           struct Screen * rscreen,
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
    function :    initialize the bitmap used for rendering the button faces

    parameters :  data = pointer to the FaceButton data to initialize bitmap for
                  rscreen = pointer to the screen to render the bitmap to

    return :      TRUE if bitmap initialized ok, FALSE if an error occured
*/
BOOL InitFaceButtonImageBM(struct FaceButtonData * data, struct Screen * rscreen)
{
    BOOL rc = TRUE;
    if (data->RenderScreen == NULL  ||  rscreen != data->RenderScreen  ||
        (data->Flags & FBDF_NEWIMAGE) != 0)
    {
        data->Flags &= ~FBDF_NEWIMAGE;
        FreeFaceButtonImageBM(data);
        rc = LoadFaceButtonImageBM(data, rscreen, (data->ImageFile)
                                                        ? data->ImageFile
                                                        : DEFAULT_IMAGEFILE);
        if (!rc  &&  data->ImageFile)
        {
            rc = LoadFaceButtonImageBM(data, rscreen, DEFAULT_IMAGEFILE);
        }
    }

    return rc;
}

/*
    function :    frees bitmap data allocated by InitFaceButtonImageBM()

    parameters :  data = pointer to the FaceButton data to free bitmap for

    return :      none
*/
void FreeFaceButtonImageBM(struct FaceButtonData * data)
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
    function :    creates the FaceButton MUI custom class

    return :      pointer to the created custom class or NULL
*/
struct MUI_CustomClass * CreateFaceButtonClass()
{
    return MUI_CreateCustomClass(NULL, MUIC_Area, NULL,
                                       sizeof(struct FaceButtonData),
                                       FaceButtonDispatcher);
}

/*
    function :    deletes of the FaceButton custom class

    parameters :  mcc = pointer to the FaceButton MUI_CustomClass to delete
*/
void DeleteFaceButtonClass(struct MUI_CustomClass * mcc)
{
    MUI_DeleteCustomClass(mcc);
}

