extern ULONG cxmask;
extern BOOL cxinactive;

#define RACODE_APPEAR 	 1
#define RACODE_DISAPPEAR 2

struct RAMsg
{
	struct Message mn;
	LONG reqattack;
	LONG code;
};

void InitCX(void);
void CleanupCX(void);
void HandleCX(void);

