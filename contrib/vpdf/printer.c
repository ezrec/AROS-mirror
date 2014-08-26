#if defined(__AROS__)
#define MUIMASTER_YES_INLINE_STDARG
#endif

/// System includes
#define AROS_ALMOST_COMPATIBLE
#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <proto/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <exec/libraries.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <mui/Rawimage_mcc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>

#include <proto/alib.h>
#include <proto/utility.h>

////

#include <private/vapor/vapor.h>
#include "util.h"
#include "poppler_printer.h"
#include "printer.h"
#include "pagenumberclass.h"
#include "arrowstring_class.h"
#include "system/functions.h"
#include "locale.h"
#include "settings.h"
#include "poppler.h"
#include "printer_image.h"



struct Data
{
	Object *strOutput;
	Object *sldFirst;
	Object *sldLast;
	Object *cycOddEven;
	Object *txtStatus;
	Object *btnPrint;
	Object *btnCancel;
	Object *chkPS;
	Object *btnPrefs;
	Object *chkAutorotatescale;
	Object *strNumbers;
	Object *grpRight;
	Object *chkReverse;
	Object *chkCollate;
	Object *strCopies;
	Object *cycPSVersion;
	Object *grpCollate;
	Object *btnReset;
	Object *grpPage;
	Object *gauge;
	Object *cycPageScaling;
	Object *cycPagesPerSheet;
	Object *cycPagesOrder;
	Object *proc;
	
	void *doc;
	char status[256];
	int printing;
	int quit;
	int close;
	int max;
	
	struct printerjob pj;

};


#define D(x)
void kprintf(char *fmt,...);

static STRPTR oddeven[4] = {NULL};
static STRPTR scaling_modes[4] = {NULL};
static STRPTR pages_per_sheet[] =  {"2", "4", "6", "9", "Custom", NULL};
static STRPTR pages_order[10] = {NULL}; 
static char *ps_levels[] = {"Level 2", "Level 3", NULL};


int pages_per_sheet_table[] = {2, 4, 6, 9, 99};

DEFNEW
{
	Object *grpPage, *gauge, *strOutput, *sldFirst, *sldLast, *cycOddEven, *btnPrint, *btnCancel, 
	       *btnCancel2, *txtStatus, *chkPS, *btnPrefs, *chkAutorotatescale;
	Object *right_group, *strNumbers, *grpRight, *grpLeft, *chkReverse, 
	       *strCopies, *chkCollate, *grpCollate, *btnReset, *cycPSVersion, *grpPSOutput;
	Object *second_page, *cycPageScaling, *grpPageHandling, *grpDriver, *grpPagesPerSheet1, 
	       *grpPagesPerSheet2, *grpPagesOrder1, *grpPagesOrder2, *cycPagesPerSheet, *cycPagesOrder;
	Object *free_grp;
	
	oddeven[0]       = LOCSTR(MSG_PRINTER_ODDEVEN);
	oddeven[1]       = LOCSTR(MSG_PRINTER_ODD); 
	oddeven[2]       = LOCSTR(MSG_PRINTER_EVEN);
	scaling_modes[0] = LOCSTR(MSG_PRINTER_NONE);
	scaling_modes[1] = LOCSTR(MSG_PRINTER_BEST);
	scaling_modes[2] = LOCSTR(MSG_PRINTER_MULTIPLE);
	pages_per_sheet[4] = LOCSTR(MSG_PRINTER_CUSTOM);
	pages_order[0]   = LOCSTR(MSG_PRINTER_HORIZONTAL_VERTICAL);
	pages_order[1]   = LOCSTR(MSG_PRINTER_HORIZONTAL_VERTICALREVERSED);
	pages_order[2]   = LOCSTR(MSG_PRINTER_HORIZONTALREVERSED_VERTICAL) ;
	pages_order[3]   = LOCSTR(MSG_PRINTER_HORIZONTALREVERSED_VERTICAL_REVERSED);
	pages_order[4]   = LOCSTR(MSG_PRINTER_VERTICAL_HORIZONTAL);
	pages_order[5]   = LOCSTR(MSG_PRINTER_VERTICALREVERSED_HORIZONTAL);
	pages_order[6]   = LOCSTR(MSG_PRINTER_VERTICAL_HORIZONTALREVERSED) ;
	pages_order[7]   = LOCSTR(MSG_PRINTER_VERTICAL_REVERSED_HORIZONTALREVERSED);

	second_page  = VGroup,
		Child, HVSpace,
		Child, HGroup,
			Child, HSpace(0),
			Child, 	RawimageObject, MUIA_Rawimage_Data, Printer, End,

				//MUI_NewObject("Dtpic.mui",MUIA_Dtpic_Name, "progdir:printer.png",	End,
			Child,  HSpace(0),
		End,	
		Child, HVSpace,
		Child, HGroup,
			Child, HSpace(0),
			Child, gauge = GaugeObject,  
				MUIA_Gauge_Horiz, TRUE, 
				MUIA_Frame, MUIV_Frame_Gauge,
				MUIA_Gauge_InfoText, " ",
			End,
			Child, HSpace(0),
		End,
		Child, HGroup,
			Child, HSpace(0),
			Child, btnCancel = SimpleButton( LOCSTR( MSG_PRINTER_ABORT )),
			Child, HSpace(0),
		End,
		Child, HVSpace,
	End;
	
	right_group = MUI_NewObject(MUIC_Group,
		MUIA_Group_Child, MUI_NewObject(MUIC_Group,
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_Background, MUII_GroupBack,
			MUIA_FrameTitle, LOCSTR( MSG_PRINTER_RANGE ),
			MUIA_Group_Child, MUI_NewObject(MUIC_Group,
				MUIA_Group_Columns, 2,
				MUIA_Group_Child, MUI_NewObject(MUIC_Text,
					MUIA_Text_Contents, LOCSTR( MSG_PRINTER_PAGES ),
					MUIA_Text_SetMax, FALSE,
					MUIA_Text_PreParse, (LONG)"\33r",
					MUIA_Frame, MUIV_Frame_Button,
					MUIA_FramePhantomHoriz, TRUE,
					MUIA_HorizWeight, 1,
				TAG_END),
				MUIA_Group_Child, cycOddEven = MUI_NewObject(MUIC_Cycle,
					MUIA_Cycle_Entries, oddeven,
				TAG_END),
				MUIA_Group_Child, (grpLeft = MUI_NewObject(MUIC_Group,
					MUIA_HorizWeight, 1,
					MUIA_Group_Child, MUI_NewObject(MUIC_Group,
						MUIA_Group_Child, MUI_NewObject(MUIC_Group,	
							MUIA_Frame, MUIV_Frame_Slider,
							MUIA_FramePhantomHoriz, TRUE,
							MUIA_Group_Child, MUI_NewObject(MUIC_Text,
								MUIA_Text_Contents,  LOCSTR( MSG_PRINTER_FIRSTPAGE  ),
								MUIA_Text_PreParse, "\33r",
#if !defined(__AROS__)
// FIXMI: AROS
								MUIA_Frame, MUIV_Frame_SliderKnob,
#endif
								MUIA_FramePhantomHoriz, TRUE,
							TAG_END),
						TAG_END),
						MUIA_Group_Child, MUI_NewObject(MUIC_Group,
							MUIA_Frame, MUIV_Frame_Slider,
							MUIA_FramePhantomHoriz, TRUE,
							MUIA_Group_Child, MUI_NewObject(MUIC_Text,
								MUIA_Text_Contents,  LOCSTR( MSG_PRINTER_LASTPAGE  ),
								MUIA_Text_PreParse, "\33r",
#if !defined(__AROS__)
// FIXMI: AROS
								MUIA_Frame, MUIV_Frame_SliderKnob,
#endif
								MUIA_FramePhantomHoriz, TRUE,
							TAG_END),
						TAG_END),
					TAG_END),
					MUIA_Group_Child, MUI_NewObject(MUIC_Group,
						MUIA_Group_Child, MUI_NewObject(MUIC_Text,
							MUIA_Text_Contents, LOCSTR(MSG_PRINTER_MANUAL),
							MUIA_Text_PreParse, "\33r",
							MUIA_Frame, MUIV_Frame_String,
							MUIA_FramePhantomHoriz, TRUE,
						TAG_END),
						MUIA_Group_Child, MUI_NewObject(MUIC_Rectangle,
						TAG_END),
					TAG_END),
				TAG_END)),
				MUIA_Group_Child, (grpRight  = MUI_NewObject(MUIC_Group,
					MUIA_Group_Child, MUI_NewObject(MUIC_Group,
						MUIA_Group_Child, sldFirst = MUI_NewObject(MUIC_Slider,
							MUIA_Slider_Min, 1,
							MUIA_Slider_Max, 1, // will be corrected when opened
							MUIA_Frame, MUIV_Frame_Slider,
						TAG_END),
						MUIA_Group_Child, sldLast = MUI_NewObject(MUIC_Slider,
							MUIA_Slider_Min, 1,
							MUIA_Slider_Max, 1, // will be corrected when opened      
							MUIA_Frame, MUIV_Frame_Slider,
						TAG_END),
					TAG_END),
					MUIA_Group_Child, HGroup, 
						Child, MUI_NewObject(MUIC_Group,
							MUIA_Group_Child, strNumbers =  VPDFNumberGenerator, End,	
						TAG_END),
						Child, 	btnReset = TextObject,
											ButtonFrame,
												//MUIA_ShowMe, FALSE,
												MUIA_Font , MUIV_Font_Button,
												MUIA_Text_Contents , LOCSTR(MSG_PRINTER_RESET),
												MUIA_Text_PreParse , "\33c",
												MUIA_InputMode     , MUIV_InputMode_RelVerify,
												MUIA_Background    , MUII_ButtonBack,
												MUIA_Text_SetMax, TRUE,
											End,
					End,
				TAG_END)),
			
			TAG_END),
				Child, HGroup,
					Child, chkReverse = CheckMark(FALSE),
					Child, TextObject, MUIA_Text_Contents, LOCSTR(MSG_PRINTER_REVERSE), End,
				End, 
		TAG_END),
	TAG_END);
	
	grpPageHandling =  VGroup,
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_Background, MUII_GroupBack,
			MUIA_FrameTitle, (LONG)LOCSTR(MSG_PRINTER_ORIENTATION),
			Child, ColGroup(2),
				Child, Label(LOCSTR(MSG_PRINTER_COPIES)),
				Child, HGroup, 
					Child, strCopies= ArrowStringObject,
						MA_ArrowString_Min, 1,
					End,
					Child, HVSpace,
					Child, grpCollate = HGroup, 
						MUIA_ShowMe, FALSE,
						Child, chkCollate = CheckMark(TRUE),
						Child, 	MUI_NewObject(MUIC_Text,
							MUIA_Text_Contents, LOCSTR(MSG_PRINTER_COLLATE),
							MUIA_Text_SetMax, TRUE,
						TAG_END),
					End,
				End,
				Child, Label(LOCSTR(MSG_PRINTER_SCALING)),
				Child, cycPageScaling = CycleObject,
						MUIA_Cycle_Entries, scaling_modes,
				End,
				Child,  grpPagesPerSheet1 = PageGroup,
				MUIA_HorizWeight, 1,
						Child, Label(" "),
						Child, Label(LOCSTR(MSG_PRINTER_PAGESPERSHEET)),
				End,
				Child, grpPagesPerSheet2 = PageGroup,
				MUIA_HorizWeight, 1,
					Child, Label("          "),
					Child, HGroup, 
						Child,  cycPagesPerSheet = CycleObject,
								MUIA_Cycle_Entries, pages_per_sheet,
						End,
						Child,  free_grp = HGroup, 
								MUIA_ShowMe, FALSE,
								Child, StringObject, 
									MUIA_String_Accept, "0123456789",
									MUIA_Frame, MUIV_Frame_String,
									MUIA_String_MaxLen, 3,
									MUIA_FixWidthTxt, "12",
								End,
								Child, MUI_NewObject(MUIC_Text,
									MUIA_Text_Contents, "/",
									MUIA_Text_SetMax, TRUE,
								TAG_END),
								Child, StringObject, 
									MUIA_String_Accept, "0123456789",
									MUIA_Frame, MUIV_Frame_String,
									MUIA_String_MaxLen, 3,
									MUIA_FixWidthTxt, "12",
								End,
						
						End,
					End,
				End,
				Child, grpPagesOrder1 = PageGroup,
				MUIA_HorizWeight, 1,
					Child, Label(" "),
					Child, Label(LOCSTR(MSG_PRINTER_ORDER)),
				End,
				Child, grpPagesOrder2 = PageGroup,
				MUIA_HorizWeight, 1,
					Child, Label("             "),
					Child, cycPagesOrder = CycleObject,
						MUIA_Cycle_Entries, pages_order,
					End,
				End,
			End,
			Child, HVSpace,	
			Child, HGroup, 
				Child, chkAutorotatescale = CheckMark(TRUE),
				Child, TextObject, 
					MUIA_Text_Contents, LOCSTR(MSG_PRINTER_AUTOROTATE),
				End,
			End,	
		
	End;

	grpDriver = VGroup,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_Background, MUII_GroupBack,
		MUIA_FrameTitle, (LONG)LOCSTR(MSG_PRINTER_DRIVER),
		Child, btnPrefs = SimpleButton(LOCSTR(MSG_PRINTER_PREFERENCES)),
		Child, HGroup, 
		 	Child, HGroup,
		  	    MUIA_Weight, 1,
				MUIA_Frame, MUIV_Frame_Button,
				MUIA_FramePhantomHoriz, TRUE,
				Child, chkPS = CheckMark(TRUE),
				Child, TextObject, 
					MUIA_Text_Contents, "PostScript",
                 
				End,
			End,
			Child, grpPSOutput = HGroup, 
                MUIA_Weight, 90,
                Child, cycPSVersion = MUI_NewObject(MUIC_Cycle,
					MUIA_Cycle_Entries, ps_levels,
				TAG_END),
				Child, HVSpace,
				Child, Label(LOCSTR(MSG_PRINTER_OUTPUT)), 
				Child, strOutput = StringObject, 	
                            MUIA_Frame, MUIV_Frame_String,
                            MUIA_String_Contents, "PS:",
                            MUIA_String_MaxLen, 512,       
							End,
			End,
		End,
	End;
	
	
	obj = DoSuperNew(cl, obj,
						MUIA_Window_Title, LOCSTR(MSG_PRINTER_WINTITLE),
					//	MUIA_Window_Width, MUIV_Window_Width_Screen(50),
					//	MUIA_Window_Height, MUIV_Window_Height_Screen(50),
						MUIA_Window_ID, MAKE_ID('P','R','T','N'),
						MUIA_Window_AppWindow, FALSE,
						MUIA_Window_RootObject, grpPage = PageGroup,
					    Child, VGroup,
							Child, HGroup,
								Child, VGroup,
									Child, grpDriver,
									Child, right_group,
								End,
								Child, grpPageHandling,
							End,
						
							Child, HGroup,
								Child, btnPrint = SimpleButton(LOCSTR(MSG_PRINTER_PRINT)),
								Child, HSpace(0),
								Child, btnCancel2 = SimpleButton(LOCSTR(MSG_PRINTER_CANCEL)),
							End,
						End,	
						Child, second_page,
					End,
					TAG_MORE, INITTAGS);
	if (obj != NULL)
	{
    
		GETDATA;
		memset(data, 0, sizeof(struct Data));
	
		data->sldFirst   = sldFirst;
		data->sldLast    = sldLast;
		data->btnPrint   = btnPrint;
		data->btnCancel  = btnCancel;
		data->strOutput  = strOutput;
		data->txtStatus  = txtStatus;
		data->cycOddEven = cycOddEven;
		data->chkPS      = chkPS;
		data->chkAutorotatescale = chkAutorotatescale;
		data->strNumbers = strNumbers;
		data->grpRight   = grpRight;
		data->chkReverse = chkReverse;
		data->chkCollate = chkCollate;
		data->strCopies  = strCopies;
		data->grpCollate = grpCollate;
		data->btnReset   = btnReset;
		data->grpPage    = grpPage;
		data->cycPSVersion = cycPSVersion;
		data->gauge      = gauge;
		data->cycPageScaling = cycPageScaling;
        data->cycPagesPerSheet = cycPagesPerSheet;
        data->cycPagesOrder = cycPagesOrder;
		data->max = 1;
		
		data->proc = MUI_NewObject(MUIC_Process,
					MUIA_Process_SourceClass , cl,
					MUIA_Process_SourceObject, obj,
					MUIA_Process_Name        , "[VPDF]: Printing process",
					MUIA_Process_Priority    , -1,
					MUIA_Process_AutoLaunch  , FALSE,
					MUIA_Process_StackSize   , 1000000,
					TAG_DONE);

		DoMethod(data->proc, MUIM_Process_Launch);
		
		DoMethod(btnPrint,  MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_VPDFPrinter_Start);
		DoMethod(btnCancel, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_VPDFPrinter_Stop);
		DoMethod(btnPrefs,  MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_VPDFPrinter_PrinterPrefs);
		DoMethod(chkPS, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, grpPSOutput, 3, MUIM_Set, MUIA_ShowMe, MUIV_TriggerValue);
		DoMethod(strCopies, MUIM_Notify, MUIA_String_Integer, MUIV_EveryTime, obj, 2, MUIM_VPDFPrinter_HideCollate, MUIV_TriggerValue);
		
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 2, grpPagesPerSheet1, 3, MUIM_Set, MUIA_Group_ActivePage, 1); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 2, grpPagesPerSheet2, 3, MUIM_Set, MUIA_Group_ActivePage, 1); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 0, grpPagesPerSheet1, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 0, grpPagesPerSheet2, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 1, grpPagesPerSheet1, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 1, grpPagesPerSheet2, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 2, grpPagesOrder1, 3, MUIM_Set, MUIA_Group_ActivePage, 1); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 2, grpPagesOrder2, 3, MUIM_Set, MUIA_Group_ActivePage, 1); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 0, grpPagesOrder2, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 0, grpPagesOrder1, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 1, grpPagesOrder2, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		DoMethod(cycPageScaling, MUIM_Notify, MUIA_Cycle_Active, 1, grpPagesOrder1, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 	

		DoMethod(cycPagesPerSheet, MUIM_Notify, MUIA_Cycle_Active, 4, free_grp, 3, MUIM_Set, MUIA_ShowMe, TRUE); 
		DoMethod(cycPagesPerSheet, MUIM_Notify, MUIA_Cycle_Active, 1, free_grp, 3, MUIM_Set, MUIA_ShowMe, FALSE);
		DoMethod(cycPagesPerSheet, MUIM_Notify, MUIA_Cycle_Active, 0, free_grp, 3, MUIM_Set, MUIA_ShowMe, FALSE);
		DoMethod(cycPagesPerSheet, MUIM_Notify, MUIA_Cycle_Active, 2, free_grp, 3, MUIM_Set, MUIA_ShowMe, FALSE);
		DoMethod(cycPagesPerSheet, MUIM_Notify, MUIA_Cycle_Active, 3, free_grp, 3, MUIM_Set, MUIA_ShowMe, FALSE);
		
		DoMethod(sldFirst, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime, data->strNumbers, 3, MUIM_Set, MUIA_VPDFNumberGeneratorPageStart, MUIV_TriggerValue);
		DoMethod(sldLast,  MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime, data->strNumbers, 3, MUIM_Set, MUIA_VPDFNumberGeneratorPageEnd, MUIV_TriggerValue);  
		DoMethod(btnReset, MUIM_Notify, MUIA_Pressed, FALSE, data->sldFirst, 3, MUIM_Set, MUIA_Slider_Level, 1);  
	
		DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, MUIV_EveryTime,  obj, 1, MUIM_VPDFPrinter_Close);
		DoMethod(btnCancel2, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_VPDFPrinter_Close);	
		
		DoMethod(data->chkPS, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,  obj, 3,  MUIM_Set,  MUIA_VPDFPrinter_PrintingMode, MUIV_TriggerValue);
		DoMethod(data->strOutput, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,  obj, 3,  MUIM_Set,  MUIA_VPDFPrinter_PSName, MUIV_TriggerValue);
		DoMethod(data->cycPSVersion, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,  obj, 3,  MUIM_Set,  MUIA_VPDFPrinter_PSMode, MUIV_TriggerValue);
	}       

	return (ULONG)obj;
}



DEFDISP
{
	GETDATA;
	
	data->quit = TRUE;
	MUI_DisposeObject(data->proc);
	
	return DOSUPER;
}


DEFMMETHOD(VPDFPrinter_PrintDocument)
{
	GETDATA;
	
	
	set(data->sldFirst, MUIA_Slider_Max,   pdfGetPagesNum(msg->doc));
	set(data->sldLast,  MUIA_Slider_Max,   pdfGetPagesNum(msg->doc)); 
	set(data->sldLast,  MUIA_Slider_Level, pdfGetPagesNum(msg->doc));
	
	set(data->strNumbers, MUIA_VPDFNumberGeneratorPageMax, pdfGetPagesNum(msg->doc));
	set(data->strNumbers, MUIA_VPDFNumberGeneratorPageEnd, pdfGetPagesNum(msg->doc));
	data->max = pdfGetPagesNum(msg->doc);
	data->doc = msg->doc;
	data->close = FALSE;
	data->quit = FALSE;
	DoMethod(data->btnReset, MUIM_Notify, MUIA_Pressed, FALSE, data->sldLast, 3, MUIM_Set, MUIA_Slider_Level, data->max);  
	set(obj, MUIA_Window_Open, TRUE);
	
	return TRUE;
}


DEFMMETHOD(VPDFPrinter_Start)
{
	GETDATA;
	char params[512], args[512], arg[32];
	data->pj.first = 1;
	data->pj.last = pdfGetPagesNum(data->doc); 

	data->pj.mode = !xget(data->chkPS, MUIA_Selected);  // PS or Turboprint
	
	if (data->pj.mode==1)
        data->pj.mode = 2;
    else 
        data->pj.mode = xget(data->cycPSVersion, MUIA_Cycle_Active);
	
	data->pj.autorotatescale = xget(data->chkAutorotatescale, MUIA_Radio_Active);
	data->pj.step = 1;
	data->pj.reverse = xget(data->chkReverse, MUIA_Selected);
	data->pj.step = xget(data->cycOddEven, MUIA_Cycle_Active);
	
	
	data->pj.total_pages = DoMethod(data->strNumbers,  MUIM_VPDFNumberGeneratorFirst, data->pj.reverse); 
	D(kprintf("total pages: %d", data->pj.total_pages));

	data->pj.collate = xget(data->chkCollate, MUIA_Selected);
	data->pj.copies = xget(data->strCopies, MUIA_String_Integer);
	
	if (data->pj.copies==0)
	{
		data->pj.copies = 1;
		set(data->strCopies, MUIA_String_Integer, 1);
	}
	
	if (data->pj.collate)
		data->pj.total_pages *= data->pj.copies;
		
	data->pj.page_scaling    = xget(data->cycPageScaling,   MUIA_Cycle_Active);
	data->pj.pages_per_sheet = xget(data->cycPagesPerSheet, MUIA_Cycle_Active);
	data->pj.pages_per_sheet = pages_per_sheet_table[data->pj.pages_per_sheet];
    data->pj.pages_order     = xget(data->cycPagesOrder,    MUIA_Cycle_Active);
		
	set (data->gauge, MUIA_Gauge_Max, data->pj.total_pages);	 
	strncpy(data->pj.output, (char*)xget(data->strOutput, MUIA_String_Contents), sizeof(data->pj.output));
	
	args[0]=0;
	switch (data->pj.page_scaling)
	{
		case 1:
			strcat(args,  "fit-to-page ");
		break;
		case 2:
			sprintf(arg, "number-up=%d ", data->pj.pages_per_sheet);
			strcat(args,  arg);
		break;
		default:
		break;
	}

	sprintf(params, "COPIES=%d ARGS=/%s/ FORMAT=PS", data->pj.copies, args);
	strcat(data->pj.output, params);
	set(data->grpPage,  MUIA_Group_ActivePage, 1);
	
	// assuming printer thread is in ready status siting on Wait()
	DoMethod(data->proc, MUIM_Process_Signal, SIGBREAKF_CTRL_D);
	
	return TRUE;
}

DEFMMETHOD(VPDFPrinter_Stop)
{
	GETDATA;
	if (data->printing)
		data->quit = TRUE; // this will signal printing thread which will 'recall' this method later
	else
	{
		data->quit = FALSE;
		
		DoMethod(obj, MUIM_VPDFPrinter_Done);	
	}
	return TRUE;
}


DEFMMETHOD(VPDFPrinter_Done)
{
	GETDATA;
	
	data->printing = FALSE;
	data->quit = FALSE;

	if (data->close)
		DoMethod(obj, MUIM_VPDFPrinter_Close);
		
	set(data->grpPage,  MUIA_Group_ActivePage, 0);
	
	return TRUE;
}

DEFMMETHOD(VPDFPrinter_Close)
{
	GETDATA;
	
	if (!data->printing)
	{
		data->close = FALSE;
		set(obj, MUIA_Window_Open, FALSE);
	}
	else
	{	
		data->quit = TRUE;
		data->close = TRUE;
	}
	
	return TRUE;
}




DEFMMETHOD(Process_Process)
{
	GETDATA;

	while(!*msg->kill)
	{
		Wait(SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_C);

		if(data->quit == FALSE)
		{
			int i, j, k, h, pages_no;
			struct printerjob *pj = &data->pj;
			void *pctx;

			D(kprintf("started printing:%d to %d\n", pj->first, pj->last));

			data->printing = TRUE;
			pctx = pdfPrintInit(data->doc, pj->output, pj->first, pj->last, pj);
			if(pctx != NULL)
			{
				j = 0;
				h = 0;
				do
				{	
                    pages_no = 0; // for multipage mode
					while((MUIV_VPDFNumberGeneratorEndPos != (i = DoMethod(data->strNumbers, MUIM_VPDFNumberGeneratorNext))) || 
                          ((pj->page_scaling==2 && (pages_no))))
					{
                        if((pj->step == 1) && (i % 2 == 0)) continue;
                        if((pj->step == 2) && (i % 2 == 1)) continue;
                     
                        DoMethod(_app(data->sldFirst), MUIM_Application_PushMethod, obj, 3, MUIM_VPDFPrinter_StatusUpdate, i, h);
                            
                        k = 0;
                        do
                        {
                            int aaa;
							pj->pages[0] = i;
                            pdfPrintPage(pctx, pj->pages, pj->autorotatescale, (pj->page_scaling==2)?pj->pages_per_sheet:1);

                            if(data->quit)
                                break;
                            k++;
                        } while ((k < pj->copies) && !pj->collate);	
                        
                        if(data->quit || (i==MUIV_VPDFNumberGeneratorEndPos))
                            break;
                        h++;
                        pages_no = 0;
					}				
					j++;
					if(data->quit)
						break;
						
					DoMethod(data->strNumbers,  MUIM_VPDFNumberGeneratorFirst, pj->reverse); 
				}
				while((j<pj->copies) && pj->collate);
			}
			else
			{
                DoMethod(_app(data->sldFirst), MUIM_Application_PushMethod, obj, 1, MUIM_VPDFPrinter_Error);
			}


			pdfPrintEnd(pctx);
			D(kprintf("printing done...\n"));
			DoMethod(_app(data->sldFirst), MUIM_Application_PushMethod, obj, 1, MUIM_VPDFPrinter_Done);


		}

		data->printing = FALSE;
	}

	return 0;
}

DEFMMETHOD(VPDFPrinter_StatusUpdate)
{
	GETDATA;
	
	snprintf(data->status, sizeof(data->status), "Printing page %ld...", msg->page_number);
	D(kprintf("%s\n", data->status));
	set(data->gauge, MUIA_Gauge_Current, msg->actual_page);
	set(data->gauge, MUIA_Gauge_InfoText, data->status);
	
	return TRUE;
}


DEFMMETHOD(VPDFPrinter_PrinterPrefs)
{
	GETDATA;	
	if (xget(data->chkPS, MUIA_Selected))
		SystemTagList("MOSSYS:Prefs/Preferences MOSSYS:Prefs/MPrefs/Printers.mprefs", TAG_DONE);
	else
		SystemTagList("MOSSYS:Prefs/Preferences MOSSYS:Prefs/MPrefs/Printer.mprefs", TAG_DONE);

	return TRUE;
}

DEFMMETHOD(VPDFPrinter_HideCollate)
{
	GETDATA;
	
	if (msg->copies == 1)
	{
		set(data->grpCollate, MUIA_ShowMe, FALSE);
	}
	else
	{
		set(data->grpCollate, MUIA_ShowMe, TRUE);
	}
	
	return 0;
}


DEFSET
{
	GETDATA;

	FORTAG(INITTAGS)
	{
        case MUIA_VPDFPrinter_PSName:
		{
			set(data->strOutput, MUIA_String_Contents, tag->ti_Data);
        }
        break;
        case MUIA_VPDFPrinter_PrintingMode:
		{
			set(data->chkPS, MUIA_Selected, tag->ti_Data);
        }
        break;      
        case MUIA_VPDFPrinter_PSMode:
		{
			set(data->cycPSVersion, MUIA_Cycle_Active, tag->ti_Data);
        }
        break;
    }
	NEXTTAG

	return DOSUPER;
}

DEFMMETHOD(VPDFPrinter_Error)
{
	GETDATA;
	MUI_Request(_app(data->sldFirst), _window(obj), 0, "VPDF  Print Document Error", "OK", "Failed to initialize output device", NULL);
	return TRUE;
}

BEGINMTABLE
	DECNEW
	DECDISP
	DECSET
	DECMMETHOD(VPDFPrinter_PrintDocument)
	DECMMETHOD(VPDFPrinter_Start)
	DECMMETHOD(VPDFPrinter_Stop)
	DECMMETHOD(VPDFPrinter_Done)
	DECMMETHOD(VPDFPrinter_Close)
	DECMMETHOD(VPDFPrinter_StatusUpdate)
	DECMMETHOD(VPDFPrinter_PrinterPrefs)
	DECMMETHOD(VPDFPrinter_HideCollate)
	DECMMETHOD(VPDFPrinter_Error)
	DECMMETHOD(Process_Process)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Window, VPDFPrinterClass)   
