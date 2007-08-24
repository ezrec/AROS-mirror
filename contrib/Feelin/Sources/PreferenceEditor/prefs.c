#include "Private.h"

/* Applications set a notification on the FA_Preference_Update attribute of
their  FC_Preference  object to be notified of preference modifications and
adjust  themselves.  The  notification  calls  a  pushed  method   on   the
application.  You must pay attention to this, in order to get things in the
right order. Don't forget that the FA_Window_Close  attribute  also  pushes
the method FM_Window_Close

Preference files are watched using a FC_DOSNotify object. If  the  file  is
modified  the  attribute  FA_Preference_Update  is set to TRUE, pushing the
update of the application. */

/*** Methods ***************************************************************/

///PE_Load
F_METHOD(void,PE_Load)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct FeelinPGroup *node;

	for (node = (struct FeelinPGroup *) LOD -> GroupList.Head ; node ; node = node -> Next)
	{
		F_Do(node -> Object,F_IDO(FM_PreferenceGroup_Load),LOD -> Preferences,F_IDO(FM_Preference_Resolve),F_IDO(FM_Preference_ResolveInt));
	}
}
//+
///PE_Save
F_METHOD(void,PE_Save)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct FeelinPGroup *node;

	for (node = (struct FeelinPGroup *) LOD -> GroupList.Head ; node ; node = node -> Next)
	{
		F_Do(node -> Object,F_IDO(FM_PreferenceGroup_Save),LOD -> Preferences,F_IDO(FM_Preference_Add),F_IDO(FM_Preference_AddLong),F_IDO(FM_Preference_AddString));
	}

	F_Do(LOD -> Preferences,F_IDO(FM_Preference_Write),FV_Preference_BOTH);

	F_Set(Obj,FA_Window_CloseRequest,TRUE);
}
//+
///PE_Use
F_METHOD(void,PE_Use)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct FeelinPGroup *node;

	for (node = (struct FeelinPGroup *) LOD -> GroupList.Head ; node ; node = node -> Next)
	{
		F_Do(node -> Object,F_IDO(FM_PreferenceGroup_Save),LOD -> Preferences,F_IDO(FM_Preference_Add),F_IDO(FM_Preference_AddLong),F_IDO(FM_Preference_AddString));
	}

	F_Do(LOD -> Preferences,F_IDO(FM_Preference_Write),FV_Preference_ENV);

	F_Set(Obj,FA_Window_CloseRequest,TRUE);
}
//+
///PE_Cancel
F_METHOD(void,PE_Cancel)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	F_Do(LOD -> Preferences,F_IDO(FM_Preference_Clear));
	F_Do(LOD -> Preferences,F_IDO(FM_Preference_Read),FV_Preference_ENV);

	F_Set(LOD -> Preferences,(ULONG) "FA_Preference_Update",TRUE);

	F_Set(Obj,FA_Window_CloseRequest,TRUE);
}
//+
///PE_Test
F_METHOD(void,PE_Test)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct FeelinPGroup *node;

	for (node = (struct FeelinPGroup *) LOD -> GroupList.Head ; node ; node = node -> Next)
	{
		F_Do(node -> Object,F_IDO(FM_PreferenceGroup_Save),LOD -> Preferences,F_IDO(FM_Preference_Add),F_IDO(FM_Preference_AddLong),F_IDO(FM_Preference_AddString));
	}

	F_Set(LOD -> Preferences,(ULONG) "FA_Preference_Update",TRUE);
	F_Set(Obj,FA_Window_Open,TRUE);
}
//+

