#ifndef MYREQUESTER_H
#define MYREQUESTER_H

#define REQATTACK_DISPLAYREQ 127

#define RAPREFSMUI_VERSION 1

struct Window *MakeMyRequester(struct ReqNode *reqnode,struct Window *win,ULONG idcmp);
LONG HandleMyRequester(struct ReqNode *reqnode,ULONG *idcmpflags,BOOL wait);
void FreeMyRequesterPens(struct ReqNode *reqnode);

struct RAPrefsMsg
{
	struct Message msg;
	struct ReqNode *reqnode;
	char *proc;
	WORD version;
};

#endif

