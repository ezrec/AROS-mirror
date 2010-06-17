#ifndef TOOLLIB_CALLBACK_H
#define TOOLLIB_CALLBACK_H

/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id: callback.h 30792 2009-03-07 22:40:04Z neil $
*/

typedef void * CBD;
typedef int (*CB) (void * obj, int, CBD);

#define CallCB(cb,obj,cmd,data)     ((*(cb))(obj,cmd,data))

#endif /* TOOLLIB_CALLBACK_H */
