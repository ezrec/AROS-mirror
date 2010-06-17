#ifndef CLIB_COOLIMAGES_PROTOS_H
#define CLIB_COOLIMAGES_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/coolimages/coolimages.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <libraries/coolimages.h>

__BEGIN_DECLS

AROS_LP2(const struct CoolImage *, COOL_ObtainImageA,
         AROS_LPA(ULONG, imageid, D0),
         AROS_LPA(struct TagItem *, tags, A0),
         LIBBASETYPEPTR, CoolImagesBase, 5, CoolImages
);

__END_DECLS

#endif /* CLIB_COOLIMAGES_PROTOS_H */
