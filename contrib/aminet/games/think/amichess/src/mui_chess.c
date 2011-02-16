#ifdef __AROS__

#include <libraries/mui.h>
#include <aros/asmcall.h>
#include <mui/NListview_mcc.h>
#include <mui/NList_mcc.h>

#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#warning "Fix DoSuperNew and NotifyWinCloseSelf"

Object *DoSuperNew(struct IClass *cl,Object *obj,Tag tag1,...);

#define NotifyWinCloseSelf(o) DoMethod(o, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, o, 3, MUIM_Set, MUIA_Window_Open, FALSE)

#else

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/muimaster_protos.h>
#include <mui/NListview_mcc.h>
#include <mui/NList_mcc.h>

#endif

#include <libraries/asl.h>
#include <libraries/gadtools.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

void LoadPieces(char *name);
void LoadBoard(char *name);
void PlaySound(Object *snd,BOOL);

extern char *pieces_folder;
extern struct Screen *wbscreen;
extern struct MUI_CustomClass *MUI_Board_Class;
extern Object *snd_move;
extern ULONG voice,promotepiece;

static Object *mui_menu;

#define find(id) ((APTR)DoMethod(mui_menu,MUIM_FindUData,id))

struct Data
{
Object *win,*col,*mymove,*think,*board,*lv_white,*lv_black;
Object *timewin,*timem,*timet,*times;
Object *promotewin;
Object *tx_nodes,*tx_moves,*tx_score;
Object *evalwin;
Object *tx_w_mat1,*tx_w_mat2,*tx_w_mat3;
Object *tx_b_mat1,*tx_b_mat2,*tx_b_mat3;
Object *tx_eval_score;

struct FileRequester *filereq;
};

enum
{
ID_MENU_New=1,
ID_MENU_OpenEPD,
ID_MENU_OpenPGN,
ID_MENU_SaveEPD,
ID_MENU_SavePGN,
ID_MENU_Autoplay,
ID_MENU_Supervisor,
ID_MENU_Swap,
ID_MENU_Switch,
ID_MENU_Hint,
ID_MENU_Undo,
ID_MENU_Remove,
ID_MENU_ShowThinking,
ID_MENU_NullMove,
ID_MENU_BookAdd,
ID_MENU_BookOff,
ID_MENU_BookOn,
ID_MENU_BookBest,
ID_MENU_BookWorst,
ID_MENU_BookRandom,
ID_MENU_Depth0,
ID_MENU_Depth1,
ID_MENU_Depth2,
ID_MENU_Depth3,
ID_MENU_Depth4,
ID_MENU_Depth5,
ID_MENU_Depth6,
ID_MENU_Depth7,
ID_MENU_Depth8,
ID_MENU_Depth9,
ID_MENU_Depth10,
ID_MENU_Time,
ID_MENU_ReverseBoard,
ID_MENU_Board,
ID_MENU_Pieces,
ID_MENU_Voice,
ID_MENU_Eval,
ID_MENU_Stats,
ID_Promote_Queen,
ID_Promote_Rook,
ID_Promote_Bishop,
ID_Promote_Knight
};

struct MUI_CustomClass *MUI_Chess_Class;

static struct NewMenu nm[]={
{NM_TITLE,"Project"},
{NM_ITEM,"N\0New game",0,0,0,(APTR)ID_MENU_New},
{NM_ITEM,"Open EPD ...",0,0,0,(APTR)ID_MENU_OpenEPD},
{NM_ITEM,"Open PGN ...",0,0,0,(APTR)ID_MENU_OpenPGN},
{NM_ITEM,"Save EPD ...",0,0,0,(APTR)ID_MENU_SaveEPD},
{NM_ITEM,"Save PGN ...",0,0,0,(APTR)ID_MENU_SavePGN},
{NM_ITEM,NM_BARLABEL},
{NM_ITEM,"Q\0Quit",0,0,0,(APTR)MUIV_Application_ReturnID_Quit},
{NM_TITLE,"Game"},
{NM_ITEM,"A\0Autoplay",0,CHECKIT,0,(APTR)ID_MENU_Autoplay},
{NM_ITEM,"V\0Supervisor",0,CHECKIT,0,(APTR)ID_MENU_Supervisor},
{NM_ITEM,"W\0Swap sides",0,0,0,(APTR)ID_MENU_Swap},
{NM_ITEM,"Switch sides",0,0,0,(APTR)ID_MENU_Switch},
{NM_ITEM,"H\0Hint",0,0,0,(APTR)ID_MENU_Hint},
{NM_ITEM,"U\0Undo",0,0,0,(APTR)ID_MENU_Undo},
{NM_ITEM,"R\0Remove",0,0,0,(APTR)ID_MENU_Remove},
{NM_ITEM,NM_BARLABEL},
{NM_ITEM,"Book"},
{NM_SUB,"Add ...",0,0,0,(APTR)ID_MENU_BookAdd},
{NM_SUB,NM_BARLABEL},
{NM_SUB,"Off",0,CHECKIT,~0x04,(APTR)ID_MENU_BookOff},
{NM_SUB,"On",0,CHECKIT|CHECKED,~0x08,(APTR)ID_MENU_BookOn},
{NM_SUB,"Best",0,CHECKIT,~0x10,(APTR)ID_MENU_BookBest},
{NM_SUB,"Worst",0,CHECKIT,~0x20,(APTR)ID_MENU_BookWorst},
{NM_SUB,"Random",0,CHECKIT,~0x40,(APTR)ID_MENU_BookRandom},
{NM_ITEM,"Search depth"},
{NM_SUB,"Use time settings",0,CHECKIT|CHECKED,~0x01,(APTR)ID_MENU_Depth0},
{NM_SUB," 1",0,CHECKIT,~0x02,(APTR)ID_MENU_Depth1},
{NM_SUB," 2",0,CHECKIT,~0x04,(APTR)ID_MENU_Depth2},
{NM_SUB," 3",0,CHECKIT,~0x08,(APTR)ID_MENU_Depth3},
{NM_SUB," 4",0,CHECKIT,~0x10,(APTR)ID_MENU_Depth4},
{NM_SUB," 5",0,CHECKIT,~0x20,(APTR)ID_MENU_Depth5},
{NM_SUB," 6",0,CHECKIT,~0x40,(APTR)ID_MENU_Depth6},
{NM_SUB," 7",0,CHECKIT,~0x80,(APTR)ID_MENU_Depth7},
{NM_SUB," 8",0,CHECKIT,~0x100,(APTR)ID_MENU_Depth8},
{NM_SUB," 9",0,CHECKIT,~0x200,(APTR)ID_MENU_Depth9},
{NM_SUB,"10",0,CHECKIT,~0x400,(APTR)ID_MENU_Depth10},
{NM_ITEM,"Time settings",0,0,0,(APTR)ID_MENU_Time},
{NM_ITEM,NM_BARLABEL},
{NM_ITEM,"T\0Show thinking",0,CHECKIT|CHECKED,0,(APTR)ID_MENU_ShowThinking},
{NM_ITEM,"Null moves",0,CHECKIT|CHECKED,0,(APTR)ID_MENU_NullMove},
{NM_TITLE,"Display"},
{NM_ITEM,"Boards",0,0,0,(APTR)ID_MENU_Board},
{NM_ITEM,"Pieces",0,0,0,(APTR)ID_MENU_Pieces},
{NM_ITEM,NM_BARLABEL},
{NM_ITEM,"Reverse board",0,CHECKIT,0,(APTR)ID_MENU_ReverseBoard},
{NM_ITEM,"Voice",0,CHECKIT,0,(APTR)ID_MENU_Voice},
{NM_TITLE,"Extras"},
{NM_ITEM,"Evaluate",0,0,0,(APTR)ID_MENU_Eval},
{NM_ITEM,"Statistics",0,0,0,(APTR)ID_MENU_Stats},
{NM_END}};

static ULONG playcol[][3]={{0xffffffff,0xffffffff,0xffffffff},{0,0,0}};

static IPTR mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
Object *win,*col,*mymove,*think,*board,*lv_white,*lv_black;
Object *timewin,*timem,*timet,*times;
Object *promotewin,*promote_q,*promote_r,*promote_b,*promote_n;
Object *statswin,*tx_nodes,*tx_moves,*tx_score;
Object *evalwin,*tx_w_mat1,*tx_w_mat2,*tx_w_mat3;
Object *tx_b_mat1,*tx_b_mat2,*tx_b_mat3,*tx_eval_score;

obj=DoSuperNew(cl,obj,
	MUIA_Application_Title,"AmiChess",
	MUIA_Application_Base,"AmiChess",
	MUIA_Application_Author,"Achim Stegemann",
	MUIA_Application_Copyright,"Based on GnuChess 5.04",
	MUIA_Application_Version,"$VER: AmiChess 1.0 (13.11.2002)",
	MUIA_Application_Description,"Amiga port of GNUChess 5.04",
	MUIA_Application_Menustrip,mui_menu=MUI_MakeObject(MUIO_MenustripNM,nm,MUIO_MenustripNM_CommandKeyCheck),
	SubWindow,win=WindowObject,
		MUIA_Window_Title,"AmiChess 1.0 by Achim Stegemann",
		MUIA_Window_Screen,wbscreen,
		WindowContents,VGroup,
			Child,VGroup,
				GroupFrame,
				MUIA_Background,MUII_GroupBack,
				Child,HGroup,
					Child,col=ColorfieldObject,
						TextFrame,
						MUIA_FixWidthTxt,"W",
						MUIA_FixHeightTxt,"W",
						MUIA_Colorfield_RGB,playcol[0],
					End,
					Child,Label1("My move"),
					Child,mymove=TextObject,
						TextFrame,
						MUIA_Font,MUIV_Font_Fixed,
						MUIA_Background,MUII_TextBack,
						MUIA_Text_PreParse,MUIX_PH,
					End,
				End,
				Child,HGroup,
					Child,Label1("Best line"),
					Child,think=TextObject,
						TextFrame,
						MUIA_Font,MUIV_Font_Fixed,
						MUIA_Background,MUII_TextBack,
						MUIA_Text_PreParse,MUIX_PH,
					End,
				End,
			End,
			Child,HGroup,
				GroupFrame,
				MUIA_Background,MUII_GroupBack,
				Child,board=NewObject(MUI_Board_Class->mcc_Class,0,TAG_END),
				Child,lv_white=NListviewObject,
					ReadListFrame,
					MUIA_NListview_NList,NListObject,
						MUIA_Font,MUIV_Font_Fixed,
						MUIA_NList_Input,0,
						MUIA_NList_ConstructHook,MUIV_NList_ConstructHook_String,
						MUIA_NList_DestructHook,MUIV_NList_DestructHook_String,
						MUIA_NList_Title,"White",
					End,
				End,
				Child,lv_black=NListviewObject,
					ReadListFrame,
					MUIA_NListview_NList,NListObject,
						MUIA_Font,MUIV_Font_Fixed,
						MUIA_NList_Input,0,
						MUIA_NList_ConstructHook,MUIV_NList_ConstructHook_String,
						MUIA_NList_DestructHook,MUIV_NList_DestructHook_String,
						MUIA_NList_Title,"Black",
					End,
				End,
			End,
		End,
	End,
	SubWindow,timewin=WindowObject,
		MUIA_Window_Title,"Time settings",
		MUIA_Window_Screen,wbscreen,
		WindowContents,VGroup,
			Child,HGroup,
				Child,timem=StringObject,
					StringFrame,
					MUIA_CycleChain,1,
					MUIA_String_Accept,"0123456789",
					MUIA_String_MaxLen,4,
					MUIA_String_Integer,0,
				End,
				Child,Label2("moves in"),
				Child,timet=StringObject,
					StringFrame,
					MUIA_CycleChain,1,
					MUIA_String_Accept,"0123456789",
					MUIA_String_MaxLen,4,
					MUIA_String_Integer,0,
				End,
				Child,Label2("minutes."),
			End,
			Child,HGroup,
				Child,Label2("Searchtime in sec"),
				Child,times=StringObject,
					StringFrame,
					MUIA_CycleChain,1,
					MUIA_String_Accept,"0123456789",
					MUIA_String_MaxLen,4,
					MUIA_String_Integer,0,
				End,
			End,
		End,
	End,
	SubWindow,promotewin=WindowObject,
		MUIA_Window_Title,"Promote Pawn to ...",
		MUIA_Window_Screen,wbscreen,
		WindowContents,VGroup,
			Child,HGroup,
				Child,promote_q=ImageObject,
					ImageButtonFrame,
					MUIA_CycleChain,1,
					MUIA_FixWidth,56,
					MUIA_FixHeight,56,
					MUIA_Image_FreeVert,1,
					MUIA_InputMode,MUIV_InputMode_RelVerify,
					MUIA_Image_FreeVert,1,
					MUIA_Image_FreeHoriz,1,
					MUIA_Image_Spec,"5:PROGDIR:Promote/Queen",
				End,
				Child,promote_r=ImageObject,
					ImageButtonFrame,
					MUIA_CycleChain,1,
					MUIA_FixWidth,56,
					MUIA_FixHeight,56,
					MUIA_InputMode,MUIV_InputMode_RelVerify,
					MUIA_Image_FreeVert,1,
					MUIA_Image_FreeHoriz,1,
					MUIA_Image_Spec,"5:PROGDIR:Promote/Rook",
				End,
				Child,promote_b=ImageObject,
					ImageButtonFrame,
					MUIA_CycleChain,1,
					MUIA_FixWidth,56,
					MUIA_FixHeight,56,
					MUIA_InputMode,MUIV_InputMode_RelVerify,
					MUIA_Image_FreeVert,1,
					MUIA_Image_FreeHoriz,1,
					MUIA_Image_Spec,"5:PROGDIR:Promote/Bishop",
				End,
				Child,promote_n=ImageObject,
					ImageButtonFrame,
					MUIA_CycleChain,1,
					MUIA_FixWidth,56,
					MUIA_FixHeight,56,
					MUIA_InputMode,MUIV_InputMode_RelVerify,
					MUIA_Image_FreeVert,1,
					MUIA_Image_FreeHoriz,1,
					MUIA_Image_Spec,"5:PROGDIR:Promote/Knight",
				End,
			End,
		End,
	End,
	SubWindow,statswin=WindowObject,
		MUIA_Window_Title,"Statistics",
		MUIA_Window_Screen,wbscreen,
		MUIA_Window_Width,MUIV_Window_Width_Screen(25),
		WindowContents,VGroup,
			Child,VGroup,
				MUIA_Background,MUII_TextBack,
				Child,tx_nodes=TextObject,
					MUIA_Text_Contents,"Nodes: 0 -> 0 per sec",
				End,
				Child,tx_moves=TextObject,
					MUIA_Text_Contents,"Moves: 0 -> 0 per sec",
				End,
				Child,tx_score=TextObject,
					MUIA_Text_Contents,"Score: 0 <-> 0",
				End,
			End,
		End,
	End,
	SubWindow,evalwin=WindowObject,
		MUIA_Window_Title,"Position evaluation",
		MUIA_Window_Screen,wbscreen,
		MUIA_Window_Width,MUIV_Window_Width_Screen(25),
		WindowContents,VGroup,
			Child,VGroup,
				GroupFrameT("White"),
				MUIA_Background,MUII_TextBack,
				Child,tx_w_mat1=TextObject,
					MUIA_Text_Contents,"Mat: 0/0  Dev: 0",
				End,
				Child,tx_w_mat2=TextObject,
					MUIA_Text_Contents,"P: 0  N: 0  B: 0",
				End,
				Child,tx_w_mat3=TextObject,
					MUIA_Text_Contents,"R: 0  Q: 0  K: 0",
				End,
			End,
			Child,VGroup,
				GroupFrameT("Black"),
				MUIA_Background,MUII_TextBack,
				Child,tx_b_mat1=TextObject,
					MUIA_Text_Contents,"Mat: 0/0  Dev: 0",
				End,
				Child,tx_b_mat2=TextObject,
					MUIA_Text_Contents,"P: 0  N: 0  B: 0",
				End,
				Child,tx_b_mat3=TextObject,
					MUIA_Text_Contents,"R: 0  Q: 0  K: 0",
				End,
			End,
			Child,RectangleObject,
				MUIA_VertWeight,0,
				MUIA_Rectangle_HBar,1,
			End,
			Child,tx_eval_score=TextObject,
				TextFrame,
				MUIA_Background,MUII_TextBack,
			End,
		End,
	End,

TAG_MORE,msg->ops_AttrList);
if(obj)
	{
	ULONG i;
	struct Data *data=(struct Data *)INST_DATA(cl,obj);
	struct FileInfoBlock *fib;
	Object *menu,*sub;
	BPTR lock;
	data->win=win;
	data->col=col;
	data->mymove=mymove;
	data->think=think;
	data->lv_white=lv_white;
	data->lv_black=lv_black;
	data->board=board;
	data->timewin=timewin;
	data->timem=timem;
	data->timet=timet;
	data->times=times;
	data->promotewin=promotewin;
	data->tx_nodes=tx_nodes;
	data->tx_moves=tx_moves;
	data->tx_score=tx_score;
	data->evalwin=evalwin;
	data->tx_w_mat1=tx_w_mat1;
	data->tx_w_mat2=tx_w_mat2;
	data->tx_w_mat3=tx_w_mat3;
	data->tx_b_mat1=tx_b_mat1;
	data->tx_b_mat2=tx_b_mat2;
	data->tx_b_mat3=tx_b_mat3;
	data->tx_eval_score=tx_eval_score;
	NotifyWinCloseSelf(evalwin);
	NotifyWinCloseSelf(statswin);
	DoMethod(find(ID_MENU_New),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_New);
	DoMethod(find(ID_MENU_OpenEPD),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_OpenEPD);
	DoMethod(find(ID_MENU_OpenPGN),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_OpenPGN);
	DoMethod(find(ID_MENU_SaveEPD),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_SaveEPD);
	DoMethod(find(ID_MENU_SavePGN),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_SavePGN);
	DoMethod(find(ID_MENU_Autoplay),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Autoplay,MUIV_TriggerValue);
	DoMethod(find(ID_MENU_Supervisor),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Supervisor,MUIV_TriggerValue);
	DoMethod(find(ID_MENU_Swap),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_SwapSides);
	DoMethod(find(ID_MENU_Switch),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_SwitchSides);
	DoMethod(find(ID_MENU_Hint),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_Hint);
	DoMethod(find(ID_MENU_Undo),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_Undo);
	DoMethod(find(ID_MENU_Remove),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_Remove);
	DoMethod(find(ID_MENU_ShowThinking),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Post,MUIV_TriggerValue);
	DoMethod(find(ID_MENU_NullMove),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_NullMove,MUIV_TriggerValue);
	DoMethod(find(ID_MENU_BookAdd),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_BookAdd);
	DoMethod(find(ID_MENU_BookOff),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,1,MUIM_Chess_BookOff);
	DoMethod(find(ID_MENU_BookOn),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,1,MUIM_Chess_BookOn);
	DoMethod(find(ID_MENU_BookBest),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,1,MUIM_Chess_BookBest);
	DoMethod(find(ID_MENU_BookWorst),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,1,MUIM_Chess_BookWorst);
	DoMethod(find(ID_MENU_BookRandom),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,1,MUIM_Chess_BookRandom);
	DoMethod(find(ID_MENU_Depth0),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,0);
	DoMethod(find(ID_MENU_Depth1),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,1);
	DoMethod(find(ID_MENU_Depth2),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,2);
	DoMethod(find(ID_MENU_Depth3),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,3);
	DoMethod(find(ID_MENU_Depth4),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,4);
	DoMethod(find(ID_MENU_Depth5),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,5);
	DoMethod(find(ID_MENU_Depth6),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,6);
	DoMethod(find(ID_MENU_Depth7),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,7);
	DoMethod(find(ID_MENU_Depth8),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,8);
	DoMethod(find(ID_MENU_Depth9),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,9);
	DoMethod(find(ID_MENU_Depth10),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_Depth,10);
	DoMethod(find(ID_MENU_Time),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,timewin,3,MUIM_Set,MUIA_Window_Open,1);
	DoMethod(find(ID_MENU_ReverseBoard),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,2,MUIM_Chess_ReverseBoard,MUIV_TriggerValue);
	DoMethod(find(ID_MENU_Voice),MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,obj,3,MUIM_WriteLong,MUIV_TriggerValue,&voice);
	DoMethod(find(ID_MENU_Eval),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,MUIM_Chess_Eval);
	DoMethod(find(ID_MENU_Stats),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,statswin,3,MUIM_Set,MUIA_Window_Open,1);
	DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,1,obj,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
	DoMethod(timewin,MUIM_Notify,MUIA_Window_CloseRequest,1,MUIV_Notify_Self,3,MUIM_Set,MUIA_Window_Open,0);
	DoMethod(timem,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,obj,2,MUIM_Chess_Time,0);
	DoMethod(timet,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,obj,2,MUIM_Chess_Time,1);
	DoMethod(times,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,obj,2,MUIM_Chess_Time,2);
	DoMethod(promotewin,MUIM_Notify,MUIA_Window_CloseRequest,1,obj,2,MUIM_Application_ReturnID,ID_Promote_Queen);
	DoMethod(promote_q,MUIM_Notify,MUIA_Pressed,0,obj,2,MUIM_Application_ReturnID,ID_Promote_Queen);
	DoMethod(promote_r,MUIM_Notify,MUIA_Pressed,0,obj,2,MUIM_Application_ReturnID,ID_Promote_Rook);
	DoMethod(promote_b,MUIM_Notify,MUIA_Pressed,0,obj,2,MUIM_Application_ReturnID,ID_Promote_Bishop);
	DoMethod(promote_n,MUIM_Notify,MUIA_Pressed,0,obj,2,MUIM_Application_ReturnID,ID_Promote_Knight);
	SetAttrs(promotewin,MUIA_Window_ActiveObject,promote_q,TAG_END);
	data->filereq=(struct FileRequester *)MUI_AllocAslRequestTags(ASL_FileRequest,ASLFR_RejectIcons,TRUE,TAG_END);
	if((fib=(struct FileInfoBlock *)AllocDosObject(DOS_FIB,0)))
		{
		if((lock=Lock("PROGDIR:Boards",SHARED_LOCK)))
			{
			if(Examine(lock,fib)!=DOSFALSE)
				{
				i=0;
				menu=find(ID_MENU_Board);
				while(ExNext(lock,fib)!=DOSFALSE)
					{
					if(fib->fib_DirEntryType>0)
						{
						char *str;
#ifdef __cplusplus
						str=new char[strlen(fib->fib_FileName)+1];
#else
						str=malloc(strlen(fib->fib_FileName)+1);
#endif
						strcpy(str,fib->fib_FileName);
						sub=MenuitemObject,
							MUIA_Menuitem_Checkit,1,
							MUIA_Menuitem_Checked,strcmp(fib->fib_FileName,"Default")?0:1,
							MUIA_Menuitem_Title,str,
							MUIA_Menuitem_Exclude,~(1<<i),
						End;
						DoMethod(menu,OM_ADDMEMBER,sub);
						DoMethod(sub,MUIM_Notify,MUIA_Menuitem_Checked,1,obj,2,MUIM_Chess_Board,sub);
						i++;
						}
					}
				}
			UnLock(lock);
			}
		FreeDosObject(DOS_FIB,fib);
		}
	if((fib=(struct FileInfoBlock *)AllocDosObject(DOS_FIB,0)))
		{
		if(lock=Lock(pieces_folder,SHARED_LOCK))
			{
			if(Examine(lock,fib)!=DOSFALSE)
				{
				i=0;
				menu=find(ID_MENU_Pieces);
				while(ExNext(lock,fib)!=DOSFALSE)
					{
					char *str;
#ifdef __cplusplus
						str=new char[strlen(fib->fib_FileName)+1];
#else
						str=malloc(strlen(fib->fib_FileName)+1);
#endif
					strcpy(str,fib->fib_FileName);
					sub=MenuitemObject,
						MUIA_Menuitem_Checkit,1,
						MUIA_Menuitem_Checked,strcmp(fib->fib_FileName,"Default")?0:1,
						MUIA_Menuitem_Title,str,
						MUIA_Menuitem_Exclude,~(1<<i),
					End;
					DoMethod(menu,OM_ADDMEMBER,sub);
					DoMethod(sub,MUIM_Notify,MUIA_Menuitem_Checked,1,obj,2,MUIM_Chess_Pieces,sub);
					i++;
					}
				}
			UnLock(lock);
			}
		FreeDosObject(DOS_FIB,fib);
		}
	}
return(IPTR)obj;
}

static IPTR mDispose(struct IClass *cl,Object *obj,Msg msg)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
if(data->filereq) MUI_FreeAslRequest(data->filereq);
return DoSuperMethodA(cl,obj,msg);
}

static void mChessNewgame(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
InitVars();
NewPosition();
myrating=opprating=0;
DoMethod(obj,MUIM_Chess_ShowThinking,0);
DoMethod(obj,MUIM_Chess_MyMove,0);
SetAttrs(find(ID_MENU_Autoplay),MUIA_Menuitem_Checked,0,TAG_END);
SetAttrs(find(ID_MENU_Supervisor),MUIA_Menuitem_Checked,0,TAG_END);
DoMethod(obj,MUIM_Chess_ClearList);
DoMethod(obj,MUIM_Chess_ShowBoard);
}

static void mChessOpenEPD(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
if(MUI_AslRequestTags(data->filereq,ASLFR_InitialPattern,"#?",ASLFR_DoSaveMode,0,TAG_END))
	{
	BPTR lock;
	if(lock=Lock(data->filereq->fr_Drawer,SHARED_LOCK))
		{
		BPTR olddir=CurrentDir(lock);
		DoMethod(obj,MUIM_Chess_ClearList);
		LoadEPD(data->filereq->fr_File);
		if(!ValidateBoard())
			{
			SETFLAG(flags,ENDED);
			DoMethod(obj,MUIM_Chess_ShowThinking,"Board is wrong!");
			}
		CurrentDir(olddir);
		UnLock(lock);
		}
	}
}

static void mChessOpenPGN(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
if(MUI_AslRequestTags(data->filereq,ASLFR_InitialPattern,"#?.pgn",ASLFR_DoSaveMode,0,TAG_END))
	{
	BPTR lock;
	if((lock=Lock(data->filereq->fr_Drawer,SHARED_LOCK)))
		{
		BPTR olddir=CurrentDir(lock);
		PGNReadFromFile(data->filereq->fr_File);
		CurrentDir(olddir);
		UnLock(lock);
		}
	}
}

static void mChessSaveEPD(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
if(MUI_AslRequestTags(data->filereq,ASLFR_InitialPattern,"#?",ASLFR_DoSaveMode,1,TAG_END))
	{
	BPTR lock;
	if((lock=Lock(data->filereq->fr_Drawer,SHARED_LOCK)))
		{
		BPTR olddir=CurrentDir(lock);
		SaveEPD(data->filereq->fr_File);
		CurrentDir(olddir);
		UnLock(lock);
		}
	}
}

static void mChessSavePGN(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
if(MUI_AslRequestTags(data->filereq,ASLFR_InitialPattern,"#?.pgn",ASLFR_DoSaveMode,1,TAG_END))
	{
	BPTR lock;
	if((lock=Lock(data->filereq->fr_Drawer,SHARED_LOCK)))
		{
		BPTR olddir=CurrentDir(lock);
		PGNSaveToFile(data->filereq->fr_File,"");
		CurrentDir(olddir);
		UnLock(lock);
		}
	}
}

static void mChessAutoplay(struct MUIP_Chess_Autoplay *msg)
{
if(msg->autoplay) SETFLAG(flags,AUTOPLAY);
else CLEARFLAG(flags,AUTOPLAY);
}

static void mChessSwapSides(Object *obj)
{
if(flags&SUPERVISOR) DoMethod(obj,MUIM_Chess_ShowThinking,"Not possible in supervisor mode.");
else
	{
	CLEARFLAG(flags,TIMEOUT);
	CLEARFLAG(flags,ENDED);
	computer=board.side;
	//SetAttrs(obj,MUIA_Application_Sleep,1,TAG_END);
	Iterate();
	if(flags&ENDED)
		{
		SetAttrs(find(ID_MENU_Autoplay),MUIA_NoNotify,1,MUIA_Menuitem_Checked,0,TAG_END);
		CLEARFLAG(flags,AUTOPLAY);
		}
	//SetAttrs(obj,MUIA_Application_Sleep,0,TAG_END);
	}
}

static void mChessSwitchSides(Object *obj)
{
board.side=1^board.side;
DoMethod(obj,MUIM_Chess_Side);
}

static void mChessHint(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
int pvar[2];
if((flags&USEHASH))
	{
	if(TTGetPV(board.side,2,rootscore,&pvar[2]))
		{
		GenMoves(2); 
		SANMove(pvar[2],2);
		SetAttrs(data->mymove,MUIA_Text_Contents,SANmv,TAG_END);
		}
	}
}

static void mChessUndo(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
if(GameCnt>=0) UnmakeMove(board.side,&Game[GameCnt].move);
else DisplayBeep(0);
MoveLimit[board.side]++;
TimeLimit[board.side]+=Game[GameCnt+1].et;
DoMethod(board.side==white?data->lv_white:data->lv_black,MUIM_NList_Remove,MUIV_NList_Remove_Last);
DoMethod(obj,MUIM_Chess_ShowThinking,0);
DoMethod(obj,MUIM_Chess_Side);
DoMethod(obj,MUIM_Chess_ShowBoard);
}

static void mChessRemove(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
if(GameCnt>=0)
	{
	CLEARFLAG(flags,ENDED);
	CLEARFLAG(flags,TIMEOUT);
	DoMethod(board.side==white?data->lv_white:data->lv_black,MUIM_NList_Remove,MUIV_NList_Remove_Last);
	UnmakeMove(board.side,&Game[GameCnt].move);
	if(GameCnt>=0)
		{
		DoMethod(board.side==white?data->lv_white:data->lv_black,MUIM_NList_Remove,MUIV_NList_Remove_Last);
		UnmakeMove(board.side,&Game[GameCnt].move);
		DoMethod(obj,MUIM_Chess_ShowThinking,0);
		DoMethod(obj,MUIM_Chess_ShowBoard);
		}
	DoMethod(obj,MUIM_Chess_Side);
	}
else DisplayBeep(0);
}

static void mChessSupervisor(struct MUIP_Chess_Supervisor *msg)
{
if(msg->value) SETFLAG(flags,SUPERVISOR);
else CLEARFLAG(flags,SUPERVISOR);
}

static void mChessPost(Object *obj,struct MUIP_Chess_Post *msg)
{
if(msg->value) SETFLAG(flags,POST);
else
	{
	CLEARFLAG(flags,POST);
	DoMethod(obj,MUIM_Chess_ShowThinking,0);
	}
}

static void mChessBookAdd(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
if(MUI_AslRequestTags(data->filereq,ASLFR_InitialPattern,"#?.pgn",ASLFR_DoSaveMode,0,TAG_END))
	{
	BPTR lock;
	if((lock=Lock(data->filereq->fr_Drawer,SHARED_LOCK)))
		{
		BPTR olddir=CurrentDir(lock);
		BookPGNReadFromFile(data->filereq->fr_File);
		CurrentDir(olddir);
		UnLock(lock);
		}
	}
}

static void mChessBookOn()
{
bookmode=BOOKPREFER;
}

static void mChessBookOff()
{
bookmode=BOOKOFF;
}

static void mChessBookBest()
{
bookmode=BOOKBEST;
}

static void mChessBookWorst()
{
bookmode=BOOKWORST;
}

static void mChessBookRandom()
{
bookmode=BOOKRAND;
}

static void mChessDepth(struct MUIP_Chess_Depth *msg)
{
SearchDepth=msg->depth;
}

static void mChessTime(struct IClass *cl,Object *obj,struct MUIP_Chess_Time *msg)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
ULONG val;
char text[50];
switch(msg->flag)
	{
	case 0:
		GetAttr(MUIA_String_Integer,data->timem,&val);
		TCMove=val;
		break;
	case 1:
		GetAttr(MUIA_String_Integer,data->timet,&val);
		TCTime=val;
		break;
	default:
		GetAttr(MUIA_String_Integer,data->times,&val);
		if(!val) val=5;
		SearchTime=val;
	}
if(!TCMove&&!TCTime)
	{
/*	SearchTime=5; */
	sprintf(text,"Searchtime set to %.1f secs.",SearchTime);
	DoMethod(obj,MUIM_Chess_ShowThinking,text);
	CLEARFLAG(flags,TIMECTL);
	}
else
	{
	if(!TCMove)
		{
		TCMove=35;
	/*	printf("TCMove=%d\n",TCMove); */
		suddendeath=1;
		}
	else suddendeath=0;
	if(!TCTime)
		{
		SETFLAG(flags,TIMECTL);
		SearchTime=TCinc/2;
	/*	printf("Fischer increment of %d seconds\n",TCinc); */
		}
	else
		{
		SETFLAG(flags,TIMECTL);
	/*	MoveLimit[white]=MoveLimit[black]=TCMove-(GameCnt+1)/2; */
		MoveLimit[white]=MoveLimit[black]=TCMove;
		TimeLimit[white]=TimeLimit[black]=TCTime*60;
	/*
		printf("Time Control: %d moves in %.1f secs\n",MoveLimit[white],TimeLimit[white]);
		printf("Fischer increment of %d seconds\n",TCinc);
	*/
		}
	sprintf(text,"Time control:%d moves in %.1f secs",MoveLimit[white],TimeLimit[white]);
	DoMethod(obj,MUIM_Chess_ShowThinking,text);
	}
}

static void mChessWinOpen(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
SetAttrs(data->win,MUIA_Window_Open,1,TAG_END);
}

static void	mChessShowBoard(struct IClass *cl,Object *obj,Msg msg)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
DoMethodA(data->board,msg);
}

static void	mChessShowThinking(struct IClass *cl,Object *obj,struct MUIP_Chess_ShowThinking *msg)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
SetAttrs(data->think,MUIA_Text_Contents,msg->line,TAG_END);
}

static void mChessNullMove(struct MUIP_Chess_NullMove *msg)
{
if(msg->value) SETFLAG(flags,USENULL);
else CLEARFLAG(flags,USENULL);
}

static void mChessReverseBoard(Object *obj,struct MUIP_Chess_ReverseBoard *msg)
{
if(msg->reverse) SETFLAG(flags,REVERSEBOARD);
else CLEARFLAG(flags,REVERSEBOARD);
DoMethod(obj,MUIM_Chess_ShowBoard);
}

static void mChessMyMove(struct IClass *cl,Object *obj,struct MUIP_Chess_MyMove *msg)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
SetAttrs(data->mymove,MUIA_Text_Contents,msg->move,TAG_END);
}

static void mChessSide(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
SetAttrs(data->col,MUIA_Colorfield_RGB,playcol[board.side],TAG_END);
}

static void mChessBoard(Object *obj,struct MUIP_Chess_Board *msg)
{
char *name;
GetAttr(MUIA_Menuitem_Title,msg->menu,(IPTR *)&name);
LoadBoard(name);
DoMethod(obj,MUIM_Chess_ShowBoard);
}

static void mChessPieces(Object *obj,struct MUIP_Chess_Pieces *msg)
{
char *name;
GetAttr(MUIA_Menuitem_Title,msg->menu,(IPTR *)&name);
LoadPieces(name);
DoMethod(obj,MUIM_Chess_ShowBoard);
}

static void mChessClearFlag(struct MUIP_Chess_ClearFlag *msg)
{
if(msg->flag==AUTOPLAY)
	{
	SetAttrs(find(ID_MENU_Autoplay),MUIA_NoNotify,1,MUIA_Menuitem_Checked,0,TAG_END);
	CLEARFLAG(flags,AUTOPLAY);
	}
}

static void mChessAddMove(struct IClass *cl,Object *obj,struct MUIP_Chess_AddMove *msg)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
Object *lv=msg->side==white?data->lv_white:data->lv_black;
DoMethod(lv,MUIM_NList_InsertSingle,msg->move,MUIV_NList_Insert_Bottom);
DoMethod(lv,MUIM_NList_Jump,MUIV_NList_Jump_Bottom);
}

static void mChessClearList(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
DoMethod(data->lv_white,MUIM_NList_Clear);
DoMethod(data->lv_black,MUIM_NList_Clear);
}

static void mChessPromote(struct IClass *cl,Object *obj)
{
BOOL ok=0;
struct Data *data=(struct Data *)INST_DATA(cl,obj);
SetAttrs(data->win,MUIA_Window_Sleep,1,TAG_END);
SetAttrs(data->promotewin,MUIA_Window_Open,1,TAG_END);
while(!ok)
	{
	ULONG signals,id;
	id=DoMethod(obj,MUIM_Application_NewInput,&signals);
	switch(id)
		{
		case ID_Promote_Queen:
			promotepiece=0;
			ok=1;
			break;
		case ID_Promote_Rook:
			promotepiece=1;
			ok=1;
			break;
		case ID_Promote_Bishop:
			promotepiece=2;
			ok=1;
			break;
		case ID_Promote_Knight:
			promotepiece=3;
			ok=1;
		}
	if(signals) signals=Wait(signals);
	}
SetAttrs(data->promotewin,MUIA_Window_Open,0,TAG_END);
SetAttrs(data->win,MUIA_Window_Sleep,0,TAG_END);
}

static void mChessEval(struct IClass *cl,Object *obj)
{
char text[100];
int wp,bp,wk,bk;
BitBoard *b;
struct Data *data=(struct Data *)INST_DATA(cl,obj);
SetAttrs(data->evalwin,MUIA_Window_Open,1,TAG_END);
phase=PHASE;
GenAtaks();
FindPins(&pinned);
b=board.b[white];
pieces[white]=b[knight]|b[bishop]|b[rook]|b[queen]; 
b=board.b[black];
pieces[black]=b[knight]|b[bishop]|b[rook]|b[queen]; 
wp=ScoreP(white);
bp=ScoreP(black);
wk=ScoreK(white);
bk=ScoreK(black);
sprintf(text,"Mat: %4d/%4d  Dev: %d",board.pmaterial[white],board.material[white],ScoreDev(white));
SetAttrs(data->tx_w_mat1,MUIA_Text_Contents,text,TAG_END);
sprintf(text,"P: %d  N: %d  B: %d",wp,ScoreN(white),ScoreB(white));
SetAttrs(data->tx_w_mat2,MUIA_Text_Contents,text,TAG_END);
sprintf(text,"R: %d  Q: %d  K: %d",ScoreR(white),ScoreQ(white),wk);
SetAttrs(data->tx_w_mat3,MUIA_Text_Contents,text,TAG_END);

sprintf(text,"Mat: %4d/%4d  Dev: %d",board.pmaterial[black],board.material[black],ScoreDev(black));
SetAttrs(data->tx_b_mat1,MUIA_Text_Contents,text,TAG_END);
sprintf(text,"P: %d  N: %d  B: %d",bp,ScoreN(black),ScoreB(black));
SetAttrs(data->tx_b_mat2,MUIA_Text_Contents,text,TAG_END);
sprintf(text,"R: %d  Q: %d  K: %d",ScoreR(black),ScoreQ(black),bk);
SetAttrs(data->tx_b_mat3,MUIA_Text_Contents,text,TAG_END);

sprintf(text,"Score: %d",(EvaluateDraw()?DRAWSCORE:Evaluate(-INFINITY,INFINITY)));
SetAttrs(data->tx_eval_score,MUIA_Text_Contents,text,TAG_END);
}

static void mChessStats(struct IClass *cl,Object *obj)
{
char text[100];
struct Data *data=(struct Data *)INST_DATA(cl,obj);
sprintf(text,"Nodes: %u -> %u per sec",NodeCnt+QuiesCnt,(ULONG)((NodeCnt+QuiesCnt)/et));
SetAttrs(data->tx_nodes,MUIA_Text_Contents,text,TAG_END);
sprintf(text,"Moves: %u -> %u per sec",GenCnt,(ULONG)(GenCnt/et));
SetAttrs(data->tx_moves,MUIA_Text_Contents,text,TAG_END);
sprintf(text,"Score: %d <-> %d",maxposnscore[white],maxposnscore[black]);
SetAttrs(data->tx_score,MUIA_Text_Contents,text,TAG_END);
}

#ifdef __AROS__
AROS_UFH3S(IPTR, Dispatcher,
    AROS_UFHA(struct IClass *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
{
    AROS_USERFUNC_INIT
#else
#ifdef __GNUC__
static ULONG Dispatcher(struct IClass *cl __asm("a0"),Object *obj  __asm("a2"),Msg msg  __asm("a1"))
#elif defined __VBCC__
static ULONG Dispatcher(__reg("a0") struct IClass *cl,__reg("a2") Object *obj,__reg("a1") Msg msg)
#else
static ULONG Dispatcher(register __a0 struct IClass *cl,register __a2 Object *obj,register __a1 Msg msg)
#endif
{
#endif

ULONG retval;
switch(msg->MethodID)
	{
	case OM_NEW:
		retval=mNew(cl,obj,(struct opSet *)msg);
		break;
	case OM_DISPOSE:
		retval=mDispose(cl,obj,msg);
		break;
	case MUIM_Chess_WinOpen:
		mChessWinOpen(cl,obj);
		break;
	case MUIM_Chess_New:
		mChessNewgame(cl,obj);
		break;
	case MUIM_Chess_OpenEPD:
		mChessOpenEPD(cl,obj);
		break;
	case MUIM_Chess_OpenPGN:
		mChessOpenPGN(cl,obj);
		break;
	case MUIM_Chess_SaveEPD:
		mChessSaveEPD(cl,obj);
		break;
	case MUIM_Chess_SavePGN:
		mChessSavePGN(cl,obj);
		break;
	case MUIM_Chess_Autoplay:
		mChessAutoplay((struct MUIP_Chess_Autoplay *)msg);
		break;
	case MUIM_Chess_SwapSides:
		mChessSwapSides(obj);
		break;
	case MUIM_Chess_SwitchSides:
		mChessSwitchSides(obj);
		break;
	case MUIM_Chess_Hint:
		mChessHint(cl,obj);
		break;
	case MUIM_Chess_Undo:
		mChessUndo(cl,obj);
		break;
	case MUIM_Chess_Remove:
		mChessRemove(cl,obj);
		break;
	case MUIM_Chess_Supervisor:
		mChessSupervisor((struct MUIP_Chess_Supervisor *)msg);
		break;
	case MUIM_Chess_Post:
		mChessPost(obj,(struct MUIP_Chess_Post *)msg);
		break;
	case MUIM_Chess_BookAdd:
		mChessBookAdd(cl,obj);
		break;
	case MUIM_Chess_BookOn:
		mChessBookOn();
		break;
	case MUIM_Chess_BookOff:
		mChessBookOff();
		break;
	case MUIM_Chess_BookBest:
		mChessBookBest();
		break;
	case MUIM_Chess_BookWorst:
		mChessBookWorst();
		break;
	case MUIM_Chess_BookRandom:
		mChessBookRandom();
		break;
	case MUIM_Chess_Depth:
		mChessDepth((struct MUIP_Chess_Depth *)msg);
		break;
	case MUIM_Chess_Time:
		mChessTime(cl,obj,(struct MUIP_Chess_Time *)msg);
		break;
	case MUIM_Chess_ShowBoard:
		mChessShowBoard(cl,obj,msg);
		break;
	case MUIM_Chess_ShowThinking:
		mChessShowThinking(cl,obj,(struct MUIP_Chess_ShowThinking *)msg);
		break;
	case MUIM_Chess_NullMove:
		mChessNullMove((struct MUIP_Chess_NullMove *)msg);
		break;
	case MUIM_Chess_ReverseBoard:
		mChessReverseBoard(obj,(struct MUIP_Chess_ReverseBoard *)msg);
		break;
	case MUIM_Chess_MyMove:
		mChessMyMove(cl,obj,(struct MUIP_Chess_MyMove *)msg);
		break;
	case MUIM_Chess_Side:
		mChessSide(cl,obj);
		break;
	case MUIM_Chess_Board:
		mChessBoard(obj,(struct MUIP_Chess_Board *)msg);
		break;
	case MUIM_Chess_Pieces:
		mChessPieces(obj,(struct MUIP_Chess_Pieces *)msg);
		break;
	case MUIM_Chess_ClearFlag:
		mChessClearFlag((struct MUIP_Chess_ClearFlag *)msg);
		break;
	case MUIM_Chess_AddMove:
		mChessAddMove(cl,obj,(struct MUIP_Chess_AddMove *)msg);
		break;
	case MUIM_Chess_ClearList:
		mChessClearList(cl,obj);
		break;
	case MUIM_Chess_Promote:
		mChessPromote(cl,obj);
		break;
	case MUIM_Chess_Eval:
		mChessEval(cl,obj);
		break;
	case MUIM_Chess_Stats:
		mChessStats(cl,obj);
		break;
	default:
		retval=DoSuperMethodA(cl,obj,msg);
	}
return retval;

#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

#ifdef __cplusplus
extern "C"
#endif
void INIT_6_MUI_Chess_Class()
{
if(!(MUI_Chess_Class=MUI_CreateCustomClass(0,MUIC_Application,0,sizeof(struct Data),(APTR)Dispatcher))) exit(20);
}

#ifdef __cplusplus
extern "C"
#endif
void EXIT_6_MUI_Chess_Class()
{
if(MUI_Chess_Class) MUI_DeleteCustomClass(MUI_Chess_Class);
}

