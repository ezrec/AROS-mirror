#ifndef	UTIL_H
#define	UTIL_H

#include <libraries/mui.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <dos/dosextens.h>

static inline LONG xget(void *object, ULONG attr)
{
	LONG a = 0;
	get(object,attr,&a);
	return a;
}

static inline int sxget(void *object, LONG attr)
{
	int	a[ 0 ];
	get(object, attr, a);
	return a[ 0 ];
}

#define nfset(obj, attr, val) \
	SetAttrs(obj, MUIA_Group_Forward, FALSE, \
					attr, val, \
					TAG_DONE)

#define getstr(obj) ((char *)xget(obj, MUIA_String_Contents))


#define isDir(fib) ((fib)->fib_DirEntryType > 0)

static inline LONG isDirPath(char *path)
{
	BPTR lock = Lock(path, ACCESS_READ);
	if (lock != NULL)
	{
		struct FileInfoBlock fib;
		if (Examine(lock, &fib))
		{
			UnLock(lock);
			return isDir(&fib);
		}
	}
	return FALSE;
}

/// Morphos specific hook stuff

/*
 * Hook, called with the following:
 * n - name of the hook (_hook appened at the end)
 * y - a2
 * z - a1
 *
 * return type is LONG
 */

#ifdef __MORPHOS__

#define M_RHOOK(n, y, z) \
static int n##_GATE(void); \
static int	n##_GATE2(struct Hook *h, y, z); \
static struct EmulLibEntry n = { \
TRAP_LIB, 0, (void (*)(void))n##_GATE }; \
static int n##_GATE(void) { \
return (n##_GATE2((void *)REG_A0, (void *)REG_A2, (void *)REG_A1)); } \
static struct Hook n##Hook = { {0, 0}, (void *)&n , NULL , NULL }; \
static int	n##_GATE2(struct Hook *h, y, z)

#define M_HOOK(n, y, z) \
static void	n##_GATE(void); \
static void	n##_GATE2(struct Hook *h, y, z); \
static struct EmulLibEntry n = { \
TRAP_LIB, 0, (void (*)(void))n##_GATE }; \
static void n##_GATE(void) { \
return (n##_GATE2((void *)REG_A0, (void *)REG_A2, (void *)REG_A1)); } \
static struct Hook n##Hook = { {0, 0}, (void *)&n , NULL , NULL }; \
static void	n##_GATE2(struct Hook *h, y, z)

/* simple version */

#define	S_HOOK(n)	\
static struct EmulLibEntry n ## Func = { \
TRAP_LIB, 0, (void (*)(void))n }; \
static const struct Hook n ## Hook = { { NULL,NULL }, (void*)&n ## Func,NULL,NULL };\
void	n(void)

#else
#define M_HOOK(n, y, z) \
static void ASM SAVEDS n##_func(__reg(a0, struct Hook *h), \
__reg(a2, y), __reg(a1, z));
static struct Hook n ## Hook =
{
    0, 0, ( HOOKFUNC ) n##_func
}
;
\
static void ASM SAVEDS n##_func( __reg( a0, struct Hook *h ), \
                                 __reg( a2, y ), __reg( a1, z ) )
#endif /* !__MORPHOS__ */

////

#define FORTAG(_tagp) \
	{ \
		struct TagItem *tag, *_tags = _tagp; \
		while ((tag = NextTagItem(&_tags))) switch ((int)tag->ti_Tag)
#define NEXTTAG }

#define FORCHILD(_o, _a) \
	{ \
		APTR child, _cstate = (APTR)((struct MinList *)xget(_o, _a))->mlh_Head; \
		while ((child = NextObject(&_cstate)))

#define NEXTCHILD }

#define SUCCCHILD ({ APTR _succcstate = _cstate; \
		NextObject(&_succcstate);})

#define INITTAGS (((struct opSet *)msg)->ops_AttrList)

#undef NextObject
#define NextObject(cstate) \
({\
	struct Node **nptr = (struct Node **)cstate;\
	struct Node *this = *nptr;\
	APTR next = this->ln_Succ;\
	if (next)\
	{\
		*nptr = next;\
		next = BASEOBJECT(this);\
	}\
	next;\
})

#ifndef MADF_VISIBLE
#define MADF_VISIBLE (1<<14)
#endif

#ifndef MADF_DISABLED
#define MADF_DISABLED (1<<15)
#endif

#ifndef _isvibible
#define _isvisible(obj) (_flags(obj) & MADF_VISIBLE)
#endif

#define METHOD static ULONG
#define PUBLIC_METHOD ULONG

struct MUI_RGBcolor
{
	ULONG red;
	ULONG green;
	ULONG blue;
};

#ifndef MUIA_Pendisplay_RGBcolor
#define MUIA_Pendisplay_RGBcolor            0x8042a1a9 /* V11 isg struct MUI_RGBcolor * */
#endif

#ifndef MUIM_Text
#define MUIM_Text                           0x8042ee70 /* private */ /* V20 */
#define MUIM_TextDim                        0x80422ad7 /* private */ /* V20 */

struct  MUIP_Text                           { ULONG MethodID; LONG left;LONG top;LONG width;LONG height;STRPTR text;LONG len;STRPTR preparse;ULONG flags; }; /* private */
struct  MUIP_TextDim                        { ULONG MethodID; STRPTR text;LONG len;STRPTR preparse;ULONG flags; }; /* private */
#endif

#ifndef MUIA_List_AutoLineHeight
#define MUIA_List_AutoLineHeight            0x8042bc08 /* V20 i.. BOOL              */ /* private */
#endif

#ifndef MUIM_Group_GetChild
#define MUIM_Group_GetChild                 0x8042c556 /* private */ /* V20 */

#define MUIV_Group_GetChild_First     MUIV_Family_GetChild_First
#define MUIV_Group_GetChild_Last      MUIV_Family_GetChild_Last
#define MUIV_Group_GetChild_Next      MUIV_Family_GetChild_Next
#define MUIV_Group_GetChild_Previous  MUIV_Family_GetChild_Previous
#define MUIV_Group_GetChild_Iterate   MUIV_Family_GetChild_Iterate
#endif

#ifndef MUIM_Group_ExitChange2
#define MUIM_Group_ExitChange2              0x8042e541 /* private */ /* V12 */
#endif

#ifndef MUIA_CustomBackfill
#define MUIA_CustomBackfill                 0x80420a63 /* V11 isg BOOL              */ /* private */
#endif

#ifndef MUIA_Window_OverrideStoredDimension
#define MUIA_Window_OverrideStoredDimension 0x8042c0f5 /* V20 is. BOOL              */ /* private */
#endif

#ifndef MUIM_Backfill
#define MUIM_Backfill                       0x80428d73 /* private */ /* V11 */
struct  MUIP_Backfill                       { ULONG MethodID; LONG left; LONG top; LONG right; LONG bottom; LONG xoffset; LONG yoffset; LONG brightness; }; /* private */
#endif

#ifndef MUIA_Image_FontMatchString
#define MUIA_Image_FontMatchString          0x804263c1 /* V20 is. char *            */ /* private */
#endif

// this one is really internal
#ifndef MUIM_Group_Balance
#define MUIM_Group_Balance                  0x80425906 /* private */ /* V11 */
struct  MUIP_Group_Balance                  { ULONG MethodID; Object *bal; LONG clickx; LONG clicky; LONG mode; LONG mousex; LONG mousey; }; /* private */
#endif

#ifndef MUIM_Window_HandleRMB
#define MUIM_Window_HandleRMB               0x80421786 /* private */ /* V20 */
#endif

#ifndef MUIM_Application_KillPushMethod
#define MUIM_Application_KillPushMethod     0x80429954 /* private */ /* V15 */
#endif

#ifndef MUIM_Virtgroup_MakeVisible
#define MUIM_Virtgroup_MakeVisible          0x8042d2d2 /* private */ /* V4  */
struct  MUIP_Virtgroup_MakeVisible          { ULONG MethodID; Object *obj; ULONG flags; }; /* private */
#endif

#ifndef _parent
#define _parent(obj) ((Object*)xget(obj, MUIA_Parent))
#endif

#ifndef MUIA_Title_Sortable
#define MUIA_Title_Sortable                 0x804211f1 /* V20 isg BOOL              */ /* private */
#endif

#ifndef MUIA_Window_OverrideStoredPosition
#define MUIA_Window_OverrideStoredPosition 0x8042fc00 /* V20 is. BOOL              */ /* private */
#endif

#ifndef MUIA_Group_ChildCount
#define MUIA_Group_ChildCount               0x80420322 /* V20 ..g LONG              */ /* private */
#endif

#ifndef MUIA_Alpha
#define MUIA_Alpha                          0x8042c819 /* V20 isg LONG              */ /* private */
#endif

#ifndef MUIF_PUSHMETHOD_VERIFY
#define MUIF_PUSHMETHOD_VERIFY       (1<<30UL)
#endif

#endif
