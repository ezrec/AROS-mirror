/*
    MUI custom window class to edit MUIMine difficulty levels
*/

#include "MUIMine.h"
#include "MFStrings.h"
#include "LevelData.h"
#include "LEWindow.h"


/*
    structure for holding the level data that is editable
*/
struct EditLevelData
{
    char  Name[LEVEL_NAME_SIZE];
    ULONG Level;
};


/*
    instance data for LEWindow class
*/
struct LEWindowData
{
    struct LevelDataList * GivenLevels; // origional level data list
    struct LevelDataList * ReturnLevels;// new level data list after editing
    int NumEditLevels;                  // number of level in edit data list
    struct EditLevelData * EditData;    // edit data list
    struct EditLevelData * SelectedLevel;// selected level or NULL
    struct EditLevelData   UndoData;    // edit level data for undo
    Object * LevelList;                 // level list object
    Object * LevelListview;             // level list view object
    Object * NameString;                // name edit string object
    Object * WidthString;               // width edit string object
    Object * HeightString;              // height edit string object
    Object * MinesString;               // mines edit string object
};


/********************************************************************

    private class methods

********************************************************************/

#define MUIM_LEWindow_UndoLevel         (LEWINDOW_TAG_BASE | 0x0801)
#define MUIM_LEWindow_UndoAll           (LEWINDOW_TAG_BASE | 0x0802)
#define MUIM_LEWindow_AddLevel          (LEWINDOW_TAG_BASE | 0x0803)
#define MUIM_LEWindow_DeleteLevel       (LEWINDOW_TAG_BASE | 0x0804)
#define MUIM_LEWindow_ReturnLevels      (LEWINDOW_TAG_BASE | 0x0805)
#define MUIM_LEWindow_SelectLevel       (LEWINDOW_TAG_BASE | 0x0806)
#define MUIM_LEWindow_EditUpdate        (LEWINDOW_TAG_BASE | 0x0807)


struct EditUpdateParams{ ULONG MethodID; LONG type; };
#define MUIV_LEWindow_EditUpdate_Name   1
#define MUIV_LEWindow_EditUpdate_Width  2
#define MUIV_LEWindow_EditUpdate_Height 3
#define MUIV_LEWindow_EditUpdate_Mines  4



/********************************************************************

    prototypes

********************************************************************/

struct EditLevelData * CreateEditData(struct LevelDataList * lList);
void SetEditData(struct LEWindowData * data, struct EditLevelData * edData,
                                             int nLevels);
void SetInitialEditData(struct LEWindowData * data);
void FillLevelList(struct LEWindowData * data);

struct EditLevelData * GetSelectedLevel(struct LEWindowData * data);
LONG GetLevelIndex(struct LEWindowData * data, struct EditLevelData * edData);
LONG SelectListLevel(struct LEWindowData * data, struct EditLevelData * edData);
void SetEditStrings(struct LEWindowData * data);
int CheckCurrentEditData(Object * obj, struct LEWindowData * data);

void SetEditLevel(struct LEWindowData * data, struct EditLevelData * edData);


/*
    defines for and CheckCurrentEditData() return values
*/
#define CHECK_OK                0
#define CHECK_NONAME            1
#define CHECK_NOWIDTH           2
#define CHECK_NOHEIGHT          3
#define CHECK_NOMINES           4
#define CHECK_TOOMANYMINES      5
#define CHECK_DUPLICATE_LEVEL   6
#define CHECK_DUPLICATE_NAME    7



/*
    display function and hook for level list object
*/
#ifndef __AROS__
SAVEDS ASM LONG LevelListDisplayFunc(REG(a2) char **array,
                                     REG(a1) struct EditLevelData *data)
#else
AROS_UFH3(LONG, LevelListDisplayFunc,
 AROS_UFHA(struct Hook *    	 , hook  , A0),
 AROS_UFHA(char **               , array , A2),
 AROS_UFHA(struct EditLevelData *, data  , A1))
#endif
{
    AROS_USERFUNC_INIT

    if (data)
    {
        static char width[8], height[8], mines[8];

        sprintf(width,  "%d", (int)LVL_WIDTH(data->Level));
        sprintf(height, "%d", (int)LVL_HEIGHT(data->Level));
        sprintf(mines,  "%d", (int)LVL_MINES(data->Level));
        *array++ = data->Name;
        *array++ = width;
        *array++ = height;
        *array   = mines;
    }
    else
    {
        *array++ = GetStr(MSG_LEVELNAMECOL_HEADING);
        *array++ = GetStr(MSG_WIDTHCOL_HEADING);
        *array++ = GetStr(MSG_HEIGHTCOL_HEADING);
        *array   = GetStr(MSG_MINESCOL_HEADING);
    }

    return 0;

    AROS_USERFUNC_EXIT
}

static const struct Hook LevelListDisplayHook =
{
    { NULL,NULL }, (void *)LevelListDisplayFunc ,NULL, NULL
};


/*
    function :    OM_NEW method handler for MFWindow class
*/
static ULONG mNew(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct LevelDataList *givList, *retList;
    STRPTR okbuttlabel, cancelbuttlabel, addbuttlabel, delbuttlabel,
           undothisbuttlabel, undoallbuttlabel, nameeditlabel, widtheditlabel,
           heighteditlabel, mineseditlabel;
    Object *levelList, *levelListview, *nameString, *widthString,
           *heightString, *minesString, *addButt, *delButt, *undotButt,
           *undoaButt, *okButt, *cancelButt;

    /*
        get the control characters and labels for the various gadgets
    */
    okbuttlabel = GetStr(MSG_OKBUTT_LABEL);
    cancelbuttlabel = GetStr(MSG_CANCELBUTT_LABEL);
    addbuttlabel = GetStr(MSG_ADDBUTT_LABEL);
    delbuttlabel = GetStr(MSG_DELETEBUTT_LABEL);
    undothisbuttlabel = GetStr(MSG_UNDOTHISBUTT_LABEL);
    undoallbuttlabel = GetStr(MSG_UNDOALLBUTT_LABEL);
    nameeditlabel = GetStr(MSG_NAMEEDIT_LABEL);
    widtheditlabel = GetStr(MSG_WIDTHEDIT_LABEL);
    heighteditlabel = GetStr(MSG_HEIGHTEDIT_LABEL);
    mineseditlabel = GetStr(MSG_MINESEDIT_LABEL);

    /*
        get the given level data list initialization attribute,
        fail if not found
    */
    givList = (struct LevelDataList *)GetTagData(MUIA_LEWindow_GivenLevels,
                                                 NULL, msg->ops_AttrList);
    if (givList == NULL)
    {
        return NULL;
    }

    /*
        get the return level data list initialization attribute,
        fail if not found
    */
    retList = (struct LevelDataList *)GetTagData(MUIA_LEWindow_ReturnLevels,
                                                 NULL, msg->ops_AttrList);
    if (retList == NULL)
    {
        return NULL;
    }

    /*
        create the window object
    */
    obj = (Object *)DoSuperNew(cl, obj,
            MUIA_Window_Title, GetStr(MSG_LEWINDOW_TITLE),
            WindowContents, HGroup,
                Child, VGroup,
                    MUIA_HorizWeight, 100,
                    Child, levelListview = ListviewObject,
                        MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
                        MUIA_CycleChain, TRUE,
                        MUIA_ShortHelp, GetStr(MSG_LEVELLIST_HELP),
                        MUIA_Listview_List, levelList = ListObject,
                            InputListFrame,
                            MUIA_List_Format, "BAR,BAR,BAR,",
                            MUIA_List_Title, TRUE,
                            MUIA_List_DragSortable, TRUE,
                            MUIA_List_AutoVisible, TRUE,
                            MUIA_List_DisplayHook, &LevelListDisplayHook,
                            End,
                        End,

                    Child, HGroup,
                        Child, TextObject,
                            StringFrame,
                            MUIA_FramePhantomHoriz, TRUE,
                            MUIA_Text_Contents, nameeditlabel + 2,
                            MUIA_Text_PreParse, "\033r",
                            MUIA_Text_HiChar, (LONG)*nameeditlabel,
                            MUIA_Text_SetMax, TRUE,
                            End,

                        Child, nameString = StringObject,
                            StringFrame,
                            MUIA_ControlChar, (LONG)*nameeditlabel,
                            MUIA_String_MaxLen, LEVEL_NAME_SIZE,
                            MUIA_String_AdvanceOnCR, TRUE,
                            MUIA_CycleChain, TRUE,
                            MUIA_ShortHelp, GetStr(MSG_NAMEEDIT_HELP),
                            End,

                        End,

                    Child, HGroup,
                        MUIA_Group_SameWidth, TRUE,
                        Child, TextObject,
                            StringFrame,
                            MUIA_FramePhantomHoriz, TRUE,
                            MUIA_Text_Contents, widtheditlabel + 2,
                            MUIA_Text_PreParse, "\033r",
                            MUIA_Text_HiChar, (LONG)*widtheditlabel,
                            End,

                        Child, widthString = StringObject,
                            StringFrame,
                            MUIA_ControlChar, (LONG)*widtheditlabel,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_MaxLen, 3,
                            MUIA_String_AdvanceOnCR, TRUE,
                            MUIA_CycleChain, TRUE,
                            MUIA_ShortHelp, GetStr(MSG_WIDTHEDIT_HELP),
                            End,

                        Child, TextObject,
                            StringFrame,
                            MUIA_FramePhantomHoriz, TRUE,
                            MUIA_Text_Contents, heighteditlabel + 2,
                            MUIA_Text_PreParse, "\033r",
                            MUIA_Text_HiChar, (LONG)*heighteditlabel,
                            End,

                        Child, heightString = StringObject,
                            StringFrame,
                            MUIA_ControlChar, (LONG)*heighteditlabel,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_MaxLen, 3,
                            MUIA_String_AdvanceOnCR, TRUE,
                            MUIA_CycleChain, TRUE,
                            MUIA_ShortHelp, GetStr(MSG_HEIGHTEDIT_HELP),
                            End,

                        Child, TextObject,
                            StringFrame,
                            MUIA_FramePhantomHoriz, TRUE,
                            MUIA_Text_Contents, mineseditlabel + 2,
                            MUIA_Text_PreParse, "\033r",
                            MUIA_Text_HiChar, (LONG)*mineseditlabel,
                            End,

                        Child, minesString = StringObject,
                            StringFrame,
                            MUIA_ControlChar, (LONG)*mineseditlabel,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_MaxLen, 4,
                            MUIA_String_AdvanceOnCR, TRUE,
                            MUIA_CycleChain, TRUE,
                            MUIA_ShortHelp, GetStr(MSG_MINESEDIT_HELP),
                            End,

                        End,

                    End,

                Child, VGroup,
                    MUIA_HorizWeight, 25,
                    Child, addButt = TextObject,
                        ButtonFrame,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Text_Contents, addbuttlabel + 2,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_HiChar, (LONG)*addbuttlabel,
                        MUIA_ControlChar, (LONG)*addbuttlabel,
                        MUIA_ShortHelp, GetStr(MSG_ADDBUTT_HELP),
                        End,

                    Child, delButt = TextObject,
                        ButtonFrame,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Text_Contents, delbuttlabel + 2,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_HiChar, (LONG)*delbuttlabel,
                        MUIA_ControlChar, (LONG)*delbuttlabel,
                        MUIA_ShortHelp, GetStr(MSG_DELETEBUTT_HELP),
                        End,

                    Child, VSpace(0),

                    Child, undotButt = TextObject,
                        ButtonFrame,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Text_Contents, undothisbuttlabel + 2,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_HiChar, (LONG)*undothisbuttlabel,
                        MUIA_ControlChar, (LONG)*undothisbuttlabel,
                        MUIA_ShortHelp, GetStr(MSG_UNDOTHISBUTT_HELP),
                        End,

                    Child, undoaButt = TextObject,
                        ButtonFrame,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Text_Contents, undoallbuttlabel + 2,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_HiChar, (LONG)*undoallbuttlabel,
                        MUIA_ControlChar, (LONG)*undoallbuttlabel,
                        MUIA_ShortHelp, GetStr(MSG_UNDOALLBUTT_HELP),
                        End,

                    Child, VSpace(0),

                    Child, okButt = TextObject,
                        ButtonFrame,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Text_Contents, okbuttlabel + 2,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_HiChar, (LONG)*okbuttlabel,
                        MUIA_ControlChar, (LONG)*okbuttlabel,
                        MUIA_CycleChain, TRUE,
                        MUIA_ShortHelp, GetStr(MSG_LE_OKBUTT_HELP),
                        End,

                    Child, cancelButt = TextObject,
                        ButtonFrame,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Text_Contents, cancelbuttlabel + 2,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_HiChar, (LONG)*cancelbuttlabel,
                        MUIA_ControlChar, (LONG)*cancelbuttlabel,
                        MUIA_ShortHelp, GetStr(MSG_LE_CANCELBUTT_HELP),
                        MUIA_CycleChain, TRUE,
                        End,

                    End,

                End,
            MUIA_Window_DefaultObject, okButt,
            TAG_MORE, msg->ops_AttrList);


    if (obj)
    {
        /*
            the window object was successfully created
        */
        struct LEWindowData * data = INST_DATA(cl, obj);

        /*
            initialize the window's instance data
        */
        data->GivenLevels = givList;        // given level data list
        data->ReturnLevels = retList;       // return level data list
        data->NumEditLevels = 0;            // number of level in edit data list
        data->EditData = NULL;              // edit data list
        data->SelectedLevel = NULL;         // selected level
        data->LevelList = levelList;        // level list object
        data->LevelListview = levelListview;// level list view object
        data->NameString = nameString;      // name edit string object
        data->WidthString = widthString;    // width edit string object
        data->HeightString = heightString;  // height edit string object
        data->MinesString = minesString;    // mines edit string object

        /*
            initialize the return level data list to empty
        */
        retList->NumLevels = 0;
        retList->LevelList = NULL;

        /*
            setup the initial edit data and fill the level list
        */
        SetInitialEditData(data);

        /*
            set up notifications
        */
        DoMethod(cancelButt, MUIM_Notify, MUIA_Pressed, FALSE,
                    obj, 3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);

        DoMethod(okButt, MUIM_Notify, MUIA_Pressed, FALSE,
                    obj, 1, MUIM_LEWindow_ReturnLevels);

        DoMethod(undotButt, MUIM_Notify, MUIA_Pressed, FALSE,
                    obj, 1, MUIM_LEWindow_UndoLevel);

        DoMethod(undoaButt, MUIM_Notify, MUIA_Pressed, FALSE,
                    obj, 1, MUIM_LEWindow_UndoAll);

        DoMethod(addButt, MUIM_Notify, MUIA_Pressed, FALSE,
                    obj, 1, MUIM_LEWindow_AddLevel);

        DoMethod(delButt, MUIM_Notify, MUIA_Pressed, FALSE,
                    obj, 1, MUIM_LEWindow_DeleteLevel);

        DoMethod(levelList, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
                    obj, 1, MUIM_LEWindow_SelectLevel);

        DoMethod(nameString, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                    obj, 2, MUIM_LEWindow_EditUpdate, MUIV_LEWindow_EditUpdate_Name);

        DoMethod(widthString, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                    obj, 2, MUIM_LEWindow_EditUpdate, MUIV_LEWindow_EditUpdate_Width);

        DoMethod(heightString, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                    obj, 2, MUIM_LEWindow_EditUpdate, MUIV_LEWindow_EditUpdate_Height);

        DoMethod(minesString, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                    obj, 2, MUIM_LEWindow_EditUpdate, MUIV_LEWindow_EditUpdate_Mines);
    }

    /*
        return the object (or NULL)
    */
    return (ULONG)obj;
}


/*
    function :    OM_DELETE method handler for LEWindow class
*/
static ULONG mDispose(struct IClass *cl, Object *obj, Msg msg)
{
    struct LEWindowData *data = INST_DATA(cl, obj);

    if (data->EditData)
    {
        FreeVec((APTR)data->EditData);
    }

    return DoSuperMethodA(cl, obj, msg);
}


/*
    function :    OM_SET method handler for LEWindow class
*/
/*
static ULONG mSet(struct IClass *cl, Object *obj, struct opSet * msg)
{
    struct LEWindowData *data = INST_DATA(cl,obj);
    struct TagItem *tags, *tag;

    for (tags = msg->ops_AttrList; tag = NextTagItem(&tags); )
    {
        switch (tag->ti_Tag)
        {
        }
    }

    return DoSuperMethodA(cl, obj, (APTR)msg);
}
*/
/*
    function :    OM_GET method handler for LEWindow class
*/
/*
static ULONG mGet(struct IClass *cl, Object *obj, struct opGet * msg)
{
    struct LEWindowData *data = INST_DATA(cl, obj);
    ULONG *store = msg->opg_Storage;

    if (msg->opg_AttrID == MUIA_LEWindow_Level)
    {
        *store = (ULONG)data->DisplayedLevel;
        return TRUE;
    }

    return DoSuperMethodA(cl, obj, (APTR)msg);
}
*/


/*
    function :  MUIM_LEWindow_UndoLevel method handler for LEWindow class

                This Method reinitializes the edit strings with the level
                data from the currently selected level (if any) undoing
                any changes that have been made since the level was last
                selected.
*/
static ULONG mUndoLevel(struct IClass *cl, Object *obj, Msg msg)
{
    struct LEWindowData *data = INST_DATA(cl, obj);
    if (data->SelectedLevel)
    {
        LONG idx;
        *data->SelectedLevel = data->UndoData;
        SetEditStrings(data);

        idx = GetLevelIndex(data, data->SelectedLevel);;
        if (idx >= 0)
        {
            DoMethod(data->LevelList, MUIM_List_Redraw, idx);
        }
    }
    SetAttrs(obj, MUIA_Window_ActiveObject, data->NameString, TAG_DONE);

    return 0;
}

/*
    function :  MUIM_LEWindow_UndoAll method handler for LEWindow class

                This method deletes any existing edit data, reinitializes
                the edit data from the given level data list and rebuilds
                the level list.
*/
static ULONG mUndoAll(struct IClass *cl, Object *obj, Msg msg)
{
    struct LEWindowData *data = INST_DATA(cl, obj);

    DoMethod(data->LevelList, MUIM_KillNotify, MUIA_List_Active);

    SetInitialEditData(data);

    DoMethod(data->LevelList, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
                        obj, 1, MUIM_LEWindow_SelectLevel);

    SetAttrs(obj, MUIA_Window_ActiveObject, data->LevelListview, TAG_DONE);
    return 0;
}

/*
    function :  MUIM_LEWindow_AddLevel method handler for LEWindow class

                This method reallocates the edit data to add one extra level
                at the end initialized to defaults. The level list is rebuilt
                and the new level is selected for editing.
*/
static ULONG mAddLevel(struct IClass *cl, Object *obj, Msg msg)
{
    struct LEWindowData *data = INST_DATA(cl, obj);

    /*
        check for valid current edit level
    */
    if (CheckCurrentEditData(obj, data) == CHECK_OK)
    {
        /*
            attempt to allocate a new edit data buffer for one level
            more than the current number of levels in the level list
        */
        LONG n = 0;
        struct EditLevelData * newEdData;

        DoMethod(data->LevelList, OM_GET, MUIA_List_Entries, &n);
        n++;
        newEdData = (struct EditLevelData *)AllocVec(
                                    n * sizeof(struct EditLevelData),
                                    MEMF_CLEAR);
        if (newEdData)
        {
            /*
                read the level list and copy the existing edit level data
                to the new edit data
            */
            LONG i;
            struct EditLevelData *p1, *p2;

            for (i = 0, p1 = newEdData; i < n - 1; i++, p1++)
            {
                p2 = NULL;
                DoMethod(data->LevelList, MUIM_List_GetEntry, i, &p2);
                if (p2)
                {
                    *p1 = *p2;
                }
            }

            /*
                delete the old edit data and set the new
            */
            FreeVec((APTR)data->EditData);
            data->EditData = newEdData;
            data->NumEditLevels = n;

            /*
                fill the level list with the new edit level data,
                select the last entry in the list and make it the
                current edit level
            */
            DoMethod(data->LevelList, MUIM_KillNotify, MUIA_List_Active);
            FillLevelList(data);
            DoMethod(data->LevelList, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
                        obj, 1, MUIM_LEWindow_SelectLevel);
            p1 = &newEdData[n - 1];
            SelectListLevel(data, p1);
            SetEditLevel(data, p1);
            SetAttrs(obj, MUIA_Window_ActiveObject, data->NameString,
                          TAG_DONE);
        }
    }

    return 0;
}

/*
    function :  MUIM_LEWindow_DeleteLevel method handler for LEWindow class

                This metod checks if there is a level selected in the level
                list and if there is asks the user if they really want to
                delete the level. If the user confirms that they want to
                delete the level then the edit data is reallocated without
                the selected level and the level list is rebuilt.
*/
static ULONG mDeleteLevel(struct IClass *cl, Object *obj, Msg msg)
{
    struct LEWindowData *data = INST_DATA(cl, obj);
    struct EditLevelData * selLevel = GetSelectedLevel(data);

    /*
        make sure we have a selected level
    */
    if (selLevel)
    {
        /*
            get the number of entries in the list and make sure it is
            greater than zero (it should be since we have a selected
            entry)
        */
        LONG n = 0;
        DoMethod(data->LevelList, OM_GET, MUIA_List_Entries, &n);
        if (n > 0)
        {
            LONG i;
            /*
                query the user if they really want to delete the level
            */
            if (*selLevel->Name == 0)
            {
                i = MUI_RequestA(_app(obj), obj, 0,
                                 GetStr(MSG_LEWINDOW_TITLE),
                                 GetStr(MSG_YESNO_GADGETS),
                                 GetStr(MSG_QUERY_DELETEUNNAMED),
                                 NULL);
            }
            else
            {
                ULONG nameptr = (ULONG)selLevel->Name;
                i = MUI_RequestA(_app(obj), obj, 0,
                                 GetStr(MSG_LEWINDOW_TITLE),
                                 GetStr(MSG_YESNO_GADGETS),
                                 GetStr(MSG_QUERY_DELETELEVEL_FMT),
                                 &nameptr);
            }
            if (i)
            {
                /*
                    attempt to allocate a new edit data buffer with one level
                    less than the current number of levels in the level list
                */
                struct EditLevelData * newEdData;
                n--;
                newEdData = (struct EditLevelData *)AllocVec(
                                            n * sizeof(struct EditLevelData),
                                            MEMF_CLEAR);
                if (newEdData)
                {
                    /*
                        read the level list and copy the existing edit level
                        data to the new edit data making sure we skip the
                        selected level
                    */
                    struct EditLevelData *p1, *p2;

                    p1 = newEdData;
                    for (i = 0; i <= n; i++)
                    {
                        p2 = NULL;
                        DoMethod(data->LevelList, MUIM_List_GetEntry, i, &p2);
                        if (p2  &&  p2 != selLevel)
                        {
                            *p1++ = *p2;
                        }
                    }

                    /*
                        delete the old edit data and set the new
                    */
                    FreeVec((APTR)data->EditData);
                    data->EditData = newEdData;
                    data->NumEditLevels = n;

                    /*
                        fill the level list with the new edit level data
                        and deselect any selected data
                    */
                    DoMethod(data->LevelList, MUIM_KillNotify, MUIA_List_Active);
                    FillLevelList(data);
                    DoMethod(data->LevelList, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
                                obj, 1, MUIM_LEWindow_SelectLevel);
                    SetEditLevel(data, NULL);
                    SetAttrs(obj, MUIA_Window_ActiveObject, data->LevelListview,
                                  TAG_DONE);
                }
            }
        }
    }

    return 0;
}

/*
    function :  MUIM_LEWindow_ReturnLevels method handler for LEWindow class

                This method first checks if there is a current edit level
                and if there is it attempts to valid the current edit data.
                If the current edit data is validated ok then the current
                edit level is updated with the edit data otherwise an error
                message is given and the current level is not updated.

                Once the current edit level (if any) is updated the return
                level data list is built from the edit data and the given
                level data list. The MUIA_Window_CloseRequest attribute of
                the window is then set to TRUE to signal the close of the
                window.
*/
static ULONG mReturnLevels(struct IClass *cl, Object *obj, Msg msg)
{
    struct LEWindowData *data = INST_DATA(cl, obj);

    /*
        check for valid current edit data
    */
    if (CheckCurrentEditData(obj, data) == CHECK_OK)
    {
        /*
            get the number of entries in the level list
        */
        LONG n = 0;
        DoMethod(data->LevelList, OM_GET, MUIA_List_Entries, &n);
        if (n)
        {
            /*
                attempt to initialize the return level data list
            */
            if (InitLevelDataList(data->ReturnLevels, data->NumEditLevels))
            {
                LONG i;
                struct EditLevelData * pEd;
                struct LevelData *pLv1, *pLv = data->ReturnLevels->LevelList;

                /*
                    copy the edit data to the new level list
                */
                for (i = 0; i < data->NumEditLevels; i++)
                {
                    /*
                        get this list entry's data
                    */
                    pEd = NULL;
                    DoMethod(data->LevelList, MUIM_List_GetEntry, i, &pEd);
                    if (pEd)
                    {
                        strcpy(pLv->Name, pEd->Name);
                        pLv->Level = pEd->Level;
                    }
                    pLv++;
                }

                /*
                    for each level in the origional level data list look
                    for the same level in the new level data list and
                    when found copy the origional best time dat to the
                    new list
                */
                pLv = data->GivenLevels->LevelList;
                for (i = data->GivenLevels->NumLevels; i > 0; i--)
                {
                    pLv1 = FindLevel(data->ReturnLevels, pLv->Level);
                    if (pLv1)
                    {
                        int j;
                        struct BestTime *p1, *p2;

                        p1 = pLv1->BestTimes;
                        p2 = pLv->BestTimes;
                        for (j = 3; j > 0; j--)
                        {
                            *p1++ = *p2++;
                        }
                    }
                    pLv++;
                }
            }
        }

        /*
            set the window's close request so that the window will be closed
        */
        SetAttrs(obj, MUIA_Window_CloseRequest, TRUE, TAG_DONE);
    }

    return 0;
}

/*
    function :  MUIM_LEWindow_SelectLevel method handler for LEWindow class

                This method first checks if the level selected in the level
                list differs from the current edit level, if they are the
                same then no action is taken.

                Next attempts to update the current edit data, if the current
                edit data cannot be saved then a warning message is given and
                the problem data reselected for correction.

                Once the current level (if any) is updated the level that is
                now selected in the list becomes the currnt edit level and
                the level edit strings are set with the new level's data.
*/
static ULONG mSelectLevel(struct IClass *cl, Object *obj, Msg msg)
{
    struct LEWindowData *data = INST_DATA(cl, obj);
    struct EditLevelData * selLevel = GetSelectedLevel(data);

    /*
        check that the selected level is different from the current
        edit level
    */
    if (selLevel != data->SelectedLevel)
    {
        /*
            check for valid current edit data
        */
        if (CheckCurrentEditData(obj, data) == CHECK_OK)
        {
            /*
                make the selected level the current edit level, set up the
                edit strings for the level
            */
            SetEditLevel(data, selLevel);
        }
    }

    return 0;
}

/*
    function :  handler for MUIM_LEWindow_EditUpdate
*/
static ULONG mEditUpdate(struct IClass * cl, Object * obj, struct EditUpdateParams * msg)
{
    struct LEWindowData *data = INST_DATA(cl, obj);

    if (data->SelectedLevel)
    {
        BOOL update = FALSE;

        switch (msg->type)
        {
            case MUIV_LEWindow_EditUpdate_Name:
            {
                STRPTR p = NULL;
                DoMethod(data->NameString, OM_GET, MUIA_String_Contents, &p);
                if (p)
                {
                    strcpy(data->SelectedLevel->Name, p);
                    update = TRUE;
                }
                break;
            }

            case MUIV_LEWindow_EditUpdate_Width:
            {
                LONG w = -1;
                DoMethod(data->WidthString, OM_GET, MUIA_String_Integer, &w);
                if (w >= 0)
                {
                    data->SelectedLevel->Level = LVL_SET_WIDTH(data->SelectedLevel->Level, w);
                    update = TRUE;
                }
                break;
            }

            case MUIV_LEWindow_EditUpdate_Height:
            {
                LONG h = -1;
                DoMethod(data->HeightString, OM_GET, MUIA_String_Integer, &h);
                if (h >= 0)
                {
                    data->SelectedLevel->Level = LVL_SET_HEIGHT(data->SelectedLevel->Level, h);
                    update = TRUE;
                }
                break;
            }

            case MUIV_LEWindow_EditUpdate_Mines:
            {
                LONG m =-1;
                DoMethod(data->MinesString, OM_GET, MUIA_String_Integer, &m);
                if (m >= 0)
                {
                    data->SelectedLevel->Level = LVL_SET_MINES(data->SelectedLevel->Level, m);
                    update = TRUE;
                }
                break;
            }
        }

        /*
            check if updated edit data and if so then update the
            selected level list entry
        */
        if (update)
        {
            LONG idx = GetLevelIndex(data, data->SelectedLevel);
            if (idx >= 0  &&  idx < data->NumEditLevels)
            {
                DoMethod(data->LevelList, MUIM_List_Redraw, idx);
            }
        }
    }

    return 0;
}

/*
    function :    class dispatcher
*/
#ifndef __AROS__
SAVEDS ASM ULONG LEWindowDispatcher(
        REG(a0) struct IClass *cl,
        REG(a2) Object *obj,
        REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, LEWindowDispatcher,
 AROS_UFHA(struct IClass *, cl , A0),
 AROS_UFHA(Object *       , obj, A2),
 AROS_UFHA(Msg            , msg, A1))
#endif
{
    AROS_USERFUNC_INIT

    switch (msg->MethodID)
    {
        case OM_NEW:     return mNew    (cl, obj, (APTR)msg);
        case OM_DISPOSE: return mDispose(cl, obj, (APTR)msg);
//        case OM_SET:     return mSet    (cl, obj, (APTR)msg);
//        case OM_GET:     return mGet    (cl, obj, (APTR)msg);

        case MUIM_LEWindow_UndoLevel:
            return mUndoLevel(cl, obj, (APTR)msg);

        case MUIM_LEWindow_UndoAll:
            return mUndoAll(cl, obj, (APTR)msg);

        case MUIM_LEWindow_AddLevel:
            return mAddLevel(cl, obj, (APTR)msg);

        case MUIM_LEWindow_DeleteLevel:
            return mDeleteLevel(cl, obj, (APTR)msg);

        case MUIM_LEWindow_ReturnLevels:
            return mReturnLevels(cl, obj, (APTR)msg);

        case MUIM_LEWindow_SelectLevel:
            return mSelectLevel(cl, obj, (APTR)msg);

        case MUIM_LEWindow_EditUpdate:
            return mEditUpdate(cl, obj, (APTR)msg);
    }

    return DoSuperMethodA(cl, obj, msg);

    AROS_USERFUNC_EXIT
}

 
/*
    function :    allocates and initialize an EditLevelData array for the
                  level data in the given level data list

    parameters :  lList = pointer to the LevelDataList to create the edit
                          data for

    return :      pointer to the EditLevelData array if allocated and
                  initialized ok, NULL otherwise
*/
struct EditLevelData * CreateEditData(struct LevelDataList * lList)
{
    struct EditLevelData * eld = NULL;

    /*
        make sure there is level data
    */
    if (lList  &&  lList->NumLevels  &&  lList->LevelList)
    {
        /*
            attempt to allocate the edit level data
        */
        eld = (struct EditLevelData *)AllocVec(lList->NumLevels *
                                                sizeof(struct EditLevelData),
                                                0);
        if (eld)
        {
            /*
                initialize the edit level data from the level data list
            */
            int i;
            struct EditLevelData * eld1;
            struct LevelData * pLevel;
            for (i = lList->NumLevels, eld1 = eld, pLevel = lList->LevelList;
                 i > 0;
                 i--, eld1++, pLevel++)
            {
                strcpy(eld1->Name, pLevel->Name);
                eld1->Level = pLevel->Level;
            }
        }
    }

    /*
        return the edit level data or NULL if it was not created
    */
    return eld;
}

/*
    function :    sets the edit data for the window, first checks if there
                  is existing edit data and frees it if it exists, sets the
                  given edit data, clears the level list and rebuilds the
                  level list from the new edit data

    parameters :  data = pointer to the LEWindowData to set the edit data for
                  edData = the new edit data to set for the window
                  nLevels = number of levels in the edit level data array

    return :      none
*/
void SetEditData(struct LEWindowData * data, struct EditLevelData * edData,
                                             int nLevels)
{
    /*
        check if there is existing edit level data and delete it if it does
    */
    if (data->EditData)
    {
        FreeVec((APTR)data->EditData);
    }
    /*
        assign the new edit level data
    */
    data->NumEditLevels = nLevels;
    data->EditData = edData;
}

/*
    function :    sets up the edit data for the window from the given level
                  data list deleting any existing edit data and rebuilds the
                  level list from the newly initialized edit data

    parameters :  data = pointer to the LEWindowData to initialize the edit
                         data for

    return :      none
*/
void SetInitialEditData(struct LEWindowData * data)
{
    /*
        attempt to create the edit data for the given level data list
    */
    struct EditLevelData * eld = CreateEditData(data->GivenLevels);
    if (eld)
    {
        /*
            set the new edit data to the window and rebuild the level list
        */
        SetEditData(data, eld, data->GivenLevels->NumLevels);
        FillLevelList(data);
    }
    /*
        initialize current level to none and initialize edit strings
    */
    SetEditLevel(data, NULL);
}

/*
    function :    clears the level list then fills it from the window's
                  edit data

    parameters :  data = pointer to the LEWindowData of the window to fill
                         the level list of

    return :      none
*/
void FillLevelList(struct LEWindowData * data)
{
    int i;
    struct EditLevelData * eld;

    /*
        set the MUIA_List_Quiet attribute of the level list while it
        is being updated
    */
    SetAttrs(data->LevelList, MUIA_List_Quiet, TRUE, TAG_DONE);

    /*
        clear the level list
    */
    DoMethod(data->LevelList, MUIM_List_Clear);

    /*
        add each level from the edit level data to the list
    */
    for (i = data->NumEditLevels, eld = data->EditData;
         i > 0;
         i--, eld++)
    {
        DoMethod(data->LevelList, MUIM_List_InsertSingle,
                                  (APTR)eld, MUIV_List_Insert_Bottom);
    }

    /*
        clear the MUIA_List_Quiet attribute of the level list now that
        updating is completed
    */
    SetAttrs(data->LevelList, MUIA_List_Quiet, FALSE, TAG_DONE);
}


/*
    function :    checks if there is a selected entry in the level list
                  and if there is get the edit level data for the entry

    parameters :  data = pointer to the LEWindowData

    return :      pointer to the selected level's edit data or NULL
*/
struct EditLevelData * GetSelectedLevel(struct LEWindowData * data)
{
    struct EditLevelData * eld = NULL;

    /*
        get the active entry in the list
    */
    DoMethod(data->LevelList, MUIM_List_GetEntry,
                              MUIV_List_GetEntry_Active, &eld);

    return eld;
}

/*
    function :    Gets the level list index of the given edit level

    parameters :  data = pointer to the LEWindowData
                  edData = pointer to the edit data to find index from

    return :      the list index or -1 if not in list
*/
LONG GetLevelIndex(struct LEWindowData * data, struct EditLevelData * edData)
{
    LONG idx = -1;
    LONG i, n = 0;
    struct EditLevelData * eld;

    /*
        get number of entries in the list
    */
    DoMethod(data->LevelList, OM_GET, MUIA_List_Entries, &n);

    /*
        step through the list looking for the given entry
    */
    for (i = 0; i < n; i++)
    {
        eld = NULL;
        DoMethod(data->LevelList, MUIM_List_GetEntry, i, &eld);
        if (eld  &&  eld == edData)
        {
            /*
                found the entry, set the return index and exit the loop
            */
            idx = i;
            break;
        }
    }

    return idx;
}

/*
    function :    Selects the given edit level in the level list

    parameters :  data = pointer to the LEWindowData
                  edData = pointer to the edit data to select in level list

    return :      the list index of the selected level or -1 if not in list
*/
LONG SelectListLevel(struct LEWindowData * data, struct EditLevelData * edData)
{
    /*
        find the level in the level list
    */
    LONG idx = GetLevelIndex(data, edData);
    if (idx < 0)
    {
        idx = MUIV_List_Active_Off;
    }
    /*
        the level was found in the level list, select it
    */
    SetAttrs(data->LevelList, MUIA_NoNotify, TRUE,
                              MUIA_List_Active, idx,
                              TAG_DONE);

    return idx;
}

/*
    function :    sets the contents of the edit strings from the selected
                  level's edit data

    parameters :  data = pointer to the LEWindowData for this window

    return :      none
*/
void SetEditStrings(struct LEWindowData * data)
{
    struct EditLevelData * eld = data->SelectedLevel;
    /*
        check if there is a current level
    */
    if (eld)
    {
        /*
            set name edit string with the level's name and enable the string
        */
        SetAttrs(data->NameString, MUIA_NoNotify, TRUE,
                                   MUIA_String_Contents, eld->Name,
                                   MUIA_Disabled, FALSE,
                                   TAG_DONE);

        /*
            set the width edit string with the level's width and enable
            the string
        */
        SetAttrs(data->WidthString, MUIA_NoNotify, TRUE,
                                    MUIA_String_Integer, LVL_WIDTH(eld->Level),
                                    MUIA_Disabled, FALSE,
                                    TAG_DONE);

        /*
            set the height edit string with the level's height and enable
            the string
        */
        SetAttrs(data->HeightString, MUIA_NoNotify, TRUE,
                                     MUIA_String_Integer, LVL_HEIGHT(eld->Level),
                                     MUIA_Disabled, FALSE,
                                     TAG_DONE);

        /*
            set the mines edit string with the level's height and enable
            the string
        */
        SetAttrs(data->MinesString, MUIA_NoNotify, TRUE,
                                    MUIA_String_Integer, LVL_MINES(eld->Level),
                                    MUIA_Disabled, FALSE,
                                    TAG_DONE);
    }
    else
    {
        /*
            no selected level, disable the edut strings
        */
        SetAttrs(data->NameString,   MUIA_NoNotify, TRUE,
                                     MUIA_String_Contents, "",
                                     MUIA_Disabled, TRUE, TAG_DONE);
        SetAttrs(data->WidthString,  MUIA_NoNotify, TRUE,
                                     MUIA_String_Contents, "",
                                     MUIA_Disabled, TRUE, TAG_DONE);
        SetAttrs(data->HeightString, MUIA_NoNotify, TRUE,
                                     MUIA_String_Contents, "",
                                     MUIA_Disabled, TRUE, TAG_DONE);
        SetAttrs(data->MinesString,  MUIA_NoNotify, TRUE,
                                     MUIA_String_Contents, "",
                                     MUIA_Disabled, TRUE, TAG_DONE);
    }
}

/*
    function :    checks if there is selected edit data, if there is then
                  checks for valid edit string data, if string data valid
                  then checks for duplicate level and if there is none then
                  updates the selected edit data with the edit string values

    parameter :   obj = pointer to the LEWindow object
                  data = pointer to the LEWindowData

    return :      CHECK_xxx code (see defines above)
*/
int CheckCurrentEditData(Object * obj, struct LEWindowData * data)
{
    int ret = CHECK_OK;
    struct EditLevelData * edData = data->SelectedLevel;
    /*
        check if we have a selected level, return CHECK_OK if not
    */
    if (edData)
    {
        Object * errObj = NULL;
        STRPTR dupname = NULL;
        LONG width, height, mines;

        /*
            check that the name is not the empty string
        */
        if (edData->Name[0] == 0)
        {
            errObj = data->NameString;
            ret = CHECK_NONAME;
        }
        /*
            check that the width is set
        */
        else if ((width = LVL_WIDTH(edData->Level)) == 0)
        {
            errObj = data->WidthString;
            ret = CHECK_NOWIDTH;
        }
        /*
            check that the height is set
        */
        else if ((height = LVL_HEIGHT(edData->Level)) == 0)
        {
            errObj = data->HeightString;
            ret = CHECK_NOHEIGHT;
        }
        /*
            check that the number of mines is set
        */
        else if ((mines = LVL_MINES(edData->Level)) == 0)
        {
            errObj = data->MinesString;
            ret = CHECK_NOMINES;
        }
        /*
            check if there are less mines than cells
        */
        else if (mines >= (height * width))
        {
            errObj = data->MinesString;
            ret = CHECK_TOOMANYMINES;
        }
        /*
            check for duplicate level or name
        */
        else
        {
            int i;
            struct EditLevelData * eld = data->EditData;
            ULONG level = MAKE_LEVEL(width, height, mines);

            for (i = data->NumEditLevels; i > 0; i--, eld++)
            {
                /*
                    check each level except the selected level for a
                    matching level, set the error state and exit the
                    loop if a match is found, other wise check for a
                    duplicate name and set the error state and exit
                    the loop if a duplicate is found
                */
                if (eld != edData)
                {
                    if (eld->Level == level)
                    {
                        errObj = data->WidthString;
                        dupname = eld->Name;
                        ret = CHECK_DUPLICATE_LEVEL;
                        break;
                    }

                    if (strcmp(eld->Name, edData->Name) == 0)
                    {
                        errObj = data->NameString;
                        dupname = eld->Name;
                        ret = CHECK_DUPLICATE_NAME;
                        break;
                    }
                }
            }
        }

        /*
            if we have an error then display the appropriate error
            message, make sure that this level is still selected
            and set the input focus to the error object
        */
        if (ret != CHECK_OK)
        {
            Object * app = _app(obj);
            char errStr[256];
            LONG copyStrId = 0;

            /*
                select the error string for the problem
            */
            switch (ret)
            {
                case CHECK_NONAME:
                    copyStrId = MSG_NOLEVELNAME_ERR;
                    break;

                case CHECK_NOWIDTH:
                case CHECK_NOHEIGHT:
                case CHECK_NOMINES:
                    copyStrId = MSG_UNSETLEVELDATA_ERR;
                    break;

                case CHECK_TOOMANYMINES:
                    copyStrId = MSG_TOOMANYMINES_ERR;
                    break;

                case CHECK_DUPLICATE_LEVEL:
                    sprintf(errStr, GetStr(MSG_DUPLEVEL_ERR_FMT), dupname);
                    break;

                case CHECK_DUPLICATE_NAME:
                    sprintf(errStr, GetStr(MSG_DUPNAME_ERR_FMT), dupname);
                    break;

                default:
                    copyStrId = MSG_UNKNOWNLEVELDATA_ERR;
                    break;
            }

            /*
                if the error message is a simple error without formatting
                then copy it to the error message buffer
            */
            if (copyStrId)
            {
                strcpy(errStr, GetStr(copyStrId));
            }

            /*
                inform the user of the error
            */
            MUI_RequestA(app, obj, 0, GetStr(MSG_LEWINDOW_TITLE),
                                      GetStr(MSG_CONTINUE_GADGET),
                                      errStr, NULL);

            /*
                make sure the Selected level is still selected in the
                level list
            */
            SelectListLevel(data, edData);
            SetEditLevel(data, edData);

            /*
                select the error string object so that corrections
                may be easily made
            */
            SetAttrs(obj, MUIA_Window_ActiveObject, errObj, TAG_DONE);
        }
    }

    return ret;
}


/*
    function :    selects the given level for editing,

    parameters :  data = pointer to the LEWindowData
                  newLevel = pointer to the level edit data
*/
void SetEditLevel(struct LEWindowData * data, struct EditLevelData * newLevel)
{
    /*
        check if level is given, if so select it for editing
        otherwise clears any current selection
    */
    if (newLevel)
    {
        /*
            copy undo data
        */
        data->UndoData = *newLevel;
    }
    else
    {
        /*
            clear undo data
        */
        data->UndoData.Level = 0;
        data->UndoData.Name[0] = 0;
    }

    /*
        set the new selected level
    */
    data->SelectedLevel = newLevel;

    /*
        set edit strings
    */
    SetEditStrings(data);
}


/*
    function :    creates the LEWindow MUI custom class

    return :      pointer to the created custom class or NULL
*/
struct MUI_CustomClass * CreateLEWindowClass()
{
    return MUI_CreateCustomClass(NULL, MUIC_Window, NULL,
                                       sizeof(struct LEWindowData),
                                       LEWindowDispatcher);
}

/*
    function :    deletes of the LEWindow custom class

    parameters :  mcc = pointer to the LEWindow MUI_CustomClass to delete
*/
void DeleteLEWindowClass(struct MUI_CustomClass * mcc)
{
    MUI_DeleteCustomClass(mcc);
}

