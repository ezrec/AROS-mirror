/*
    MUI custom window class to display MUIMine best times
*/

#include "MUIMine.h"
#include "MFStrings.h"
#include "LevelData.h"
#include "BTWindow.h"


/*
    instance data for BTWindow class
*/
struct BTWindowData
{
    struct LevelDataList * Levels;  // level data to show best times for
    int HighlightRank;              // rank to highlight in current level
    ULONG DisplayedLevel;           // level whose best times are shown
    Object * Times[3], * Names[3];  // text objects for times and names
    Object * LevelCycle;            // cycle gadget to select level to show
    STRPTR * CycleLabels;           // labels for the cycle gadget
};


/*
    prototypes
*/
static void SetLevel(struct BTWindowData * data, ULONG level);
static void SetLevelIdx(struct BTWindowData * data, int idx);
static void FillBestTimeText(struct BTWindowData * data, struct LevelData * pLevel);
static void ResetBestTimes(struct LevelData * pLevel);

/*
    function :    OM_NEW method handler for MFWindow class
*/
static ULONG mNew(struct IClass *cl, Object *obj, struct opSet *msg)
{
    int i;
    struct LevelData * pLevel;
    struct LevelDataList * lList;
    STRPTR * clabels, * pstr;
    STRPTR closebuttlabel;
    Object *times[3], *names[3], *lcycle,
           *closebutt, *resetlbutt, *resetabutt;

    closebuttlabel = GetStr(MSG_CLOSEBUTT_LABEL);

    /*
        get the level data list initialization attribute,
        fail if not found or there are no levels in the list
    */
    lList = (struct LevelDataList *)GetTagData(MUIA_BTWindow_LevelList, NULL,
                                                msg->ops_AttrList);
    if (lList == NULL  ||  lList->NumLevels == 0
                       ||  lList->LevelList == NULL)
    {
        return NULL;
    }

    /*
        allocate the level cycle gadget label array,
        fail if allocation fails
    */
    clabels = (STRPTR *)AllocVec((lList->NumLevels + 1) * sizeof(STRPTR), 0);
    if (clabels == NULL)
    {
        return NULL;
    }

    /*
        initialize the cycle gadget's labels
    */
    for (i = lList->NumLevels, pLevel = lList->LevelList, pstr = clabels;
         i > 0;
         i--, pLevel++, pstr++)
    {
        *pstr = pLevel->Name;
    }
    *pstr = NULL;

    /*
        create the root object for the window
    */
    obj = (Object *)DoSuperNew(cl, obj,
            MUIA_Window_Title, GetStr(MSG_BTWINDOW_TITLE),
            WindowContents, VGroup,
                Child, lcycle = CycleObject,
                    MUIA_Cycle_Entries, clabels,
                    MUIA_CycleChain, TRUE,
                    MUIA_ShortHelp, GetStr(MSG_BTLEVELCYCLE_HELP),
                    End,

                Child, ColGroup(3),
                    TextFrame,
                    MUIA_Background, MUII_TextBack,
                    MUIA_Group_SameWidth, FALSE,
                    MUIA_ShortHelp, GetStr(MSG_BTLIST_HELP),

                    Child, TextObject,
                        MUIA_Background, MUII_TextBack,
                        MUIA_Text_Contents, "",
                        End,
                    Child, TextObject,
                        MUIA_Background, MUII_TextBack,
                        MUIA_Text_SetMax, TRUE,
                        MUIA_Text_Contents, GetStr(MSG_TIMECOL_HEADING),
                        End,
                    Child, TextObject,
                        MUIA_Background, MUII_TextBack,
                        MUIA_Text_Contents, GetStr(MSG_NAMECOL_HEADING),
                        End,

                    Child, TextObject,
                        MUIA_Background, MUII_TextBack,
                        MUIA_Text_SetMax, TRUE,
                        MUIA_Text_Contents, "1. ",
                        End,
                    Child, times[0] = TextObject,
                        MUIA_Background, MUII_TextBack,
                        MUIA_Text_PreParse, "\033r",
                        End,
                    Child, names[0] = TextObject,
                        MUIA_Background, MUII_TextBack,
                        End,

                    Child, TextObject,
                        MUIA_Background, MUII_TextBack,
                        MUIA_Text_SetMax, TRUE,
                        MUIA_Text_Contents, "2. ",
                        End,
                    Child, times[1] = TextObject,
                        MUIA_Background, MUII_TextBack,
                        MUIA_Text_PreParse, "\033r",
                        End,
                    Child, names[1] = TextObject,
                        MUIA_Background, MUII_TextBack,
                        End,

                    Child, TextObject,
                        MUIA_Background, MUII_TextBack,
                        MUIA_Text_SetMax, TRUE,
                        MUIA_Text_Contents, "3. ",
                        End,
                    Child, times[2] = TextObject,
                        MUIA_Background, MUII_TextBack,
                        MUIA_Text_PreParse, "\033r",
                        End,
                    Child, names[2] = TextObject,
                        MUIA_Background, MUII_TextBack,
                        End,

                    End,

                Child, HGroup,
                    MUIA_Group_SameSize, TRUE,
                    Child, closebutt = TextObject,
                        ButtonFrame,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_Contents, closebuttlabel + 2,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_ControlChar, (LONG)*closebuttlabel,
                        MUIA_Text_HiChar, (LONG)*closebuttlabel,
                        MUIA_CycleChain, TRUE,
                        MUIA_ShortHelp, GetStr(MSG_BTCLOSEBUTT_HELP),
                        End,

                    Child, resetlbutt = TextObject,
                        ButtonFrame,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_Text_Contents, GetStr(MSG_RESETLEVELBUTT_TEXT),
                        MUIA_Text_PreParse, "\033c",
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_ShortHelp, GetStr(MSG_RESETLEVELBUTT_HELP),
                        End,

                    Child, resetabutt = TextObject,
                        ButtonFrame,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_Contents, GetStr(MSG_RESETALLBUTT_TEXT),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_ShortHelp, GetStr(MSG_RESETALLBUTT_HELP),
                        End,

                    End,

                End,

            TAG_MORE, msg->ops_AttrList);

    if (obj)
    {
        struct BTWindowData * data = INST_DATA(cl, obj);

        data->Levels = lList;
        data->HighlightRank = (int)GetTagData(MUIA_BTWindow_HighlightRank,
                                                MUIV_BTWindow_HighlightRank_None,
                                                msg->ops_AttrList);
        data->LevelCycle = lcycle;
        data->CycleLabels = clabels;
        data->DisplayedLevel = 0;

        for (i = 0; i < 3; i++)
        {
            data->Times[i] = times[i];
            data->Names[i] = names[i];
        }

        /*
            make the close button the default object
        */
        SetAttrs(obj, MUIA_Window_DefaultObject, closebutt, TAG_DONE);

        /*
            set the best times and names text for the current level
        */
        SetLevel(data, MUIV_BTWindow_Level_Current);

        /*
            set up notifications
        */
        DoMethod(closebutt, MUIM_Notify, MUIA_Pressed, FALSE,
                    obj, 3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);

        DoMethod(lcycle, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
                    obj, 3, MUIM_Set, MUIA_BTWindow_LevelIdx, MUIV_TriggerValue);

        DoMethod(resetlbutt, MUIM_Notify, MUIA_Pressed, FALSE,
                    obj, 1, MUIM_BTWindow_ResetLevel);

        DoMethod(resetabutt, MUIM_Notify, MUIA_Pressed, FALSE,
                    obj, 1, MUIM_BTWindow_ResetAll);
    }
    else
    {
        /*
            if we could not create the window object then delete
            the cycle gadget labels
        */
        FreeVec(clabels);
    }

    /*
        return the object (or NULL)
    */
    return (ULONG)obj;
}


/*
    function :    OM_DELETE method handler for BTWindow class
*/
static ULONG mDispose(struct IClass *cl, Object *obj, Msg msg)
{
    struct BTWindowData *data = INST_DATA(cl, obj);

    if (data->CycleLabels)
    {
        FreeVec((APTR)data->CycleLabels);
    }

    return DoSuperMethodA(cl, obj, msg);
}


/*
    function :    OM_SET method handler for BTWindow class
*/
static ULONG mSet(struct IClass *cl, Object *obj, struct opSet * msg)
{
    struct BTWindowData *data = INST_DATA(cl,obj);
    struct TagItem *tags, *tag;

    for (tags = msg->ops_AttrList; tag = NextTagItem(&tags); )
    {
        switch (tag->ti_Tag)
        {
            case MUIA_BTWindow_Level:
            {
                SetLevel(data, (int)tag->ti_Data);
                break;
            }

            case MUIA_BTWindow_LevelIdx:
            {
                SetLevelIdx(data, (int)tag->ti_Data);
                break;
            }
        }
    }

    return DoSuperMethodA(cl, obj, (APTR)msg);
}

/*
    function :    OM_GET method handler for BTWindow class
*/
static ULONG mGet(struct IClass *cl, Object *obj, struct opGet * msg)
{
    struct BTWindowData *data = INST_DATA(cl, obj);
    ULONG *store = msg->opg_Storage;

    if (msg->opg_AttrID == MUIA_BTWindow_Level)
    {
        *store = (ULONG)data->DisplayedLevel;
        return TRUE;
    }

    return DoSuperMethodA(cl, obj, (APTR)msg);
}



/*
    function :    MUIM_BTWindow_ResetLevel method handler for BTWindow class
*/
static ULONG mResetLevel(struct IClass *cl, Object *obj, Msg msg)
{
    struct BTWindowData *data = INST_DATA(cl, obj);
    struct LevelData * pLevel;
    /*
        check if there is a level displayed to be reset
    */
    pLevel = FindLevel(data->Levels, data->DisplayedLevel);
    if (pLevel)
    {
        struct Object * app = NULL;
        /*
            get application object
        */
        DoMethod(obj, OM_GET, MUIA_ApplicationObject, &app);

        /*
            query the reset
        */
        if (MUI_RequestA(app, obj, 0, GetStr(MSG_BTWINDOW_TITLE),
                                      GetStr(MSG_YESNO_GADGETS),
                                      GetStr(MSG_QUERY_RESETLEVEL),
                                      NULL))
        {
            /*
                reset the best time data for this level and set
                the window's close request
            */
            ResetBestTimes(pLevel);
            SetAttrs(obj, MUIA_Window_CloseRequest, TRUE, TAG_DONE);
        }
    }

    return 0;
}


/*
    function :    MUIM_BTWindow_ResetAll method handler for BTWindow class
*/
static ULONG mResetAll(struct IClass *cl, Object *obj, Msg msg)
{
    struct Object * app = NULL;
    struct BTWindowData *data = INST_DATA(cl, obj);

    /*
        get application object
    */
    DoMethod(obj, OM_GET, MUIA_ApplicationObject, &app);

    /*
        query the reset
    */
    if (MUI_RequestA(app, obj, 0, GetStr(MSG_BTWINDOW_TITLE),
                                  GetStr(MSG_YESNO_GADGETS),
                                  GetStr(MSG_QUERY_RESETALL),
                                  NULL))
    {
        /*
            reset the best time for all levels then set the
            window's close request attribute
        */
        int i;
        struct LevelData * pLevel = data->Levels->LevelList;
        for (i = data->Levels->NumLevels; i > 0; i--)
        {
            ResetBestTimes(pLevel++);
        }

        SetAttrs(obj, MUIA_Window_CloseRequest, TRUE, TAG_DONE);
    }

    return 0;
}


/*
    function :    class dispatcher
*/
#ifndef __AROS__
SAVEDS ASM ULONG BTWindowDispatcher(
        REG(a0) struct IClass *cl,
        REG(a2) Object *obj,
        REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, BTWindowDispatcher,
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
        case OM_SET:     return mSet    (cl, obj, (APTR)msg);
        case OM_GET:     return mGet    (cl, obj, (APTR)msg);

        case MUIM_BTWindow_ResetLevel:
            return mResetLevel(cl, obj, (APTR)msg);

        case MUIM_BTWindow_ResetAll:
            return mResetAll(cl, obj, (APTR)msg);
    }

    return DoSuperMethodA(cl, obj, msg);

    AROS_USERFUNC_EXIT
}


/*
    function :    sets the level shown to the given level, checks for the
                  special value MUIV_BTWindow_Level_Current and sets the
                  level to the current level if it exists or to the first
                  levele in the list if there is no current level, if the
                  level given is not a special value then searches the
                  level list for the given level and if found then sets
                  the level to the given level otherwise no action is taken

                  if a level is set the cycle gadget is set to that level and
                  the text objects are filled with the best time data for the
                  level

    parameters :  data = pointer to the BTWindowData
                  level = the level to set to

    return :      none
*/
static void SetLevel(struct BTWindowData * data, ULONG level)
{
    struct LevelData * pLevel;

    /*
        check for special values
    */
    if (level == MUIV_BTWindow_Level_Current)
    {
        /*
            set the level to the current level if it exists
            otherwise set to the first level in the list
        */
        pLevel = FindCurrentLevel(data->Levels);
        if (!pLevel)
        {
            pLevel = data->Levels->LevelList;
        }
    }
    else
    {
        /*
            level is not a special value, search for the given
            level in the level list
        */
        pLevel = FindLevel(data->Levels, level);
    }

    /*
        check if we found a level to set window contents to
    */
    if (pLevel)
    {
        int idx;
        struct LevelData * p;
        ULONG useLevel = pLevel->Level;
        /*
            step through level list until we reach this level
            counting the index along the way
        */
        for (idx = 0, p = data->Levels->LevelList;
             idx < data->Levels->NumLevels;
             idx++, p++)
        {
            if (p->Level == useLevel)
            {
                break;
            }
        }

        /*
            set the cycle gadget to the index of this level
            do this without a notification so that the best
            time text will not bell filled for a second time
        */
        SetAttrs(data->LevelCycle, MUIA_NoNotify, TRUE,
                                   MUIA_Cycle_Active, idx,
                                   TAG_DONE);

        /*
            fill the best time text objects with this level's
            best time data
        */
        FillBestTimeText(data, pLevel);
    }
}


/*
    function :    sets the best time text to the best time for the level
                  at the given zero-based index in the level list
                  the cycle gadget is not set here as this function is only
                  called as a result of the cycle gadget active entry changing

    parameters :  data = pointer to the BTWindow data
                  idx = index of level to set

    return :      none
*/
static void SetLevelIdx(struct BTWindowData * data, int idx)
{
    /*
        check for valid index
    */
    if (idx >= 0  &&  idx < data->Levels->NumLevels)
    {
        /*
            fill the best time text objects with this level's
            best time data
        */
        FillBestTimeText(data, &data->Levels->LevelList[idx]);
    }
}


/*
    function :    fills the best time text objects with the data from
                  the given level

    parameters :  data = pointer to the BTWindowData
                  pLevel = pointer to the LevelData

    return :      none
*/
static void FillBestTimeText(struct BTWindowData * data, struct LevelData * pLevel)
{
    /*
        check if the given level is different from the level
        currently displayed
    */
    if (pLevel->Level != data->DisplayedLevel)
    {
        int i;
        char timestr[12];
        char namestr[BT_NAME_SIZE + 8];
        struct BestTime * p;
        int hiRank = -1;

        /*
            if this is the game's current level then set the highlight rank
        */
        if (pLevel->Level == data->Levels->CurrentLevel)
        {
            hiRank = data->HighlightRank;
        }

        /*
            for each of the three best times for the level
        */
        for (i = 0, p = pLevel->BestTimes; i < 3; i++, p++)
        {
            /*
                check if best time for this rank has been set
            */
            if (p->Time > 0)
            {
                /*
                    if this is the highlighted rank the set the text for
                    the rank to bold otherwise display it normally
                */
                if (i == hiRank)
                {
                    sprintf(timestr, "\033b%d ", p->Time);
                    sprintf(namestr, "\033b%s",  p->Name);
                }
                else
                {
                    sprintf(timestr, "%d ", p->Time);
                    strcpy(namestr, p->Name);
                }
            }
            else
            {
                /*
                    best time for this rank has not been set
                */
                strcpy(timestr, "- ");
                strcpy(namestr, "-");
            }
            /*
                set the text for this rank
            */
            SetAttrs(data->Times[i], MUIA_Text_Contents, timestr, TAG_DONE);
            SetAttrs(data->Names[i], MUIA_Text_Contents, namestr, TAG_DONE);
        }

        /*
            set the displayed level for the window
        */
        data->DisplayedLevel = pLevel->Level;
    }
}


/*
    function :    resets the best time data for a level

    parameters :  pLevel = pointer to level to reset best times for

    return :      none
*/
static void ResetBestTimes(struct LevelData * pLevel)
{
    int i;
    struct BestTime * p;

    for (i = 0, p = pLevel->BestTimes; i < 3; i++, p++)
    {
        p->Time = -1;
        p->Name[0] = 0;
    }
}


/*
    function :    creates the BTWindow MUI custom class

    return :      pointer to the created custom class or NULL
*/
struct MUI_CustomClass * CreateBTWindowClass()
{
    return MUI_CreateCustomClass(NULL, MUIC_Window, NULL,
                                       sizeof(struct BTWindowData),
                                       BTWindowDispatcher);
}

/*
    function :    deletes of the BTWindow custom class

    parameters :  mcc = pointer to the BTWindow MUI_CustomClass to delete
*/
void DeleteBTWindowClass(struct MUI_CustomClass * mcc)
{
    MUI_DeleteCustomClass(mcc);
}


