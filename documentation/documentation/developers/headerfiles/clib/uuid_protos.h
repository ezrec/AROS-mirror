#ifndef CLIB_UUID_PROTOS_H
#define CLIB_UUID_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/uuid/uuid.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <libraries/uuid.h>

__BEGIN_DECLS

AROS_LP2(int, UUID_Compare,
         AROS_LPA(const uuid_t *, u1, A0),
         AROS_LPA(const uuid_t *, u2, A1),
         LIBBASETYPEPTR, UUIDBase, 5, UUID
);
AROS_LP2(void, UUID_GetNameSpace,
         AROS_LPA(uuid_namespace_t, NameSpace, D0),
         AROS_LPA(uuid_t *, uuid, A0),
         LIBBASETYPEPTR, UUIDBase, 6, UUID
);
AROS_LP2(int, UUID_Parse,
         AROS_LPA(const char *, in, A0),
         AROS_LPA(uuid_t *, uuid, A1),
         LIBBASETYPEPTR, UUIDBase, 7, UUID
);
AROS_LP2(void, UUID_Unparse,
         AROS_LPA(const uuid_t *, uuid, A0),
         AROS_LPA(char *, out, A1),
         LIBBASETYPEPTR, UUIDBase, 8, UUID
);
AROS_LP2(void, UUID_Pack,
         AROS_LPA(const uuid_t *, uuid, A0),
         AROS_LPA(uint8_t *, out, A1),
         LIBBASETYPEPTR, UUIDBase, 9, UUID
);
AROS_LP2(void, UUID_Unpack,
         AROS_LPA(const uint8_t *, in, A0),
         AROS_LPA(uuid_t *, uuid, A1),
         LIBBASETYPEPTR, UUIDBase, 10, UUID
);
AROS_LP1(void, UUID_Clear,
         AROS_LPA(uuid_t *, uuid, A0),
         LIBBASETYPEPTR, UUIDBase, 11, UUID
);
AROS_LP2(void, UUID_Copy,
         AROS_LPA(const uuid_t *, src, A0),
         AROS_LPA(uuid_t *, dst, A1),
         LIBBASETYPEPTR, UUIDBase, 12, UUID
);
AROS_LP2(void, UUID_Generate,
         AROS_LPA(uuid_type_t, type, D0),
         AROS_LPA(uuid_t *, uuid, A0),
         LIBBASETYPEPTR, UUIDBase, 13, UUID
);

__END_DECLS

#endif /* CLIB_UUID_PROTOS_H */
