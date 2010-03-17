#ifndef _NOUVEAU_H
#define _NOUVEAU_H

/*
    Copyright 2010, The AROS Development Team. All rights reserved.
    $Id$
*/


#include LC_LIBDEFS_FILE

#define CLID_Hidd_GalliumNouveauDriver   "hidd.gallium.nouveaudriver"

struct HIDDGalliumNouveauDriverData
{
};

struct nouveaustaticdata 
{
    OOP_Class       *galliumNouveauDriverClass;
    OOP_AttrBase    hiddGalliumBaseDriverAB;
    BOOL            nouveauinited;
};

LIBBASETYPE 
{
    struct Library              LibNode;
    struct nouveaustaticdata    sd;
};

#define METHOD(base, id, name) \
  base ## __ ## id ## __ ## name (OOP_Class *cl, OOP_Object *o, struct p ## id ## _ ## name *msg)

#define BASE(lib) ((LIBBASETYPEPTR)(lib))

#define SD(cl) (&BASE(cl->UserData)->sd)

#endif
