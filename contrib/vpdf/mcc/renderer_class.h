
#ifndef RENDERER_MCC_CLASS_H
#define	RENDERER_MCC_CLASS_H

#include "system/chunky.h"
#include "classes.h"

/* interface methods and attributes */

#define	MUIM_Renderer_Enqueue 				     (MUIM_DocumentLayout_Dummy + 1)
#define	MUIM_Renderer_Remove                     (MUIM_DocumentLayout_Dummy + 2)

struct MUIP_Renderer_Enqueue{ULONG MethodID; LONG pageIndex; Object *grpLayout; LONG priority;};
struct MUIP_Renderer_Remove{ULONG MethodID; LONG pageIndex; Object *grpLayout;};

#define MUIV_Renderer_Remove_All -1

#define MUIV_Renderer_Enqueue_Priority_Normal 0
#define MUIV_Renderer_Enqueue_Priority_Low -1
#define MUIV_Renderer_Enqueue_Priority_High 1

/* */

#define RendererObject   NewObject(getRendererClass() , NULL

DEFCLASS(Renderer);

#endif
