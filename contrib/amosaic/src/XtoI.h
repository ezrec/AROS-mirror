#ifndef XTOI_H
#define XTOI_H

#include <exec/types.h>
#include <proto/graphics.h>

#ifdef USE_ZUNE
#include <mui.h>
#else
#include <proto/muimaster.h>
#endif

typedef struct TextFont XFontStruct;
typedef struct Hook XtCallbackList;
typedef Object *Widget;
typedef void *XtPointer;
typedef int Time;

#define XCharStruct struct TextExtent
#define XTextExtents(a,b,c,d,e,f,g) SetFont(HTML_Data->rp,a);TextExtent(HTML_Data->rp,b,c,g)
#define XSetForeground(a,b,c) SetAPen(HTML_Data->rp,c)
#define XSetBackground(a,b,c) SetBPen(HTML_Data->rp,c)
#define XFillRectangle(a,b,c,d,e,f,g) RectFill(HTML_Data->rp,(HTML_Data->view_left+d),(HTML_Data->view_top+e),(HTML_Data->view_left+d+f),(HTML_Data->view_top+e+g))
#define XDrawImageString(a,b,c,d,e,f,g) Move(HTML_Data->rp,(HTML_Data->view_left+d),(HTML_Data->view_top+e));Text(HTML_Data->rp,f,g)
#define XDrawLine(a,b,c,d,e,f,g) Move(HTML_Data->rp,(HTML_Data->view_left+d),(HTML_Data->view_top+e));Draw(HTML_Data->rp,(HTML_Data->view_left+f),(HTML_Data->view_top+g))
#define XFillArc(a,b,c,d,e,f,g,h,i) DrawCircle(HTML_Data->rp,(HTML_Data->view_left+d),(HTML_Data->view_top+e),f);DrawCircle(HTML_Data->rp,(HTML_Data->view_left+d),(HTML_Data->view_top+e),f-1);DrawCircle(HTML_Data->rp,(HTML_Data->view_left+d),(HTML_Data->view_top+e),f-2);DrawCircle(HTML_Data->rp,(HTML_Data->view_left+d),(HTML_Data->view_top+e),f-3);
#define XDrawArc(a,b,c,d,e,f,g,h,i) DrawCircle(HTML_Data->rp,(HTML_Data->view_left+d),(HTML_Data->view_top+e),f)
#define XDrawRectangle(a,b,c,d,e,f,g) Move(HTML_Data->rp,(HTML_Data->view_left+d-f),(HTML_Data->view_top+e-g));Draw(HTML_Data->rp,(HTML_Data->view_left+d+f),(HTML_Data->view_top+e-g));Draw(HTML_Data->rp,(HTML_Data->view_left+d+f),(HTML_Data->view_top+e+g));Draw(HTML_Data->rp,(HTML_Data->view_left+d-f),(HTML_Data->view_top+e+g));Draw(HTML_Data->rp,(HTML_Data->view_left+d-f),(HTML_Data->view_top+e-g));

#define XCopyArea(a,b,c,d,e,f,g,h,i,j) SetBPen(HTML_Data->rp,0);ScrollRaster(HTML_Data->rp,e-i,f-j,HTML_Data->view_left,HTML_Data->view_top,HTML_Data->view_left+HTML_Data->view_width,HTML_Data->view_top+HTML_Data->view_height);

#define XClearArea(a,b,c,d,e,f,g) SetAPen(HTML_Data->rp,0);RectFill(HTML_Data->rp,HTML_Data->view_left+c,HTML_Data->view_top+d,HTML_Data->view_left+e+c,HTML_Data->view_top+f+d);SetAPen(HTML_Data->rp,1);

#define Boolean BOOL
#define False FALSE
#define True TRUE
#define Dimension int
#define Pixel int

typedef struct BitMap *Pixmap;
typedef struct BitMap XImage;

/*
 * Data structure used by color operations
 */
typedef struct {
        unsigned long pixel;
        unsigned short red, green, blue;
        char flags;  /* do_red, do_green, do_blue */
        char pad;
} XColor;

#endif /* XTOI_H */
