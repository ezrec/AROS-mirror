#ifndef CHUNKYICLASS_H
#define CHUNKYICLASS_H
/*
 * chunkyiclass.h
 * ==============
 * Interface to chunky image class.
 *
 * Copyright © 1995 Lorens Younes (d93-hyo@nada.kth.se)
 */

#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/imageclass.h>
#include <utility/tagitem.h>


/* attributes defined by chunkyiclass */
#define CHUNKYIA_SelectedBGPen   (TAG_USER + 1)
#define CHUNKYIA_SelectedData    (TAG_USER + 2)
#define CHUNKYIA_Palette         (TAG_USER + 3)
#define CHUNKYIA_Screen          (TAG_USER + 4)


Class *
init_chunkyiclass (void);

BOOL
free_chunkyiclass (
   Class  *cl);

#endif
