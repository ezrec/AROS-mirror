struct translation
{
	char *string;
	LONG value;
};

#define TRANS_NUMBER ((char *)1)
#define TRANS_STRING ((char *)3)
#define TRANS_PEN		((char *)5)
#define TRANS_TSTYLE ((char *)7)
#define TRANS_BFLAGS ((char *)9)

#define TRANS_END		((char *)0)

static struct translation image_translation[] =
{
	{"$infologo",			(LONG)INTERNAL_LOGO_INFO		},
	{"$asklogo",			(LONG)INTERNAL_LOGO_ASK			},
	{"$multiasklogo",		(LONG)INTERNAL_LOGO_MULTIASK	},
	{"$okimage",			(LONG)INTERNAL_IMAGE_OK			},
	{"$cancelimage",		(LONG)INTERNAL_IMAGE_CANCEL	},
	{"$thirdimage",		(LONG)INTERNAL_IMAGE_THIRD		},
	{"$off",					(LONG)IMAGE_OFF					},
	{TRANS_STRING,			0										},
	{TRANS_END														}
};

static struct translation pen_translation[] =
{
	{"detailpen",			-DETAILPEN			},
	{"blockpen",			-BLOCKPEN			},
	{"textpen",				-TEXTPEN				},
	{"shinepen",			-SHINEPEN			},
	{"shadowpen",			-SHADOWPEN			},
	{"fillpen",				-FILLPEN				},
	{"filltextpen",		-FILLTEXTPEN		},
	{"backgroundpen",		-BACKGROUNDPEN		},
	{"highlighttextpen",	-HIGHLIGHTTEXTPEN	},
	{"bardetailpen",		-BARDETAILPEN		},
	{"barblockpen",		-BARBLOCKPEN		},
	{"bartrimpen",			-BARTRIMPEN			},
	{"<ignore>",			RCOLOR_IGNORE		},
	{TRANS_PEN,				0						},
	{TRANS_END										}
};

static struct translation align_translation[] =
{
	{"top",		-1	},
	{"left",	-1	},
	{"bottom",	1	},
	{"right",	1	},
	{"center",	0	},
	{"spread",	2	},
	{TRANS_END		}
};

static struct translation balign_translation[] = 
{
	{"left",	-1	},
	{"top",		0	},
	{"right",	1	},
	{"bottom",	2	},
	{TRANS_END		}
};

static struct translation reqpos_translation[] =
{
	{"normal",	0	},
	{"center",	1	},
	{"mouse",	2	},
	{TRANS_END		}
};

static struct translation quali_translation[] =
{
	{"lalt"		,IEQUALIFIER_LALT										},
	{"ralt"		,IEQUALIFIER_RALT										},
	{"alt"		,IEQUALIFIER_LALT | IEQUALIFIER_RALT			},
	{"lshift"	,IEQUALIFIER_LSHIFT									},
	{"rshift"	,IEQUALIFIER_RSHIFT									},
	{"shift"		,IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT		},
	{"lcommand" ,IEQUALIFIER_LCOMMAND								},
	{"rcommand" ,IEQUALIFIER_RCOMMAND								},
	{"command"	,IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND	},
	{"control"	,IEQUALIFIER_CONTROL									},
	{"any"		,0															},
	{TRANS_END																}
};

static struct translation bool_translation[] =
{
	{"yes",	1},
	{"true",	1},
	{"on",	1},
	{"no",	0},
	{"false",0},
	{"off",	0},
	{TRANS_END}
};

static struct translation string_translation[] =
{
	{TRANS_STRING,0},
	{TRANS_END		}
};

static struct translation number_translation[] =
{
	{TRANS_NUMBER,0},
	{TRANS_END		}
};

static struct translation gstyle_translation[] =
{
	{"win95a",		GADSTYLE_WIN95A	},
	{"win95b",		GADSTYLE_WIN95B	},
	{"kde",			GADSTYLE_KDE		},
	{"qt",			GADSTYLE_QT			},
	{"monoa",		GADSTYLE_MONOA		},
	{"monob",		GADSTYLE_MONOB		},
	{"amithick",	GADSTYLE_AMITHICK	},
	{"amithin",		GADSTYLE_AMITHIN	},
	{TRANS_END								}
};

static struct translation tstyle_translation[] =
{
	{TRANS_TSTYLE,	0},
	{TRANS_END		 }
};

static struct translation bflags_translation[] =
{
	{TRANS_BFLAGS,	0},
	{TRANS_END		 }
};

/* J adds!*/
static struct translation font_translation[] =
{
	{"$screenfont", 0},
	{TRANS_STRING,	 0},
	{TRANS_END}
	
};

static struct translation appicon_translation[] =
{
	{"$defappicon", 0},
	{TRANS_STRING,	 0},
	{TRANS_END		  }
};
