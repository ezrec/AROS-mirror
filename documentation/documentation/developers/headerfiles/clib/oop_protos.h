#ifndef CLIB_OOP_PROTOS_H
#define CLIB_OOP_PROTOS_H

/*
    *** Automatically generated from 'oop.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <oop/oop.h>
AROS_LP3(APTR, OOP_NewObject,
         AROS_LPA(struct OOP_IClass *, classPtr, A0),
         AROS_LPA(UBYTE *, classID, A1),
         AROS_LPA(struct TagItem *, tagList, A2),
         LIBBASETYPEPTR, OOPBase, 5, OOP
);
AROS_LP1(OOP_AttrBase, OOP_ObtainAttrBase,
         AROS_LPA(STRPTR, interfaceID, A0),
         LIBBASETYPEPTR, OOPBase, 6, OOP
);
AROS_LP2(OOP_MethodID, OOP_GetMethodID,
         AROS_LPA(STRPTR, interfaceID, A0),
         AROS_LPA(ULONG, methodOffset, D0),
         LIBBASETYPEPTR, OOPBase, 7, OOP
);
AROS_LP1(void, OOP_AddClass,
         AROS_LPA(OOP_Class *, classPtr, A0),
         LIBBASETYPEPTR, OOPBase, 8, OOP
);
AROS_LP1(void, OOP_ReleaseAttrBase,
         AROS_LPA(STRPTR, interfaceID, A0),
         LIBBASETYPEPTR, OOPBase, 9, OOP
);
AROS_LP1(void, OOP_DisposeObject,
         AROS_LPA(OOP_Object *, obj, A0),
         LIBBASETYPEPTR, OOPBase, 10, OOP
);
AROS_LP1(void, OOP_RemoveClass,
         AROS_LPA(OOP_Class *, classPtr, A0),
         LIBBASETYPEPTR, OOPBase, 11, OOP
);
AROS_LP1(OOP_AttrBase, OOP_GetAttrBase,
         AROS_LPA(STRPTR, interfaceID, A0),
         LIBBASETYPEPTR, OOPBase, 15, OOP
);
AROS_LP3(IPTR, OOP_GetAttr,
         AROS_LPA(OOP_Object *, object, A0),
         AROS_LPA(OOP_AttrID, attrID, A1),
         AROS_LPA(IPTR *, storage, A2),
         LIBBASETYPEPTR, OOPBase, 16, OOP
);
AROS_LP2(IPTR, OOP_SetAttrs,
         AROS_LPA(OOP_Object *, object, A0),
         AROS_LPA(struct TagItem *, attrList, A1),
         LIBBASETYPEPTR, OOPBase, 17, OOP
);
AROS_LP1(BOOL, OOP_ObtainAttrBases,
         AROS_LPA(struct OOP_ABDescr *, abd, A0),
         LIBBASETYPEPTR, OOPBase, 18, OOP
);
AROS_LP1(void, OOP_ReleaseAttrBases,
         AROS_LPA(struct OOP_ABDescr *, abd, A0),
         LIBBASETYPEPTR, OOPBase, 19, OOP
);
AROS_LP5(LONG, OOP_ParseAttrs,
         AROS_LPA(struct TagItem *, tags, A0),
         AROS_LPA(IPTR *, storage, A1),
         AROS_LPA(ULONG, numattrs, D0),
         AROS_LPA(OOP_AttrCheck *, attrcheck, A2),
         AROS_LPA(OOP_AttrBase, attrbase, D1),
         LIBBASETYPEPTR, OOPBase, 20, OOP
);
AROS_LP2(void *, OOP_GetMethod,
         AROS_LPA(OOP_Object *, obj, A0),
         AROS_LPA(OOP_MethodID, mid, D0),
         LIBBASETYPEPTR, OOPBase, 21, OOP
);

#endif /* CLIB_OOP_PROTOS_H */
