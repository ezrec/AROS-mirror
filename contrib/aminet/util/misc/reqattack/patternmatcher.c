#include <exec/ports.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/dos_lib.h>
#else
#include <proto/exec.h>
#include <proto/dos.h>
#endif

#include <clib/alib_protos.h>

#include <string.h>

#include "global.h"
#include "patternmatcher.h"

#define MAX_PATTERN_LEN 200

ULONG patternmatchermask;

static struct MsgPort patternmatcherport;
static char *pat[MAX_PATTERN_LEN * 2 + 2 + 1];

void InitPatternMatcher(void)
{
	patternmatcherport.mp_Node.ln_Type = NT_MSGPORT;
	patternmatcherport.mp_Flags = PA_SIGNAL;
	patternmatcherport.mp_SigBit = AllocSignal(-1);
	patternmatcherport.mp_SigTask = FindTask(0);
	NewList(&patternmatcherport.mp_MsgList);

	patternmatchermask = 1L << patternmatcherport.mp_SigBit;
}

void CleanupPatternMatcher(void)
{
	if (patternmatcherport.mp_SigBit)
	{
		FreeSignal(patternmatcherport.mp_SigBit);
	}
}

BOOL SendPatternMatcherMsg(char *pattern,char *string)
{
	struct PatternMatcherMsg msg;
	struct MsgPort replyport;
	
	if (!(pattern)) return -1;
	if (!(string)) return -1;

	replyport.mp_Node.ln_Type = NT_MSGPORT;
	replyport.mp_Flags = PA_SIGNAL;
	replyport.mp_SigBit = SIGB_SINGLE;
	replyport.mp_SigTask = FindTask(0);
	NewList(&replyport.mp_MsgList);

	SetSignal(0,SIGF_SINGLE);

	msg.msg.mn_ReplyPort = &replyport;
	msg.msg.mn_Length = sizeof(msg);
	
	msg.pattern = pattern;
	msg.string = string;
	
	PutMsg(&patternmatcherport,&msg.msg);
	WaitPort(&replyport);
	
	return msg.rc;
}

void HandlePatternMatcher(void)
{
	struct PatternMatcherMsg *msg;
	WORD i;

	while ((msg = (struct PatternMatcherMsg *)GetMsg(&patternmatcherport)))
	{
		msg->rc = -1;
		
		if (strlen(msg->pattern) <= MAX_PATTERN_LEN)
		{
			i = ParsePatternNoCase(msg->pattern,(STRPTR)pat,MAX_PATTERN_LEN * 2 + 2);
			if (i == 1)
			{
				/* pattern has wildcards */

				if (MatchPatternNoCase((STRPTR)pat,msg->string) != 0)
				{
					msg->rc = 0;
				}

			} else if (i == 0)
			{
				/* pattern does not have wildcards */

				msg->rc = stricmp(msg->string,msg->pattern);
			}

		} /* if (strlen(msg->pattern) <= MAX_PATTERN_LEN) */
		
		ReplyMsg(&msg->msg);

	} /* 	while ((*msg = (struct PatternMatcherMsg)GetMsg(patternmatcherport) */
}


