/*********************************************************************
----------------------------------------------------------------------

	MysticView
	definitions for picture management

----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_WRAPPEHANDLER_H
#define	MYSTIC_WRAPPEDHANDLER_H 1

#include "Mystic_Global.h"
#include "Mystic_Handler.h"
#include <guigfx/guigfx.h>


#define PICHSTATUS_IDLE			0
#define PICHSTATUS_LOADING		1
#define PICHSTATUS_SCANNING		2


/*
**	wrapped object structure:
*/

struct pichandler
{
	struct PicHandler *newPicHandler;
	ULONG lockedID;				// currently locked picture
	ULONG requestedID;			// currently requested picture
	BOOL waitforfirstpicture;
	PICTURE *pic;				// the locked picture
//	char *picname;
	PICINFO *picinfo;
	BOOL atliststart;
	BOOL asyncscanning;
	int status;
	char *errortext;
	LONG error;
};



//#define	PICH_Dummy				TAG_USER
//#define	PICH_Recursive			(PICH_Dummy+1)
//#define	PICH_SortMode			(PICH_Dummy+2)
//#define PICH_Reject				(PICH_Dummy+4)
//#define PICH_SortReverse		(PICH_Dummy+5)



struct piclist
{
	struct SignalSemaphore semaphore;
	struct picscanner *scanner;
	int currentpic;
	char **patternlist;
	char *rejectpattern;
};



extern void SetPicHandlerBuffer(struct pichandler *ph, int percent);

extern void ReleasePic(struct pichandler *ph);

extern void SetSortMode(struct pichandler *pichandler, ULONG sortmode, BOOL reverse);
extern BOOL AppendPicHandler(struct pichandler *ph, char **filepatterns);

extern struct pichandler * STDARGS CreatePicHandler(char **filepatternlist, BOOL asyncscanning, ...);
extern void DeletePicHandler(struct pichandler *pichandler);
extern ULONG NewPicSignal(struct pichandler *pichandler);

extern BOOL GetPicFileType(struct pichandler *ph);
extern PICTURE *GetPic(struct pichandler *handler);
extern BOOL PrevPic(struct pichandler *handler);
extern BOOL NextPic(struct pichandler *handler);
extern BOOL CurrentPic(struct pichandler *handler);
extern char *GetPicFile(struct pichandler *handler);
extern int GetNextPicNumber(struct pichandler *pichandler);
extern int GetCurrentPicNumber(struct pichandler *pichandler);
extern int GetTotalPicNumber(struct pichandler *pichandler);
extern void ResetPicHandler(struct pichandler *pichandler);
extern BOOL ReloadPic(struct pichandler *pichandler);
extern void ExpungePicHandler(struct pichandler *handler);
extern LONG GetPicError(struct pichandler *handler);
extern char **PicHandlerCreateFileList(struct pichandler *handler);

//extern void LockPicHandlerShared(struct pichandler *pichandler);
//extern void UnLockPicHandler(struct pichandler *pichandler);

extern BOOL AtListStart(struct pichandler *pichandler);

extern BOOL NewPicAvailable(struct pichandler *ph, LONG *error);

extern int GetPicHandlerStatus(struct pichandler *ph);
extern void SetPicHandlerAutoCrop(struct pichandler *ph, BOOL autocrop);

extern BOOL RenamePicture(struct pichandler *ph, char *newname);
extern BOOL RemovePicture(struct pichandler *ph);

extern char *GetPicFormat(struct pichandler *ph);

#endif
