#define CHECKTYPE_STRING  1
#define CHECKTYPE_INTEGER 2

#define NUM_CHECKS 16

struct check
{
	char *variable;
	WORD checktype;
	LONG (*getfunc)(struct ReqNode *reqnode);
};

static struct check checkarray[NUM_CHECKS] =
{
	{"taskname",		CHECKTYPE_STRING, (APTR)GetTaskName			},
	{"procname",		CHECKTYPE_STRING, (APTR)GetProcName			},
	{"progname",		CHECKTYPE_STRING, (APTR)GetProgName			},
	{"progname2",		CHECKTYPE_STRING,	(APTR)GetProgName2		},
	{"screentitle",	CHECKTYPE_STRING, (APTR)GetScreenTitle		},
	{"reqtitle",		CHECKTYPE_STRING, (APTR)GetReqTitle			},
	{"reqtext",			CHECKTYPE_STRING, (APTR)GetReqText			},
	{"screendepth",	CHECKTYPE_INTEGER,(APTR)GetScreenDepth		},
	{"screenwidth",	CHECKTYPE_INTEGER,(APTR)GetScreenWidth		},
	{"screenheight",	CHECKTYPE_INTEGER,(APTR)GetScreenHeight	},
	{"visiblewidth",	CHECKTYPE_INTEGER,(APTR)GetVisibleWidth	},
	{"visibleheight",	CHECKTYPE_INTEGER,(APTR)GetVisibleHeight	},
	{"resolutionx",	CHECKTYPE_INTEGER,(APTR)GetResolutionX		},
	{"resolutiony",	CHECKTYPE_INTEGER,(APTR)GetResolutionY		},
	{"aspect",			CHECKTYPE_INTEGER,(APTR)GetAspect			},
	{"numbuttons",		CHECKTYPE_INTEGER,(APTR)GetNumButtons		}
};

