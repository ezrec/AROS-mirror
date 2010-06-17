#ifndef DEFINES_KEYMAP_H
#define DEFINES_KEYMAP_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/keymap/keymap.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for keymap
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __SetKeyMapDefault_WB(__KeymapBase, __arg1) \
        AROS_LC1NR(void, SetKeyMapDefault, \
                  AROS_LCA(struct KeyMap *,(__arg1),A0), \
        struct Library *, (__KeymapBase), 5, Keymap)

#define SetKeyMapDefault(arg1) \
    __SetKeyMapDefault_WB(KeymapBase, (arg1))

#define __AskKeyMapDefault_WB(__KeymapBase) \
        AROS_LC0(struct KeyMap *, AskKeyMapDefault, \
        struct Library *, (__KeymapBase), 6, Keymap)

#define AskKeyMapDefault() \
    __AskKeyMapDefault_WB(KeymapBase)

#define __MapRawKey_WB(__KeymapBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(WORD, MapRawKey, \
                  AROS_LCA(struct InputEvent *,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D1), \
                  AROS_LCA(struct KeyMap *,(__arg4),A2), \
        struct Library *, (__KeymapBase), 7, Keymap)

#define MapRawKey(arg1, arg2, arg3, arg4) \
    __MapRawKey_WB(KeymapBase, (arg1), (arg2), (arg3), (arg4))

#define __MapANSI_WB(__KeymapBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(LONG, MapANSI, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(STRPTR,(__arg3),A1), \
                  AROS_LCA(LONG,(__arg4),D1), \
                  AROS_LCA(struct KeyMap *,(__arg5),A2), \
        struct Library *, (__KeymapBase), 8, Keymap)

#define MapANSI(arg1, arg2, arg3, arg4, arg5) \
    __MapANSI_WB(KeymapBase, (arg1), (arg2), (arg3), (arg4), (arg5))

__END_DECLS

#endif /* DEFINES_KEYMAP_H*/
