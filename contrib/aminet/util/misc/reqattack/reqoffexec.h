
void ReqOFFExecute(struct ReqNode *reqnode);
void ReqOFFExecProcess(void);

struct ReqOFFData
{
	struct Task		*task;
	struct ReqNode	*reqnode;
};

struct ReplacerData
{
	struct Window *window;
	struct EasyStruct *easystr;
	APTR	*idcmp;
	APTR	*args;
};

