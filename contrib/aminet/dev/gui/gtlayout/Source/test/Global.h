#include <stdio.h>
#include <stdarg.h>

#include <dos/dos.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <gtlayout.h>
#include <utility/hooks.h>
#ifdef __AROS__
#include <proto/gtlayout.h>
#else
#include <gtlayout_protos.h>
#endif
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/locale.h>
#include <clib/alib_protos.h>

#define CATCOMP_ARRAY
