//
// PopupMenu
// ©1996-2002 Henrik Isaksson
//
// Menu creation/disposal & id list functions
//
// $Date$
// $Revision$
//

#include "pmpriv.h"

void FreeIDList(struct PM_IDLst *f)
{
    if (f) {
        struct PM_IDLst *n;

        while (f) {
            n=f->Next;
            PM_Mem_Free(f);
            f=n;
        }
    }
}

void PM_FreeTitle(struct PopupMenu *p)
{
    if (!p) return;

    if (p->TitleUnion.Title && GET_TXTMODE(p)==0) PM_Mem_Free(p->TitleUnion.Title);

    p->TitleUnion.Title=NULL;
}

LIBFUNC_P2(void, LIBPM_FreePopupMenu,
	A1, struct PopupMenu *, p,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	pm_FreePopupMenu(p, PopupMenuBase);
}
LIBFUNC_END


void pm_FreePopupMenu(struct PopupMenu *p, struct PopupMenuBase *PopupMenuBase)
{

    if (p) {
        struct PopupMenu *n;

        while (p) {
            n=p->Next;
	    if (p->SubGroup.Sub)
		pm_FreePopupMenu(p->SubGroup.Sub, PopupMenuBase);
            PM_FreeTitle(p);
            if (p->UserData && (p->Flags&NPM_UDATASTRING)) PM_Mem_Free(p->UserData);
            if (p->Exclude && !(p->Flags&NPM_EXCLUDE_SHARED)) {
                FreeIDList(p->Exclude);
            }
            PM_Mem_Free(p);
            p=n;
        }
    }
}

LIBFUNC_P2(void, LIBPM_FreeIDList,
	A0, struct PM_IDLst *, f,
	A6, struct PopupMenuBase *, PopupMenuBase);
{
	(void) PopupMenuBase;
	FreeIDList(f);
}
LIBFUNC_END

LIBFUNC_P2(struct PopupMenu *, LIBPM_MakeItemA,
	A1, struct TagItem *, tags,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
    struct PopupMenu *p;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  START\n", __LINE__));

    p = PM_Mem_Alloc(sizeof(struct PopupMenu));
    if (p) {
	pm_SetItemAttrsA(p, tags, PopupMenuBase);

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  END  p=%08lx\n", __LINE__, p));
        return p;
    }

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  END  first=%08lx\n", __LINE__, NULL));

    return NULL;
}
LIBFUNC_END

LIBFUNC_P2(struct PopupMenu *, LIBPM_MakeMenuA,
	A1, struct TagItem *, tags,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
    struct TagItem *tag, *tstate;
    struct PopupMenu *first=0L, *last=0L;
    BOOL error=0;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  START\n", __LINE__));

	(void) PopupMenuBase;

    tstate = tags;
    while ((tag=NextTagItem(&tstate))) {
        switch(tag->ti_Tag) {
            case PM_Item:
                if (tag->ti_Data) {
                    if (last) {
                        last->Next=(struct PopupMenu *)tag->ti_Data;
                        last=last->Next;
                    } else {
                        last=first=(struct PopupMenu *)tag->ti_Data;
                    }
                } else error=1;
                break;
        }
    }

    if (error) {
	pm_FreePopupMenu(first, PopupMenuBase);
        first=0L;
    }

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  END  first=%08lx\n", __LINE__, first));

    return first;
}
LIBFUNC_END

LIBFUNC_P2(struct PM_IDLst *, LIBPM_MakeIDListA,
	A1, struct TagItem *, tags,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
    struct TagItem *tag, *tstate;
    struct PM_IDLst *first=0L, *last=0L, *n;
    BOOL error=0;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  START\n", __LINE__));

	(void) PopupMenuBase;

    tstate = tags;
    while ((tag=NextTagItem(&tstate))) {
        switch(tag->ti_Tag) {
            case PM_ExcludeID:
                n=PM_Mem_Alloc(sizeof(struct PM_IDLst));
                if (n) {
                    n->Next=0L;
                    n->ID=tag->ti_Data;
                    n->Kind=IDKND_EXCLUDE;
                    n->Flags=0L;

                    if (last) {
                        last->Next=n;
                        last=last->Next;
                    } else {
                        last=first=n;
                    }
                } else error=1;
                break;
            case PM_IncludeID:
                n=PM_Mem_Alloc(sizeof(struct PM_IDLst));
                if (n) {
                    n->Next=0L;
                    n->ID=tag->ti_Data;
                    n->Kind=IDKND_INCLUDE;
                    n->Flags=0L;

                    if (last) {
                        last->Next=n;
                        last=last->Next;
                    } else {
                        last=first=n;
                    }
                } else error=1;
                break;
            case PM_ReflectID:
                n=PM_Mem_Alloc(sizeof(struct PM_IDLst));
                if (n) {
                    n->Next=0L;
                    n->ID=tag->ti_Data;
                    n->Kind=IDKND_REFLECT;
                    n->Flags=0L;

                    if (last) {
                        last->Next=n;
                        last=last->Next;
                    } else {
                        last=first=n;
                    }
                } else error=1;
                break;
            case PM_InverseID:
                n=PM_Mem_Alloc(sizeof(struct PM_IDLst));
                if (n) {
                    n->Next=0L;
                    n->ID=tag->ti_Data;
                    n->Kind=IDKND_INVERSE;
                    n->Flags=0L;

                    if (last) {
                        last->Next=n;
                        last=last->Next;
                    } else {
                        last=first=n;
                    }
                } else error=1;
                break;
        }
    }

    if (error) {
	FreeIDList(first);
        first=0L;
    }

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  END  first=%08lx\n", __LINE__, first));

    return first;
}
LIBFUNC_END

LIBFUNC_P2(struct PM_IDLst *, LIBPM_ExLstA,
	A1, ULONG *, id,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	struct PM_IDLst *first=0L, *last=0L, *n;
	BOOL error=0;
	int i=0;

	(void) PopupMenuBase;

	while (id[i])
		{
		n=PM_Mem_Alloc(sizeof(struct PM_IDLst));
		if (n)
			{
			n->Next=0L;
			n->ID=id[i];
			n->Kind=IDKND_EXCLUDE;
			n->Flags=0L;

			if (last)
				{
				last->Next=n;
				last=last->Next;
				}
			else
				{
				last=first=n;
				}
			}
		else
			error=1;
		i++;
		}

	if (error)
		{
		FreeIDList(first);
		first=0L;
		}

	return first;
}
LIBFUNC_END

//
// Allocate local variables to save stack
//
struct PM_Root *PM_AllocPMRoot(struct Window *w)
{
        struct PM_Root *p;

        p=PM_Mem_Alloc(sizeof(struct PM_Root));
        if (p) {
                p->ShadowWidth=p->ShadowHeight=4;
                p->ShadowAddX=p->ShadowAddY=2;

                p->BorderWidth=p->BorderHeight=1;

                p->DrawInfo=GetScreenDrawInfo(w->WScreen);
                
                p->PM=0L;

                return p;
        }
        return NULL;
}
