#ifndef WB_COMMON_H
#define WB_COMMON_H

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/tasks.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <proto/bgui.h>

#include <bgui/bgui.h>
#include <bgui/bgui_macros.h>

#include <dos/dostags.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _AROS
#include <proto/alib.h>
#else
#include <clib/alib_protos.h>
#endif

#ifdef _AROS
#define DEBUG 1
#include <aros/debug.h>
#endif

#endif /* WB_COMMON_H */
