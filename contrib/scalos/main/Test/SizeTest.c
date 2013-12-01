// Window.c
// 03 Mar 2001 10:46:03

#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <dos/dostags.h>
#include <scalos/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "defs.h"
#include "functions.h"
#include "variables.h"

#include "IconImageClass.h"



main()
{
	printf("internalScaWindowTask : %lu\n", sizeof(struct internalScaWindowTask));
}

