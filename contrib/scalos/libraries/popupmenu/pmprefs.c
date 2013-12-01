// pmprefs.c
//
// $Date$
// $Revision$

#include <dos/dos.h>
#include <proto/dos.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <string.h>
#include "pmprefs.h"

//#ifdef __AROS__
#include "pmpriv.h"
void PM_LoadPrefsFile(CONST_STRPTR filename, ULONG flags,
	struct oldPopupMenuPrefs *prefs, const struct oldPopupMenuPrefs *defprefs);
//#endif

static struct PopupMenuPrefs DefaultPrefs =
	{
	PMP_FLAGS_SHADOWS,	/* pmp_Flags		*/
	0,			/* pmp_SubMenuDelay	*/
	PMP_ANIM_NONE,		/* pmp_Animation	*/
	PMP_PD_SCREENBAR,	/* pmp_PulldownPos	*/
	FALSE,			/* pmp_Sticky		*/
	0,			/* pmp_MenuBorder	*/
	0,			/* pmp_SelItemBorder	*/
	0,			/* pmp_SeparatorBar	*/
	0,			/* pmp_MenuTitles	*/
	0,			/* pmp_MenuItems	*/
	2,			/* pmp_XOffset		*/
	2,			/* pmp_YOffset		*/
	2,			/* pmp_XSpace		*/
	2,			/* pmp_YSpace		*/
	2,			/* pmp_Intermediate	*/
	0,			/* pmp_TextDisplace	*/
	0,			/* pmp_TransparencyBlur	*/
	};

struct PopupMenuPrefs *PM_Prefs = &DefaultPrefs;


static void ConvertPmPrefs(CONST_STRPTR prefsFileOld, CONST_STRPTR prefsFileNew);


void PM_Prefs_Free(void)
{
}

void PM_Prefs_Load(CONST_STRPTR prefsFileOld, CONST_STRPTR prefsFileNew)
{
	static struct PopupMenuPrefs pmPrefs;
	APTR myPrefsHandle;

	ConvertPmPrefs(prefsFileOld, prefsFileNew);

	myPrefsHandle = AllocPrefsHandle("PopupMenuPrefs");
	if (myPrefsHandle)
		{
		LONG lID = MAKE_ID('M', 'A', 'I', 'N');

		ReadPrefsHandle(myPrefsHandle, prefsFileNew);

		GetPreferences(myPrefsHandle, lID, PMP_Flags, &pmPrefs.pmp_Flags, sizeof(pmPrefs.pmp_Flags) );
		GetPreferences(myPrefsHandle, lID, PMP_SubMenuDelay, &pmPrefs.pmp_SubMenuDelay, sizeof(pmPrefs.pmp_SubMenuDelay) );
		GetPreferences(myPrefsHandle, lID, PMP_AnimationType, &pmPrefs.pmp_Animation, sizeof(pmPrefs.pmp_Animation) );
		GetPreferences(myPrefsHandle, lID, PMP_PullDownMenuPos, &pmPrefs.pmp_PulldownPos, sizeof(pmPrefs.pmp_PulldownPos) );
		GetPreferences(myPrefsHandle, lID, PMP_Sticky, &pmPrefs.pmp_Sticky, sizeof(pmPrefs.pmp_Sticky) );
		GetPreferences(myPrefsHandle, lID, PMP_MenuBorderType, &pmPrefs.pmp_MenuBorder, sizeof(pmPrefs.pmp_MenuBorder) );
		GetPreferences(myPrefsHandle, lID, PMP_SelItemBorderType, &pmPrefs.pmp_SelItemBorder, sizeof(pmPrefs.pmp_SelItemBorder) );
		GetPreferences(myPrefsHandle, lID, PMP_SeparatorBarStyle, &pmPrefs.pmp_SeparatorBar, sizeof(pmPrefs.pmp_SeparatorBar) );
		GetPreferences(myPrefsHandle, lID, PMP_XOffset, &pmPrefs.pmp_XOffset, sizeof(pmPrefs.pmp_XOffset) );
		GetPreferences(myPrefsHandle, lID, PMP_YOffset, &pmPrefs.pmp_YOffset, sizeof(pmPrefs.pmp_YOffset) );
		GetPreferences(myPrefsHandle, lID, PMP_XSpace, &pmPrefs.pmp_XSpace, sizeof(pmPrefs.pmp_XSpace) );
		GetPreferences(myPrefsHandle, lID, PMP_YSpace, &pmPrefs.pmp_YSpace, sizeof(pmPrefs.pmp_YSpace) );
		GetPreferences(myPrefsHandle, lID, PMP_Intermediate, &pmPrefs.pmp_Intermediate, sizeof(pmPrefs.pmp_Intermediate) );
		GetPreferences(myPrefsHandle, lID, PMP_TransparencyBlur, &pmPrefs.pmp_TransparencyBlur, sizeof(pmPrefs.pmp_TransparencyBlur) );
		GetPreferences(myPrefsHandle, lID, PMP_TextDisplace, &pmPrefs.pmp_TextDisplace, sizeof(pmPrefs.pmp_TextDisplace) );
		GetPreferences(myPrefsHandle, lID, PMP_MenuTitleStyle, &pmPrefs.pmp_MenuTitles, sizeof(pmPrefs.pmp_MenuTitles) );
		GetPreferences(myPrefsHandle, lID, PMP_MenuItemStyle, &pmPrefs.pmp_MenuItems, sizeof(pmPrefs.pmp_MenuItems) );

		FreePrefsHandle(myPrefsHandle);
		}

	PM_Prefs = &pmPrefs;
}

LIBFUNC_P1(void, LIBPM_ReloadPrefs,
	A6, struct PopupMenuBase *, l)
{
	(void) l;

	PM_Prefs_Free();
	PM_Prefs_Load(PMP_PATH_OLD, PMP_PATH_NEW);
}
LIBFUNC_END


static void ConvertPmPrefs(CONST_STRPTR prefsFileOld, CONST_STRPTR prefsFileNew)
{
	BPTR fLock;

	fLock = Lock(prefsFileNew, ACCESS_READ);
	if (fLock)
		{
		UnLock(fLock);
		}
	else
		{
		struct oldPopupMenuPrefs LoadedPrefs;
		APTR myPrefsHandle;
		static const struct oldPopupMenuPrefs DefaultPrefs =
			{
			PMP_FLAGS_SHADOWS,	/* pmp_Flags		*/
			0,			/* pmp_SubMenuDelay	*/
			PMP_ANIM_NONE,		/* pmp_Animation	*/
			PMP_PD_SCREENBAR,	/* pmp_PulldownPos	*/
			FALSE,			/* pmp_Sticky		*/
			FALSE,			/* pmp_SameHeight	unused */
			0,			/* pmp_MenuBorder	*/
			0,			/* pmp_SelItemBorder	*/
			0,			/* pmp_SeparatorBar	*/
			0,			/* pmp_MenuTitles	*/
			0,			/* pmp_MenuItems	*/
			2,			/* pmp_XOffset		*/
			2,			/* pmp_YOffset		*/
			2,			/* pmp_XSpace		*/
			2,			/* pmp_YSpace		*/
			2,			/* pmp_Intermediate	*/
			0,			/* pmp_TextDisplace	*/
			-30,			/* pmp_ShadowR		unused */
			-30,			/* pmp_ShadowG		unused */
			-30,			/* pmp_ShadowB		unused */
			0,			/* pmp_TransparencyR	unused */
			0,			/* pmp_TransparencyG	unused */
			0,			/* pmp_TransparencyB	unused */
			0,			/* pmp_TransparencyBlur	*/
			0,			/* pmp_AnimationSpeed	unused */

			{0},			/* pmp_Reserved		*/
			};

		PM_LoadPrefsFile(prefsFileOld, 0, &LoadedPrefs, &DefaultPrefs);

		d1(KPrintF("%s/%s/%ld: pmp_Animation=%ld\n", __FILE__, __FUNC__, __LINE__, LoadedPrefs.pmp_Animation));

		myPrefsHandle = AllocPrefsHandle("PopupMenuPrefs");
		if (myPrefsHandle)
			{
			LONG lID = MAKE_ID('M', 'A', 'I', 'N');

			SetPreferences(myPrefsHandle, lID, PMP_Flags, &LoadedPrefs.pmp_Flags, sizeof(LoadedPrefs.pmp_Flags) );
			SetPreferences(myPrefsHandle, lID, PMP_SubMenuDelay, &LoadedPrefs.pmp_SubMenuDelay, sizeof(LoadedPrefs.pmp_SubMenuDelay) );
			SetPreferences(myPrefsHandle, lID, PMP_AnimationType, &LoadedPrefs.pmp_Animation, sizeof(LoadedPrefs.pmp_Animation) );
			SetPreferences(myPrefsHandle, lID, PMP_PullDownMenuPos, &LoadedPrefs.pmp_PulldownPos, sizeof(LoadedPrefs.pmp_PulldownPos) );
			SetPreferences(myPrefsHandle, lID, PMP_Sticky, &LoadedPrefs.pmp_Sticky, sizeof(LoadedPrefs.pmp_Sticky) );
			SetPreferences(myPrefsHandle, lID, PMP_MenuBorderType, &LoadedPrefs.pmp_MenuBorder, sizeof(LoadedPrefs.pmp_MenuBorder) );
			SetPreferences(myPrefsHandle, lID, PMP_SelItemBorderType, &LoadedPrefs.pmp_SelItemBorder, sizeof(LoadedPrefs.pmp_SelItemBorder) );
			SetPreferences(myPrefsHandle, lID, PMP_SeparatorBarStyle, &LoadedPrefs.pmp_SeparatorBar, sizeof(LoadedPrefs.pmp_SeparatorBar) );
			SetPreferences(myPrefsHandle, lID, PMP_XOffset, &LoadedPrefs.pmp_XOffset, sizeof(LoadedPrefs.pmp_XOffset) );
			SetPreferences(myPrefsHandle, lID, PMP_YOffset, &LoadedPrefs.pmp_YOffset, sizeof(LoadedPrefs.pmp_YOffset) );
			SetPreferences(myPrefsHandle, lID, PMP_XSpace, &LoadedPrefs.pmp_XSpace, sizeof(LoadedPrefs.pmp_XSpace) );
			SetPreferences(myPrefsHandle, lID, PMP_YSpace, &LoadedPrefs.pmp_YSpace, sizeof(LoadedPrefs.pmp_YSpace) );
			SetPreferences(myPrefsHandle, lID, PMP_Intermediate, &LoadedPrefs.pmp_Intermediate, sizeof(LoadedPrefs.pmp_Intermediate) );
			SetPreferences(myPrefsHandle, lID, PMP_TransparencyBlur, &LoadedPrefs.pmp_TransparencyBlur, sizeof(LoadedPrefs.pmp_TransparencyBlur) );
			SetPreferences(myPrefsHandle, lID, PMP_TextDisplace, &LoadedPrefs.pmp_TextDisplace, sizeof(LoadedPrefs.pmp_TextDisplace) );
			SetPreferences(myPrefsHandle, lID, PMP_MenuTitleStyle, &LoadedPrefs.pmp_MenuTitles, sizeof(LoadedPrefs.pmp_MenuTitles) );
			SetPreferences(myPrefsHandle, lID, PMP_MenuItemStyle, &LoadedPrefs.pmp_MenuItems, sizeof(LoadedPrefs.pmp_MenuItems) );

			WritePrefsHandle(myPrefsHandle, prefsFileNew);

			FreePrefsHandle(myPrefsHandle);
			}
		}
}


