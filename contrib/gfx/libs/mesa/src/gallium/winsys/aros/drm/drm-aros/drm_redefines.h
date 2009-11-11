/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#if !defined(DRM_REDEFINES_H)
#define DRM_REDEFINES_H

/* THESE REDEFINES ARE NECESSARY AS WE BUILD USER AND KERNEL INTO ONE MODULE
   AND THEY HAVE OVERLAPPING FUNCTION NAMES! */

#define nouveau_notifier_alloc              _redefined_nouveau_notifier_alloc
#define nouveau_channel_alloc               _redefined_nouveau_channel_alloc

#endif