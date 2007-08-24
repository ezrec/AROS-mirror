/* FC_List */

/* not implemented yet */

/*
#define FA_List_AutoVisible
#define FA_List_DragSortable
#define FA_List_DropMark
#define FA_List_InsertPosition
#define FA_List_MinLineHeight
#define FA_List_MultiTestHook
#define FA_List_ShowDropMarks
*/

#define FV_List_CursorType_None          0
#define FV_List_CursorType_Bar           1
#define FV_List_CursorType_Rect          2

enum    {

        FV_List_Select_Active = -1,
        FV_List_Select_All    = -2,
        FV_List_Select_Off    = 0,
        FV_List_Select_On     = 1,
        FV_List_Select_Toggle = 2,
        FV_List_Select_Ask    = 3

        };

enum    {

        FV_List_Move_Top      =  0,
        FV_List_Move_Active   = -1,
        FV_List_Move_Bottom   = -2,
        FV_List_Move_Next     = -3,     // for 2nd parameter only
        FV_List_Move_Previous = -4      // for 2nd parameter only

        };  

enum    {

        FV_List_Exchange_Top      =  0,
        FV_List_Exchange_Active   = -1,
        FV_List_Exchange_Bottom   = -2,
        FV_List_Exchange_Next     = -3,     // for 2nd parameter only
        FV_List_Exchange_Previous = -4      // for 2nd parameter only

        };  

enum    {

        FV_List_Jump_Top    =  0,
        FV_List_Jump_Active = -1,
        FV_List_Jump_Bottom = -2,
        FV_List_Jump_Down   = -3,
        FV_List_Jump_Up     = -4

        };

