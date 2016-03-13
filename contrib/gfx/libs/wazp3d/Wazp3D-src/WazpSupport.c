/* Wazp3D Beta 52 : Alain THELLIER - Paris - FRANCE - (November 2006 to 2012)		*/
/* Adaptation to AROS from Matthias Rustler 						   */
/* Code clean-up and library enhancements from Gunther Nikl 				   */
/* LICENSE: GNU General Public License (GNU GPL) for this file 			   */

/*==================================================================================*/
/* OS dependant usefull functions */
/*==================================================================================*/
#include "Wazp3D.h"

#ifdef AMIGA
ULONG  StartTime=0;

#ifdef SOFT3DLIB 
struct Library *		Soft3DBase			=NULL;
#endif

#if !defined(__AROS__) 

struct ExecBase*		SysBase			=NULL;
struct IntuitionBase*	IntuitionBase		=NULL;
struct DosLibrary*	DOSBase			=NULL;
struct GfxBase*		GfxBase			=NULL;
struct Library*		CyberGfxBase		=NULL;
struct Library*		UtilityBase			=NULL;
struct Library*		GadToolsBase		=NULL;

#endif

struct Device *		TimerBase			=NULL;
struct timerequest tr;

/*======================================================================================*/
#ifdef __amigaos4__

struct Library*				NewlibBase			=NULL;
struct Interface*				INewlib			=NULL;

struct Library* 				P96Base 			=NULL;

struct ExecIFace*				IExec				=NULL;
struct DOSIFace*				IDOS				=NULL;
struct GraphicsIFace*			IGraphics			=NULL;
struct IntuitionIFace*			IIntuition			=NULL;
struct GadToolsIFace*			IGadTools			=NULL;
struct CyberGfxIFace*			ICyberGfx			=NULL;
struct TimerIFace*			ITimer			=NULL;
struct UtilityIFace*			IUtility			=NULL;
struct P96IFace* 				IP96 				=NULL;

#endif

/*==================================================================================*/
void *Libmalloc(ULONG size )
{
void *pt;

	pt=AllocVec(size,MEMF_FAST | MEMF_CLEAR);
	if(pt==NULL)
		return(AllocVec(size,MEMF_ANY | MEMF_CLEAR));
	else
		return(pt);
}
/*==================================================================================*/
void Libfree(void *p)
{	FreeVec(p); }
/*==================================================================================*/
void Libmemcpy(void *s1,void *s2,LONG n)
{	CopyMem(s2,s1,n); }
/*==================================================================================*/
void Libstrcpy(void *p1,void *p2)
{
UBYTE *s1=p1;
UBYTE *s2=p2;
UBYTE *s=s1;

  do
	*s++=*s2;
  while(*s2++!='\0');
}
/*==================================================================================*/
void Libstrcat(void *p1,void *p2)
{
UBYTE *s1=p1;
UBYTE *s2=p2;
UBYTE *s =s1;

  while(*s++)
	;
  --s;
  while((*s++=*s2++))
	;
}
/*==================================================================================*/
ULONG Libstrlen(void *p)
{
UBYTE *string=p;
UBYTE *s=string;

	while(*s++)
		;
	return ~(string-s);
}
/*==================================================================================*/

#if defined(__amigaos4__)
int  printf(char *string,...);
int sprintf(char *buffer,char *string,...);
#define Libprintf if(LibDebug) printf
#define Libsprintf sprintf

#elif defined(__AROS__)
/*==================================================================================*/
#include <exec/rawfmt.h>
void Libprintf(UBYTE *string, ...)
{
char buffer[256];
va_list args;

	//if(!LibDebug ) return;
	if(SysBase==0) return;
	va_start(args, string);
	VNewRawDoFmt(string, RAWFMTFUNC_STRING, buffer, args);
	va_end(args);
	Write(Output(), buffer, Libstrlen(buffer));
}
/*==================================================================================*/
void Libsprintf(UBYTE *buffer,UBYTE *string, ...)
{
va_list args;

	if(SysBase==0) return;
	va_start(args, string);
	VNewRawDoFmt(string, RAWFMTFUNC_STRING, buffer, args);
	va_end(args);
}
#else
/*==================================================================================*/
#define PUTCHARFUNC (void (*))"\x16\xc0\x4e\x75"
void Libsprintf(void *buffer,void *string,...)
{
va_list args;

	if(SysBase==0) return;
	va_start(args, string);
 	RawDoFmt(string, args, PUTCHARFUNC, buffer);
	va_end(args);

}
/*==================================================================================*/
void Libprintf(void *string,...)
{
char buffer[256];
va_list args;

	if(!LibDebug ) return;
	if(SysBase==0) return;
	va_start(args, string);
	RawDoFmt(string, args, PUTCHARFUNC, buffer);
	va_end(args);

/* for redirecting debugger's output to PC side */
/*
#ifdef SOFT3DDLL
	buffer[Libstrlen(buffer)]=0;
	SOFT3D_Debug(buffer);
	return;
#endif
*/
	Write(Output(), buffer, Libstrlen(buffer));
}
#endif
/*==================================================================================*/
void LibAlert(void *p1)
{
UBYTE *t=p1;
void *Data=&t+1L;
struct EasyStruct EasyStruct;
ULONG IDCMPFlags;

	if(!LibDebug ) return;
	if(SysBase==0) return;
	EasyStruct.es_StructSize=sizeof(struct EasyStruct);
	EasyStruct.es_Flags=0L;
	EasyStruct.es_Title=(void *)"Message:";
	EasyStruct.es_TextFormat=(void*)t;
	EasyStruct.es_GadgetFormat=(void*)"OK";

	IDCMPFlags=0L;
	(void)EasyRequestArgs(NULL,&EasyStruct,&IDCMPFlags,Data);
	return;
}
/*==================================================================================*/
void Libsavefile(void *filename,void *pt,ULONG size)
{
BPTR file;

	if ((file = Open(filename,MODE_NEWFILE)))
	{
	Write(file,pt,size);
	Close(file);
	}
}
/*==================================================================================*/
void Libloadfile(void *filename,void *pt,ULONG size)
{
BPTR file;

	if ((file = Open(filename,MODE_OLDFILE)))
	{
	Read(file,pt,size);
	Close(file);
	}
}
/*==================================================================================*/
ULONG LibMilliTimer(void)
{
struct timeval tv;
ULONG	MilliFrequence1=1000;
ULONG	MilliFrequence2=1000000/MilliFrequence1;
ULONG MilliTime;

	GetSysTime((void *)&tv);
	if(StartTime==0)	StartTime=tv.tv_secs;
	MilliTime  = (tv.tv_secs-StartTime) *  MilliFrequence1 + tv.tv_micro/MilliFrequence2;
	return(MilliTime);
}
/*==================================================================================*/
BOOL OpenAmigaLibraries(void *exec)
{
/* Initialize the needed libraries  */

	if(TimerBase!=NULL)	return(TRUE);	/* libs are already opened */

#if !defined(__AROS__) 

#ifdef __amigaos4__
	IExec	=exec;
	if (IExec ==NULL)		return(FALSE);
	SysBase	=(void*)IExec->Data.LibBase;
#else
	SysBase	=exec;
#endif

	if (SysBase==NULL)		return(FALSE);
	DOSBase=			(void*)OpenLibrary("dos.library",36L);
	GfxBase=			(void*)OpenLibrary("graphics.library",0L);
	IntuitionBase=		(void*)OpenLibrary("intuition.library", 0L);
	UtilityBase=		(void*)OpenLibrary("utility.library",36L);
	GadToolsBase=		(void*)OpenLibrary("gadtools.library", 37);
	CyberGfxBase=		(void*)OpenLibrary("cybergraphics.library", 0L);

	if(DOSBase==NULL)			return(FALSE);
	if(GfxBase==NULL)			return(FALSE);
	if(IntuitionBase==NULL)		return(FALSE);
	if(UtilityBase==NULL)		return(FALSE);
	if(GadToolsBase==NULL)		return(FALSE);
	if(CyberGfxBase==NULL)		return(FALSE);

#endif

	if (OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *)&tr, 0L) != 0)
		return(FALSE);
	TimerBase = (struct Device  *) tr.tr_node.io_Device;

#ifdef __amigaos4__
	NewlibBase = 			(void*)OpenLibrary("newlib.library", 52);
	if (NewlibBase ==NULL)		return(FALSE);

	if (IExec ==NULL)		return(FALSE);

	INewlib = (struct Interface*)GetInterface((struct Library *)NewlibBase, "main", 1, NULL);
	IDOS = (struct DOSIFace*)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	IGraphics = (struct GraphicsIFace*)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
	IIntuition = (struct IntuitionIFace*)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	IUtility = (struct UtilityIFace*)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
	IGadTools = (struct GadToolsIFace*)GetInterface((struct Library *)GadToolsBase, "main", 1, NULL);
	ICyberGfx = (struct CyberGfxIFace*)GetInterface((struct Library *)CyberGfxBase, "main", 1, NULL);
	ITimer = (struct TimerIFace*)GetInterface((struct Library *)TimerBase, "main", 1, NULL);

	if (INewlib ==NULL)		return(FALSE);
	if (IDOS==NULL)		return(FALSE);
	if (IGraphics==NULL)	return(FALSE);
	if (IIntuition==NULL)	return(FALSE);
	if (IUtility==NULL)		return(FALSE);
	if (IGadTools==NULL)	return(FALSE);
	if (ICyberGfx==NULL)	return(FALSE);

	if (ITimer==NULL)		return(FALSE);
#endif

	return(TRUE);
}
/*======================================================================================*/
void CloseAmigaLibraries()
{
#define LIBCLOSE(Lbase)	 if(Lbase!=NULL)	{ CloseLibrary((struct Library  *)Lbase );Lbase=NULL;}
#define LIBCLOSE4(Iface) if(Iface!=NULL)	{DropInterface((struct Interface*)Iface );Iface=NULL;}

#ifdef __amigaos4__
	LIBCLOSE4(INewlib)
	LIBCLOSE(NewlibBase)

	LIBCLOSE4(IDOS)
	LIBCLOSE4(IGraphics)
	LIBCLOSE4(IIntuition)
	LIBCLOSE4(IGadTools)
	LIBCLOSE4(ICyberGfx)
	LIBCLOSE4(ITimer)
#endif

#ifdef SOFT3DLIB
	LIBCLOSE(Soft3DBase)
#endif

#if !defined(__AROS__) 
	LIBCLOSE(DOSBase)
	LIBCLOSE(GfxBase)
	LIBCLOSE(IntuitionBase)
	LIBCLOSE(UtilityBase)
	LIBCLOSE(CyberGfxBase)
#endif

	CloseDevice((struct IORequest *)&tr);
	LibDebug=FALSE;	/* with OS libraries closed cant print anymore */
}
#endif
/*==================================================================================*/
#if !defined(AMIGA) /* if is a PC */
void Libprintf(void *string,...);
#endif
/*==================================================================================*/
void DumpMem(UBYTE *pt,LONG nb)
{
#ifdef WAZP3DDEBUG
LONG n;
NLOOP(nb/4)
	{
	Libprintf("[%ld\t][%ld\t] %ld\t%ld\t%ld\t%ld\n",(IPTR)pt,(ULONG)4*n,(ULONG)pt[0],(ULONG)pt[1],(ULONG)pt[2],(ULONG)pt[3]);
	pt=&(pt[4]);
	}
#endif
}
