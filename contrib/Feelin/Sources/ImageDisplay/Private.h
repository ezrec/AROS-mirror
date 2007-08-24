#include <stdlib.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <proto/feelin.h>

#include <exec/memory.h>
#include <datatypes/pictureclass.h>
#include <intuition/intuitionbase.h>
#include <cybergraphx/cybergraphics.h>
#include <libraries/feelin.h>

#define FV_DTVERSION 39

#define DOSBase                     FeelinBase -> DOS
#define GfxBase                     FeelinBase -> Graphics
#define IntuitionBase               FeelinBase -> Intuition
#define UtilityBase                 FeelinBase -> Utility
#define LayersBase                  FeelinBase -> Layers

extern struct ClassUserData        *CUD;

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    { // autos

        FM_BitMap_Render,
        FM_BitMap_Blit,

        FA_BitMap_TargetScreen,
        FA_BitMap_TargetWidth,
        FA_BitMap_TargetHeight,
        FA_BitMap_RenderedWidth,
        FA_BitMap_RenderedHeight,

        FA_Display_Screen

        };

/****************************************************************************
*** Types *******************************************************************
****************************************************************************/

struct in_DrawMessage
{
    FRender                        *Render;
    FBox                           *Origin;
    FRect                          *Rect;
    bits32                          Flags;
};

typedef APTR (in_Code_Create)                   (STRPTR Spec);
typedef APTR (in_Code_CreateDouble)             (STRPTR RSpec,STRPTR SSpec);
typedef void (in_Code_Delete)                   (APTR Image);
typedef int32 (in_Code_Setup)                   (APTR Image,FClass *Class, FRender *Render);
typedef void (in_Code_Cleanup)                  (APTR Image,FClass *Class, FRender *Render);
typedef int32 (in_Code_Get)                     (APTR Image,uint32 Attribute);
typedef int32 (in_Code_Draw)                    (APTR Image,FClass *Class,struct in_DrawMessage *Msg);

/* Except for 'delete', ALL members MUST be filled !! */
 
struct in_CodeTable
{
    in_Code_Create                 *create;
    in_Code_Delete                 *delete;
    in_Code_Setup                  *setup;
    in_Code_Cleanup                *cleanup;
    in_Code_Get                    *get;
    in_Code_Draw                   *draw;
};

/* use the following macros to create image type functions */

#define F_CODE_CREATE(name)                     APTR name(STRPTR Spec)
#define F_CODE_CREATE_XML(name)                 APTR name(FXMLAttribute *Attribute)
#define F_CODE_CREATE_DOUBLE(name)              APTR name(STRPTR RSpec,STRPTR SSpec)
#define F_CODE_DELETE(name)                     void name(FImage *image)
#define F_CODE_SETUP(name)                      int32 name(FImage *image,FClass *Class,FRender *Render)
#define F_CODE_CLEANUP(name)                    void name(FImage *image,FClass *Class,FRender *Render)
#define F_CODE_GET(name)                        int32 name(FImage *image,uint32 Attribute)
#define F_CODE_DRAW(name)                       int32 name(FImage *image,FClass *Class,struct in_DrawMessage *Msg)

struct in_Double
{
   struct in_CodeTable             *Methods[2];
   APTR                             Data[2];
};

/****************************************************************************
*** Class *******************************************************************
****************************************************************************/

struct ClassUserData
{
    struct Library                 *DTBase;
    struct Library                 *CGBase;

    APTR                            Pool;
    struct FeelinSharedBrush       *SharedBrushes;
    FObject                         XMLDocument;
    FDynamicEntry                  *IDs;
};

enum    {

        FA_Document_Source,
        FA_Document_SourceType,
        FM_Document_AddIDs,
        FM_Document_Resolve,
        FA_XMLDocument_Markups

        };

#define DataTypesBase                           CUD -> DTBase
#define CyberGfxBase                            CUD -> CGBase

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    bits32                          Flags;
    FRender                        *Render;
    FBox                           *Origin;

    struct in_CodeTable            *Methods;
    APTR                            Data;
};

/* WARNING!! 'Image' can also be a raster image e.g. FI_Shine */

#define FF_IMAGE_SELECTED                       (1 << 0)

enum    {

        FV_XML_ID_IMAGE = FV_XMLDOCUMENT_ID_DUMMY,
        FV_XML_ID_TYPE,

        FV_XML_ID_RASTER,
        FV_XML_ID_ID,

        FV_XML_ID_GRADIENT,
        FV_XML_ID_ANGLE,
        FV_XML_ID_START,
        FV_XML_ID_MIDDLE,
        FV_XML_ID_END,
        FV_XML_ID_FIRST_START,
        FV_XML_ID_FIRST_END,
        FV_XML_ID_SECOND_START,
        FV_XML_ID_SECOND_END,

        FV_XML_ID_BRUSH,
        FV_XML_ID_SRC,

        FV_XML_ID_PICTURE,
        FV_XML_ID_MODE,
        FV_XML_ID_FILTER,

        FV_XML_ID_HOOK

        };

/*** Prototypes ************************************************************/

extern struct in_CodeTable          id_double_table;
extern struct in_CodeTable          id_raster_table;
extern struct in_CodeTable          id_color_table;
extern struct in_CodeTable          id_gradient_table;
extern struct in_CodeTable          id_brush_table;
extern struct in_CodeTable          id_picture_table;
extern struct in_CodeTable          id_hook_table;

void id_image_dispose(struct in_CodeTable *Table,APTR Data);
APTR id_image_new(STRPTR Spec,struct in_CodeTable **TablePtr);
