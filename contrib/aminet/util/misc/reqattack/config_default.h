
struct config defaultconfig = 
{
	INTERNAL_LOGO_INFO,		/* infologo					*/
	INTERNAL_LOGO_ASK,		/* asklogo					*/
	INTERNAL_LOGO_MULTIASK,	/* multiasklogo			*/
	INTERNAL_IMAGE_OK,		/* okimage					*/
	INTERNAL_IMAGE_CANCEL,	/* cancelimage				*/
	INTERNAL_IMAGE_THIRD,	/* thirdimage				*/
	0,								/* logo						*/
	0,								/* backfill					*/
 	0,								/* textbackground			*/
 	0,								/*	buttonbackfill			*/
	0x56,//"V",							/* okkey						*/
	0x42,//"B",							/* cancelkey				*/
	0x52,//"R",							/* runkey				J	*/
	0x45,//"E",							/* editkey				J	*/
	0x53,//"S",							/* iconifykey			J	*/

	"RAAHISound",				/* soundport				*/
	0,								/* sound						*/
	0,								/* oksound					*/
	0,								/* cancelsound				*/
	0,								/* othersound1				*/
	0,								/* othersound2				*/
	0,								/* othersound3				*/
	0,								/* textfont					*/
	0,								/* textfontsize			*/
	0,								/* buttonfont				*/
	0,								/* buttonfontsize			*/
	
	{
	 {-SHADOWPEN},				/* hilightpen				*/
	 {-SHINEPEN},				/* pattern1pen				*/
	 {-BACKGROUNDPEN},		/* pattern2pen				*/
	 {-BACKGROUNDPEN},		/* textboxbgpen			*/
	 {-TEXTPEN},				/* textboxfgpen			*/
	 {RCOLOR_IGNORE},			/* textboxshadowpen		*/
	 {-SHADOWPEN},				/* otextboxedge1			*/
	 {-SHINEPEN},				/* otextboxedge2			*/
	 {RCOLOR_IGNORE},			/* itextboxedge1			*/
	 {RCOLOR_IGNORE},			/* itextboxedge2			*/
	 {-BACKGROUNDPEN},		/* buttonbgpen				*/
	 {-FILLPEN},				/* buttonbgselpen			*/
	 {-BACKGROUNDPEN},		/* buttonbghipen			*/
	 {-TEXTPEN},				/* buttonfgpen				*/
	 {-FILLTEXTPEN},			/* buttonfgselpen			*/
	 {-TEXTPEN},				/* buttonfghipen			*/
	 {-SHINEPEN},				/* obuttonedge1			*/
	 {-SHADOWPEN},				/* obuttonedge2			*/
	 {RCOLOR_IGNORE},			/* ibuttonedge1			*/
	 {RCOLOR_IGNORE},			/* ibuttonedge2			*/
	 {-SHADOWPEN},				/* obuttonedge1sel		*/
	 {-SHINEPEN},				/* obuttonedge2sel		*/
	 {RCOLOR_IGNORE},			/* ibuttonedge1sel		*/
	 {RCOLOR_IGNORE},			/* ibuttonedge2sel		*/
	 {-SHINEPEN},				/* obuttonedge1hi			*/
	 {-SHADOWPEN},				/* obuttonedge2hi			*/
	 {RCOLOR_IGNORE},			/* ibuttonedge1hi			*/
	 {RCOLOR_IGNORE}			/* ibuttonedge2hi			*/
	},
	
	5,								/* imagecaches				*/
	BITMAPF_FRIEND |
	BITMAPF_MINPLANES,		/* bitmapflags				*/
	
	IEQUALIFIER_LCOMMAND,	/* keyqualifier			*/
	TRUE,							/* cooldragging			*/
	4,								/* outerspacingx			*/
	4,								/* outerspacingy			*/
	7,								/* innerspacingx			*/
	7,								/* innerspacingy			*/
	4,								/* bodygadgetspacing		*/
	8,								/* buttonspacing			*/
	8,								/* bodylogospacing		*/
	ALIGN_CENTER,				/* logoalign				*/
	ALIGN_CENTER,				/* bodyxalign				*/
	ALIGN_CENTER,				/* bodyyalign				*/
	ALIGN_LEFT,					/* reqtextalign			*/
	ALIGN_SPREAD,				/* buttonalign				*/
	BALIGN_BOTTOM,				/* buttonyalign				*/
	1,								/* hithickx					*/
	1,								/* hithicky					*/
	RPOS_NORMAL,				/* reqpos					*/
	FALSE,						/* nobuttonimages			*/
	FALSE,						/* nologo					*/
	FALSE,						/* samebuttonwidth		*/
	FS_NORMAL,					/* buttontextstyle		*/
	FS_NORMAL,					/* buttontextselstyle	*/
	FS_NORMAL,					/* buttontexthistyle		*/
	FALSE,						/* shiftbuttontext      */
	4,								/* buttonispacingx		*/
	2,								/* buttonispacingy		*/
	2,								/* buttonispacingy2		*/
	8,								/* buttonextrawidth		*/
	4,								/* bimagetextspacing		*/
	FALSE,						/* functionkeys			*/
	FALSE,						/* donotpatch				*/
	FALSE,						/* roundgadgets		J	*/
	-1,							/* reqoff 					*/
	ALIGN_LEFT,					/* logoalign2 JADDS	*/
	0,								/* previewmode				*/
	0,								/* info						*/
	NULL						/* reqoffexec				*/
};

