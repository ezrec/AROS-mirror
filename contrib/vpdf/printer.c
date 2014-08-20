/// System includes
#define AROS_ALMOST_COMPATIBLE
#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <libraries/asl.h>
#include <workbench/workbench.h>

#include <proto/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <exec/libraries.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <proto/locale.h>
#include <dos/dos.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>

#include <proto/alib.h>
#include <proto/utility.h>

#include <proto/datatypes.h>
#include <clib/dtclass_protos.h>
#include <datatypes/pictureclass.h>
#include <devices/rawkeycodes.h>

#include <libraries/gadtools.h>

#include <emul/emulregs.h>
#include <emul/emulinterface.h>
////

#include <private/vapor/vapor.h>
#include "util.h"
#include "poppler.h"
#include "application.h"
#include "printer.h"
#include "pagenumberclass.h"
#include "arrowstring_class.h"
#include "system/functions.h"
#include "locale.h"
#include "mcc/documentview_class.h"
#include "mcc/toolbar_class.h"
#include "mcc/renderer_class.h"
#include "mcc/title_class.h"

struct printerjob
{
	int first;
	int last;
	int step;
	char mode;
	char reverse;
	char autorotatescale;
	char selection_mode;
	int copies;
	char collate;
	int total_pages;
	char output[1024];
};

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
	Object *grpCollate;
	Object *btnReset;
	Object *grpPage;
	Object *gauge;
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
// {"Odd and Even", "Odd", "Even", NULL};
static STRPTR scaling_modes[4] = {NULL};
//{"None", "Best", "Multiple pages per sheet", NULL}; 
static STRPTR pages_per_sheet[] =  {"2", "4", "6", "9", "Custom", NULL};
static STRPTR pages_order[5] = {NULL}; 
//{"Horizontal", "Vertical", "Horizontal Reversed", "Vertial Reversed", NULL};


DEFNEW
{
	Object *grpPage, *gauge, *strOutput, *sldFirst, *sldLast, *cycOddEven, *btnPrint, *btnCancel, *btnCancel2, *txtStatus, *chkPS, *btnPrefs, *chkAutorotatescale;
	Object *right_group, *strNumbers, *grpRight, *grpLeft, *chkReverse, *strCopies, *chkCollate, *grpCollate, *btnReset, *grpPSOutput;
	Object *second_page, *grpPageHandling, *grpDriver, *page_scaling,  *grpPagesPerSheet1, *grpPagesPerSheet2, *grpPagesOrder1, *grpPagesOrder2, *cycPagesPerSheet;
	Object *free_grp;
	
	oddeven[0]       = LOCSTR(MSG_PRINTER_ODDEVEN);
	oddeven[1]       = LOCSTR(MSG_PRINTER_ODD); 
	oddeven[2]       = LOCSTR(MSG_PRINTER_EVEN);
	scaling_modes[0] = LOCSTR(MSG_PRINTER_NONE);
	scaling_modes[1] = LOCSTR(MSG_PRINTER_BEST);
	scaling_modes[2] = LOCSTR(MSG_PRINTER_MULTIPLE);
	pages_per_sheet[4] = LOCSTR(MSG_PRINTER_CUSTOM);
	pages_order[0]   = LOCSTR(MSG_PRINTER_HORIZONTAL);
	pages_order[1]   = LOCSTR(MSG_PRINTER_VERTICAL);
	pages_order[2]   = LOCSTR(MSG_PRINTER_HORIZONTALREVERT) ;
	pages_order[3]   = LOCSTR(MSG_PRINTER_VERTICALREVERT);
	
	second_page  = VGroup,
		Child, HVSpace,
		Child, HGroup,
			Child, HSpace(0),
			Child, MUI_NewObject("Dtpic.mui",
				MUIA_Dtpic_Name, "progdir:printer.png",
			End,
			Child,  HSpace(0),
		End,	
		Child, HGroup,
			Child, HSpace(0),
			Child, gauge = GaugeObject,  
				MUIA_Gauge_Horiz, TRUE, 
				MUIA_Frame, MUIV_Frame_Gauge,
				MUIA_Gauge_InfoText, " ",
			End,
			Child, HSpace(0),
		End,
		Child, HVSpace,
		Child, HGroup,
			Child, HSpace(0),
			Child, btnCancel = SimpleButton( LOCSTR( MSG_PRINTER_CANCEL )),
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
								MUIA_Frame, MUIV_Frame_SliderKnob,
								MUIA_FramePhantomHoriz, TRUE,
							TAG_END),
						TAG_END),
						MUIA_Group_Child, MUI_NewObject(MUIC_Group,
							MUIA_Frame, MUIV_Frame_Slider,
							MUIA_FramePhantomHoriz, TRUE,
							MUIA_Group_Child, MUI_NewObject(MUIC_Text,
								MUIA_Text_Contents,  LOCSTR( MSG_PRINTER_LASTPAGE  ),
								MUIA_Text_PreParse, "\33r",
								MUIA_Frame, MUIV_Frame_SliderKnob,
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
				Child, page_scaling = CycleObject,
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
					Child, CycleObject,
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
				MUIA_Frame, MUIV_Frame_String,
				MUIA_FramePhantomHoriz, TRUE,
				Child, chkPS = CheckMark(TRUE),
				Child, TextObject, 
					MUIA_Text_Contents, "PostScript",
				End,
			End,
			Child, grpPSOutput = HGroup, 
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
						MUIA_Window_Width, MUIV_Window_Width_Screen(50),
						MUIA_Window_Height, MUIV_Window_Height_Screen(50),
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
		data->gauge      = gauge;
		
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
		
		
		DoMethod(page_scaling,MUIM_Notify, MUIA_Cycle_Active, 2, grpPagesPerSheet1, 3, MUIM_Set, MUIA_Group_ActivePage, 1); 
		DoMethod(page_scaling,MUIM_Notify, MUIA_Cycle_Active, 2, grpPagesPerSheet2, 3, MUIM_Set, MUIA_Group_ActivePage, 1); 
		DoMethod(page_scaling,MUIM_Notify, MUIA_Cycle_Active, 0, grpPagesPerSheet1, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		DoMethod(page_scaling,MUIM_Notify, MUIA_Cycle_Active, 0, grpPagesPerSheet2, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		DoMethod(page_scaling,MUIM_Notify, MUIA_Cycle_Active, 2, grpPagesOrder1, 3, MUIM_Set, MUIA_Group_ActivePage, 1); 
		DoMethod(page_scaling,MUIM_Notify, MUIA_Cycle_Active, 2, grpPagesOrder2, 3, MUIM_Set, MUIA_Group_ActivePage, 1); 
		DoMethod(page_scaling,MUIM_Notify, MUIA_Cycle_Active, 0, grpPagesOrder2, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		DoMethod(page_scaling,MUIM_Notify, MUIA_Cycle_Active, 0, grpPagesOrder1, 3, MUIM_Set, MUIA_Group_ActivePage, 0); 
		
		
		DoMethod(cycPagesPerSheet, MUIM_Notify, MUIA_Cycle_Active, 4, free_grp, 3, MUIM_Set, MUIA_ShowMe, TRUE); 
		DoMethod(cycPagesPerSheet, MUIM_Notify, MUIA_Cycle_Active, 1, free_grp, 3, MUIM_Set, MUIA_ShowMe, FALSE);
		DoMethod(cycPagesPerSheet, MUIM_Notify, MUIA_Cycle_Active, 0, free_grp, 3, MUIM_Set, MUIA_ShowMe, FALSE);
		DoMethod(cycPagesPerSheet, MUIM_Notify, MUIA_Cycle_Active, 2, free_grp, 3, MUIM_Set, MUIA_ShowMe, FALSE);
		DoMethod(cycPagesPerSheet, MUIM_Notify, MUIA_Cycle_Active, 3, free_grp, 3, MUIM_Set, MUIA_ShowMe, FALSE);
		
		DoMethod(sldFirst, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime, data->strNumbers, 3, MUIM_Set, MUIA_VPDFNumberGeneratorPageStart, MUIV_TriggerValue);
		DoMethod(sldLast,  MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime, data->strNumbers, 3, MUIM_Set, MUIA_VPDFNumberGeneratorPageEnd, MUIV_TriggerValue);  
		DoMethod(btnReset, MUIM_Notify, MUIA_Pressed, FALSE, data->sldFirst, 3, MUIM_Set, MUIA_Slider_Level, 1);  
	
		DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, MUIV_EveryTime,  obj, 1, MUIM_VPDFPrinter_Close);
	
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
	
	data->pj.first = 1;
	data->pj.last = pdfGetPagesNum(data->doc); 

	data->pj.mode = !xget(data->chkPS, MUIA_Selected);  // PS or Turboprint
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
	set (data->gauge, MUIA_Gauge_Max, data->pj.total_pages);	 
	strncpy(data->pj.output, (char*)xget(data->strOutput, MUIA_String_Contents), sizeof(data->pj.output));
	
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
			int i, j, k, h;
			struct printerjob *pj = &data->pj;
			void *pctx;

			D(kprintf("started printing:%d to %d\n", pj->first, pj->last));

			data->printing = TRUE;
			pctx = pdfPrintInit(data->doc, pj->output, pj->first, pj->last, pj->mode);
			if(pctx != NULL)
			{
				j = 0;
				h = 0;
				do
				{	
					while(-1 != (i = DoMethod(data->strNumbers, MUIM_VPDFNumberGeneratorNext)))
					{
							if((pj->step == 1) && (i % 2 == 0)) continue;
							if((pj->step == 2) && (i % 2 == 1)) continue;
							
							
							DoMethod(_app(data->sldFirst), MUIM_Application_PushMethod, obj, 3, MUIM_VPDFPrinter_StatusUpdate, i, h);
							k = 0;
							do
							{
								pdfPrintPage(pctx, i, pj->autorotatescale);

								if(data->quit)
									break;
								k++;
							} while ((k < pj->copies) && !pj->collate);	
							
							if(data->quit)
									break;
							h++;
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
				MUI_Request(_app(data->sldFirst), obj, 0, "VPDF · Print Document Error", "Failed to initialize output device", TAG_DONE);
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
//	set(data->txtStatus, MUIA_Text_Contents, data->status);
	set(data->gauge, MUIA_Gauge_Current, msg->actual_page);
	set(data->gauge, MUIA_Gauge_InfoText, data->status);
	
	return TRUE;
}


DEFMMETHOD(VPDFPrinter_PrinterPrefs)
{
	GETDATA;
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


BEGINMTABLE
	DECNEW
	DECDISP
	DECMMETHOD(VPDFPrinter_PrintDocument)
	DECMMETHOD(VPDFPrinter_Start)
	DECMMETHOD(VPDFPrinter_Stop)
	DECMMETHOD(VPDFPrinter_Done)
	DECMMETHOD(VPDFPrinter_Close)
	DECMMETHOD(VPDFPrinter_StatusUpdate)
	DECMMETHOD(VPDFPrinter_PrinterPrefs)
	DECMMETHOD(VPDFPrinter_HideCollate)
	DECMMETHOD(Process_Process)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Window, VPDFPrinterClass)   
