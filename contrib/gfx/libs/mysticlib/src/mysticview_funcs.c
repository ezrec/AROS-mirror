#include <libraries/mysticview.h>
#include <aros/libcall.h>

#include "mystic_global.h"
#include "mystic_libprotos.h"


AROS_LH3(APTR, MV_CreateA,
    AROS_LHA(struct Screen *, screen, A0),
    AROS_LHA(struct RastPort *, rastport, A1),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, MysticBase, 5, MysticView)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MysticBase)
    
    return CreateView(screen, rastport, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, MV_Delete,
    AROS_LHA(APTR, mview, A0),
    struct Library *, MysticBase, 6, MysticView)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MysticBase)
    
    return DeleteView(mview);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, MV_SetAttrsA,
    AROS_LHA(APTR, mview, A0),
    AROS_LHA(struct TagItem *, tags, A1),
    struct Library *, MysticBase, 7, MysticView)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MysticBase)
    
    return SetViewAttrs(mview, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(BOOL, MV_DrawOn,
    AROS_LHA(APTR, mview, A0),
    struct Library *, MysticBase, 8, MysticView)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MysticBase)
    
    return DrawOn(mview);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, MV_DrawOff,
    AROS_LHA(APTR, mview, A0),
    struct Library *, MysticBase, 9, MysticView)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MysticBase)
    
    DrawOff(mview);
    
    AROS_LIBFUNC_EXIT
}


AROS_LH1(void, MV_Refresh,
    AROS_LHA(APTR, mview, A0),
    struct Library *, MysticBase, 10, MysticView)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MysticBase)
    
    return RefreshView(mview);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, MV_GetAttrsA,
    AROS_LHA(APTR, mview, A0),
    AROS_LHA(struct TagItem *, tags, A1),
    struct Library *, MysticBase, 11, MysticView)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MysticBase)
    
    SetViewAttrs(mview, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, MV_SetViewStart,
    AROS_LHA(APTR, mview, A0),
    AROS_LHA(LONG, x, D0),
    AROS_LHA(LONG, y, D1),
    struct Library *, MysticBase, 12, MysticView)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MysticBase)
    
    SetViewStart(mview, x, y);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, MV_SetViewRelative,
    AROS_LHA(APTR, mview, A0),
    AROS_LHA(LONG, x, D0),
    AROS_LHA(LONG, y, D1),
    struct Library *, MysticBase, 13, MysticView)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MysticBase)
    
    SetViewRelative(mview, x, y);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, MV_Lock,
    AROS_LHA(APTR, mview, A0),
    AROS_LHA(BOOL, lock, D0),
    struct Library *, MysticBase, 14, MysticView)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MysticBase)
    
    LockView(mview, lock);
    
    AROS_LIBFUNC_EXIT
}

