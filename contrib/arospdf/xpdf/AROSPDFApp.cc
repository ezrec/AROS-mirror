//========================================================================
//
// AROSPDFApp.cc
//
// Copyright 2009 Craig Kiesau
//
//========================================================================

#include <time.h>
#include <limits.h>
#include "PDFDoc.h"
#include "AROSPDFApp.h"
long __stack = 8192;

#define RES_LOWER_LIMIT 200
#define RES_HIGHER_LIMIT 2000

#define XGET(OBJ, ATTR) ({IPTR b; GetAttr(ATTR, OBJ, &b); b; })

enum {
    ADD_METHOD=1,
    MEN_PROJECT,MEN_OPEN, MEN_ABOUT,MEN_QUIT,
    MEN_EDIT,MEN_FIND,MEN_FINDNEXT,
    MEN_VIEW,MEN_FULLSCREEN,MEN_CONTINUOUS,MEN_ZOOMIN,MEN_ZOOMOUT,MEN_BESTFIT,MEN_FITWIDTH,
    MEN_GO,MEN_FIRSTPAGE,MEN_PREVPAGE,MEN_NEXTPAGE,MEN_LASTPAGE
};

static struct NewMenu MenuData1[]=
{
    {NM_TITLE, "AROSPDF",                      0 , 0, 0,  (APTR)MEN_PROJECT },
    {NM_ITEM,  "Open File...",              	"O", 0, 0,  (APTR)MEN_OPEN   },
    {NM_ITEM,  NM_BARLABEL,                    0 , 0, 0,  (APTR)0           },
    {NM_ITEM,  "About AROSPDF",              "?", 0, 0,  (APTR)MEN_ABOUT   },
    {NM_ITEM,  NM_BARLABEL,                    0 , 0, 0,  (APTR)0           },
    {NM_ITEM,  "Quit",                        "Q", 0, 0,  (APTR)MEN_QUIT    },
    {NM_TITLE, "Edit",                      0 , NM_MENUDISABLED, 0,  (APTR)MEN_EDIT },
    {NM_ITEM,  "Find...",                        0, 0, 0,  (APTR)MEN_FIND    },
    {NM_ITEM,  "Find Next",                        0, 0, 0,  (APTR)MEN_FINDNEXT    },

    {NM_TITLE, "View",                      0 , 0, 0,  (APTR)MEN_VIEW },
    //{NM_ITEM,  "Fullscreen",                        0, NM_ITEMDISABLED, 0,  (APTR)MEN_FULLSCREEN    },
    //{NM_ITEM,  "Continuous",                        0, NM_ITEMDISABLED, 0,  (APTR)MEN_CONTINUOUS }, //CHECKIT|MENUTOGGLE|
    {NM_ITEM,  NM_BARLABEL,                    0 , 0, 0,  (APTR)0           },
    {NM_ITEM,  "Zoom In",                        0, 0, 0,  (APTR)MEN_ZOOMIN    },
    {NM_ITEM,  "Zoom Out",                        0, 0, 0,  (APTR)MEN_ZOOMOUT   },
    {NM_ITEM,  "Best Fit",                        0, CHECKIT|MENUTOGGLE, 0,  (APTR)MEN_BESTFIT    },
    {NM_ITEM,  "Fit Width",                        0, CHECKIT|MENUTOGGLE, 0,  (APTR)MEN_FITWIDTH },
    {NM_TITLE, "Go",                      0 , 0, 0,  (APTR)MEN_GO },
    {NM_ITEM,  "First Page",                        0, 0, 0,  (APTR)MEN_FIRSTPAGE    },
    {NM_ITEM,  "Previous Page",                        0, 0, 0,  (APTR)MEN_PREVPAGE   },
    {NM_ITEM,  NM_BARLABEL,                    0 , 0, 0,  (APTR)0           },
    {NM_ITEM,  "Next Page",                        0, 0, 0,  (APTR)MEN_NEXTPAGE    },
    {NM_ITEM,  "Last Page",                        0, 0, 0,  (APTR)MEN_LASTPAGE    },
    {NM_END,   NULL,                           0 , 0, 0,  (APTR)0           },
};

char about_text[] ="\33cAROSPDF\n\nCopyright 2009-2010 Craig Kiesau\nver 0.2.32 (2010/07/01)\nckiesau@aros.org\n\nBased on xpdf 3.0.2 Copyright 1996-2007 Glyph and Cog";

long
min(long a, long b)
{
    return (a < b ? a : b);
}

BOOL LoadASL(struct Screen * curscreen,STRPTR filename,STRPTR title,STRPTR ifile,STRPTR pattern,BOOL folders) {
	char current_dir[300] = "";
	BOOL ok=FALSE;
	struct FileRequester * freq=NULL;
	
	if(freq = (struct FileRequester *)AllocAslRequest(ASL_FileRequest,NULL)) {
		if(AslRequestTags(freq,
								ASLFR_TitleText,			(ULONG)title,
								ASLFR_Window,				0,
								ASLFR_Flags1,				FRF_DOPATTERNS,
								ASLFR_InitialFile,		(ULONG)ifile,
								ASLFR_InitialDrawer,		(ULONG)current_dir,
								ASLFR_InitialPattern,	(ULONG)pattern,
								ASLFR_DrawersOnly,		folders,
								ASLFR_Screen, curscreen,
								/*ASLFR_UserData,			app,
								ASLFR_IntuiMsgFunc,		&IntuiMsgHook,*/
								TAG_DONE)) {
			strcpy(current_dir,freq->rf_Dir);
			
			strcpy(filename,freq->rf_Dir);
			if(AddPart(filename,freq->rf_File,255))//MAXFILENAME))
				ok=TRUE;
		}
		FreeAslRequest(freq);
	}
	return ok;
}

struct CustomImageData
{
	AROSPDFApp * app;
        struct	MUI_EventHandlerNode	ehnode;
        struct BitMap * bitmap;
        LONG width, height;
};
#define MY_APP  0x8022UL
#define MY_RP   0x8023UL
#define MY_BM   0x8024UL
#define MY_BMWIDTH   0x8025UL
#define MY_BMHEIGHT   0x8026UL

SAVEDS IPTR mNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct CustomImageData *data;
	struct TagItem *tags,*tag;

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg)))
		return(0);

	data = (CustomImageData*)INST_DATA(cl,obj);

	for (tags=((struct opSet *)msg)->ops_AttrList ; tag=NextTagItem(&tags);)
	{
		switch (tag->ti_Tag)
		{
			case MY_APP:
				if (tag->ti_Data)
					data->app = ((AROSPDFApp *)tag->ti_Data);
				break;
			case MY_BM:
				if (tag->ti_Data) 
                                {
					data->bitmap = ((struct BitMap *)tag->ti_Data);
                                }                                        
				break;
			case MY_BMWIDTH:
				if (tag->ti_Data)
					data->width = tag->ti_Data;
				break;
			case MY_BMHEIGHT:
				if (tag->ti_Data)
					data->height = tag->ti_Data;
				break;
		
                }
	}

	return (IPTR)obj;
}

SAVEDS IPTR mSet(struct IClass *cl,Object *obj,Msg msg)
{
	struct CustomImageData *data = (CustomImageData*)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

	for (tags=((struct opSet *)msg)->ops_AttrList ; tag=NextTagItem(&tags);)
	{
		switch (tag->ti_Tag)
		{
			case MY_APP:
				if (tag->ti_Data)
                                	data->app = ((AROSPDFApp *)tag->ti_Data);
				break;
                        case MY_BM:
				if (tag->ti_Data) 
                                {
					data->bitmap = ((struct BitMap *)tag->ti_Data);
                                }                                        
				break;
			case MY_BMWIDTH:
				if (tag->ti_Data)
					data->width = tag->ti_Data;
				break;
			case MY_BMHEIGHT:
				if (tag->ti_Data)
					data->height = tag->ti_Data;
				break;
		}
	}

	return DoSuperMethodA(cl,obj,msg);
}

SAVEDS IPTR mGet(struct IClass *cl,Object *obj,Msg msg)
{
	struct CustomImageData *data = INST_DATA(cl,obj);
	IPTR *store = ((struct opGet *)msg)->opg_Storage;

	switch (((struct opGet *)msg)->opg_AttrID)
	{

	}

	return DoSuperMethodA(cl,obj,msg);
}


SAVEDS IPTR mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
        struct CustomImageData *data = INST_DATA(cl,obj);
	DoSuperMethodA(cl,obj,(Msg)msg);
        msg->MinMaxInfo->MinWidth  += 1;
	msg->MinMaxInfo->DefWidth  += _width(obj);
	msg->MinMaxInfo->MaxWidth  += MUI_MAXMAX;

	msg->MinMaxInfo->MinHeight += 1;
	msg->MinMaxInfo->DefHeight += _mheight(obj);
	msg->MinMaxInfo->MaxHeight += MUI_MAXMAX;
	return 0;
}

 ULONG mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
 {
 	struct CustomImageData *data = (CustomImageData*)INST_DATA(cl,obj);

        struct RastPort *rp;
	ULONG mleft, mtop, mwidth, mheight;

	DoSuperMethodA(cl, obj, (Msg)msg);
        if (data->app->isReady()) {
		if ((data->width != XGET(data->app->getBM(),MUIA_Width)) || (data->height != XGET(data->app->getBM(),MUIA_Height)))
		data->app->ResizeBitMap(XGET(data->app->getBM(),MUIA_Width),XGET(data->app->getBM(),MUIA_Height));
 	}

	rp = _rp(obj);
	mleft = _mleft(obj);
	mtop = _mtop(obj);
	mwidth = _width(obj);
	mheight = _mheight(obj);

	mwidth = min(mwidth, data->width);
	mheight = min(mheight, data->height);

        if (data->app->isReady()) {
	        BltBitMapRastPort(data->bitmap, 0, 0, rp, mleft, mtop, mwidth-5, mheight-1, 0xc0);
        }
	return 0;
}

SAVEDS IPTR mSetup(struct IClass *cl,Object *obj,Msg msg)
{
	struct CustomImageData *data = INST_DATA(cl,obj);

	if (!DoSuperMethodA(cl,obj,msg))
		return(FALSE);

 data->ehnode.ehn_Object = obj;
           data->ehnode.ehn_Class  = cl;
           data->ehnode.ehn_Events = IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY;
           DoMethod(_win(obj),MUIM_Window_AddEventHandler,&data->ehnode);

	return(TRUE);
}

SAVEDS IPTR mHandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
	struct CustomImageData *data = INST_DATA(cl,obj);

        if (msg->imsg)
        {
                switch (msg->imsg->Class)
                {
                 case IDCMP_RAWKEY:
                        switch (msg->imsg->Code)
                        {
                         case RAWKEY_UP:
                         case RAWKEY_NM_WHEEL_UP:
                            data->app->ScrollUpDown(FALSE);
                            break;
                         case RAWKEY_DOWN:
                         case RAWKEY_NM_WHEEL_DOWN:
                            data->app->ScrollUpDown(TRUE);
                            break;
                         case RAWKEY_PAGEUP:
                            data->app->PageForwardBack(FALSE);
                            break;
                         case RAWKEY_PAGEDOWN:
                            data->app->PageForwardBack(TRUE);
                            break;
                        }
                 break;
                }
        }
        return 0;
}

SAVEDS IPTR mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
	struct CustomImageData *data = INST_DATA(cl,obj);

   DoMethod(_win(obj),MUIM_Window_RemEventHandler,&data->ehnode);



	return DoSuperMethodA(cl,obj,msg);
}

BOOPSI_DISPATCHER(IPTR, MyDispatcher, cl, obj, msg)
{
	switch (msg->MethodID)
	{
		case OM_NEW        : return(mNew      (cl,obj,(APTR)msg));
                case OM_SET        : return(mSet      (cl,obj,(APTR)msg));
                case OM_GET        : return(mGet      (cl,obj,(APTR)msg));
		case MUIM_AskMinMax: return(mAskMinMax(cl,obj,(APTR)msg));
		case MUIM_Draw     : return(mDraw     (cl,obj,(APTR)msg));
                case MUIM_Setup    : return(mSetup    (cl,obj,(APTR)msg));
                case MUIM_Cleanup  : return(mCleanup  (cl,obj,(APTR)msg));
                case MUIM_HandleEvent: return(mHandleEvent(cl,obj,(struct MUIP_HandleEvent*)msg));
	}

	return DoSuperMethodA(cl,obj,msg);
}
BOOPSI_DISPATCHER_END

void AROSPDFApp::OpenFile(GString *fileNameA, GString *ownerPWA, GString *userPWA) {

 int pw, ph;

  if (docLoaded) {
	delete doc;
	ready=FALSE;
	docLoaded=FALSE;
	}
else {
  dispW=BITMAPX;
  dispH=BITMAPY;
	}
  doc = new PDFDoc(fileNameA, ownerPWA, userPWA);
  if (userPWA) {
    delete userPWA;
  }
  if (ownerPWA) {
    delete ownerPWA;
  }

  if (!doc->isOk()) {
    fprintf(stderr, "error opening document\n");
    ok = gFalse;
    delete doc;
    return;
  }
  GString * title = new GString("AROSPDF - ");
  title->append(fileNameA);
  SetAttrs(wnd,MUIA_Window_Title,title->getCString() ,TAG_DONE);
  delete title;
  resolution = 100;
  page = 1;
  posX = 0;
  posY = 0;

  splashOut->startDoc(doc->getXRef());
  JumpToPage(1);
  return;
}

void AROSPDFApp::BackFillFullScreen() {
	BltBitMapRastPort(bm, 0, 0, &fsscreen->RastPort, 0, 0, dispW-5, dispH-1, 0xc0);
}
 
void AROSPDFApp::ToggleFullscreen() {
	FullScreen=!FullScreen;
	if (FullScreen) {
		origscreen=XGET(wnd,MUIA_Window_Screen);
		int scx=origscreen->Width;
		int scy=origscreen->Height;
		fsscreen=OpenScreenTags(NULL, SA_Width,scx,
                              SA_Height,scy,TAG_DONE);
		if (fsscreen!=NULL) {
			SetRast( &fsscreen->RastPort, 1 );
			fswindow=OpenWindowTags(NULL,
			WA_Width,		fsscreen->Width,
			WA_Height,		fsscreen->Height,
			WA_CustomScreen,	(ULONG)fsscreen,
			WA_Backdrop,		TRUE,
			WA_Borderless,		TRUE,
			WA_RMBTrap,		TRUE,
			WA_Activate,		TRUE,
			WA_ReportMouse,		TRUE,
			WA_IDCMP,		IDCMP_MOUSEBUTTONS
					      | IDCMP_RAWKEY
					      | IDCMP_DISKINSERTED
					      | IDCMP_DISKREMOVED
					      | IDCMP_ACTIVEWINDOW
					      | IDCMP_INACTIVEWINDOW
					      | IDCMP_MOUSEMOVE
					      | IDCMP_DELTAMOVE,
			TAG_DONE);
			BestFit=TRUE;
			DisableFitWidth();		
                        ResizeBitMap(scx,scy);
			ZoomToFit(FALSE);
			SetAttrs(wnd,MUIA_Window_Open,FALSE,TAG_DONE);
			BackFillFullScreen();	
                        		
		} 
	} else {
		if (fsscreen!=NULL) {
			SetAttrs(wnd,MUIA_Window_Open,TRUE,TAG_DONE);
			CloseWindow(fswindow);
			CloseScreen(fsscreen);
		}
	}
}

AROSPDFApp::AROSPDFApp() {
	ready=FALSE;
	docLoaded=FALSE;
        if (initAROS() != 0) {
	    fprintf(stderr, "error initializing AROS\n");
	    ok = gFalse;
	    return;
	}
        EnableDisableGUI();
	ok = gTrue;
	return;
}
void AROSPDFApp::JumpToPage(int pageno) {
	int pw,ph;
	

	if (pageno==0)  return;
	if ((pageno > (doc->getNumPages())) && (page==(doc->getNumPages()))) return;

	page=pageno;

	if (page > (doc->getNumPages())) page=(doc->getNumPages());
     if (!FullScreen) { 
	SetAttrs(txtPage, MUIA_NoNotify,TRUE, MUIA_String_Integer, page, TAG_DONE);
     	DoMethod(txtPageCount, MUIM_SetAsString,
                MUIA_Text_Contents,"of %ld",doc->getNumPages()); 
	}
	posY=0;
  if (FitWidth) {
        int cropwidth=doc->getPageCropWidth(page);
        resolution=72*(double)dispW/cropwidth;
   }
   if (BestFit) {
		int cropheight=doc->getPageCropHeight(page);
                resolution=72*(double)dispH/cropheight;
   }

  doc->displayPage(splashOut, page, resolution, resolution, 0, gFalse,
		   gFalse, gFalse);

  pw = splashOut->getBitmapWidth();
  ph = splashOut->getBitmapHeight();
  resLowLimit = gTrue;
  resHiLimit = gTrue;
  if (pw <= RES_HIGHER_LIMIT && ph <= RES_HIGHER_LIMIT)
    resHiLimit = gFalse;
  if (pw >= RES_LOWER_LIMIT && pw >= RES_LOWER_LIMIT)
    resLowLimit = gFalse;
   ready=TRUE;
   docLoaded=TRUE;
   if (!FullScreen) EnableDisableGUI();
   SetAttrs(vslider, MUIA_Prop_Entries, (LONG)(ph/SCROLLSPEED),MUIA_Prop_Visible,(LONG)(dispH/SCROLLSPEED),TAG_DONE);
   SetAttrs(hslider, MUIA_Prop_Entries, (LONG)(pw/SCROLLSPEED),MUIA_Prop_Visible,(LONG)(dispW/SCROLLSPEED),TAG_DONE);
   ok = gTrue;
   SetAttrs(vslider, MUIA_NoNotify,TRUE,MUIA_Prop_First, 0,hslider, MUIA_Prop_First, 0,TAG_DONE);
   DoMethod(Bmp,MUIM_Draw);
}
void AROSPDFApp::PageForwardBack(bool Forward) {
	//Page forward true = next page, false = prev page
	int pageno=page;
	if (Forward) pageno+=1;
	else pageno-=1;
	JumpToPage(pageno);
}
void AROSPDFApp::ScrollUpDown(bool Down) {
        if (Down) {
                DoMethod(vslider,MUIM_Prop_Increase,SCROLLSPEED);
        } else {
                DoMethod(vslider,MUIM_Prop_Decrease,SCROLLSPEED);
        }
}
AROSPDFApp::AROSPDFApp(GString *fileNameA, GString *ownerPWA, GString *userPWA) {
  ready=FALSE;
  docLoaded=FALSE;
  if (initAROS() != 0) {
    fprintf(stderr, "error initializing AROS\n");
    ok = gFalse;
    return;
  }
	OpenFile(fileNameA,ownerPWA,userPWA);
	return;
 }

void AROSPDFApp::quit() {
  exitAROS();
  delete splashOut;
if (docLoaded==TRUE) {
  delete doc;
}
}

int AROSPDFApp::run() {
  int oldX, oldY;
  int pw, ph;
  ULONG sigs=0;
 // Check that the window opened
             if (XGET(wnd, MUIA_Window_Open))
             {
		 // Main loop
                 while((LONG)DoMethod(muiapp, MUIM_Application_NewInput, (IPTR)&sigs)
                       != MUIV_Application_ReturnID_Quit)
                 {
                     if (sigs)
                     {
                         sigs = Wait(sigs | SIGBREAKF_CTRL_C);
                         if (sigs & SIGBREAKF_CTRL_C)
                             break;
                     }
                 }
             }
	return 0;
}
AROS_UFH3(ULONG, myfunction,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(Object *, object, A2),
AROS_UFHA(APTR *, msg, A1))
{
    AROS_USERFUNC_INIT
    AROSPDFApp *me=(AROSPDFApp *)msg[0];
    int item=(int)msg[1]; //vert=1, horiz=2
    LONG val=(LONG)msg[2]; //position in scrollbar
    if (item==1)
    me->setVert(val*SCROLLSPEED);
    else if (item==2) me->setHoriz(val*SCROLLSPEED);
    return 0;
    AROS_USERFUNC_EXIT
}
static struct Hook myhook;


AROS_UFH3(ULONG, buttonhookfunc,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(Object *, object, A2),
AROS_UFHA(APTR *, msg, A1))
{
    AROS_USERFUNC_INIT
    AROSPDFApp *me=(AROSPDFApp *)msg[0];
    int item=(int)msg[1]; //vert=1, horiz=2
    switch (item) {
		case 2:
			me->PageForwardBack(FALSE);
			break;
		case 3:
			me->PageForwardBack(TRUE);
			break;
                case 4:
                        me->JumpToPage(INT_MAX);
                        break;
                case 1:
                        me->JumpToPage(1);
                        break;
                case 5:
                        me->ZoomInOut(FALSE);
                        break;
                case 6:
                        me->ZoomInOut(TRUE);
                        break;
                case 7:
                        me->JumpToPageTextbox();
    }
    return 0;
    AROS_USERFUNC_EXIT
}
static struct Hook buttonhook;
AROS_UFH3(ULONG, menufunction,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(Object *, object, A2),
AROS_UFHA(APTR *, msg, A1))
{
    AROS_USERFUNC_INIT
    AROSPDFApp *me=(AROSPDFApp *)msg[0];
    int menuitem=(int)msg[1];
	switch (menuitem) {
		case MEN_ABOUT:
			me->AboutMenu();
			break;
		case MEN_OPEN:
			me->RequestOpenFile();
			break;
		case MEN_PREVPAGE:
			me->PageForwardBack(FALSE);
			break;
		case MEN_NEXTPAGE:
			me->PageForwardBack(TRUE);
			break;
                case MEN_LASTPAGE:
                        me->JumpToPage(INT_MAX);
                        break;
                case MEN_FIRSTPAGE:
                        me->JumpToPage(1);
                        break;
                case MEN_FITWIDTH:
                        me->ToggleFitWidth();
                        me->DisableBestFit();
                        me->JumpToPageTextbox();
                        break;
                case MEN_BESTFIT:
			me->ToggleBestFit();
			me->DisableFitWidth();
                        me->ZoomToFit(FALSE);
                        break;
		case MEN_FULLSCREEN:
			me->ToggleFullscreen();
			break;
	}
    return 0;
    AROS_USERFUNC_EXIT
}
static struct Hook menuhook;

void AROSPDFApp::ZoomInOut(bool In) {
        int pw,ph;
        if (In) {
                if (resHiLimit==gTrue) return;
                else resolution+=20;
        } else {
                if (resLowLimit==gTrue) return;
                else resolution-=20;
        }
	DisableFitWidth();
	DisableBestFit();
        JumpToPage(page);
}


void AROSPDFApp::DisableBestFit() {
	BestFit=FALSE;
        SetAttrs((APTR)DoMethod(menustrip,MUIM_FindUData,MEN_BESTFIT), MUIA_Menuitem_Checked, FALSE, TAG_DONE);
}

void AROSPDFApp::DisableFitWidth() {
        FitWidth=FALSE;
        SetAttrs((APTR)DoMethod(menustrip,MUIM_FindUData,MEN_FITWIDTH), MUIA_Menuitem_Checked, FALSE, TAG_DONE);
}

void AROSPDFApp::JumpToPageTextbox() {
        JumpToPage(XGET(txtPage,MUIA_String_Integer));
}

void AROSPDFApp::RequestOpenFile() {
	static char infile[1024] = "";
 	GString *fileName;
	struct Screen * curscreen;
	curscreen=XGET(wnd,MUIA_Window_Screen);
	if (LoadASL(curscreen,infile,"Open file:","","#?.pdf",FALSE)) {
	fileName=new GString(infile);
	OpenFile(fileName,(GString*)NULL,(GString*)NULL);
	}
	return;
}

void AROSPDFApp::AboutMenu() {
	MUI_RequestA(muiapp,wnd,0,"About AROSPDF","*OK",about_text,NULL);
	return;
}

int AROSPDFApp::initAROS() {
	SplashColor paperColor;
        FitWidth=FALSE;
	BestFit=FALSE;
        FullScreen=FALSE;
        mcc = MUI_CreateCustomClass(NULL,MUIC_Area,NULL,sizeof(struct CustomImageData),MyDispatcher);
         // GUI creation    
        muiapp = ApplicationObject,
	  MUIA_Application_Title  , "AROSPDF",
	    MUIA_Application_Version , "$VER: AROSPDF-0.2.1 (2009/07/15)",
	    MUIA_Application_Copyright , "©2009, Craig Kiesau",
	    MUIA_Application_Author  , "Craig Kiesau",
	    MUIA_Application_Description, "A PDF viewer",
	    MUIA_Application_Base  , "APDF",
             SubWindow, wnd = WindowObject,
                 MUIA_Window_Title, "AROSPDF",
                 MUIA_Window_Width, 640,
                 MUIA_Window_Height, 480,
		 MUIA_Window_Menustrip,      menustrip=MUI_MakeObject(MUIO_MenustripNM,MenuData1,0),
                 WindowContents, VGroup,
                        Child, HGroup,
                          Child, butFirst = ImageObject,
                                 MUIA_Frame,  MUIV_Frame_ImageButton,
                                 MUIA_InputMode, MUIV_InputMode_RelVerify,
                                 MUIA_Image_Spec, "3:PROGDIR:resources/dblLeftArrow.png",
                          End,
                          Child, butPrev = ImageObject,
                                 MUIA_Frame, MUIV_Frame_ImageButton,
			         MUIA_InputMode, MUIV_InputMode_RelVerify,
                                 MUIA_Image_Spec, "3:PROGDIR:resources/leftArrow.png",
                          End,
                          Child, txtPage=StringObject,
                                MUIA_Frame, MUIV_Frame_String,
                                MUIA_FixWidth, 40,
                                MUIA_Background, MUII_TextBack,
                                MUIA_String_Accept, "0123456789",
                                MUIA_String_Format, MUIV_String_Format_Right,
                          End,
                          Child, txtPageCount=TextObject,
                                MUIA_FixWidth, 40,
                                MUIA_Text_PreParse, "\33l",
                          End,
                          Child, butNext = ImageObject,
                                 MUIA_Frame, MUIV_Frame_ImageButton,
                                 MUIA_InputMode, MUIV_InputMode_RelVerify,
                                 MUIA_Image_Spec, "3:PROGDIR:resources/rightArrow.png",
                          End,
                          Child, butLast = ImageObject,
                                 MUIA_Frame, MUIV_Frame_ImageButton,
                                 MUIA_InputMode, MUIV_InputMode_RelVerify,
                                 MUIA_Image_Spec, "3:PROGDIR:resources/dblRightArrow.png",
                          End,
                          Child, butZoomOut = TextObject,
                                 MUIA_FixWidth, 40,
                                 MUIA_Frame, MUIV_Frame_Button,
                                 MUIA_InputMode, MUIV_InputMode_RelVerify,
                                 MUIA_Text_Contents, "\33c-",
                          End,
                          Child, butZoomIn = TextObject,
                                 MUIA_FixWidth, 40,
                                 MUIA_Frame, MUIV_Frame_Button,
                                 MUIA_InputMode, MUIV_InputMode_RelVerify,
                                 MUIA_Text_Contents, "\33c+",
                          End,
                          Child, RectangleObject,End,
                      End,//HGroup
                      Child, HGroup,
                        Child, Bmp = NewObject(mcc->mcc_Class,NULL,
					TextFrame,MY_APP,(void *)this, MY_BMWIDTH, BITMAPX, MY_BMHEIGHT, BITMAPY,
					MUIA_Background, MUII_BACKGROUND,
					TAG_DONE),
                        //End, //Bmp
                        Child, vslider = ScrollbarObject,
			MUIA_Prop_Entries, 100,                                                                             
                        End,
                      End, // HGroup
		        Child, hslider = ScrollbarObject,                                                                               
                        MUIA_Group_Horiz, TRUE,
			MUIA_Prop_Entries, 100, 
			End,
                        End,
                 End, //VGroup
             End;
         if (muiapp != NULL)
         {
       ULONG sigs = 0;
       myhook.h_Entry = (HOOKFUNC)myfunction;
       menuhook.h_Entry = (HOOKFUNC)menufunction;
       buttonhook.h_Entry = (HOOKFUNC)buttonhookfunc;
	
	DoMethod(vslider, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
        (IPTR)vslider, 5, MUIM_CallHook, (IPTR)&myhook,(AROSPDFApp *)this,1,MUIV_TriggerValue);
	DoMethod(hslider, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
        (IPTR)hslider, 5, MUIM_CallHook, (IPTR)&myhook,(AROSPDFApp *)this,2,MUIV_TriggerValue);
        DoMethod(wnd, MUIM_Notify, MUIA_Window_MenuAction, MUIV_EveryTime,
	(IPTR)wnd, 4, MUIM_CallHook, (IPTR)&menuhook, (AROSPDFApp *)this, MUIV_TriggerValue);
        DoMethod(butFirst,MUIM_Notify,MUIA_Pressed,FALSE,(IPTR)muiapp,4,MUIM_CallHook,(IPTR)&buttonhook,(AROSPDFApp*)this,1);
        DoMethod(butPrev,MUIM_Notify,MUIA_Pressed,FALSE,(IPTR)muiapp,4,MUIM_CallHook,(IPTR)&buttonhook,(AROSPDFApp*)this,2);
        DoMethod(butNext,MUIM_Notify,MUIA_Pressed,FALSE,(IPTR)muiapp,4,MUIM_CallHook,(IPTR)&buttonhook,(AROSPDFApp*)this,3);
        DoMethod(butLast,MUIM_Notify,MUIA_Pressed,FALSE,(IPTR)muiapp,4,MUIM_CallHook,(IPTR)&buttonhook,(AROSPDFApp*)this,4);
        DoMethod(butZoomOut,MUIM_Notify,MUIA_Pressed,FALSE,(IPTR)muiapp,4,MUIM_CallHook,(IPTR)&buttonhook,(AROSPDFApp*)this,5);
        DoMethod(butZoomIn,MUIM_Notify,MUIA_Pressed,FALSE,(IPTR)muiapp,4,MUIM_CallHook,(IPTR)&buttonhook,(AROSPDFApp*)this,6);
        DoMethod(txtPage,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime,(IPTR)muiapp, 4, MUIM_CallHook, (IPTR)&buttonhook, (AROSPDFApp*)this,7);
        // Click Close gadget or hit Escape to quit
        DoMethod(wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
                (IPTR)muiapp, 2,
                MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);     
        DoMethod((APTR)DoMethod(menustrip,MUIM_FindUData,MEN_QUIT), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            (IPTR) muiapp, 2, MUIM_Application_ReturnID,
            MUIV_Application_ReturnID_Quit);

        // Open the window
        SetAttrs(wnd, MUIA_Window_Open, (LONG)TRUE,TAG_DONE);
        
        struct Window *window = (struct Window *)XGET(wnd, MUIA_Window_Window);
        rp=CreateRastPort();
        bm = AllocBitMap(BITMAPX, BITMAPY, 
        	         GetBitMapAttr(window->RPort->BitMap, BMA_DEPTH), BMF_CLEAR,
        	         window->RPort->BitMap);
        rp->BitMap=bm;
        SetAttrs(Bmp, MY_BM,bm,TAG_DONE);
        
  	paperColor[0] = paperColor[1] = paperColor[2] = 255;
  	splashOut = new AROSSplashOutputDev(rp, gFalse, paperColor, gFalse,
					     &redraw, (AROSPDFApp *)this);
	}
  return 0;
}
void AROSPDFApp::exitAROS()
{
   FreeRastPort(rp);
   FreeBitMap(bm);
   SetAttrs(wnd, MUIA_Window_Open, FALSE,TAG_DONE);
   if (FullScreen) CloseScreen(fsscreen);
   MUI_DisposeObject(muiapp);
   MUI_DeleteCustomClass(mcc);
}

void AROSPDFApp::redraw(AROSPDFApp *data)
{
  AROSPDFApp *me = (AROSPDFApp *)data;
  int pw, ph;
  int localx, localy;
  int destw, desth;
  pw = me->splashOut->getBitmapWidth();
  ph = me->splashOut->getBitmapHeight();
  
  if (me->posX + me->dispW > pw) {
    me->posX = pw - me->dispW;
    if (me->posX < 0)
      me->posX = 0;
  }
  if (me->posY + me->dispH > ph || me->posY == INT_MAX) {
    me->posY = ph - me->dispH;
    if (me->posY < 0)
      me->posY = 0;
  }
  localx=0;
  localy=0;
  destw=me->dispW;
  desth=me->dispH;
  //do we need to center the doc because it's smaller than the display?
  if (pw<(me->dispW)) {
	localx=localx+(me->dispW-pw)/2;
        destw=destw-(localx*2);
  }
  if (ph<(me->dispH)) {
	localy=localy+(me->dispH-ph)/2;
        desth=desth-(localy*2);
  }
  me->splashOut->redraw(me->posX, me->posY, localx, localy, destw, desth,me->dispW,me->dispH);
  DoMethod(me->Bmp, MUIM_Draw);
}

AROSPDFApp::~AROSPDFApp() {
  return;
}

void AROSPDFApp::ZoomToFit(bool WidthOnly) {
        int cropwidth,cropheight;
	if (!docLoaded) return;
        if (!BestFit) return;
        cropwidth=doc->getPageCropWidth(page);
        cropheight=doc->getPageCropHeight(page);
        //determine what res should be based on our display size
        if (WidthOnly) {
           //we're gonna fit by width
                resolution=72*(double)dispW/cropwidth;
        } else {
                //we're gonna fit by height
                resolution=72*(double)dispH/cropheight;
        }
        JumpToPage(page);
}

void AROSPDFApp::EnableDisableGUI() {
        //based on status we enable/disable buttons and menu options
        LONG docInvert=!docLoaded;
        DoMethod(wnd,MUIM_MultiSet,MUIA_Disabled, (LONG)!docLoaded,
                 butFirst,butPrev,butNext,butLast,butZoomOut,butZoomIn,txtPage,NULL);
        DoMethod(wnd,MUIM_MultiSet,MUIA_Menuitem_Enabled,(LONG)docLoaded,
                 (APTR)DoMethod(menustrip,MUIM_FindUData,MEN_PREVPAGE),
                 (APTR)DoMethod(menustrip,MUIM_FindUData,MEN_NEXTPAGE),
                 (APTR)DoMethod(menustrip,MUIM_FindUData,MEN_FIRSTPAGE),
                 (APTR)DoMethod(menustrip,MUIM_FindUData,MEN_LASTPAGE),
                 (APTR)DoMethod(menustrip,MUIM_FindUData,MEN_ZOOMIN),
                 (APTR)DoMethod(menustrip,MUIM_FindUData,MEN_ZOOMOUT),
                 NULL);
        if (docLoaded) {
                //we only do this stuff when there's a doc
                if (page==1) {
                DoMethod(wnd,MUIM_MultiSet,MUIA_Disabled,TRUE,
                         butFirst,butPrev,NULL);
                DoMethod(wnd,MUIM_MultiSet,MUIA_Menuitem_Enabled, FALSE,
                         (APTR)DoMethod(menustrip,MUIM_FindUData,MEN_PREVPAGE),
                         (APTR)DoMethod(menustrip,MUIM_FindUData,MEN_FIRSTPAGE),
                         NULL);
                }
                if (page==doc->getNumPages()) {
                DoMethod(wnd,MUIM_MultiSet,MUIA_Disabled,TRUE,
                         butNext,butLast,NULL);
                DoMethod(wnd,MUIM_MultiSet,MUIA_Menuitem_Enabled, FALSE,
                         (APTR)DoMethod(menustrip,MUIM_FindUData,MEN_NEXTPAGE),
                         (APTR)DoMethod(menustrip,MUIM_FindUData,MEN_LASTPAGE),
                         NULL);
                }
        }
}

void AROSPDFApp::ResizeBitMap(int width, int height){
        //get rid of old bitmap, set new one and change the slidebars accordingly
        int pw,ph;
FreeBitMap(bm);
        if (!FullScreen) {
	
	
        struct Window *window = (struct Window *)XGET(wnd, MUIA_Window_Window);
        bm = AllocBitMap(width, height, 
                 GetBitMapAttr(window->RPort->BitMap, BMA_DEPTH), BMF_CLEAR,
                 window->RPort->BitMap);
	 rp->BitMap=bm;
        SetAttrs(Bmp, MY_BM, bm,MY_BMHEIGHT, height, MY_BMWIDTH,width,TAG_DONE);
        }
	else
	{
		bm = AllocBitMap(width, height, 
                 GetBitMapAttr(fsscreen->RastPort.BitMap, BMA_DEPTH), BMF_CLEAR,
                 fsscreen->RastPort.BitMap);
	 	rp->BitMap=bm;
	}
        dispW=width;
        dispH=height;
        if(FitWidth) {
                int cropwidth=doc->getPageCropWidth(page);
                resolution=72*(double)dispW/cropwidth;
                  doc->displayPage(splashOut, page, resolution, resolution, 0, gFalse,
		   gFalse, gFalse);
        }
	if(BestFit) {
		int cropheight=doc->getPageCropHeight(page);
                resolution=72*(double)dispH/cropheight;
                  doc->displayPage(splashOut, page, resolution, resolution, 0, gFalse,
		   gFalse, gFalse);
	}

        pw = splashOut->getBitmapWidth();
        ph = splashOut->getBitmapHeight();
	if (!FullScreen) {
        	SetAttrs(vslider, MUIA_Prop_Entries, (LONG)(ph/SCROLLSPEED),MUIA_Prop_Visible,(LONG)(height/SCROLLSPEED),TAG_DONE);
	        SetAttrs(hslider, MUIA_Prop_Entries, (LONG)(pw/SCROLLSPEED),MUIA_Prop_Visible,(LONG)(width/SCROLLSPEED),TAG_DONE);
	}
	bug("Before redraw\n");
        redraw((void *)this);
}

void AROSPDFApp::setVert(int n) {
	if (docLoaded) {
	posY=n;
        redraw((void *)this);
	}
}
void AROSPDFApp::setHoriz(int n) {
	if (docLoaded) {
	posX=n;
        redraw((void *)this);
	}
}

