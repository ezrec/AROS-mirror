// FileTypesPrefs_proto.h
// $Date$
// $Revision$


#ifndef FILETYPESPREFS_PROTO_H
#define	FILETYPESPREFS_PROTO_H

//----------------------------------------------------------------------------

#include <DefIcons.h>
#include <defs.h>

//----------------------------------------------------------------------------

// defined in DefIconsPrefs.c
SAVEDS(APTR) INTERRUPT FileTypesActionConstructFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *ltcm);
SAVEDS(void) INTERRUPT FileTypesActionDestructFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(ULONG) INTERRUPT FileTypesActionDisplayFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm);

void SetFileTypesIcon(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *ln);

SAVEDS(ULONG) INTERRUPT SelectFileTypesEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(ULONG) INTERRUPT SelectFileTypesActionEntryHookFunc(struct Hook *hook, Object *o, Msg msg);

SAVEDS(void) INTERRUPT ChangedFileTypesActionMatchMatchHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionMatchCaseHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionMatchOffsetHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);

SAVEDS(void) INTERRUPT ChangedFileTypesActionSearchSearchHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionSearchCaseHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionSearchSkipSpacesHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);

SAVEDS(void) INTERRUPT ChangedFileTypesActionFilesizeHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionMinSizeHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionPatternHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionProtectionHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionDosDeviceHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionDeviceNameHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionContentsHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
SAVEDS(void) INTERRUPT ChangedFileTypesActionDosTypeHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);

SAVEDS(void) INTERRUPT RenameFileTypeHookFunc(struct Hook *hook, APTR obj, Msg *msg);

SAVEDS(void) INTERRUPT AddFileTypeHookFunc(struct Hook *hook, APTR obj, Msg *msg);
SAVEDS(void) INTERRUPT RemoveFileTypeHookFunc(struct Hook *hook, APTR obj, Msg *msg);

SAVEDS(void) INTERRUPT AddFileTypesMethodHookFunc(struct Hook *hook, APTR obj, Msg msg);
SAVEDS(void) INTERRUPT AddFileTypesActionHookFunc(struct Hook *hook, APTR obj, Msg msg);
SAVEDS(void) INTERRUPT RemoveFileTypesActionHookFunc(struct Hook *hook, APTR obj, Msg *msg);

SAVEDS(APTR) INTERRUPT FileTypesConstructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_ConstructMessage *ltcm);
SAVEDS(void) INTERRUPT FileTypesDestructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DestructMessage *ltdm);
SAVEDS(ULONG) INTERRUPT FileTypesDisplayFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DisplayMessage *ltdm);

SAVEDS(void) INTERRUPT FileTypesActionDragDropSortHookFunc(struct Hook *hook, APTR obj, Msg msg);
SAVEDS(void) INTERRUPT LearnFileTypeHookFunc(struct Hook *hook, APTR obj, Msg msg);

SAVEDS(void) INTERRUPT CreateNewFileTypesIconHookFunc(struct Hook *hook, APTR obj, Msg msg);
SAVEDS(void) INTERRUPT EditFileTypesIconHookFunc(struct Hook *hook, APTR obj, Msg msg);

BOOL DisplayFileTypesIcon(struct FileTypesPrefsInst *inst, CONST_STRPTR FileTypesName);

//----------------------------------------------------------------------------

// defined in FileTypesPrefs.c
CONST_STRPTR GetAttributeName(enum ftAttributeType Type);
STRPTR GetAttributeValueString(const struct FtAttribute *fta, char *Buffer, size_t BuffLen);
void HideEmptyNodes(struct FileTypesPrefsInst *inst);
STRPTR GetLocString(ULONG MsgId);
void UpdateMenuImage(struct FileTypesPrefsInst *inst, ULONG ListTree, struct FileTypesListEntry *fte);
void CleanupFoundNodes(struct FileTypesPrefsInst *inst);

//----------------------------------------------------------------------------

// defined in ReadFtPrefs.c
void ReadFileTypes(struct FileTypesPrefsInst *inst,
	CONST_STRPTR LoadName, CONST_STRPTR DefIconsName);
struct FtAttribute *AddAttribute(struct FileTypesListEntry *fte,
	enum ftAttributeType AttrType, ULONG DataLength, const void *AttrData);
CONST_STRPTR GetNameFromEntryType(enum ftAttributeType EntryType);
void GenerateFileTypesNewEntry(struct FileTypesPrefsInst *inst, CONST_STRPTR Name);

//----------------------------------------------------------------------------

// defined in WriteFtPrefs.c
LONG WriteFileTypes(struct FileTypesPrefsInst *inst, CONST_STRPTR SaveName, ULONG Flags);

//----------------------------------------------------------------------------

// defined in DefIcons.c
LONG InitDefIcons(struct FileTypesPrefsInst *inst, CONST_STRPTR DefIconPrefsName);
void CleanupDefIcons(struct FileTypesPrefsInst *inst);
BOOL WriteDefIconsPrefs(struct FileTypesPrefsInst *inst, CONST_STRPTR FileName);

//----------------------------------------------------------------------------

#if defined(__SASC)
int snprintf(char *, size_t, const char *, /*args*/ ...);
int vsnprintf(char *, size_t, const char *, va_list ap);
#endif /* __SASC */

//----------------------------------------------------------------------------

#endif /* FILETYPESPREFS_PROTO_H */
