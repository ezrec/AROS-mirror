#define WHATKIND_STRING  1
#define WHATKIND_INTEGER 2

#define POKEB  1
#define POKEW  2
#define POKEL  3
#define POKES  4
#define POKES0 5  /* "" --> NULL */
#define POKEF  6	/* special function */
#define POKEC  7	/* char poking...*/
#define POKESB 8  /* copy string to backconfig */

#define NUM_WHATS 96

#define ofs(x) offsetof(struct config,x)
#define ofsb(x) offsetof(struct backconfig,x)

struct what
{
	char *variable;
	WORD poke;
	LONG offset;
	LONG (*translatefunc)(char *s,APTR data);
	APTR data;
};

static struct what whatarray[NUM_WHATS] =
{
	{"infologo",			POKES	,ofs(infologo),				(APTR)TranslateTable,image_translation		},
	{"asklogo",				POKES	,ofs(asklogo),				(APTR)TranslateTable,image_translation		},
	{"multiasklogo",		POKES	,ofs(multiasklogo),			(APTR)TranslateTable,image_translation		},
	{"logo",				POKES	,ofs(logo),					(APTR)TranslateTable,image_translation		},
	{"okimage",				POKES	,ofs(okimage),				(APTR)TranslateTable,image_translation		},
	{"cancelimage",			POKES	,ofs(cancelimage),			(APTR)TranslateTable,image_translation  	},
	{"thirdimage",			POKES	,ofs(thirdimage),			(APTR)TranslateTable,image_translation  	},
	{"background",			POKES0,ofs(backfill),				(APTR)TranslateTable,image_translation		},
	{"textboxbackground", 	POKES0,ofs(textbackground),			(APTR)TranslateTable,image_translation		},
	{"buttonbackground",	POKES0,ofs(buttonbackfill),			(APTR)TranslateTable,image_translation		},
	{"okkey",				POKEC	,ofs(okkey),				(APTR)TranslateTable,string_translation	},
	{"cancelkey",			POKEC	,ofs(cancelkey),		 	(APTR)TranslateTable,string_translation	},
	{"runkey",				POKEC	,ofs(runkey),				(APTR)TranslateTable,string_translation	},
	{"editkey",				POKEC	,ofs(editkey),			 	(APTR)TranslateTable,string_translation	},
	{"iconifykey",			POKEC	,ofs(iconifykey),			(APTR)TranslateTable,string_translation	},

	{"soundport",			POKESB,ofsb(soundport),				(APTR)TranslateTable,string_translation	},
	{"playsound",			POKES	,ofs(sound),				(APTR)TranslateTable,string_translation	},
	{"oksound",				POKESB,ofsb(oksound),				(APTR)TranslateTable,string_translation	},
	{"cancelsound",			POKESB,ofsb(cancelsound),			(APTR)TranslateTable,string_translation	},
	{"othersound1",			POKESB,ofsb(othersound1),			(APTR)TranslateTable,string_translation	},
	{"othersound2",			POKESB,ofsb(othersound2),			(APTR)TranslateTable,string_translation	},
	{"othersound3",			POKESB,ofsb(othersound3),			(APTR)TranslateTable,string_translation	},
	
	{"textfont",			POKES, ofs(textfont),				(APTR)TranslateTable,font_translation	},
	{"textfontsize",		POKEW, ofs(textfontsize),			(APTR)TranslateTable,number_translation	},
	{"buttonfont",			POKES, ofs(buttonfont),				(APTR)TranslateTable,font_translation	},
	{"buttonfontsize",		POKEW, ofs(buttonfontsize),			(APTR)TranslateTable,number_translation	},

	{"hilightpen",			POKEL	,ofs(pens[0].color),		(APTR)TranslateTable,pen_translation		},
	{"bgpattern1pen",		POKEL	,ofs(pens[1].color),		(APTR)TranslateTable,pen_translation		},
	{"bgpattern2pen",		POKEL	,ofs(pens[2].color),		(APTR)TranslateTable,pen_translation		},
	{"textboxbgpen",		POKEL	,ofs(pens[3].color),		(APTR)TranslateTable,pen_translation		},
	{"textboxtextpen",		POKEL	,ofs(pens[4].color),		(APTR)TranslateTable,pen_translation		},
	{"textboxshadowpen",	POKEL	,ofs(pens[5].color),		(APTR)TranslateTable,pen_translation		},
	{"otextboxedge1pen",	POKEL	,ofs(pens[6].color),		(APTR)TranslateTable,pen_translation		},
	{"otextboxedge2pen",	POKEL	,ofs(pens[7].color),		(APTR)TranslateTable,pen_translation		},
	{"itextboxedge1pen",	POKEL	,ofs(pens[8].color),		(APTR)TranslateTable,pen_translation		},
	{"itextboxedge2pen",	POKEL	,ofs(pens[9].color),		(APTR)TranslateTable,pen_translation		},
	{"buttonbgpen",			POKEL	,ofs(pens[10].color),		(APTR)TranslateTable,pen_translation		},
	{"buttonbgselpen",		POKEL	,ofs(pens[11].color),		(APTR)TranslateTable,pen_translation		},
	{"buttonbghipen",		POKEL	,ofs(pens[12].color),		(APTR)TranslateTable,pen_translation		},
	{"buttontextpen",		POKEL	,ofs(pens[13].color),		(APTR)TranslateTable,pen_translation		},
	{"buttontextselpen",	POKEL	,ofs(pens[14].color),		(APTR)TranslateTable,pen_translation		},
	{"buttontexthipen",		POKEL	,ofs(pens[15].color),		(APTR)TranslateTable,pen_translation		},
	{"obuttonedge1pen",		POKEL	,ofs(pens[16].color),		(APTR)TranslateTable,pen_translation		},
	{"obuttonedge2pen",		POKEL	,ofs(pens[17].color),		(APTR)TranslateTable,pen_translation		},
	{"ibuttonedge1pen",		POKEL	,ofs(pens[18].color),		(APTR)TranslateTable,pen_translation		},
	{"ibuttonedge2pen",		POKEL	,ofs(pens[19].color),		(APTR)TranslateTable,pen_translation		},
	{"obuttonedge1selpen",	POKEL	,ofs(pens[20].color),		(APTR)TranslateTable,pen_translation		},
	{"obuttonedge2selpen",	POKEL	,ofs(pens[21].color),		(APTR)TranslateTable,pen_translation		},
	{"ibuttonedge1selpen",	POKEL	,ofs(pens[22].color),		(APTR)TranslateTable,pen_translation		},
	{"ibuttonedge2selpen",	POKEL	,ofs(pens[23].color),		(APTR)TranslateTable,pen_translation		},
	{"obuttonedge1hipen",	POKEL	,ofs(pens[24].color),		(APTR)TranslateTable,pen_translation		},
	{"obuttonedge2hipen",	POKEL	,ofs(pens[25].color),		(APTR)TranslateTable,pen_translation		},
	{"ibuttonedge1hipen",	POKEL	,ofs(pens[26].color),		(APTR)TranslateTable,pen_translation		},
	{"ibuttonedge2hipen",	POKEL	,ofs(pens[27].color),		(APTR)TranslateTable,pen_translation		},

	{"buttonstyle",			POKEF	,(LONG)Poke_GadStyle,		(APTR)TranslateTable,gstyle_translation 	},

	{"imagecaches",			POKEW	,ofs(imagecaches),			(APTR)TranslateTable,number_translation	},
	{"bitmapflags",			POKEB	,ofs(bitmapflags),			(APTR)TranslateTable,bflags_translation	},
		
	{"keyqualifier",		POKEB	,ofs(keyqualifier),			(APTR)TranslateTable,quali_translation		},
	{"cooldragging",		POKEB	,ofs(cooldragging),			(APTR)TranslateTable,bool_translation		},
	{"outerspacingx",		POKEB	,ofs(outerspacingx),		(APTR)TranslateTable,number_translation	},
	{"outerspacingy",		POKEB	,ofs(outerspacingy),		(APTR)TranslateTable,number_translation	},
	{"innerspacingx",		POKEB	,ofs(innerspacingx),		(APTR)TranslateTable,number_translation	},
	{"innerspacingy",		POKEB	,ofs(innerspacingy),		(APTR)TranslateTable,number_translation	},
	{"bodygadgetspacing",	POKEB	,ofs(bodygadgetspacing),	(APTR)TranslateTable,number_translation	},
	{"buttonspacing",		POKEB	,ofs(buttonspacing),		(APTR)TranslateTable,number_translation	},
	{"bodylogospacing",		POKEB	,ofs(bodylogospacing),		(APTR)TranslateTable,number_translation	},
	{"logoalign",			POKEB	,ofs(logoalign),			(APTR)TranslateTable,align_translation		},
	{"bodyxalign",			POKEB	,ofs(bodyxalign),			(APTR)TranslateTable,align_translation		},
	{"bodyyalign",			POKEB	,ofs(bodyyalign),			(APTR)TranslateTable,align_translation		},
	{"reqtextalign",		POKEB	,ofs(reqtextalign),			(APTR)TranslateTable,align_translation		},
	{"buttonalign",			POKEB	,ofs(buttonalign),			(APTR)TranslateTable,align_translation		},
	{"buttonyalign",		POKEB	,ofs(buttonyalign),			(APTR)TranslateTable,balign_translation		},
	{"hithickx",			POKEB	,ofs(hithickx),		  		(APTR)TranslateTable,number_translation	},
	{"hithicky",			POKEB	,ofs(hithicky),				(APTR)TranslateTable,number_translation	},
	{"reqpos",				POKEB	,ofs(reqpos),				(APTR)TranslateTable,reqpos_translation	},
	{"nobuttonimages",		POKEB	,ofs(nobuttonimages),		(APTR)TranslateTable,bool_translation		},
	{"nologo",				POKEB	,ofs(nologo),				(APTR)TranslateTable,bool_translation		},
	{"samebuttonwidth",		POKEB	,ofs(samebuttonwidth),		(APTR)TranslateTable,bool_translation		},
	{"buttontextstyle",		POKEB	,ofs(buttontextstyle),		(APTR)TranslateTable,tstyle_translation	},
	{"buttontextselstyle",	POKEB	,ofs(buttontextselstyle),	(APTR)TranslateTable,tstyle_translation 	},
	{"buttontexthistyle",	POKEB	,ofs(buttontexthistyle),	(APTR)TranslateTable,tstyle_translation 	},
	{"shiftbuttontext",		POKEB	,ofs(shiftbuttontext),		(APTR)TranslateTable,bool_translation		},
	{"buttonispacingx",		POKEB	,ofs(buttonispacingx),		(APTR)TranslateTable,number_translation	},
	{"buttonispacingy",		POKEB	,ofs(buttonispacingy),		(APTR)TranslateTable,number_translation	},
	{"buttonispacingy2",	POKEB	,ofs(buttonispacingy2),		(APTR)TranslateTable,number_translation	},
	{"buttonextrawidth",	POKEB	,ofs(buttonextrawidth),		(APTR)TranslateTable,number_translation	},
	{"bimagetextspacing",	POKEB	,ofs(bimagetextspacing),	(APTR)TranslateTable,number_translation	},
	{"functionkeys",		POKEB	,ofs(functionkeys),			(APTR)TranslateTable,bool_translation		},
	{"donotpatch",			POKEB	,ofs(donotpatch),			(APTR)TranslateTable,bool_translation		},
	{"roundbuttons",		POKEB	,ofs(roundbuttons),			(APTR)TranslateTable,bool_translation		},
	{"reqoff",				POKEB	,ofs(reqoff),				(APTR)TranslateTable,number_translation	},
	{"logo2align",			POKEB	,ofs(logoalign2),			(APTR)TranslateTable,align_translation		},
	{"previewmode",			POKEB	,ofs(previewmode),			(APTR)TranslateTable,bool_translation		},
	{"info",				POKESB	,ofsb(info),				(APTR)TranslateTable,appicon_translation	},
	{"reqoffexec",			POKES	,ofs(reqoffexec),			(APTR)TranslateTable,string_translation		}	
};

