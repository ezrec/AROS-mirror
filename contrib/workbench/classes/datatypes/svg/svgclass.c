/*
    Copyright © 1995-2005, The AROS Development Team. All rights reserved.
    $Id$
*/

/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dostags.h>
#include <graphics/gfxbase.h>
#include <graphics/rpattr.h>
#include <cybergraphx/cybergraphics.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/cghooks.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/iffparse.h>
#include <proto/datatypes.h>

#include <aros/symbolsets.h>

#include <svgtiny.h>

#include "debug.h"
#include "methods.h"
#include "svg_intern.h"

/**************************************************************************************************/

static BOOL LoadSVG(struct IClass *cl, Object *o)
{
    struct SVG_Data *sd = (struct SVG_Data *)INST_DATA(cl,o);
    union {
        struct IFFHandle   *iff;
        BPTR                bptr;
    } filehandle;
    UWORD                   width, height;
    IPTR                    sourcetype;
    struct svgtiny_diagram  *svg;

    D(bug("svg.datatype/LoadSVG()\n"));

    if( GetDTAttrs(o,   DTA_SourceType    , (IPTR)&sourcetype ,
                        DTA_Handle        , (IPTR)&filehandle,
                        TAG_DONE) != 2 )
    {
        return FALSE;
    }
    
    if ( sourcetype == DTST_RAM && filehandle.iff == NULL )
    {
        D(bug("svg.datatype/LoadSVG(): Creating an empty object\n"));
        return TRUE;
    }
    if ( sourcetype != DTST_FILE || !filehandle.bptr )
    {
        D(bug("svg.datatype/LoadSVG(): unsupported mode\n"));
        return FALSE;
    }

    /* We always parse into a 'virtual' 256x256 window. 
     * The SVG will correct us if we're wrong.
     */
    width = height = 256;

    if ((svg = svgtiny_create())) {
        LONG len;
        UBYTE *buff;

        Seek(filehandle.bptr, 0, OFFSET_END);
        len = Seek(filehandle.bptr, 0, OFFSET_BEGINNING);

        if (len > 0) {
            buff = AllocMem(len, MEMF_ANY);
            if (buff) {
                if (Read(filehandle.bptr, buff, len) == len) {
                    svgtiny_code err;
                    err = svgtiny_parse(svg, buff, len, "file://", width, height);
                    FreeMem(buff, len);

                    if (err == svgtiny_OK) {
                        sd->diagram = svg;
                        /* Pass picture size to datatype */
                        SetDTAttrs(o, NULL, NULL, DTA_NominalHoriz, svg->width,
                                                  DTA_NominalVert , svg->height,
                                                  TAG_DONE);

                        D(bug("svg.datatype/LoadSVG(): Normal Exit - %dx%d, %d shapes\n", svg->width, svg->height, svg->shape_count));
                        return TRUE;
                    }

                    D(bug("svg.datatype/LoadSVG(): Error %d\n", err));
                }
                FreeMem(buff, len);
            }
        }
        svgtiny_free(svg);
    }

    D(bug("svg.datatype/LoadSVG(): Failed Exit\n"));
    return TRUE;
}

/* OM_NEW - Load a new SVG object
 */
IPTR SVG__OM_NEW(Class *cl, Object *o, Msg msg)
{
    Object *newobj;
    
    D(bug("svg.datatype/DT_Dispatcher: Method OM_NEW\n"));
    newobj = (Object *)DoSuperMethodA(cl, o, (Msg)msg);
    if (newobj)
    {
        if (!LoadSVG(cl, newobj))
        {
            D(bug("svg.datatype/OM_NEW: Can't load SVG\n", __func__));
            CoerceMethod(cl, newobj, OM_DISPOSE);
            newobj = NULL;
        }
    }
    
    return (IPTR)newobj;
}

/*
 * OM_DISPOSE - Get rid of a SVG object
 */
IPTR SVG__OM_DISPOSE(Class *cl, Object *o, Msg msg)
{
    struct SVG_Data *sd = INST_DATA(cl, o);

    D(bug("svg.datatype/OM_DISPOSE\n"));
    svgtiny_free(sd->diagram);

    return DoSuperMethodA(cl, o, msg);
}

/* cairo-based rendering.
 * Would have loved to use graphics.library, but the Area*
 * functions lack a Bezier curve method.
 */
#include <cairo/cairo.h>
#include <cairo/cairo-aros.h>

/**
 * Render an svgtiny path using cairo.
 */
static void render_path(cairo_t *cr, float scale, struct svgtiny_shape *path)
{
    unsigned int j;

    cairo_new_path(cr);
    for (j = 0; j != path->path_length; ) {
        switch ((int) path->path[j]) {
        case svgtiny_PATH_MOVE:
            cairo_move_to(cr,
                            scale * path->path[j + 1],
                            scale * path->path[j + 2]);
            j += 3;
            break;
        case svgtiny_PATH_CLOSE:
            cairo_close_path(cr);
            j += 1;
            break;
        case svgtiny_PATH_LINE:
            cairo_line_to(cr,
                            scale * path->path[j + 1],
                            scale * path->path[j + 2]);
            j += 3;
            break;
        case svgtiny_PATH_BEZIER:
            cairo_curve_to(cr,
                            scale * path->path[j + 1],
                            scale * path->path[j + 2],
                            scale * path->path[j + 3],
                            scale * path->path[j + 4],
                            scale * path->path[j + 5],
                            scale * path->path[j + 6]);
            j += 7;
            break;
        default:
            printf("error ");
            j += 1;
        }
    }
    if (path->fill != svgtiny_TRANSPARENT) {
        cairo_set_source_rgb(cr,
                        svgtiny_RED(path->fill) / 255.0,
                        svgtiny_GREEN(path->fill) / 255.0,
                        svgtiny_BLUE(path->fill) / 255.0);
        cairo_fill_preserve(cr);
    }
    if (path->stroke != svgtiny_TRANSPARENT) {
        cairo_set_source_rgb(cr,
                        svgtiny_RED(path->stroke) / 255.0,
                        svgtiny_GREEN(path->stroke) / 255.0,
                        svgtiny_BLUE(path->stroke) / 255.0);
        cairo_set_line_width(cr, scale * path->stroke_width);
        cairo_stroke_preserve(cr);
    }
}

static BOOL cairo_render(struct svgtiny_diagram *diagram, float scale, struct RastPort *rp, UWORD xoff, UWORD yoff, UWORD width, UWORD height)
{
    cairo_surface_t *surface;
    cairo_t *cr;
    cairo_status_t status;
    
    unsigned int i;

    surface = cairo_aros_surface_create(rp, xoff, yoff, width, height);
    if (!surface) {
            D(bug("cairo_aros_surface_create: %p\n", surface));
            return FALSE;
    }

    cr = cairo_create(surface);
    status = cairo_status(cr);
    if (status != CAIRO_STATUS_SUCCESS) {
            cairo_destroy(cr);
            cairo_surface_destroy(surface);
            D(bug("cairo_create: %d\n", status));
            return FALSE;
    }

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    D(bug("svg.datatype/Processing %d shapes\n", diagram->shape_count));
    for (i = 0; i != diagram->shape_count; i++) {
        if (diagram->shape[i].path) {
            render_path(cr, scale, &diagram->shape[i]);

        } else if (diagram->shape[i].text) {
            cairo_set_source_rgb(cr,
                    svgtiny_RED(diagram->shape[i].stroke) / 255.0,
                    svgtiny_GREEN(diagram->shape[i].stroke) / 255.0,
                    svgtiny_BLUE(diagram->shape[i].stroke) / 255.0);
            cairo_move_to(cr,
                            scale * diagram->shape[i].text_x,
                            scale * diagram->shape[i].text_y);
            cairo_show_text(cr, diagram->shape[i].text);
        }
    }

    status = cairo_status(cr);
    if (status != CAIRO_STATUS_SUCCESS) {
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        D(bug("cairo_status: %d\n", status));
        return FALSE;
    }

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return TRUE;
}

/*
 * GM_DOMAIN - Sizing information
 *
 * In this implementation, all we do is report our height
 */
IPTR SVG__GM_DOMAIN(Class *cl, Object *o, Msg msg)
{
    struct gpDomain *gpd = (struct gpDomain *)msg;
    struct SVG_Data *sd = INST_DATA(cl, o);

    gpd->gpd_Domain.Left = 0;
    gpd->gpd_Domain.Top  = 0;
    gpd->gpd_Domain.Width = sd->diagram->width;
    gpd->gpd_Domain.Height = sd->diagram->height;

    D(bug("svg.datatype/GM_DOMAIN: %dx%d @%dx%d\n", gpd->gpd_Domain.Width, gpd->gpd_Domain.Height, gpd->gpd_Domain.Left, gpd->gpd_Domain.Top));

    return TRUE;
}

/*
 * GM_LAYOUT
 */
IPTR SVG__GM_LAYOUT(Class *cl, Object *o, Msg msg)
{
    struct SVG_Data *sd = INST_DATA(cl, o);
    struct Gadget *gad = (struct Gadget *)o;

    D(bug("svg.datatype/GM_LAYOUT\n"));

    gad->Width = sd->diagram->width;
    gad->Height = sd->diagram->height;

    return TRUE;
}

/*
 * DTM_DRAW
 */
IPTR SVG__DTM_DRAW(Class *cl, Object *o, Msg msg)
{
    D(bug("svg.datatype/DTM_DRAW\n"));
    return TRUE;
}
    
/*
 * GM_RENDER - Render to a RastPort
 */
IPTR SVG__GM_RENDER(Class *cl, Object *o, Msg msg)
{
    struct gpRender *gpr = (struct gpRender *)msg;
    struct Gadget *gad = (struct Gadget *)o;
    struct SVG_Data *sd = INST_DATA(cl, o);

    D(bug("svg.datatype/GM_RENDER: %dx%d @%dx%d\n", gad->Width, gad->Height, gad->LeftEdge, gad->TopEdge));
    return cairo_render(sd->diagram, 1.0, gpr->gpr_RPort, gad->LeftEdge, gad->TopEdge, gad->Width, gad->Height);
}
    

