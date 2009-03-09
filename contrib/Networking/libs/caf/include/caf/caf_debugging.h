#ifndef CAF_DEBUGGING_H
#define CAF_DEBUGGING_H

enum debugmode {
	CAF_DEBUG_NONE		= 0,
	CAF_DEBUG_FILE		= 1,
	CAF_DEBUG_SOCKET	= 1<<1,
	CAF_DEBUG_SERIAL	= 1<<2,
	CAF_DEBUG_STDOUT	= 1<<3
};

void CafDebugInit(int mode);
void CafDebugCleanup(void);
void SetDebugMode(int mode);
void SetDebugFile(char *fname);
void SetDebugIPPort(char *ip, char *port);
void DebugOutput(char *buf);
void DebugOutputNL(char *buf, bool b_addnl);

#endif

