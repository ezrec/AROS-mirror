#include <exec/semaphores.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <utility/hooks.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/dos_lib.h>
#include <pragma/intuition_lib.h>
#else
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#endif

#include <clib/alib_protos.h>

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
//#include <functions.h>

#include "global.h"
#include "patch.h"
#include "misc.h"
#include "asmmisc.h"
#include "config.h"
#include "myimage.h"
#include "myimage_protos.h"
#include "reqlist.h"
#include "patternmatcher.h"
#include "config_protos.h"

#define CONFIGFILE "ENV:ReqAttack.prefs"
#define MAX_NESTING 10

ULONG configmask;

static void InitCompile(void);
static void CompileLine(char *line);
static void EndCompile(void);
static void Poke_GadStyle(struct ReqNode *reqnode,LONG data);

static void ExecuteList(struct List *list,struct ReqNode *reqnode);

/*****************************************************/

static struct List configcmdlist,alloclist;
static struct SignalSemaphore configsem;
static struct NotifyRequest notify;
static char s[256],token[260];
static APTR mempool;
static BPTR MyHandle;
static char *errorstring;
static char *configfilename = CONFIGFILE;
static WORD nest_count,nest_count_inc,line_count;
static UWORD config_age;
static BOOL notify_started,config_stopped;

#include "config_default.h"
#include "config_gadstyles.h"
#include "config_trans.h"

static LONG TranslateTable(char *s,struct translation *trans);

#include "config_whats.h"
#include "config_checks.h"
#include "config_ops.h"
#include "config_err.h"

static struct List *nesttable[MAX_NESTING];

static void ConfigError(char *err)
{
	errorstring = err;
};

static APTR ConfigAlloc(ULONG size)
{
	struct configmem *rc = 0;

	size += sizeof(struct configmem);

	if ((rc = AllocPooled(mempool,size)))
	{
		rc->size = size;
		rc->age  = config_age;

		AddTail(&alloclist,(struct Node *)rc);

		rc++;
	}

	return rc;
}

static void ConfigFree(APTR mem)
{
	struct configmem *cm;

	if (mem)
	{
		cm = (struct configmem *)mem;
		cm--;
		Remove((struct Node *)cm);

		FreePooled(mempool,cm,cm->size);
	}
}

static void ConfigFreeAll(UWORD age)
{
	struct configmem *cm,*succ;

	cm = (struct configmem *)alloclist.lh_Head;
	while ((succ = (struct configmem *)cm->node.mln_Succ))
	{
		if ((age == CONFIGAGE_ALL) ||
			 (cm->age == age))
		{
			ConfigFree(++cm);
		}
		cm = succ;
	}
}

/*****************************************************/

void InitConfig(void)
{
	NewList(&alloclist);
	InitSemaphore(&configsem);

	if (!(mempool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR,2000,2000)))
	{
		Cleanup("InitConfig: Out of memory!");
	}

	ReadConfig();

	notify.nr_Name = configfilename;
	notify.nr_Flags = NRF_SEND_SIGNAL;
	notify.nr_stuff.nr_Signal.nr_Task = FindTask(0);
	notify.nr_stuff.nr_Signal.nr_SignalNum = AllocSignal(-1);

	if (StartNotify(&notify))
	{
		configmask = 1L << notify.nr_stuff.nr_Signal.nr_SignalNum;
		notify_started = TRUE;
	}
}

void CleanupConfig(void)
{
	if (MyHandle) Close(MyHandle);
	if (mempool) DeletePool(mempool);
	if (notify_started)
	{
		EndNotify(&notify);
	}
	if (notify.nr_stuff.nr_Signal.nr_SignalNum)
	{
		FreeSignal(notify.nr_stuff.nr_Signal.nr_SignalNum);
	}
}

void ReadConfig(void)
{
	struct List old_configcmdlist;
	char tempstring[256];
	WORD i;

	ObtainSemaphore(&configsem);

	errorstring = 0;
	old_configcmdlist = configcmdlist;
	config_age++;

	InitCompile();

	if ((MyHandle = Open(configfilename,MODE_OLDFILE)))
	{
		while(FGets(MyHandle,s,255))
		{
			i = strlen(s) - 1;
			if (i >= 0)
			{
				if (s[i] == '\n') s[i] = '\0';
			};

			CompileLine(s);
			if (errorstring) break;
		}

		Close(MyHandle);
		MyHandle = 0;

	}

	EndCompile();

	if (errorstring)
	{
		if (config_age == 1)
		{
			ULONG argt[] = {(ULONG)line_count,(ULONG)tempstring};

			strcpy(tempstring,errorstring);

		    
    	#ifdef NO_ASM
	    {
	    	STRPTR strptr = s;
			RawDoFmt("Configuration file [line %ld]: %s!",argt,
					 			 (APTR)Copy_PutChFunc,&strptr);
	    }
	#else
			RawDoFmt("Configuration file [line %ld]: %s!",argt,
					 			 (APTR)Copy_PutChFunc,s);
    	#endif
			//sprintf(s,,line_count,tempstring);
			Cleanup(s);
		} else {
			configcmdlist = old_configcmdlist;
			ConfigFreeAll(config_age);
			config_age--;
			DisplayBeep(0);
		}
	} else {
		if (config_age != 1)
		{
			ConfigFreeAll(config_age - 1);
		}
	}

	ReleaseSemaphore(&configsem);

	progport.configcounter++;
}

void InitExecuteConfig(void)
{
	ObtainSemaphoreShared(&configsem);
	config_stopped = FALSE;
}

void ExecuteConfig(struct ReqNode *reqnode)
{

	reqnode->cfg = defaultconfig;

	ExecuteList(&configcmdlist,reqnode);

	defaultconfig.imagecaches = reqnode->cfg.imagecaches;

	// fill the backconfig structure!!!
/*	if (reqnode->cfg.soundport) strncpy(reqnode->bcfg.soundport,reqnode->cfg.soundport,256);
	if (reqnode->cfg.oksound) strncpy(reqnode->bcfg.oksound,reqnode->cfg.oksound,256);
	if (reqnode->cfg.cancelsound) strncpy(reqnode->bcfg.cancelsound,reqnode->cfg.cancelsound,256);
	if (reqnode->cfg.othersound1) strncpy(reqnode->bcfg.othersound1,reqnode->cfg.othersound1,256);
	if (reqnode->cfg.othersound2) strncpy(reqnode->bcfg.othersound2,reqnode->cfg.othersound2,256);
	if (reqnode->cfg.othersound3) strncpy(reqnode->bcfg.othersound3,reqnode->cfg.othersound3,256);
	if (reqnode->cfg.info) strncpy(reqnode->bcfg.info,reqnode->cfg.info,256);
*/
}

void DoneExecuteConfig(void)
{
	ReleaseSemaphore(&configsem);
}

void HandleConfig(void)
{
	ReadConfig();
}

/*****************************************************/

static LONG TranslateTable(char *s,struct translation *trans)
{
	LONG i,rc;

	rc = 0;

	while(1)
	{
		if (trans->string == TRANS_NUMBER)
		{
			rc = strtol(s,0,0);
			break;

		} else if (trans->string == TRANS_STRING)
		{
			i = strlen(s) + 1;
			if (!(rc = (LONG)ConfigAlloc(i)))
			{
				ConfigError(ERR_MEM);
				return 0;
			}
			memcpy((APTR)rc,s,i);
			break;

		} else if (trans->string == TRANS_PEN)
		{
			if (s[0] == '#')
			{
				rc = RCOLOR_DIRECT;
				s++;
			}
			rc += strtol(s,0,0);
			break;

		} else if (trans->string == TRANS_TSTYLE)
		{
			while(*s)
			{
				switch(*s)
				{
					case 'B':
						rc |= FSF_BOLD;
						break;

					case 'I':
						rc |= FSF_ITALIC;
						break;

					case 'U':
						rc |= FSF_UNDERLINED;
						break;
				}
				s++;
			}
			break;
		} else if (trans->string == TRANS_BFLAGS)
		{
			while(*s)
			{
				switch(*s)
				{
					case 'D':
						rc |= BITMAPF_DISPLAYABLE;
						break;

					case 'M':
						rc |= BITMAPF_MINPLANES;
						break;

					case 'F':
						rc |= BITMAPF_FRIEND;
						break;

					case 'I':
						rc |= BITMAPF_INTERLEAVED;
						break;
				}
				s++;
			}
			break;
		} else if (trans->string == TRANS_END)
		{
			ConfigError(ERR_BADVALUE);
			return 0;

		} else {
			if (!stricmp(trans->string,s))
			{
				rc = trans->value;
				break;
			}
		}

		trans++;

	} /* while(1) */

	return rc;
};

static void Poke_GadStyle(struct ReqNode *reqnode,LONG data)
{
	struct gadgetstyle *style;
	struct config *cfg;

	cfg = &reqnode->cfg;
	style = &gadstylearray[data];

	cfg->pens[RPEN_BUTTONBG].color			= 	style->bg;
	cfg->pens[RPEN_BUTTONBGSEL].color		=	style->bgsel;
	cfg->pens[RPEN_BUTTONBGHI].color			=	style->bghi;

	cfg->pens[RPEN_BUTTONFG].color			=	style->fg;
	cfg->pens[RPEN_BUTTONFGSEL].color		=	style->fgsel;
	cfg->pens[RPEN_BUTTONFGHI].color			=	style->fghi;

	cfg->pens[RPEN_OBUTTONEDGE1].color		=	style->outer1;
	cfg->pens[RPEN_OBUTTONEDGE2].color		=	style->outer2;
	cfg->pens[RPEN_IBUTTONEDGE1].color		=	style->inner1;
	cfg->pens[RPEN_IBUTTONEDGE2].color		=	style->inner2;

	cfg->pens[RPEN_OBUTTONEDGE1SEL].color	=	style->outer1sel;
	cfg->pens[RPEN_OBUTTONEDGE2SEL].color	=	style->outer2sel;
	cfg->pens[RPEN_IBUTTONEDGE1SEL].color	=	style->inner1sel;
	cfg->pens[RPEN_IBUTTONEDGE2SEL].color	=	style->inner2sel;

	cfg->pens[RPEN_OBUTTONEDGE1HI].color	=	style->outer1hi;
	cfg->pens[RPEN_OBUTTONEDGE2HI].color	=	style->outer2hi;
	cfg->pens[RPEN_IBUTTONEDGE1HI].color	=	style->inner1hi;
	cfg->pens[RPEN_IBUTTONEDGE2HI].color	=	style->inner2hi;

};

/*****************************************************/

static void InitCompile(void)
{
	NewList(&configcmdlist);

	nest_count = 0;
	line_count = 0;
	nesttable[0] = &configcmdlist;
}

static void EndCompile(void)
{
	if (!errorstring)
	{
		if (nest_count)
		{
			ConfigError(ERR_EOF);
		}
	}
}

static char *SkipSpaces(char *s)
{
	while((*s == ' ') || (*s == '\t')) s++;

	return s;
}

static char *GetToken(char *s)
{
	char c,*dest;
	BOOL quoted = FALSE,done = FALSE,alwaysok = FALSE;

	s = SkipSpaces(s);

	dest = token;

	while(!done)
	{
		c = *s;

		switch(c)
		{
			case '\t':
			case ' ':
				if (!quoted)
				{
					done = TRUE;
				} else {
					*dest++ = c;
					s++;
				}
				break;

			case '\0':
				done = TRUE;
				break;

			case '_':
				if (quoted) *dest++ = c;
				s++;
				break;

			case '"':
				quoted = !quoted;
				s++;
				alwaysok = TRUE;
				break;

			default:
				*dest++ = c;
				s++;
				break;

		} /* switch(c) */

	} /* while(!done) */

	*dest++ = '\0';

	return (token[0] || alwaysok) ? s : 0;
}

static BOOL CompileSet(char *line,struct configcmd *cmd)
{
	LONG i;

	cmd->cmd = CMD_SET;

	for(i = 0;i < NUM_WHATS;i++)
	{
		if (!stricmp(whatarray[i].variable,token))
		{
			cmd->par.set.what = i;
			break;
		}
	}

	if (i == NUM_WHATS)
	{
		ULONG argt[] = {(ULONG)token};

    	#ifdef NO_ASM
	    {
	    	STRPTR strptr = s;
		RawDoFmt(ERR_UNKNOWNSET,argt,
				 (APTR)Copy_PutChFunc,&strptr);
	    }
	#else
		RawDoFmt(ERR_UNKNOWNSET,argt,
				 (APTR)Copy_PutChFunc,s);
	#endif
		//sprintf(s,ERR_UNKNOWNSET,token);
		ConfigError(s);
		return FALSE;
	}

	if (!(line = GetToken(line)))
	{
		ConfigError(ERR_MISSINGISANDVALUE);
		return FALSE;
	}

	if (strcmp(token,"="))
	{
		ConfigError(ERR_SET);
		return FALSE;
	}

	if (!(line = GetToken(line)))
	{
		ConfigError(ERR_MISSINGVALUE);
		return FALSE;
	}

	cmd->par.set.data = whatarray[i].translatefunc(token,whatarray[i].data);

	return TRUE;
}

static BOOL CompileIf(char *line,struct configcmd *cmd)
{
	LONG i;

	cmd->cmd = CMD_IF;

	if (nest_count >= (MAX_NESTING - 1))
	{
		ConfigError(ERR_NESTING);
		return FALSE;
	}

	nest_count_inc = 1;

	NewList((struct List *)&cmd->par._if.then);
	nesttable[nest_count + 1] = (struct List *)&cmd->par._if.then;

	if (!(line = GetToken(line)))
	{
		ConfigError(ERR_MISSINGCONDITION);
		return FALSE;
	}

	for(i = 0;i < NUM_CHECKS;i++)
	{
		if (!stricmp(checkarray[i].variable,token))
		{
			cmd->par._if.check = i;
			break;
		}
	};

	if (i == NUM_CHECKS)
	{
		ULONG argt[] = {(ULONG)token};
    	#ifdef NO_ASM
	    {
	    	STRPTR strptr = s;
		RawDoFmt(ERR_UNKNOWNGET,argt,
				 (APTR)Copy_PutChFunc,&strptr);
	    }
	#else
		RawDoFmt(ERR_UNKNOWNGET,argt,
				 (APTR)Copy_PutChFunc,s);
    	#endif
		//sprintf(s,ERR_UNKNOWNGET,token);
		ConfigError(s);
		return FALSE;
	}

	if (!(line = GetToken(line)))
	{
		ConfigError(ERR_MISSINGCONDITION);
		return FALSE;
	}

	for(i = 0;i < NUM_OPTOKENS;i++)
	{
		if (!stricmp(optokenarray[i].token,token))
		{
			cmd->par._if.op = optokenarray[i].tokenid;
			break;
		}
	}

	if (i == NUM_OPTOKENS)
	{
		ULONG argt[] = {(ULONG)token};
    	#ifdef NO_ASM
	    {
	    	STRPTR strptr = s;
		RawDoFmt(ERR_UNKNOWNOP,argt,
				 (APTR)Copy_PutChFunc,&strptr);
	    }
	#else
		RawDoFmt(ERR_UNKNOWNOP,argt,
				 (APTR)Copy_PutChFunc,s);
	#endif
//		sprintf(s,ERR_UNKNOWNOP,token);
		ConfigError(s);
		return FALSE;
	}

	if (!(line = GetToken(line)))
	{
		ConfigError(ERR_MISSINGVALUE);
		return FALSE;
	}

	if ((cmd->par._if.op == OP_IN) ||
	    (checkarray[cmd->par._if.check].checktype == CHECKTYPE_STRING))
	{
		i = strlen(token) + 1;
		if (!(cmd->par._if.data = (LONG)ConfigAlloc(i)))
		{
			ConfigError(ERR_MEM);
			return FALSE;
		}

		memcpy((APTR)cmd->par._if.data,token,i);

	} else {
		i = 0;
		i = strtol(token,0,0);
		cmd->par._if.data = i;
	}

	return TRUE;
}

static BOOL CompileEndIf(char *line,struct configcmd *cmd)
{
	if (--nest_count < 0)
	{
		ConfigError(ERR_ENDIFWOIF);
	}

	return FALSE;
}

static BOOL CompileElse(char *line,struct configcmd *cmd)
{
	if (--nest_count < 0)
	{
		ConfigError(ERR_ELSEWOIF);
		return FALSE;
	}

	cmd->cmd = CMD_ELSE;

	NewList((struct List *)&cmd->par._else.this);
	nesttable[nest_count + 1] = (struct List *)&cmd->par._else.this;

	nest_count_inc = 1;

	return TRUE;
}

static BOOL CompileStop(char *line,struct configcmd *cmd)
{
	cmd->cmd = CMD_STOP_;

	return TRUE;
}

static void CompileLine(char *line)
{
	struct configcmd *cmd;
	BOOL addcmd = FALSE;

	line_count++;
	nest_count_inc = 0;

	if (!(cmd = ConfigAlloc(sizeof(struct configcmd))))
	{
		ConfigError(ERR_MEM);
		return;
	};

	line = SkipSpaces(line);

	if (line[0] && (line[0] != ';'))
	{
		if ((line = GetToken(line)))
		{
			if (!stricmp(token,"IF"))
			{
				addcmd = CompileIf(line,cmd);
			} else if (!stricmp(token,"ENDIF"))
			{
				addcmd = CompileEndIf(line,cmd);
			} else if (!stricmp(token,"ELSE"))
			{
				addcmd = CompileElse(line,cmd);
			} else if (!stricmp(token,"STOP"))
			{
				addcmd = CompileStop(line,cmd);
			} else {
				addcmd = CompileSet(line,cmd);
			}

		} /* if ((line = GetToken(line))) */

	} /* if (line[0] && (line[0] != ';')) */

	if (!errorstring)
	{
		if (addcmd)
		{
			AddTail(nesttable[nest_count],(struct Node *)cmd);
		} else {
			ConfigFree(cmd);
		}
	}

	nest_count += nest_count_inc;
};

/*****************************************************/

static BOOL Operation_IN_Integer(LONG val,char *instring)
{
	LONG inval;
	BOOL rc = FALSE;

	while (1)
	{
		inval = 0x1234ABCD;
		inval = strtol(instring,0,0);

		if ((inval != 0x1234ABCD) && (inval == val))
		{
			rc = TRUE;
			break;
		}

		if (!(instring = strchr(instring,','))) break;

		instring++;
	}

	return rc;
}

static BOOL Operation_IN_String(char *val,char *instring)
{
	char *next_instring;
	BOOL rc = FALSE;

	instring--;

	while(instring && !rc)
	{
		instring++;

		if ((next_instring = strchr(instring,',')))
		{
			*next_instring = '\0';
		}

		if (SendPatternMatcherMsg(instring,val) == 0)
		{
			rc = TRUE;
		}

		if ((instring = next_instring))
		{
			*next_instring = ',';
		};

	} /* while(instring && !rc) */

	return rc;
}

static void ExecuteList(struct List *list,struct ReqNode *reqnode)
{
	struct configcmd *cmd = NULL,*succ = NULL;
	void (*pokefunc)(struct ReqNode *reqnode,LONG data);
	LONG check,data;
	WORD	i = 0;
	BOOL checktrue,skipelse = 0;
	char *temp;

	if (!config_stopped)
	{

		if (list->lh_Head) cmd = (struct configcmd *)list->lh_Head;

	if (cmd)
	{

		while ((succ = (struct configcmd *)cmd->node.mln_Succ))
		{
			switch(cmd->cmd)
			{
				case CMD_SET:
					switch(whatarray[cmd->par.set.what].poke)
					{
						case POKEB:
							*(BYTE *)((UBYTE *)&reqnode->cfg + whatarray[cmd->par.set.what].offset) = (BYTE)cmd->par.set.data;
							break;

						case POKEW:
							*(WORD *)((UBYTE *)&reqnode->cfg + whatarray[cmd->par.set.what].offset) = (WORD)cmd->par.set.data;
							break;

						case POKESB: //backup the string for use after freeing cfg data
							if (cmd->par.set.data)
							{
								strncpy((UBYTE*)&reqnode->bcfg + whatarray[cmd->par.set.what].offset,(char *)cmd->par.set.data,256);
							};
							break;

						case POKES0:
							if (cmd->par.set.data) //enforcer hit found and removed by Jaca/Dreamolers-CAPS
							{
								if (*(char *)cmd->par.set.data == '\0')
								{
									cmd->par.set.data = 0;
								};
							}
							/* fall through */

						case POKEL:
						case POKES:
							*(LONG *)((UBYTE *)&reqnode->cfg + whatarray[cmd->par.set.what].offset) = (LONG)cmd->par.set.data;
							break;

						case POKEF:
							pokefunc = (APTR)(whatarray[cmd->par.set.what].offset);
							pokefunc(reqnode,cmd->par.set.data);
							break;
						case POKEC:
							temp = (char *)cmd->par.set.data;
							if (temp)
							{
								*(BYTE *)((UBYTE *)&reqnode->cfg + whatarray[cmd->par.set.what].offset) = (BYTE)temp[0];
							} else {
								*(BYTE *)((UBYTE *)&reqnode->cfg + whatarray[cmd->par.set.what].offset) = 0;
							}
							break;

					} /* switch(whatarray[cmd->set.what].poke) */
					break;

				case CMD_IF:
					check = checkarray[cmd->par._if.check].getfunc(reqnode);
					data = cmd->par._if.data;

					checktrue = FALSE;

					switch(checkarray[cmd->par._if.check].checktype)
					{
						case CHECKTYPE_INTEGER:
							switch(cmd->par._if.op)
							{
								case OP_EQ:
									if (check == data) checktrue = TRUE;
									break;

								case OP_NE:
									if (check != data) checktrue = TRUE;
									break;

								case OP_LT:
									if (check < data) checktrue = TRUE;
									break;

								case OP_GT:
									if (check > data) checktrue = TRUE;
									break;

								case OP_LE:
									if (check <= data) checktrue = TRUE;
									break;

								case OP_GE:
									if (check >= data) checktrue = TRUE;
									break;

								case OP_IN:
									checktrue = Operation_IN_Integer(check,(char *)data);
									break;

							} /* switch(cmd->par._if.op) */;
							break;

						case CHECKTYPE_STRING:
							if (cmd->par._if.op != OP_IN)
							{
								i = SendPatternMatcherMsg((char *)data,(char *)check);
							}

							switch(cmd->par._if.op)
							{
								case OP_EQ:
									if (!i) checktrue = TRUE;
									break;

								case OP_NE:
									if (i) checktrue = TRUE;
									break;

								case OP_LT:
									if (i < 0) checktrue = TRUE;
									break;

								case OP_GT:
									if (i > 0) checktrue = TRUE;
									break;

								case OP_LE:
									if (i <= 0) checktrue = TRUE;
									break;

								case OP_GE:
									if (i >= 0) checktrue = TRUE;
									break;

								case OP_IN:
									checktrue = Operation_IN_String((char *)check,(char *)data);
									break;

							} /* switch(cmd->par._if.op) */;

					} /* switch(checkarray[cmd->par._if.check].checktype) */

					if (checktrue)
					{
						ExecuteList((struct List *)&cmd->par._if.then,reqnode);
						skipelse = TRUE;
					} else {
						skipelse = FALSE;
					}
					break;

				case CMD_ELSE:
					if (!skipelse)
					{
						ExecuteList((struct List *)&cmd->par._else.this,reqnode);
					}
					break;

				case CMD_STOP_:
					config_stopped = TRUE;
					break;

			} /* switch(cmd->cmd) */

			if (config_stopped) break;

			cmd = succ;

		} /* while ((succ = (struct configcmd *)cmd->node.mln_Succ)) */

	} //if (cmd)
	} /* if (!config_stopped) */
};


