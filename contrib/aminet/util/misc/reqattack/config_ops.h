#define OP_EQ         1
#define OP_NE         2
#define OP_LT         3
#define OP_GT         4
#define OP_LE         5
#define OP_GE         6
#define OP_IN			 7

#define NUM_OPTOKENS 9

struct optoken
{
	char *token;
	WORD tokenid;
};

static struct optoken optokenarray[NUM_OPTOKENS] =
{
	{"=", OP_EQ},
	{"==",OP_EQ},
	{"<>",OP_NE},
	{"!=",OP_NE},
	{"<", OP_LT},
	{">", OP_GT},
	{"<=",OP_LE},
	{">=",OP_GE},
	{"in",OP_IN}
};

