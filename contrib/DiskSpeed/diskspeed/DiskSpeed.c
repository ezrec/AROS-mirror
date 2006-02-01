/*
   Copyright © 1995-2006, The AROS Development Team. All rights reserved.
   $Id$

   Desc: Disk subsystem Benchmarking Utility..
   Lang: English.

   Based on DiskSpeed v4.2 by Michael Sinz
*/

/*********************************************************************************************/

#ifndef PROTO_ALIB_H
#   include <proto/alib.h>
#endif

#ifndef PROTO_ICON_H
#   include <proto/icon.h>
#endif

#ifndef PROTO_EXEC_H
#   include <proto/exec.h>
#endif

#ifndef PROTO_DOS_H
#   include <proto/dos.h>
#endif

#ifndef PROTO_TIMER_H
#   include <proto/timer.h>
#endif

#ifndef PROTO_UTILITY_H
#   include <proto/utility.h>
#endif

#ifndef PROTO_INTUITION_H
#   include <proto/intuition.h>
#endif

#ifndef PROTO_GRAPHICS_H
#   include <proto/graphics.h>
#endif

#ifndef PROTO_MUIMASTER_H
#   include <proto/muimaster.h>
#endif

#ifndef PROTO_LAYERS_H
#   include <proto/layers.h>
#endif

/*********************************************************************************************/

#include <exec/resident.h>
#include <exec/execbase.h>

#include <libraries/coolimages.h>
#include <libraries/mui.h>

#include <zune/customclasses.h>
#include <zune/aboutwindow.h>

#include <aros/locale.h>

#include <exec/io.h>
#include	<exec/ports.h>
#include	<devices/timer.h>
#include	<dos/dos.h>
#include	<dos/dosextens.h>
#include	<devices/trackdisk.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include	<intuition/screens.h>
#include	<workbench/workbench.h>
#include	<workbench/startup.h>

#include <clib/exec_protos.h>
#include	<clib/dos_protos.h>
#include	<clib/timer_protos.h>
#include	<clib/intuition_protos.h>
#include	<clib/icon_protos.h>

/*********************************************************************************************/

struct DiskSpeedMUI_Intern {
   BOOL         app_ret_val;
   Object        *disk_app;              /* our application itself..    */
   Object        *disk_main_win;          /* The Main Benchmark Window   */
   Object        *disk_config_win;         /* The Configuration Window    */
   Object        *disk_about_win;         /* The Main Benchmark Window   */

   Object        *about_item;
   Object        *quit_item;

   Object        *welctext;

   Object        *devicestr;
   Object        *commentstr;

   Object        *resultlv;

   Object        *configobj;             /* Open Config Window gadget */
   Object        *beginobj;
	Object        *stopobj;
	Object        *saveobj;

   Object        *conokobj;              /* Configuration Windows OK gadget    */
   Object        *consaveobj;            /* Configuration Windows Save gadget   */
   Object        *conuseobj;             /* Configuration Windows Use gadget   */
   Object        *concancelobj;           /* Configuration Windows Cancel gadget */
};

struct DiskSpeedConfig_Intern {
#ifndef SCSI_SPEED
   Object        *dir_checkmark;          /* Benchmark Configuration Option Objects */
#endif
   Object        *seek_checkmark;
   Object        *nocpu_checkmark;
   Object        *mintime;

   Object        *buffer1;
   Object        *buffer2;
   Object        *buffer3;
   Object        *buffer4;

   Object        *long_checkmark;
   Object        *word_checkmark;
   Object        *byte_checkmark;

   Object        *fast_checkmark;
   Object        *chip_checkmark;
};

struct DiskSpeedMUI_Intern diskspeedmui_intern;
struct DiskSpeedConfig_Intern diskspeedmui_config;

enum
{
   RETURNID_BEGIN = 1,
   RETURNID_STOP,
   RETURNID_SAVE,
   RETURNID_SAVECONFIG,
   RETURNID_USECONFIG,
};

/*********************************************************************************************/

#include <limits.h>
#include <string.h>
#include <stdio.h>

#undef DEBUG
#define DEBUG 1
#   include <aros/debug.h>

/*********************************************************************************************/

#ifdef	SCSI_SPEED
#   include	"ScsiSpeed_rev.h"
#else
#   include	"DiskSpeed_rev.h"
#endif	/* SCSI_SPEED */

/*********************************************************************************************/

#define VERS               APPNAME " 4.10"
#define VSTRING             APPNAME " 4.10 (" DATE ")\n\r"
#define VERSTAG             "\0$VER: " APPNAME " 4.10 (" DATE ")"
#define CXVERSION            VERS " (" DATE ")" 
#define STR_SEPERATOR         "------------------------------------------------------------------"

const char COPYRIGHT[] =      "Aros " VERS " Copyright © 2004-2006 The AROS Development Team" VERSTAG;

#ifdef SCSI_SPEED
#   define MSG_BYTES_READ      MSG_BYTES_READ_S
   const char	RESULTS_FILE[] = APPNAME ".Results";
#else
#   define MSG_BYTES_READ      MSG_BYTES_READ_N
   const char FILE_STRING[] =  "%04lx " APPNAME " Test File ";
   const char TEST_DIR[] =    " " APPNAME " Test Directory ";
   const char RESULTS_FILE[] = APPNAME ".Results";
#endif	/* SCSI_SPEED */

/*********************************************************************************************/
/*
   Timer related stuff...
*/

struct SpeedTimer
{
   struct  MsgPort             *tmPort;
   struct	timerequest         *tmReq;
   struct	timeval            tmVal;

   LONG                     Open;
};

/*********************************************************************************************/
/*
   Our Disk/ScsiSpeed global structure...
*/

struct DiskSpeed
{
   struct	Window              *Window;            /* The DiskSpeed window... */
   struct	RenderInfo           *ri;               /* MKSoft Render Info */

#ifdef	SCSI_SPEED

   struct	IOExtTD	            DiskIO;             /* Disk IO request */

#endif	/* SCSI_SPEED */

	BPTR                     Output;             /* The output file handle! */
   struct	Process             *Me;               /* Pointer to my process... */
   struct	SpeedTimer           *timer;             /* Pointer to a timer structure */

#ifndef	SCSI_SPEED

   struct	FileInfoBlock         *fib;               /* Pointer to a FileInfoBlock */

#endif	/* !SCSI_SPEED */

/* */
   struct	MinList             TextList;            /* The list the results test is linked to... */
   struct	DisplayList          *List;              /* The "List" gadget */
/* */
	ULONG                     Min_Time;            /* Minimum tmVal in seconds for a test */
	ULONG                     Base_CPU;            /* Base CPU available... */
	ULONG                     CPU_Total;           /* Sum of CPU availability */
	ULONG                     CPU_Count;           /* Count of CPU availability */

/* Testing parameters */
	unsigned short              HighDMA;            /* Set to TRUE for high video DMA... */
	unsigned short              Test_DIR;            /* Set to test directory stuff */
	unsigned short              Test_SEEK;           /* Set to test SEEK/READ */
	unsigned short              pad;

	ULONG                     Align_Types;         /* Set bits of alignment types... */
	ULONG                     Mem_TYPES;           /* Set memory type flags to test... */
	ULONG                     Test_Size[4];         /* The four test sizes... */

	short                     StringVectors[5*2*4];
	short                     ActionVectors[5*2*2];

/* */
	char                     Device[256];         /* Device name under test... */
	char                     Comments[256];        /* Comments string gadget... */
	char                     Undo[256];           /* Our one UNDO buffer... */

/* */
	char                     CPU_Type[22];         /* Processor name in this string (plus NULL) */
	char                     Exec_Ver[14];         /* Version of Exec */

/* */
	char                     tmp1[128];           /* Temporary buffer space... */
	char                     tmp2[128];           /* Temporary buffer space... */
};

struct DiskSpeed *global;

/*********************************************************************************************/

#define	DEVICE_GADGET            1
#define	COMMENT_GADGET           2
#define	TEST_GADGET             3
#define	SAVE_GADGET             4
#define	STOP_GADGET             5

/*********************************************************************************************/
/*
   This is the minimum timeval for test that can be extended/shorted automatically
   This number should not be set too low otherwise the test results will be
   inaccurate due to timer granularity.  (in seconds)
*/

#ifdef	SCSI_SPEED

#define	MIN_TEST_TIME            20

#else	/* SCSI_SPEED */

#define	MIN_TEST_TIME            8
#define	NUM_FILES               200

#endif	/* SCSI_SPEED */

/*********************************************************************************************/
/*
   This section of code is used to test CPU availability.
*/

struct CPU_AVAIL
{
   ULONG   _CPU_Use_Base;           /* Set to TRUE in order to use CPU...   */
   ULONG   _CPU_State_Flag;         /* Used to control this routine...     */
   ULONG   _CPU_Count_Low;          /* The low-order word for CPU count    */
   ULONG   _CPU_Count_High;         /* The high-order word for CPU count   */
   ULONG   CPU_Task;               /* The task pointer...              */
};

struct  Task *Init_CPU_Available( struct CPU_AVAIL *CPU_AVAIL_struct );
void   Free_CPU_Available( struct CPU_AVAIL *CPU_AVAIL_struct );
void   CPU_Calibrate( struct CPU_AVAIL *CPU_AVAIL_struct );

/*********************************************************************************************/
unsigned short BusyPointer[36] =
{
	0x0000, 0x0000, 0x0400, 0x07C0, 0x0000, 0x07C0, 0x0100, 0x0380, 0x0000,
	0x07E0, 0x07C0, 0x1FF8, 0x1FF0, 0x3FEC, 0x3FF8, 0x7FDE, 0x3FF8, 0x7FBE,
	0x7FFC, 0xFF7F, 0x7EFC, 0xFFFF, 0x7FFC, 0xFFFF, 0x3FF8, 0x7FFE, 0x3FF8,
	0x7FFE, 0x1FF0, 0x3FFC, 0x07C0, 0x1FF8, 0x0000, 0x07E0, 0x0000, 0x0000
};

/*
   These two defines set up and clear the BusyPointer...
*/

#define SetWait(x)     SetPointer(x,BusyPointer,16L,16L,-6L,0L)
#define ClearWait(x)   ClearPointer(x)

#if !defined(__AROS__)
/*********************************************************************************************/
static struct Catalog *MyCatalog;

static char *GetString(LONG num)
{
   const struct CatCompArrayType *x = CatCompArray;
   STRPTR s;
   
   for (;;)
   {
     if (x->cca_ID == num)
     {
       s = x->cca_Str;
       break;
     }
     x++;
   }
   s = GetCatalogStr(MyCatalog, num, s);
   return s;
}
#endif

static ULONG getv(APTR obj, ULONG tag)
{
   ULONG x;
   GetAttr(tag, obj, &x);
   return x;
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*
            This routine returns the amount of timeval in the timer...
            The number returned is in Seconds...
*/

static ULONG Read_Timer(struct SpeedTimer *spt)
{
   struct	Library    *TimerBase = (struct Library *)(spt->tmReq->tr_node.io_Device);

	/* Get the current timeval... */
	spt->tmReq->tr_node.io_Command = TR_GETSYSTIME;
	spt->tmReq->tr_node.io_Flags = IOF_QUICK;
	DoIO((struct IORequest *)(spt->tmReq));

	/* Subtract last timer result and store as the timer result */
	SubTime(&(spt->tmReq->tr_time),&(spt->tmVal));
	return(spt->tmReq->tr_time.tv_secs);
}

/*
            Start the timer...
*/

static void Start_Timer(struct SpeedTimer *spt)
{
	/* Get the current timeval... */
	spt->tmReq->tr_node.io_Command = TR_GETSYSTIME;
	spt->tmReq->tr_node.io_Flags = IOF_QUICK;
	DoIO((struct IORequest *)(spt->tmReq));

	/* Store current timeval as the timer result */
	spt->tmVal = spt->tmReq->tr_time;

	/*
	   This here is a nasty trick...  Since the timer device
	   has a low resolution, we wait until we get to the exact
	   cross-over from one TICK to the next.  We know that the
	   tick value is larger than 10 so if the difference
	   between two calls to the timer is > 10 then it must
	   have been a TICK that just went through.  This is
	   not "friendly" code but since we are testing the system
	   and it is not "application" code, it is not a problem.
	*/

	while ((spt->tmReq->tr_time.tv_micro-spt->tmVal.tv_micro) < 10)
	{
		/* Store current timeval as the timer result */
		spt->tmVal = spt->tmReq->tr_time;

		/* Get the current timeval... */
		spt->tmReq->tr_node.io_Command = TR_GETSYSTIME;
		spt->tmReq->tr_node.io_Flags = IOF_QUICK;
		DoIO((struct IORequest *)(spt->tmReq));
	}

	/* Store current timeval as the timer result */
	spt->tmVal = spt->tmReq->tr_time;
}

/*
            Stop the timer...
*/

static void Stop_Timer(struct SpeedTimer *spt)
{
   struct	Library    *TimerBase = (struct Library *)(spt->tmReq->tr_node.io_Device);

	/*  Get the current timeval.                           */
	spt->tmReq->tr_node.io_Command = TR_GETSYSTIME;
	spt->tmReq->tr_node.io_Flags = IOF_QUICK;

	DoIO((struct IORequest *)(spt->tmReq));

	/*  Subtract last timer result and store as the timer result   */
	SubTime(&(spt->tmReq->tr_time),&(spt->tmVal));
	spt->tmVal=spt->tmReq->tr_time;
}

/*
            Free a SpeedTimer structure as best as possible.  Do all of the error checks
            here since this will also be called for partial timer initializations.
*/

static void Free_Timer(struct SpeedTimer *spt)
{
	if (spt)
	{
		if (spt->tmPort)
		{
			if (spt->tmReq)
			{
				if (spt->Open) CloseDevice((struct IORequest *)(spt->tmReq));
				DeleteIORequest((struct IORequest *)(spt->tmReq));
			}
			DeleteMsgPort(spt->tmPort);
		}
		FreeMem(spt,sizeof(struct SpeedTimer));
	}
}

/*
            Initialize a SpeedTimer structure.  It will return NULL if it did not work.
*/

static struct SpeedTimer *Init_Timer(void)
{
   struct	SpeedTimer     *spt;

	if ((spt = AllocMem(sizeof(struct SpeedTimer),MEMF_PUBLIC|MEMF_CLEAR)))
	{
		spt->Open = FALSE;
		if ((spt->tmPort = CreateMsgPort()))
		{
			if ((spt->tmReq = (struct timerrequest *)CreateIORequest(spt->tmPort,sizeof(struct timerequest))))
			{
				if (!OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest *)(spt->tmReq),0L)) spt->Open = TRUE;
			}
		}
		if (!(spt->Open))
		{
			Free_Timer(spt);
			spt = NULL;
		}
	}
	return(spt);
}

/*
            Now, for the routines that will pull in the lines and display them as
            needed in the display...
*/

static void AddDisplayLine(struct DiskSpeed *global,char *line)
{
	ULONG		         size = strlen(line);

	if (diskspeedmui_intern.disk_main_win) /* add the line to the zune listview */
	{
      DoMethod(diskspeedmui_intern.resultlv, MUIM_List_InsertSingle, (IPTR)line, MUIV_List_Insert_Bottom);
      DoMethod(diskspeedmui_intern.resultlv, MUIM_List_Jump, 0x7fffffff);
      DoMethod(diskspeedmui_intern.disk_app, MUIM_Application_CheckRefresh);
   }
	else if (global->Output)
	{
		Write(global->Output,(APTR)line,size);
		Write(global->Output,"\n",1);
	}
}

/*...*/

static LONG Check_Quit(struct DiskSpeed *global)
{
   LONG	            worked = TRUE;
 
	if (SetSignal(0,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) worked = FALSE;

	if (!worked) AddDisplayLine(global,GetString(MSG_USERINT));

	return(worked);
}

/*
            It knows that the Y value is fixed point by n-digits...
*/

static ULONG MaxDivide(ULONG x,ULONG y,ULONG digits)
{
   ULONG	         result;
   ULONG	         num = 0;	               /* Number of 10 units adjusted for so far */

	while ((x<399999999) && (num<digits))
	{
		x *= 10;
		num++;
	}

	while (num<digits)
	{
		num++;
		if (num == digits) y+=5;	               /* Round it if last digit... */
		y = y/10;
	}

	if (y) result = x/y;
	else result = -1;	                        /* MAX INT if y=0 */

	return(result);
}

/*
            Build a string and add it to the display of results.
            This routine knows how to take the timer results and the CPU
            results and format them into a string with the given header
            and the given unit of measure.
*/

static VOID Display_Result(struct DiskSpeed *global,char *Header,ULONG number,char *Units, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   char	         *st_string = global->tmp1;
   char	         format[48];
   ULONG	         clicks = 0;                /* To figure out the number of clicks/second */
   ULONG	         tmVal;
   ULONG	         tmp_time;

	/* First, make sure (as best as possible) that the CPU values are right */
	CPU_AVAIL_struct->_CPU_State_Flag = TRUE;
	Delay(1);                                 /* Let it run into the TRUE _CPU_State_Flag */

	/* 1,000,000 micro = 1 second... */
	tmVal = (global->timer->tmVal.tv_secs * 1000000) + global->timer->tmVal.tv_micro;
	/* tmVal is now in micro seconds... */

	number=MaxDivide(number,tmVal,6);

	strcpy(format,"%s %9ld %s");
	if (!number)
	{
		strcpy(format,"%s      < %ld %s");
		number = 1;
	}

	if (global->Base_CPU)
	{
		tmp_time = tmVal;	/* For below... */

		while (CPU_AVAIL_struct->_CPU_Count_High)
		{
			/* Adjust the tmVal and the CPU count as needed */
			tmp_time = tmp_time >> 1;
			CPU_AVAIL_struct->_CPU_Count_Low = CPU_AVAIL_struct->_CPU_Count_Low >> 1;
			if (CPU_AVAIL_struct->_CPU_Count_High & 1) CPU_AVAIL_struct->_CPU_Count_Low += 0x80000000;
			CPU_AVAIL_struct->_CPU_Count_High = CPU_AVAIL_struct->_CPU_Count_High >> 1;
		}

		clicks = MaxDivide(CPU_AVAIL_struct->_CPU_Count_Low,tmp_time,6);
		clicks = (MaxDivide(clicks,global->Base_CPU,3)+5)/10;
		global->CPU_Total += clicks;
		global->CPU_Count++;

		strcat(format,GetString(MSG_CPUAVAIL));
	}

	sprintf(st_string,format,Header,number,Units,clicks);

	AddDisplayLine(global,st_string);
}

static VOID Display_Error(struct DiskSpeed *global,char *test)
{
	sprintf(global->tmp1,GetString(MSG_ERR_FAIL),test);
	AddDisplayLine(global,global->tmp1);
}

#ifdef	SCSI_SPEED

static BOOL SpeedTest(struct DiskSpeed *global,ULONG size,ULONG offset,ULONG mem_type, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   BOOL	         worked=TRUE;
   char	         *buffer;
   char	         *mem;		                     /* What we really allocated */
   char	         *type;
   char	         *type2;
   ULONG	         count;

	AddDisplayLine(global,"");

	type = "FAST";
	if (mem_type & MEMF_CHIP) type = "CHIP";

	type2="LONG";
	if (offset & 2) type2 = "WORD";
	if (offset & 1) type2 = "BYTE";

	/* Round to block sizes */
	size = (size+511) & (~511);

	if ((mem = AllocMem(size+offset,mem_type|MEMF_PUBLIC)))
	{
		/* Set up memory... */
		buffer = &(mem[offset]);

		sprintf(global->tmp1,GetString(MSG_TSTBUFF),size,type,type2);
		AddDisplayLine(global,global->tmp1);

		count = 0;

		Start_Timer(global->timer);
		Init_CPU_Available(CPU_AVAIL_struct);		         /* Start counting free CPU cycles... */
		while ((worked &= Check_Quit(global)) && (Read_Timer(global->timer) < global->Min_Time))
		{
			global->DiskIO.iotd_Req.io_Command = CMD_READ;
			global->DiskIO.iotd_Req.io_Flags = NULL;
			global->DiskIO.iotd_Req.io_Length = size;
			global->DiskIO.iotd_Req.io_Data = buffer;
			global->DiskIO.iotd_Req.io_Offset = count;

			DoIO((struct IORequest *)&global->DiskIO);
			count += global->DiskIO.iotd_Req.io_Actual;

			if (global->DiskIO.iotd_Req.io_Error)
			{
				worked = FALSE;
				Display_Error(global,GetString(MSG_ERR_RDEV));
			}
		}
		Stop_Timer(global->timer);

		if (worked) Display_Result(global,GetString(MSG_BYTES_READ),count,GetString(MSG_BYTE_UNITS), CPU_AVAIL_struct);

		Free_CPU_Available(CPU_AVAIL_struct);
		FreeMem(mem,size+offset);
	}
	else
	{
		sprintf(global->tmp1,GetString(MSG_SKIPMEM),size,type);
		AddDisplayLine(global,global->tmp1);
	}

	return(worked);
}

#else	/* SCSI_SPEED */

/*
            In order to keep the file create test fair, it must always do
            the same number of files.  The way filing systems work, many times
            the get slower as the number of files in a directory grow
*/

static LONG CreateFileTest(struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   BPTR	         file;
   ULONG	       count;
   LONG	         worked = TRUE;
   STRPTR         st_string = global->tmp1;	         /* For speed reasons */

	Start_Timer(global->timer);
	Init_CPU_Available(CPU_AVAIL_struct);		            /* Start counting free CPU cycles... */

	for (count=0;(count<NUM_FILES) && (worked &= Check_Quit(global));count++)
	{
		sprintf(st_string,FILE_STRING,count);
		if (file = Open(st_string,MODE_NEWFILE)) Close(file);
		else
		{
			Display_Error(global,GetString(MSG_ERR_CFILE));
			worked = FALSE;
		}
	}

	Stop_Timer(global->timer);

	if (worked) Display_Result(global,GetString(MSG_FILE_CREATE),NUM_FILES,GetString(MSG_FILE_UNITS), CPU_AVAIL_struct);

	Free_CPU_Available(CPU_AVAIL_struct);

	return(worked);
}

static LONG OpenFileTest(struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   BPTR	         file;
   ULONG	       count = 0;
   LONG	         worked = TRUE;
   STRPTR         st_string = global->tmp1;

	Start_Timer(global->timer);
	Init_CPU_Available(CPU_AVAIL_struct);		/* Start counting free CPU cycles... */
	while ((worked &= Check_Quit(global)) && (Read_Timer(global->timer) < global->Min_Time))
	{
		sprintf(st_string,FILE_STRING,(ULONG)(count % NUM_FILES));
		count++;
		if (file=Open(st_string,MODE_OLDFILE)) Close(file);
		else
		{
			Display_Error(global,GetString(MSG_ERR_OFILE));
			worked = FALSE;
		}
	}
	Stop_Timer(global->timer);

	if (worked) Display_Result(global,GetString(MSG_FILE_OPEN),count,GetString(MSG_FILE_UNITS), CPU_AVAIL_struct);

	Free_CPU_Available(CPU_AVAIL_struct);
	return(worked);
}

static LONG ScanDirectoryTest(struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   BPTR	         lock = NULL;
   ULONG	       count = 0;
   LONG	         worked = TRUE;

	Start_Timer(global->timer);
	Init_CPU_Available(CPU_AVAIL_struct);		/* Start counting free CPU cycles... */
	while ((worked &= Check_Quit(global)) && (Read_Timer(global->timer) < global->Min_Time))
	{
		if (lock)
		{
			if (!ExNext(lock,global->fib)) lock = NULL;
			else count++;
		}
		else
		{
			CurrentDir(lock = CurrentDir(NULL));
			if (Examine(lock,global->fib)) count++;
			else
			{
				Display_Error(global,GetString(MSG_ERR_DIRSCAN));
				worked = FALSE;
			}
		}
	}
	Stop_Timer(global->timer);

	if (worked) Display_Result(global,GetString(MSG_FILE_SCAN),count,GetString(MSG_FILE_UNITS), CPU_AVAIL_struct);

	Free_CPU_Available(CPU_AVAIL_struct);
	return(worked);
}

/*
            In order to keep the file delete test fair, it must always do
            the same number of files.  The way filing systems work, many times
            the get slower as the number of files in a directory grow
*/

static LONG DeleteFileTest(struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   ULONG	       count;
   LONG	         worked = TRUE;
   STRPTR         st_string = global->tmp1;

	Start_Timer(global->timer);
	Init_CPU_Available(CPU_AVAIL_struct);		/* Start counting free CPU cycles... */

	for (count = 0;(count<NUM_FILES) && (worked &= Check_Quit(global));count++)
	{
		sprintf(st_string,FILE_STRING,count);
		if (!DeleteFile(st_string))
		{
			Display_Error(global,GetString(MSG_ERR_DFILE));
			worked=FALSE;
		}
	}

	Stop_Timer(global->timer);

	if (worked) Display_Result(global,GetString(MSG_FILE_DELETE),NUM_FILES,GetString(MSG_FILE_UNITS), CPU_AVAIL_struct);

	Free_CPU_Available(CPU_AVAIL_struct);

	return(worked);
}

static LONG SeekReadTest(struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   BPTR	         file;
   ULONG	       size;
   ULONG	       count;
   LONG	         pos = 0;
   LONG	         buffer[16];
   LONG	         worked = FALSE;
   void	         *buf;

	/* First we build a file by writing the ROM to disk... */
	if (file = Open(FILE_STRING,MODE_NEWFILE))
	{
		size = 0x40000;	/* Start by asking for 256K */
		while (size && (!(buf = AllocMem(size,MEMF_PUBLIC)))) size = size>>1;

		if (buf)
		{
			worked = TRUE;
			/* Write a 256K file... */
			count = 0x40000/size;
			while ((count>0) && (worked&=Check_Quit(global)))
			{
				count--;
				if (size != Write(file,buf,size))
				{
					worked=FALSE;
					Display_Error(global,GetString(MSG_ERR_SEEK));
				}
			}
			FreeMem(buf,size);
		}
		else Display_Error(global,GetString(MSG_ERR_SEEK));

		Start_Timer(global->timer);
		Init_CPU_Available(CPU_AVAIL_struct);		/* Start counting free CPU cycles... */
		while ((worked &= Check_Quit(global)) && (Read_Timer(global->timer) < global->Min_Time))
		{
			Seek(file,pos,OFFSET_BEGINNING);
			Read(file,buffer,64);
			count++;

			Seek(file,-(pos+64),OFFSET_END);
			Read(file,buffer,64);
			count++;

			Seek(file,-(pos+(size/3)),OFFSET_CURRENT);
			Read(file,buffer,64);
			count++;

			/* Come up with another position... */
			pos=(pos+(size/11)) % (size/3);
		}
		Stop_Timer(global->timer);

		if (worked) Display_Result(global,GetString(MSG_SEEK_READ),count,GetString(MSG_SEEK_UNITS), CPU_AVAIL_struct);

		Free_CPU_Available(CPU_AVAIL_struct);

		Close(file);
		DeleteFile(FILE_STRING);
	}
	else Display_Error(global,GetString(MSG_ERR_SEEK));

	return(worked);
}

static BOOL SpeedTest(struct DiskSpeed *global,ULONG size,ULONG offset,ULONG mem_type, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   BOOL	worked=TRUE;
   char	*buffer;
   char	*mem;		/* What we really allocated */
   char	*type;
   char	*type2;
   ULONG	loop;
   ULONG	count;
   LONG	times;
   BPTR	file=NULL;

	AddDisplayLine(global,"");

	type="FAST";
	if (mem_type & MEMF_CHIP) type="CHIP";

	type2="LONG";
	if (offset & 2) type2="WORD";
	if (offset & 1) type2="BYTE";

	if ((mem=AllocMem(size+offset,mem_type|MEMF_PUBLIC)))
	{
		/* Set up memory... */
		buffer=&(mem[offset]);

		for (loop=0;loop<size;loop++) buffer[loop]=(unsigned short )loop;

		sprintf(global->tmp1,GetString(MSG_TSTBUFF),size,type,type2);
		AddDisplayLine(global,global->tmp1);

		count = 0;
		times = 0;

		Start_Timer(global->timer);
		Init_CPU_Available(CPU_AVAIL_struct);		/* Start counting free CPU cycles... */
		while ((worked &= Check_Quit(global)) && (Read_Timer(global->timer) < global->Min_Time))
		{
			if (times<1)
			{
				if (file) Close(file);
				DeleteFile(FILE_STRING);
				if (!(file = Open(FILE_STRING,MODE_NEWFILE)))
				{
					Display_Error(global,GetString(MSG_ERR_CFILE));
					worked = FALSE;
				}
				times = 0x40000/size;	/* Try to make file at least 256K size */
			}
			if (file)
			{
				if (size != Write(file,buffer,size))
				{
					Display_Error(global,GetString(MSG_ERR_CFILE));
					worked = FALSE;
				}
				else count += size;
				times--;
			}
		}
		Stop_Timer(global->timer);

		if (worked) Display_Result(global,GetString(MSG_BYTES_CREATE),count,GetString(MSG_BYTE_UNITS), CPU_AVAIL_struct);

		/* Fill out the file... */
		if (file) while ((worked &= Check_Quit(global)) && (times>0))
		{
			Write(file,buffer,size);
			times--;
		}

		if (file) Close(file);
		file=NULL;

		if (worked) if (!(file=Open(FILE_STRING,MODE_OLDFILE)))
		{
			Display_Error(global,GetString(MSG_ERR_WFILE));
			worked=FALSE;
		}

		count=0;
		times=0;

		Start_Timer(global->timer);
		Init_CPU_Available(CPU_AVAIL_struct);		/* Start counting free CPU cycles... */
		while ((worked &= Check_Quit(global)) && (Read_Timer(global->timer) < global->Min_Time))
		{
			if (times < 1)
			{
				Seek(file,0,OFFSET_BEGINNING);
				times = 0x40000/size;	/* Try to make file at least 256K size */
			}
			if (size != Write(file,buffer,size))
			{
				Display_Error(global,GetString(MSG_ERR_WFILE));
				worked = FALSE;
			}
			else count += size;
			times--;
		}
		Stop_Timer(global->timer);

		if (worked) Display_Result(global,GetString(MSG_BYTES_WRITE),count,GetString(MSG_BYTE_UNITS), CPU_AVAIL_struct);

		if (file) Close(file);
		file = NULL;

		if (worked) if (!(file = Open(FILE_STRING,MODE_OLDFILE)))
		{
			Display_Error(global,GetString(MSG_ERR_RFILE));
			worked = FALSE;
		}

		count = 0;
		times = 0;

		Start_Timer(global->timer);
		Init_CPU_Available(CPU_AVAIL_struct);		/* Start counting free CPU cycles... */
		while ((worked &= Check_Quit(global)) && (Read_Timer(global->timer) < global->Min_Time))
		{
			if (times < 1)
			{
				Seek(file,0,OFFSET_BEGINNING);
				times = 0x40000/size;	/* Try to make file at least 256K size */
			}
			if (size != Read(file,buffer,size))
			{
				Display_Error(global,GetString(MSG_ERR_RFILE));
				worked = FALSE;
			}
			else count += size;
			times--;
		}
		Stop_Timer(global->timer);

		if (worked)
		{
			for (loop=0;loop<size;loop++) worked &= (buffer[loop] == (char)loop);
			if (!worked) AddDisplayLine(global,GetString(MSG_ERR_DATA));
		}

		if (worked) Display_Result(global,GetString(MSG_BYTES_READ),count,GetString(MSG_BYTE_UNITS),CPU_AVAIL_struct);

		if (file) Close(file);

		Free_CPU_Available(CPU_AVAIL_struct);
		FreeMem(mem,size+offset);
		DeleteFile(FILE_STRING);
	}
	else
	{
		sprintf(global->tmp1,GetString(MSG_SKIPMEM),size,type);
		AddDisplayLine(global,global->tmp1);
	}

	return(worked);
}

/*
            Clean up (remove) all of the files in the current directory...
*/

static void CleanUpFiles(struct DiskSpeed *global)
{
   BPTR	lock;

	CurrentDir(lock = CurrentDir(NULL));	/* Get current directory lock */

	while (lock)
	{
		if (Examine(lock,global->fib))
		{
			if (ExNext(lock,global->fib)) DeleteFile(global->fib->fib_FileName);
			else lock = NULL;
		}
		else lock = NULL;
	}
}

#endif	/* SCSI_SPEED */

static void DoTests(struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   char            *st_string=global->tmp1;
   BOOL            working;
   ULONG            memtype;
   ULONG            offset;
   short            size;

#ifndef	SCSI_SPEED
   char            *fstring;
   LONG            buffers;
#endif	/* !SCSI_SPEED */

	/*
	   Ok, so now we are ready to run...  Display the
	   test conditions...
	*/

	strcpy(st_string,GetString(MSG_PROCESSOR));
	strcat(st_string,global->CPU_Type);
#if defined(__AROS__)
	strcat(st_string,GetString(MSG_BLDAROS));
#else
	strcat(st_string,GetString(MSG_BLDMOS));   
#endif
	strcat(st_string,global->Exec_Ver);
	if (global->HighDMA) strcat(st_string,GetString(MSG_DMAHIGH));
	else strcat(st_string,GetString(MSG_DMANORM));
	strcat(st_string,GetString(MSG_VIDDMA));
	AddDisplayLine(global,st_string);

	/*
	   Now, if we are in 2.0 OS, we can also find out the number of buffers
	   (maybe) on that device.  This is important.
	*/
#ifdef	SCSI_SPEED

	sprintf(st_string,"Device: %s",global->Device);

#else

	fstring = "Device: %s   Buffers: <information unavailable>";
#ifndef __MORPHOS__
	if (DOSBase->dl_lib.lib_Version > 36L)
#endif
	{
		/*
		   Ok, so we can now try to get a reading of the buffers
		   for the place we are about to test...

		   Note:  Since we are in the "CURRENTDIR" of the test disk,
		   we are using "" as the "device" to which to call AddBuffers()
		*/
		if ((buffers = AddBuffers("",0)) > 0) fstring="Device:  %s   Buffers: %ld";
	}

	sprintf(st_string,fstring,global->Device,buffers);

#endif	/* SCSI_SPEED */

	AddDisplayLine(global,st_string);

	if (global->Comments[0])
	{
		strcpy(st_string,GetString(MSG_CMMNTS));
		strcat(st_string,global->Comments);
		AddDisplayLine(global,st_string);
	}

	AddDisplayLine(global,"");

   if (CPU_AVAIL_struct->_CPU_Use_Base) Delay(60);    /* Make sure filesystem has flushed... */

	Init_CPU_Available(CPU_AVAIL_struct);
	if (CPU_AVAIL_struct->_CPU_Use_Base) Delay(225);	/* Get a quick reading (~4.5 seconds) */

	Free_CPU_Available(CPU_AVAIL_struct);

	if (CPU_AVAIL_struct->_CPU_Use_Base)
	{
		/*
		   Now, generate a countdown value that is aprox 3 times 4.5 second...
		*/
		CPU_AVAIL_struct->_CPU_Use_Base = (CPU_AVAIL_struct->_CPU_Count_Low * 3) + 1;
		CPU_AVAIL_struct->_CPU_Count_Low = CPU_AVAIL_struct->_CPU_Use_Base;

		Forbid();
		Start_Timer(global->timer);
		CPU_Calibrate(CPU_AVAIL_struct);
		Stop_Timer(global->timer);
		Permit();

		/*
		   If it looks like we did not get a good reading,
		   set up _CPU_Use_Base to 0 in order to turn off
		   CPU readings...
		*/
   
      if (global->timer->tmVal.tv_secs < 4)
		{
			AddDisplayLine(global,GetString(MSG_ERR_CALIB1));
			AddDisplayLine(global,GetString(MSG_ERR_CALIB2));
			CPU_AVAIL_struct->_CPU_Use_Base = 0;
		}
		else CPU_AVAIL_struct->_CPU_Use_Base = MaxDivide(CPU_AVAIL_struct->_CPU_Use_Base,(global->timer->tmVal.tv_secs * 1000000) + global->timer->tmVal.tv_micro,6);
	}

	global->Base_CPU = CPU_AVAIL_struct->_CPU_Use_Base;

	if (CPU_AVAIL_struct->_CPU_Use_Base) sprintf(st_string,GetString(MSG_CPUSPD),(((CPU_AVAIL_struct->_CPU_Use_Base/500)+1) >> 1 ));
	else strcpy(st_string,GetString(MSG_NOCPU));
	AddDisplayLine(global,st_string);
	AddDisplayLine(global,"");

	global->CPU_Total = 0L;
	global->CPU_Count = 0L;

	working = Check_Quit(global);

#ifndef	SCSI_SPEED

	if (working && global->Test_DIR)
	{
		AddDisplayLine(global,GetString(MSG_TSTMANP));
		if (working) working = CreateFileTest(global, CPU_AVAIL_struct);
		if (working) working = OpenFileTest(global, CPU_AVAIL_struct);
		if (working) working = ScanDirectoryTest(global, CPU_AVAIL_struct);
		if (working) working = DeleteFileTest(global, CPU_AVAIL_struct);
	}

	if (working && global->Test_SEEK)
	{
		AddDisplayLine(global,"");
		if (working) working = SeekReadTest(global, CPU_AVAIL_struct);
	}

#endif	/* !SCSI_SPEED */

	/*  Now for some of the more complex tests           */
	/*  result=SpeedTest(global,Buffer,offset,mem_type);   */

	memtype = MEMF_FAST;
	while (memtype)
	{
		if (memtype & global->Mem_TYPES) for (offset = 4;offset>0;offset=offset >> 1) if (offset & global->Align_Types)
		{
			for (size = 0;size<4;size++) if (global->Test_Size[size])
			{
				if (working) working = SpeedTest(global,global->Test_Size[size],offset&3,memtype, CPU_AVAIL_struct);
			}
		}

		if (memtype & MEMF_CHIP) memtype = 0;
		else memtype = MEMF_CHIP;
	}

#ifndef	SCSI_SPEED

   CleanUpFiles(global);

#endif	/* !SCSI_SPEED */

	if ((working) && (global->CPU_Count))
	{
		AddDisplayLine(global,"");
		global->CPU_Total = (((global->CPU_Total << 1) / global->CPU_Count)+1) >> 1;
		global->CPU_Count = (((global->Base_CPU * global->CPU_Total) / 50000)+1) >> 1;
		sprintf(st_string,GetString(MSG_AVGCPU),global->CPU_Total,global->CPU_Count);
		AddDisplayLine(global,st_string);
	}
}

#ifdef	SCSI_SPEED

static void StartTest(struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   APTR	         oldwindow;
   char	         *st_string;
   char	         *unit = NULL;
   ULONG	         scsi_unit;

	oldwindow = global->Me->pr_WindowPtr;
	global->Me->pr_WindowPtr = (APTR)(-1L);

   if (diskspeedmui_intern.resultlv)
     DoMethod(diskspeedmui_intern.resultlv, MUIM_List_Clear);

	AddDisplayLine(global,COPYRIGHT);
	AddDisplayLine(global,STR_SEPERATOR);

	st_string = global->Device;
	while (*st_string)
	{
		if (*st_string == ':') unit = st_string;
		st_string++;
	}

	st_string = unit;

	if (unit)
	{
		*unit = '\0';
		unit++;
		scsi_unit = 0;
		while ((unit) && (*unit))
		{
			if ((*unit < '0') || (*unit >'9')) unit = NULL;
			else
			{
				scsi_unit *= 10;
				scsi_unit += *unit-'0';
				unit++;
			}
		}
	}

	if (unit)
	{
		memset(&(global->DiskIO),0,sizeof(struct IOExtTD));
		if ((global->DiskIO.iotd_Req.io_Message.mn_ReplyPort = CreateMsgPort()))
		{
			if (!OpenDevice(global->Device,scsi_unit, (struct IORequest *)&global->DiskIO, NULL))
			{
				unit = NULL;
				*st_string = ':';
				DoTests(global, CPU_AVAIL_struct);
				CloseDevice((struct IORequest *)&global->DiskIO);
			}
			DeleteMsgPort(global->DiskIO.iotd_Req.io_Message.mn_ReplyPort);
		}
		if (unit) AddDisplayLine(global,GetString(MSG_ERR_NODEV));
	}
	else AddDisplayLine(global,GetString(MSG_ERR_BADSPEC));

	if (st_string) *st_string = ':';

	global->Me->pr_WindowPtr = oldwindow;
}

#else	/* SCSI_SPEED */

static void StartTest(struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   BPTR	         lock;
   BPTR	         newlock;
   APTR	         oldwindow;

	oldwindow = global->Me->pr_WindowPtr;
	global->Me->pr_WindowPtr = (APTR)(-1L);

	AddDisplayLine(global,COPYRIGHT);
	AddDisplayLine(global,STR_SEPERATOR);

	if (lock = Lock(global->Device,ACCESS_READ))
	{
		lock = CurrentDir(lock);
		if (newlock = CreateDir(TEST_DIR))
		{
			UnLock(newlock);
			if (newlock = Lock(TEST_DIR,ACCESS_READ))
			{
				newlock = CurrentDir(newlock);

				/*
				   Now do all of the tests...
				*/

				DoTests(global, CPU_AVAIL_struct);

				newlock = CurrentDir(newlock);
				UnLock(newlock);
			}
			else AddDisplayLine(global,GetString(MSG_ERR_BADDIR));
			DeleteFile(TEST_DIR);
		}
		else AddDisplayLine(global,GetString(MSG_ERR_NODIR));
		lock = CurrentDir(lock);
		UnLock(lock);
	}
	else AddDisplayLine(global,GetString(MSG_ERR_NOLOCK));

	global->Me->pr_WindowPtr = oldwindow;
}

#endif	/* SCSI_SPEED */

static VOID SetVersionStrings(struct DiskSpeed *global)
{
//  UWORD	         flags = ((struct ExecBase *)(SysBase))->AttnFlags;
   ULONG	         sysVer = ((struct ExecBase *)(SysBase))->LibNode.lib_Version;
   ULONG	         sofVer = ((struct ExecBase *)(SysBase))->SoftVer;
   char	         *st_string;

#if defined(__PPC__)
	strcpy( global->CPU_Type,"PPC" );
#else
	strcpy( global->CPU_Type,"i386" );
#endif
	st_string = &( global->CPU_Type[1] );

	sprintf(global->Exec_Ver,"%ld.%ld",(ULONG) sysVer,(ULONG) sofVer);
}

/*
            A simple string check that also works with '=' at the end of the string
*/

static char *Check_String(char *arg,char *match)
{
   char         *st_string;
   char         *next = NULL;

   D(bug("ParseArg() in Check_String()\n"));

	st_string = arg;
	while (*st_string)
	{
		if (*st_string == '=')
		{
			*st_string = '\0';
			next = st_string;
		}
		else st_string++;
	}

	if (stricmp(arg,match))
	{
		if (next) *next = '=';
		next = NULL;
	}
	else
	{
		if (next) next++;
		else next = st_string;
	}

	return(next);
}

/*
            This routine closes down the GUI
*/

static void Close_GUI(struct DiskSpeed *global)
{
   if (MUIMasterBase)
   {
      MUI_DisposeObject(diskspeedmui_intern.disk_app);
      CloseLibrary(MUIMasterBase);
   }
}

static Object *MakeCheck (CONST_STRPTR label, ULONG id, ULONG checked)
{
   Object *obj = MUI_MakeObject(MUIO_Checkmark, (IPTR)label);

   if (obj)
     SetAttrs(obj, MUIA_CycleChain, 1, MUIA_ObjectID, id, MUIA_Selected, checked, TAG_DONE);

   return obj;
}

static APTR MakeButton(char *string)
{
   APTR obj;
   if (obj = MUI_MakeObject(MUIO_Button, (IPTR)string))
     SetAttrs(obj, MUIA_CycleChain, 1, TAG_DONE);

   return obj;
}


/*
            This routine is used to open the GUI...
*/

static APTR Open_GUI(struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct, struct DiskObject *dobj, int load_config)
{
// BOOL		      worked = FALSE;

D(bug("Open_GUI()\n"));

   if ((MUIMasterBase)&&(!diskspeedmui_intern.disk_app))
   {
      sprintf(global->tmp1,GetString(MSG_MAIN_WINTITLE),APPNAME);
      sprintf(global->tmp2,GetString(MSG_Welcome),
#ifdef	SCSI_SPEED
      GetString(MSG_W_DEVICE));
#else
      GetString(MSG_W_FILESYS));
#endif /* SCSI_SPEED */

      diskspeedmui_intern.disk_app = ApplicationObject,
         MUIA_Application_DiskObject, (IPTR)dobj,
         MUIA_Application_Title, (IPTR)APPNAME,
         MUIA_Application_Version, (IPTR)CXVERSION,
         MUIA_Application_Copyright, (IPTR)"Copyright © 1995-2006, The AROS Development Team",
         MUIA_Application_Author, (IPTR)"The AROS Development Team",
         MUIA_Application_Description, (IPTR)GetString(MSG_DESCRIPTION),
         MUIA_Application_Base, (IPTR)APPNAME,
         MUIA_Application_Menustrip, (IPTR)(MenustripObject,
            MUIA_Family_Child, (IPTR)(MenuObject,
               MUIA_Menuitem_Title, (IPTR)GetString(WORD_MENU_Project),
               MUIA_Family_Child, (IPTR)(diskspeedmui_intern.about_item = MenuitemObject,
                  MUIA_Menuitem_Title, (IPTR)GetString(WORD_MENU_About),
               End),
               MUIA_Family_Child, (IPTR)(diskspeedmui_intern.quit_item  = MenuitemObject,
                  MUIA_Menuitem_Title, (IPTR)GetString(WORD_MENU_Quit),
               End),
            End),
         End),
         SubWindow, (IPTR)(diskspeedmui_intern.disk_main_win = WindowObject,
            MUIA_Window_Title, global->tmp1,
            MUIA_Window_ID, MAKE_ID('D','W','I','N'),
            MUIA_Window_Activate, TRUE,
            MUIA_Window_Height, MUIV_Window_Height_Visible(50),
            MUIA_Window_Width, MUIV_Window_Width_Visible(60),
            WindowContents, VGroup,
               Child, VGroup, /* Main Panel */
                  GroupFrame,
                  MUIA_Background, MUII_GroupBack,
                  Child, (IPTR)(diskspeedmui_intern.welctext = TextObject,
                     TextFrame,
                     MUIA_Text_Contents, global->tmp2,
                  End),
                  Child, HGroup,
                     Child, ColGroup(2),
                        MUIA_HorizWeight,100,
					         Child, Label(GetString(MSG_DEVICE)),
                        Child, (IPTR)(diskspeedmui_intern.devicestr = StringObject,
						         StringFrame,
                           MUIA_CycleChain, TRUE,
                           MUIA_String_MaxLen, 255,
                           MUIA_String_Contents, global->Device,
					         End),
                        Child, Label(GetString(MSG_CMMNTS)),
                        Child, (IPTR)(diskspeedmui_intern.commentstr = StringObject,
						         StringFrame,
                           MUIA_CycleChain, TRUE,
                           MUIA_String_MaxLen, 255,
					         End),
				         End,
     		            Child, (IPTR)(diskspeedmui_intern.configobj = CoolImageIDButton(NULL, COOL_INFOIMAGE_ID)),
                  End,
				      Child, ListviewObject,  /* Our results view */
						   MUIA_Listview_List, diskspeedmui_intern.resultlv = ListObject,
							   ReadListFrame,
							   MUIA_Listview_Input, FALSE,
							   /*MUIA_List_Title, TRUE,*/
							   MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
							   MUIA_List_DestructHook, MUIV_List_DestructHook_String,
						   End,
				      End,
               End,
		         Child, HGroup, /* save/use/cancel button row */
		            MUIA_FixHeight, 1,
		            MUIA_Group_SameWidth, TRUE,
		            Child, (IPTR)(diskspeedmui_intern.beginobj = CoolImageIDButton(GetString(MSG_START_TEST), COOL_DOTIMAGE_ID)),
                  Child, HSpace(0),
		            /*Child, diskspeedmui_intern.stopobj = CoolImageIDButton(GetString(MSG_STOP_TEST), COOL_CANCELIMAGE_ID),*/
		            Child, (IPTR)(diskspeedmui_intern.saveobj = CoolImageIDButton(GetString(MSG_SAVE_RESULTS), COOL_SAVEIMAGE_ID)),
		         End,
	         End,
	      End),
         //NEXT WINDOW.. ABOUT
         SubWindow, (IPTR)(diskspeedmui_intern.disk_about_win = AboutWindowObject,
            MUIA_AboutWindow_Authors, (IPTR) TAGLIST
            (
               SECTION
               (
                  SID_PROGRAMMING,
                  NAME("Nic Andrews")
               ),
               SECTION
               (
                  SID_TRANSLATING,
                  NAME("Adam Chodorowski"),
                  NAME("Fabio Alemagna")
               )
            ),
         End),
         //NEXT WINDOW.. CONFIG
         SubWindow, (IPTR)(diskspeedmui_intern.disk_config_win = WindowObject,
	         MUIA_Window_Title, (IPTR)GetString(MSG_CONFIG_WINTITLE),
	         MUIA_Window_ID, MAKE_ID('C','W','I','N'),
	         WindowContents, VGroup,
               Child, VGroup,
                  MUIA_Group_VertSpacing, 0,
                  Child, VSpace(0),
                  Child, HGroup,
                     MUIA_Group_VertSpacing, 2,
                     MUIA_Background, MUII_GroupBack,
                     GroupFrame,
                     Child, HSpace(0),
                     Child, ColGroup(2),
#ifndef SCSI_SPEED
                        Child, (IPTR)(diskspeedmui_config.dir_checkmark = MakeCheck(NULL, MAKE_ID('C','D','I','R'), TRUE)),
                        Child, (IPTR) LLabel("DIR"),
#endif
                        Child, (IPTR)(diskspeedmui_config.seek_checkmark = MakeCheck(NULL, MAKE_ID('S','E','E','K'), TRUE)),
                        Child, (IPTR) LLabel("SEEK"),
                        Child, (IPTR)(diskspeedmui_config.nocpu_checkmark = MakeCheck(NULL, MAKE_ID('N','C','P','U'), TRUE)),
                        Child, (IPTR) LLabel("NOCPU"),
                     End, 
                     Child, HSpace(0),
                     Child, VGroup,
                        /*MUIA_Background, MUII_GroupBack,*/
                        Child, (IPTR) CLabel("MINTIME"),
					         Child,(IPTR)(diskspeedmui_config.mintime = StringObject,
						         StringFrame,
                           MUIA_CycleChain, 1,
                           MUIA_ObjectID, MAKE_ID('M','I','N','T'),
				               MUIA_String_Accept, (IPTR)"0123456789",
					         End), 
                     End,
                     Child, HSpace(0),
                  End, 
                  Child, VSpace(0),
               End, /* Misc Options */
               Child, HSpace(0),
               Child, VGroup,
                  GroupFrameT(GetString(MSG_RWBUFS)),
                  MUIA_Group_VertSpacing, 0,
                  Child, VSpace(0),
                  Child, ColGroup(3),
                     MUIA_Group_VertSpacing, 2,
                     Child, LLabel(GetString(MSG_BUF1TXT)),
                     Child, HSpace(0),
                     Child,(IPTR)(diskspeedmui_config.buffer1 = StringObject,
                        StringFrame,
                        MUIA_CycleChain, 1,
                        MUIA_ObjectID, MAKE_ID('B','U','F','1'),
                        MUIA_String_Accept, (IPTR)"0123456789",
                        MUIA_String_Format, MUIV_String_Format_Right,
                     End), 
                     Child, LLabel(GetString(MSG_BUF2TXT)),
                     Child, HSpace(0),
                     Child,(IPTR)(diskspeedmui_config.buffer2 = StringObject,
                        StringFrame,
                        MUIA_CycleChain, 1,
                        MUIA_ObjectID, MAKE_ID('B','U','F','2'),
                        MUIA_String_Accept, (IPTR)"0123456789",
                        MUIA_String_Format, MUIV_String_Format_Right,
                     End), 
                     Child, LLabel(GetString(MSG_BUF3TXT)),
                     Child, HSpace(0),
                     Child,(IPTR)(diskspeedmui_config.buffer3 = StringObject,
                        StringFrame,
                        MUIA_CycleChain, 1,
                        MUIA_ObjectID, MAKE_ID('B','U','F','3'),
                        MUIA_String_Accept, (IPTR)"0123456789",
                        MUIA_String_Format, MUIV_String_Format_Right,
                     End), 
                     Child, LLabel(GetString(MSG_BUF4TXT)),
                     Child, HSpace(0),
                     Child,(IPTR)(diskspeedmui_config.buffer4 = StringObject,
                        StringFrame,
                        MUIA_CycleChain, 1,
                        MUIA_ObjectID, MAKE_ID('B','U','F','4'),
                        MUIA_String_Accept, (IPTR)"0123456789",
                        MUIA_String_Format, MUIV_String_Format_Right,
                     End),
                  End, 
                  Child, VSpace(0),
               End, /* Buffer Sizes */
               Child, VGroup,
                  GroupFrameT(GetString(MSG_RWBUFS)),
                  MUIA_Group_VertSpacing, 0,
                  Child, VSpace(0),
                  Child, ColGroup(5),
                     Child, HSpace(0),
                     Child, ColGroup(2),
                        MUIA_Group_VertSpacing, 2,
                        Child, (IPTR)(diskspeedmui_config.long_checkmark = MakeCheck(NULL, MAKE_ID('L','O','N','G'), TRUE)),
                        Child, (IPTR) LLabel("LONG"),
                        Child, (IPTR)(diskspeedmui_config.word_checkmark = MakeCheck(NULL, MAKE_ID('W','O','R','D'), TRUE)),
                        Child, (IPTR) LLabel("WORD"),
                        Child, (IPTR)(diskspeedmui_config.byte_checkmark = MakeCheck(NULL, MAKE_ID('B','Y','T','E'), TRUE)),
                        Child, (IPTR) LLabel("BYTE"),
                     End, 
                     Child, HSpace(0),
                     Child, ColGroup(2),
                        MUIA_Group_VertSpacing, 2,
                        Child, (IPTR)(diskspeedmui_config.fast_checkmark = MakeCheck(NULL, MAKE_ID('F','A','S','T'), TRUE)),
                        Child, (IPTR) LLabel("FAST"),
                        Child, (IPTR)(diskspeedmui_config.chip_checkmark = MakeCheck(NULL, MAKE_ID('C','H','I','P'), TRUE)),
                        Child, (IPTR) LLabel("CHIP"),
                     End,
                     Child, HSpace(0),
                  End, 
                  Child, VSpace(0),
               End, /* Buffer Types */
//               Child, diskspeedmui_intern.conokobj = CoolImageIDButton(GetString(MSG_OK), COOL_DOTIMAGE_ID),
               Child, HGroup,
                  Child, (IPTR)(diskspeedmui_intern.consaveobj = MakeButton(GetString(MSG_GAD_SAVE))),
                  Child, HSpace(30),
                  Child, (IPTR)(diskspeedmui_intern.conuseobj = MakeButton(GetString(MSG_GAD_USE))),
                  Child, HSpace(30),
                  Child, (IPTR)(diskspeedmui_intern.concancelobj = MakeButton(GetString(MSG_GAD_CANCEL))),
               End,
            End,
	      End),
      End;      

      if (diskspeedmui_intern.disk_app)
      {
         if (load_config)
         {
              /* Load Settings
              **
              ** Ditched config items stored to icon tooltypes. However DRIVE and COMMENT
              ** options are recognized still.
              */

              DoMethod(diskspeedmui_intern.disk_app, MUIM_Application_Load, MUIV_Application_Load_ENV);

              if (dobj)
              {
                  STRPTR s;
                  
                  if ((s = FindToolType(dobj->do_ToolTypes, "DRIVE")))
                  {
                     strcpy(global->Device, s);
                  }
                  
                  if ((s = FindToolType(dobj->do_ToolTypes, "COMMENT")))
                  {
                     strcpy(global->Comments, s);
                  }
              }
          }
/*********/
         DoMethod(diskspeedmui_intern.disk_main_win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

         DoMethod(diskspeedmui_intern.configobj, MUIM_Notify, MUIA_Pressed, FALSE, (IPTR)diskspeedmui_intern.disk_config_win, 3, MUIM_Set, MUIA_Window_Open, TRUE);

//        DoMethod(diskspeedmui_intern.stopobj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, RETURNID_STOP);
         DoMethod(diskspeedmui_intern.saveobj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, RETURNID_SAVE);
         DoMethod(diskspeedmui_intern.beginobj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, RETURNID_BEGIN);
/**/
         DoMethod(diskspeedmui_intern.disk_config_win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, (IPTR)diskspeedmui_intern.disk_config_win, 3, MUIM_Set, MUIA_Window_Open, FALSE);
         DoMethod(diskspeedmui_intern.consaveobj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, RETURNID_SAVECONFIG);
         DoMethod(diskspeedmui_intern.conuseobj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, RETURNID_USECONFIG);
         DoMethod(diskspeedmui_intern.concancelobj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_Open, FALSE);
/**/
         DoMethod(diskspeedmui_intern.disk_about_win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_Open, FALSE);
/**/
         DoMethod(diskspeedmui_intern.quit_item, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
         DoMethod(diskspeedmui_intern.about_item, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (IPTR)diskspeedmui_intern.disk_about_win, 3, MUIM_Set, MUIA_Window_Open, TRUE);

/**********/
         set(diskspeedmui_intern.configobj, MUIA_HorizWeight, 0); /* Set up the weight for the image ( so it takes up least space..) */ 
/*********/
         set(diskspeedmui_intern.commentstr, MUIA_String_Contents, global->Comments);
         set(diskspeedmui_intern.devicestr, MUIA_String_Contents, global->Device);

         /* Set some reasonable values if saved values are rubbish */
         if (!getv(diskspeedmui_config.mintime, MUIA_String_Integer))
            set(diskspeedmui_config.mintime, MUIA_String_Integer, global->Min_Time);

         if (!getv(diskspeedmui_config.buffer1, MUIA_String_Integer))
            set(diskspeedmui_config.buffer1, MUIA_String_Integer, global->Test_Size[0]);

         if (!getv(diskspeedmui_config.buffer2, MUIA_String_Integer))
            set(diskspeedmui_config.buffer2, MUIA_String_Integer, global->Test_Size[1]);

         if (!getv(diskspeedmui_config.buffer3, MUIA_String_Integer))
            set(diskspeedmui_config.buffer3, MUIA_String_Integer, global->Test_Size[2]);

         if (!getv(diskspeedmui_config.buffer4, MUIA_String_Integer))
            set(diskspeedmui_config.buffer4, MUIA_String_Integer, global->Test_Size[3]);

#if 0
#ifdef SCSI_SPEED			
         set( diskspeedmui_config.dir_checkmark, MUIA_Disabled, TRUE);
         set( diskspeedmui_config.seek_checkmark, MUIA_Disabled, TRUE);
         set( diskspeedmui_config.long_checkmark, MUIA_Selected, TRUE);
         set( diskspeedmui_config.long_checkmark, MUIA_Disabled, TRUE);
         set( diskspeedmui_config.word_checkmark, MUIA_Disabled, TRUE);
         set( diskspeedmui_config.byte_checkmark, MUIA_Disabled, TRUE);
#else	/* SCSI_SPEED */
         if (global->Test_DIR) set( diskspeedmui_config.dir_checkmark, MUIA_Selected,TRUE);
         if (global->Test_SEEK) set( diskspeedmui_config.seek_checkmark, MUIA_Selected,TRUE);
         if (global->Align_Types & 4) set( diskspeedmui_config.long_checkmark, MUIA_Selected, TRUE);
         if (global->Align_Types & 2) set( diskspeedmui_config.word_checkmark, MUIA_Selected, TRUE);
         if (global->Align_Types & 1) set( diskspeedmui_config.byte_checkmark, MUIA_Selected, TRUE);
#endif	/* SCSI_SPEED */
         if (CPU_AVAIL_struct->_CPU_Use_Base==0) set( diskspeedmui_config.nocpu_checkmark, MUIA_Selected,TRUE);
            
         if (global->Mem_TYPES & MEMF_FAST) set( diskspeedmui_config.fast_checkmark,MUIA_Selected,TRUE);
         if (global->Mem_TYPES & MEMF_CHIP) set( diskspeedmui_config.chip_checkmark,MUIA_Selected,TRUE);
#endif
         if (SysBase->MaxLocMem == 0)
           set( diskspeedmui_config.chip_checkmark, MUIA_Disabled, TRUE);
      }
   }

   return(diskspeedmui_intern.disk_app);
}

/*
            This routine will append to the end of a file the results currently in memory...
*/
static void Save_Results(struct DiskSpeed *global)
{
	BPTR            fh;
   struct	Node		*node;

	if ((fh = Open(RESULTS_FILE,MODE_OLDFILE)))
	{
		Seek(fh,0,OFFSET_END);
		Write(fh,"\n\n\n",2);
	}
	else fh = Open(RESULTS_FILE,MODE_NEWFILE);

	if (fh)
	{
		node = (struct Node *)(global->TextList.mlh_Head);
		while (node->ln_Succ)
		{
			Write(fh,node->ln_Name,strlen(node->ln_Name));
			Write(fh,"\n",1);
			node = node->ln_Succ;
		}

		Close(fh);
	}
}

/*
            This routine will retrieve the config settings from the MUI_Objects
 */
static void GET_Config(struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct)
{
	//BOOL                  error = TRUE;
   ULONG                  tmp;

	/*
		Ok, now get the information back out...
	*/

   get(diskspeedmui_config.mintime, MUIA_String_Integer, &global->Min_Time);

   get(diskspeedmui_config.buffer1, MUIA_String_Integer, &global->Test_Size[0]);
   get(diskspeedmui_config.buffer2, MUIA_String_Integer, &global->Test_Size[1]);
   get(diskspeedmui_config.buffer3, MUIA_String_Integer, &global->Test_Size[2]);
   get(diskspeedmui_config.buffer4, MUIA_String_Integer, &global->Test_Size[3]);

#ifndef SCSI_SPEED
   global->Test_DIR=0;
   get( diskspeedmui_config.dir_checkmark, MUIA_Selected, &tmp);
   if (tmp) global->Test_DIR=1;
#endif

   global->Test_SEEK=0;
   get( diskspeedmui_config.seek_checkmark, MUIA_Selected, &tmp);
   if (tmp) global->Test_SEEK=1;

   CPU_AVAIL_struct->_CPU_Use_Base=1;
   get( diskspeedmui_config.nocpu_checkmark, MUIA_Selected, &tmp);
   if (tmp) CPU_AVAIL_struct->_CPU_Use_Base=0;

   global->Align_Types &= (~4);
   get( diskspeedmui_config.long_checkmark, MUIA_Selected,&tmp);
   if (tmp) global->Align_Types |= 1<<2;

   global->Align_Types &= (~2);
   get( diskspeedmui_config.word_checkmark, MUIA_Selected,&tmp);
   if (tmp) global->Align_Types |= 1<<1;
   
   global->Align_Types &= (~1);
   get( diskspeedmui_config.byte_checkmark, MUIA_Selected,&tmp);
   if (tmp) global->Align_Types |= 1;

   global->Mem_TYPES &= (~MEMF_FAST);
   get( diskspeedmui_config.fast_checkmark,MUIA_Selected,&tmp);
   if (tmp) global->Mem_TYPES |= (1 * MEMF_FAST);

   global->Mem_TYPES &= (~MEMF_CHIP);
   if (SysBase->MaxLocMem)
   {
      get( diskspeedmui_config.chip_checkmark,MUIA_Selected,&tmp);
      if (tmp) global->Mem_TYPES |= (1 * MEMF_CHIP);
   }
		
   set (diskspeedmui_intern.disk_config_win, MUIA_Window_Open, FALSE);
}

/*
   DRIVE/K	- select drive  (Default is current directory or scsi.device:6)
   COMMENT/K	- set comment string
   ALL/S	- select all tests
   DIR/S	- setect DIR tests
   SEEK/S	- select SEEK tests
   CHIP/S	- select CHIP memory buffer tests
   FAST/S	- select FAST memory buffer tests
   LONG/S	- select LONG aligned tests
   WORD/S	- select WORD aligned tests
   BYTE/S	- select BYTE aligned tests
   NOCPU/S	- turn off CPU availability tests
   BUF1/K/N	- select buffer size 1
   BUF2/K/N	- select buffer size 2
   BUF3/K/N	- select buffer size 3
   BUF4/K/N	- select buffer size 4
   MINTIME/K/N	- select the minimum test tmVal (default=8) in seconds
   WINDOW/S	- use the GUI even though started from the CLI
 */

/*
         do the command line parsing here...
 */
static LONG ParseArg(struct DiskSpeed *global,int argc,char *argv[],int start,struct CPU_AVAIL *CPU_AVAIL_struct)
{
   int    loop;
   char	*arg;
   char	*next;
   LONG	working = TRUE;
   LONG	window = FALSE;

   D(bug("ParseArg()\n"));

#ifdef	SCSI_SPEED
	global->Align_Types = 4;
#endif	/* SCSI_SPEED */

	for (loop = start;loop<argc;loop++)
	{
		if (loop >= 10) break;
      D(bug("ParseArg() in loop(argv=%ld,argc=%ld,start=%ld,loop=%ld))\n",argv,argc,start,loop));
      arg = argv[loop];
		if (*arg == '?')
		{
			D(bug("ParseArg() Arg = ?\n"));
         loop = argc;
			working = FALSE;
		}
		else if ((next=Check_String(arg,"DRIVE")))
		{
			D(bug("ParseArg() Arg = DRIVE\n"));
         if (!(*next))
			{
				loop++;
				if (loop<argc) next = argv[loop];
			}
			if (strlen(next)>255) *next = '\0';
			if (*next) strcpy(global->Device,next);
			else working = FALSE;
		}
		else if ((next=Check_String(arg,"COMMENT")))
		{
			D(bug("ParseArg() Arg = COMMENT\n"));
         if (!(*next))
			{
				loop++;
				if (loop<argc) next = argv[loop];
			}
			if (strlen(next)>255) *next = '\0';
			if (*next) strcpy(global->Comments,next);
			else working = FALSE;
		}
		else if (Check_String(arg,"ALL"))
		{
         D(bug("ParseArg() Arg = ALL\n"));
         /* All tests */

#ifndef	SCSI_SPEED

			global->Test_DIR = TRUE;
			global->Test_SEEK = TRUE;
			global->Align_Types = 4|2|1;

#endif	/* !SCSI_SPEED */

			global->Mem_TYPES = (SysBase->MaxLocMem ? MEMF_CHIP : 0) | MEMF_FAST;
		}
		else if ((next = Check_String(arg,"BUF1")))
		{
			D(bug("ParseArg() Arg = BUF1\n"));
         if (!(*next))
			{
				loop++;
				if (loop<argc) next = argv[loop];
			}
			if (*next) stcd_l(next,&(global->Test_Size[0]));
			else working=FALSE;
		}
		else if ((next = Check_String(arg,"BUF2")))
		{
			D(bug("ParseArg() Arg = BUF2\n"));
         if (!(*next))
			{
				loop++;
				if (loop<argc) next = argv[loop];
			}
			if (*next) stcd_l(next,&(global->Test_Size[1]));
			else working = FALSE;
		}
		else if ((next = Check_String(arg,"BUF3")))
		{
			D(bug("ParseArg() Arg = BUF3\n"));
         if (!(*next))
			{
				loop++;
				if (loop<argc) next = argv[loop];
			}
			if (*next) stcd_l(next,&(global->Test_Size[2]));
			else working = FALSE;
		}
		else if ((next = Check_String(arg,"BUF4")))
		{
			D(bug("ParseArg() Arg = BUF4\n"));
         if (!(*next))
			{
				loop++;
				if (loop<argc) next=argv[loop];
			}
			if (*next) stcd_l(next,&(global->Test_Size[3]));
			else working = FALSE;
		}
		else if ((next = Check_String(arg,"MINTIME")))
		{
			D(bug("ParseArg() Arg = MINTIME\n"));
         if (!(*next))
			{
				loop++;
				if (loop<argc) next = argv[loop];
			}
			if (*next) stcd_l(next,&(global->Min_Time));
			else working = FALSE;
		}
#ifndef	SCSI_SPEED

		else if (Check_String(arg,"DIR")) global->Test_DIR = TRUE;
		else if (Check_String(arg,"SEEK")) global->Test_SEEK = TRUE;
		else if (Check_String(arg,"LONG")) global->Align_Types |= 4;
		else if (Check_String(arg,"WORD")) global->Align_Types |= 2;
		else if (Check_String(arg,"BYTE")) global->Align_Types |= 1;

#endif	/* !SCSI_SPEED */

		else if (Check_String(arg,"CHIP")) global->Mem_TYPES |=MEMF_CHIP;
		else if (Check_String(arg,"FAST")) global->Mem_TYPES |= MEMF_FAST;
		else if (Check_String(arg,"NOCPU")) CPU_AVAIL_struct->_CPU_Use_Base = FALSE;
      else if (Check_String(arg,"WINDOW")) window = TRUE;
		else
		{	/* Did not match, so error */
			D(bug("ParseArg() NO ARG MATCHES\n"));
         working = FALSE;
		}
	}


   D(bug("ParseArg() loop().. FINISHED\n"));

	if (global->Min_Time < 1) global->Min_Time = 1;

   D(bug("ParseArg() - preparing to open MUI window\n"));

	if (working && window) working = Open_GUI(global,CPU_AVAIL_struct, NULL, 0);

	return(working);
}

void GrabComment (char *theString)
{
   strcpy(global->Comments, theString);
}

void GrabDevice (char *theString)
{
   strcpy(global->Device, theString);
}

static void HandleAll (struct DiskSpeed *global, struct CPU_AVAIL *CPU_AVAIL_struct)
{
   ULONG sigs = 0;
   LONG returnid;
   diskspeedmui_intern.app_ret_val = FALSE;

   set (diskspeedmui_intern.disk_main_win, MUIA_Window_Open, TRUE);
   
   for(;;)
   {
   	returnid = (LONG) DoMethod(diskspeedmui_intern.disk_app, MUIM_Application_NewInput, (IPTR) &sigs);

	   if (returnid == MUIV_Application_ReturnID_Quit)
      {
         diskspeedmui_intern.app_ret_val = TRUE;
         break;
      }
   	
      if ((returnid == RETURNID_SAVECONFIG) || (returnid == RETURNID_USECONFIG) || (returnid == RETURNID_BEGIN) || (returnid == RETURNID_STOP) || (returnid == RETURNID_SAVE)) break;

	   if (sigs)
	   {
	      sigs = Wait(sigs | SIGBREAKF_CTRL_C);
	      if (sigs & SIGBREAKF_CTRL_C) break;
	   }
   }

   switch(returnid)
   {
      case RETURNID_BEGIN:
      {
         ULONG str1, str2;
         
         GET_Config(global, CPU_AVAIL_struct);

         GetAttr(MUIA_String_Contents, diskspeedmui_intern.commentstr, &str1);
         GetAttr(MUIA_String_Contents, diskspeedmui_intern.devicestr, &str2);
         
         strcpy(global->Comments, (char *)str1);
         strcpy(global->Device, (char *)str2);

         set(diskspeedmui_intern.disk_app, MUIA_Application_Sleep, TRUE);
         StartTest(global, CPU_AVAIL_struct);
         set(diskspeedmui_intern.disk_app, MUIA_Application_Sleep, FALSE);
         break;
      }
      case RETURNID_STOP:
      {
         /* In order to support this the GUI would need to be threaded */
         break;
      }
   	case RETURNID_SAVE:
	   {
 			Save_Results(global);
         break;
	   }
      case RETURNID_SAVECONFIG:
         DoMethod(diskspeedmui_intern.disk_app, MUIM_Application_Save, MUIV_Application_Save_ENVARC); /* fall through to USECONFIG */
      case RETURNID_USECONFIG:
         DoMethod(diskspeedmui_intern.disk_app, MUIM_Application_Save, MUIV_Application_Save_ENV);
         set(diskspeedmui_intern.disk_config_win, MUIA_Window_Open, FALSE);
         break;
	   break;
	   
   }
}

/*
            This routine is called when we want to run from a GUI
            Normally, it is only called when started from Workbench
            or when the CLI WINDOW option is given...
 */
static void FromWanderer(struct DiskSpeed *global,int argc,char *argv[], struct CPU_AVAIL *CPU_AVAIL_struct)
{
   struct	WBStartup	*startup;
   struct	WBArg		*wbarg;

	startup = (struct WBStartup *) argv;

//   D(bug("FromWanderer()\n"));

	if ((wbarg = startup->sm_ArgList) && (startup->sm_NumArgs))
	{
   	BPTR            lock;
      struct	DiskObject	*icon;
		/*
		   Check if we were started as a project and
		   use that icon insted...
		*/
		if ((startup->sm_NumArgs) > 1) wbarg++;

		lock = CurrentDir(wbarg->wa_Lock);
		icon = GetDiskObject(wbarg->wa_Name);

#if 0
		if (!argc)
		{
			/* All tests */
			global->Test_DIR = TRUE;
			global->Test_SEEK = TRUE;
			global->Align_Types = 4|2|1;
			global->Mem_TYPES = MEMF_CHIP | MEMF_FAST;
		}
#endif

      D(bug("FromWanderer() - launching GUI\n"));

		if (Open_GUI(global,CPU_AVAIL_struct, icon, 1))
      {
         for(;;)
         {
            HandleAll(global,CPU_AVAIL_struct);
            if (diskspeedmui_intern.app_ret_val == TRUE)
            {	Close_GUI(global);
               break;
            }
         }
      }

      if (icon)
         FreeDiskObject(icon);

		CurrentDir(lock);
	}
   D(bug("FromWanderer() Finished..\n"));
}

/*
            This is the CLI starting point.  We do the command line parsing here...
 */
static void FromCLI(struct DiskSpeed *global,int argc,char *argv[], struct CPU_AVAIL *CPU_AVAIL_struct)
{
	if (ParseArg(global,argc,argv,1,CPU_AVAIL_struct))
	{
      if ( diskspeedmui_intern.disk_app )
      {
         for(;;)
         {
            HandleAll(global,CPU_AVAIL_struct);
            if (diskspeedmui_intern.app_ret_val == TRUE)
            {	Close_GUI(global);
               break;
            }
         }
      }
		else StartTest(global, CPU_AVAIL_struct);
	}
	else
	{
#ifdef	SCSI_SPEED
		AddDisplayLine(global,"DRIVE/K,ALL/S,CHIP/S,FAST/S,NOCPU/S,BUF1/K/N,BUF2/K/N,BUF3/K/N,BUF4/K/N,MINTIME/K/N,WINDOW/S");

#else	/* SCSI_SPEED */
		AddDisplayLine(global,"DRIVE/K,ALL/S,DIR/S,SEEK/S,CHIP/S,FAST/S,LONG/S,WORD/S,BYTE/S,NOCPU/S,BUF1/K/N,BUF2/K/N,BUF3/K/N,BUF4/K/N,MINTIME/K/N,WINDOW/S");

#endif	/* SCSI_SPEED */
	}
}

int main(int argc, char *argv[])
{
   struct	DiskSpeed	*global;
   struct  CPU_AVAIL  *CPU_AVAIL_struct;

	if ((CPU_AVAIL_struct = AllocMem(sizeof(struct CPU_AVAIL),MEMF_PUBLIC|MEMF_CLEAR)))
   {
	   CPU_AVAIL_struct->_CPU_Use_Base = TRUE;                      /* We want to test with CPU */

		if ((global = AllocMem(sizeof(struct DiskSpeed),MEMF_PUBLIC|MEMF_CLEAR)))
		{
#if !defined(__AROS__)
         MyCatalog = OpenCatalog(NULL, "DiskSpeed.catalog", OC_BuiltInLanguage, "english", TAG_DONE);
#endif
			NEWLIST((struct List *)&(global->TextList));
			SetVersionStrings(global);
			global->Me = (struct Process *)FindTask(NULL);

			/* Standard MinTime */
			global->Min_Time=MIN_TEST_TIME;

			/* Standard sizes */
			global->Test_Size[0] = 0x200;
			global->Test_Size[1] = 0x1000;
			global->Test_Size[2] = 0x8000;
			global->Test_Size[3] = 0x40000;
#ifdef	SCSI_SPEED
         if ((global->timer = Init_Timer()))
			{
#if defined (__MORPHOS__)
            strcpy(global->Device, SysBase->MaxLocMem ? "scsi.device:0" : "ide.device:0");
#else
            strcpy(global->Device, "ata.device:0");
#endif
				/*
					Now either set up Window or Output
					depending on where we were started...

					If we can not get Output, we set up the window...
				*/
				if ((argc) && (global->Output=Output())) FromCLI(global,argc,argv,CPU_AVAIL_struct);
				else FromWanderer(global,argc,argv,CPU_AVAIL_struct);

				Free_Timer(global->timer);
			}
#else
			if ((global->fib = AllocMem(sizeof(struct FileInfoBlock),MEMF_PUBLIC)))
			{
				if ((global->timer = Init_Timer()))
				{
				   BPTR lock;
				   if((lock = Lock("",ACCESS_READ)))
				   {
				      char  *p;
				      NameFromLock(lock, global->Device, sizeof(global->Device));
				      UnLock(lock);
				     
				      if ((p = strchr(global->Device, ':')))
				      {
				         p[1] = '\0';
				      }
				   }
					/*
						Now either set up Window or Output
						depending on where we were started...

						If we can not get Output, we set up the window...
					*/
					if ((argc) && ((global->Output = Output()))) FromCLI(global,argc,argv,CPU_AVAIL_struct);
					else FromWanderer( global,argc,argv,CPU_AVAIL_struct );

					Free_Timer(global->timer);
				}
				FreeMem(global->fib,sizeof(struct FileInfoBlock));
			}

#endif	/* SCSI_SPEED */
#if !defined(__AROS__)
         CloseCatalog(MyCatalog);
#endif
			FreeMem(global,sizeof(struct DiskSpeed));
	   }
      FreeMem(CPU_AVAIL_struct,sizeof(struct CPU_AVAIL));
   }
   return 0;
}
