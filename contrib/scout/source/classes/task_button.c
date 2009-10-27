#include "system_headers.h"

struct TaskButtonData
{
    struct Task *tbd_Task;
    struct Task *tbd_ExecTask;
    BOOL tbd_IsProcessPointer;
    struct TaskEntry tbd_Entry;
};

static BOOL IsValidTask( struct Task *task )
{
    struct Task *t;

    Forbid();

    if (SysBase->ThisTask == task) {
        Permit();
        return TRUE;
    }

    ITERATE_LIST(&SysBase->TaskWait, struct Task *, t) {
        if (t == task) {
            Permit();
            return TRUE;
        }
    }

    ITERATE_LIST(&SysBase->TaskReady, struct Task *, t) {
        if (t == task) {
            Permit();
            return TRUE;
        }
    }

    Permit();

    return FALSE;
}

static void UpdateButton( struct IClass *cl,
                          Object *obj )
{
    struct TaskButtonData *tbd = INST_DATA(cl, obj);
    TEXT tmp[FILENAME_LENGTH];

    if (tbd->tbd_IsProcessPointer && tbd->tbd_Task != NULL) {
        tbd->tbd_ExecTask = (struct Task *)((ULONG)tbd->tbd_Task - sizeof(struct Task));
    } else {
        tbd->tbd_ExecTask = tbd->tbd_Task;
    }

    Forbid();

    if (tbd->tbd_ExecTask != NULL && !IsValidTask(tbd->tbd_ExecTask)) {
        // oops, that task has died
        tbd->tbd_ExecTask = NULL;
    }

    GetTaskName(tbd->tbd_ExecTask, tmp, sizeof(tmp));

    Permit();

    if (strcmp(tmp, txtNoTask) == 0) {
        MySetContents(obj, "$%08lx", tbd->tbd_Task);
    } else {
        MySetContentsHealed(obj, "$%08lx: " MUIX_B "%s", tbd->tbd_Task, tmp);
    }
    set(obj, MUIA_Disabled, (tbd->tbd_Task == NULL) ? TRUE : FALSE);
}

static ULONG mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct TaskButtonData *tbd;

    if (!(obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg))) {
        return 0;
    }

    tbd = INST_DATA(cl, obj);
    tbd->tbd_Task = (struct Task *)GetTagData(MUIA_TaskButton_Task, (ULONG)NULL, msg->ops_AttrList);
    tbd->tbd_IsProcessPointer = GetTagData(MUIA_TaskButton_IsProcessPointer, FALSE, msg->ops_AttrList);

    UpdateButton(cl, obj);

    DoMethod(obj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Self, 1, MUIM_TaskButton_ShowTask);

    return (ULONG)obj;
}

static ULONG mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct TaskButtonData *tbd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;
    BOOL update = FALSE;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem(&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_TaskButton_Task:
                tbd->tbd_Task = (struct Task *)tag->ti_Data;
                update = TRUE;
                break;

            case MUIA_TaskButton_IsProcessPointer:
                tbd->tbd_IsProcessPointer = tag->ti_Data;
                update = TRUE;
                break;
        }
    }

    if (update) UpdateButton(cl, obj);

    return DoSuperMethodA(cl, obj, (Msg)msg);
}


static ULONG mGet( struct IClass *cl,
                   Object *obj,
                   struct opGet *msg )
{
    struct TaskButtonData *tbd = INST_DATA(cl, obj);
    ULONG *store = msg->opg_Storage;

    switch (msg->opg_AttrID) {
        case MUIA_TaskButton_Task:
            *store = (ULONG)tbd->tbd_Task; return (TRUE);
            break;
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

static ULONG mAskMinMax( struct IClass *cl,
                         Object *obj,
                         struct MUIP_AskMinMax *msg )
{
    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth = _font(obj)->tf_XSize * 11;
    msg->MinMaxInfo->DefWidth = _font(obj)->tf_XSize * 11;

    return 0;
}

static ULONG mShowTask( struct IClass *cl,
                        Object *obj,
                        Msg msg )
{
    struct TaskButtonData *tbd = INST_DATA(cl, obj);
    TEXT ctask[ADDRESS_LENGTH];
    struct Task *task;

    _snprintf(ctask, sizeof(ctask), "$%08lx", tbd->tbd_ExecTask);
    if ((task = MyFindTask(ctask)) != NULL) {
        APTR parentWin;
        APTR taskWin;

        parentWin = (APTR)xget(obj, MUIA_WindowObject);

        GetTaskEntry(task, &tbd->tbd_Entry, FALSE);
        if ((taskWin = TasksDetailWindowObject,
                MUIA_Window_ParentWindow, parentWin,
                MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
            End) != NULL) {
            COLLECT_RETURNIDS;
            SetAttrs(taskWin, MUIA_TasksDetailWin_Task, &tbd->tbd_Entry,
                              MUIA_Window_Open, TRUE,
                              TAG_DONE);
            REISSUE_RETURNIDS;
        }
    } else {
        MyRequest(msgErrorContinue, msgCantFindTask);
    }

    return 0;
}

DISPATCHER(TaskButtonDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                   return (mNew(cl, obj, (APTR)msg));
        case OM_SET:                   return (mSet(cl, obj, (APTR)msg));
        case OM_GET:                   return (mGet(cl, obj, (APTR)msg));
        case MUIM_AskMinMax:           return (mAskMinMax(cl, obj, (APTR)msg));
        case MUIM_TaskButton_ShowTask: return (mShowTask(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}
DISPATCHER_END

APTR MakeTaskButtonClass( void )
{
    return MUI_CreateCustomClass(NULL, MUIC_Text, NULL, sizeof(struct TaskButtonData), DISPATCHER_REF(TaskButtonDispatcher));
}

