#ifndef FEELIN_PREFERENCE_H
#define FEELIN_PREFERENCE_H

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack(2)
#endif
#endif /* __MORPHOS__ */

/*
**  $VER: feelin/preference.h 1.0 (2005/05/03)
**
**  © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

/****************************************************************************
*** Generation **************************************************************
****************************************************************************/

#define FC_Preference                           "Preference"
#define FC_PreferenceEditor                     "PreferenceEditor"
#define FC_PreferenceGroup                      "PreferenceGroup"

#define PreferenceObject                        F_NewObj(FC_Preference
#define PreferenceEditorObject                  F_NewObj(FC_PreferenceEditor
#define PreferenceGroupObject                   F_NewObj(FC_PreferenceGroup
		
/****************************************************************************
*** Types *******************************************************************
****************************************************************************/

typedef struct FeelinPreferenceScript
{
	STRPTR                          Name;
	ULONG                           Type;
	STRPTR                          Attribute;
	APTR                            Default;
	ULONG                           Length;
}
FPreferenceScript;

/****************************************************************************
*** Messages ****************************************************************
****************************************************************************/

struct  FS_Preference_Find                      { STRPTR Name; };
struct  FS_Preference_Add                       { STRPTR Name; APTR Data; ULONG Size; };
struct  FS_Preference_AddLong                   { STRPTR Name; ULONG Data; };
struct  FS_Preference_AddString                 { STRPTR Name; STRPTR Data; };
struct  FS_Preference_Remove                    { STRPTR Name; };
struct  FS_Preference_Read                      { STRPTR Name; };
struct  FS_Preference_Write                     { STRPTR Name; };
struct  FS_Preference_Resolve                   { STRPTR Name; APTR Default; };
struct  FS_Preference_ResolveInt                { STRPTR Name; ULONG Default; };

struct  FS_PreferenceGroup_Load                 { FObject Prefs; ULONG id_Resolve; ULONG id_ResolveInt; };
struct  FS_PreferenceGroup_Save                 { FObject Prefs; ULONG id_Add; ULONG id_AddLong; ULONG id_AddString; };

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack()
#endif
#endif /* __MORPHOS__ */

#endif
