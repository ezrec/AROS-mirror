#include <aros/oldprograms.h>
/************************************************************************
*  a terminal program that has ascii and xmodem transfer capability
*
*  use esc to abort xmodem transfer
*
*  written by Michael Mounier
************************************************************************/

/*  compiler directives to fetch the necessary header files */
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include <graphics/text.h>
#include <graphics/regions.h>
#include <graphics/copper.h>
#include <graphics/gels.h>
#include <devices/serial.h>
#include <devices/keymap.h>
#include <hardware/blit.h>
#include <stdio.h>
#include <ctype.h>
#include <libraries/dos.h>

#define INTUITION_REV 1
#define GRAPHICS_REV  1

/* things for xmodem send and recieve */
#define SECSIZ   0x80
#define TTIME    30          /* number of seconds for timeout */
#define BufSize  0x1000     /* Text buffer */
#define ERRORMAX 10        /* Max errors before abort */
#define RETRYMAX 10       /* Maximum retrys before abort */
#define SOH      1       /* Start of sector char */
#define EOT      4      /* end of transmission char */
#define ACK      6     /* acknowledge sector transmission */
#define NAK      21   /* error in transmission detected */

static char
    bufr[BufSize];
static int
    fd,
    timeout = FALSE;
static long
    bytes_xferred;

/*   Intuition always wants to see these declarations */
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

/* my window structure */
struct NewWindow NewWindow = {
   0,
   0,
   639,
   199,
   0,
   1,
   CLOSEWINDOW | RAWKEY | MENUPICK | NEWSIZE,
   WINDOWCLOSE | SMART_REFRESH | ACTIVATE | WINDOWDRAG
   | WINDOWDEPTH | WINDOWSIZING | REPORTMOUSE,
   NULL,
   NULL,
   "AMIGA Terminal",
   NULL,
   NULL,
   100, 35,
   640, 200,
   WBENCHSCREEN,
};

struct Window *mywindow;             /* ptr to applications window */
struct IntuiMessage *NewMessage;    /* msg structure for GetMsg() */

static char toasc ();
static unsigned char readchar ();

/*****************************************************
*                     File Menu
*****************************************************/

/* define maximum number of menu items */
#define FILEMAX 4

/*   declare storage space for menu items and
 *   their associated IntuiText structures
 */
struct MenuItem FileItem[FILEMAX];
struct IntuiText FileText[FILEMAX];

/*****************************************************************/
/*    The following function initializes the structure arrays    */
/*   needed to provide the File menu topic.                      */
/*****************************************************************/
InitFileItems()
{
short n;

/* initialize each menu item and IntuiText with loop */
for( n=0; n<FILEMAX; n++ )
   {
   FileItem[n].NextItem = &FileItem[n+1];
   FileItem[n].LeftEdge = 0;
   FileItem[n].TopEdge = 11 * n;
   FileItem[n].Width = 135;
   FileItem[n].Height = 11;
   FileItem[n].Flags = ITEMTEXT | ITEMENABLED | HIGHBOX;
   FileItem[n].MutualExclude = 0;
   FileItem[n].ItemFill = (APTR)&FileText[n];
   FileItem[n].SelectFill = NULL;
   FileItem[n].Command = 0;
   FileItem[n].SubItem = NULL;
   FileItem[n].NextSelect = 0;

   FileText[n].FrontPen = 0;
   FileText[n].BackPen = 1;
   FileText[n].DrawMode = JAM2;     /* render in fore and background */
   FileText[n].LeftEdge = 0;
   FileText[n].TopEdge = 1;
   FileText[n].ITextFont = NULL;
   FileText[n].NextText = NULL;
   }
FileItem[FILEMAX-1].NextItem = NULL;

/* initialize text for specific menu items */
FileText[0].IText = (UBYTE *)"Ascii Capture";
FileText[1].IText = (UBYTE *)"Ascii Send";
FileText[2].IText = (UBYTE *)"Xmodem Receive";
FileText[3].IText = (UBYTE *)"Xmodem Send";

return( 0 );
}

/*****************************************************/
/*                BaudRate  Menu                     */
/*****************************************************/

/* define maximum number of menu items */
#define RSMAX 5

/*   declare storage space for menu items and
 *   their associated IntuiText structures
 */
struct MenuItem RSItem[RSMAX];
struct IntuiText RSText[RSMAX];

/*****************************************************************/
/*    The following function initializes the structure arrays    */
/*   needed to provide the BaudRate menu topic.                  */
/*****************************************************************/
InitRSItems()
{
short n;

/* initialize each menu item and IntuiText with loop */
for( n=0; n<RSMAX; n++ )
   {
   RSItem[n].NextItem = &RSItem[n+1];
   RSItem[n].LeftEdge = 0;
   RSItem[n].TopEdge = 11 * n;
   RSItem[n].Width = 85;
   RSItem[n].Height = 11;
   RSItem[n].Flags = ITEMTEXT | ITEMENABLED | HIGHBOX | CHECKIT;
   RSItem[n].MutualExclude = (~(1 << n));
   RSItem[n].ItemFill = (APTR)&RSText[n];
   RSItem[n].SelectFill = NULL;
   RSItem[n].Command = 0;
   RSItem[n].SubItem = NULL;
   RSItem[n].NextSelect = 0;

   RSText[n].FrontPen = 0;
   RSText[n].BackPen = 1;
   RSText[n].DrawMode = JAM2;     /* render in fore and background */
   RSText[n].LeftEdge = 0;
   RSText[n].TopEdge = 1;
   RSText[n].ITextFont = NULL;
   RSText[n].NextText = NULL;
   }
RSItem[RSMAX-1].NextItem = NULL;
/* 300 baud item chekced */
RSItem[0].Flags = ITEMTEXT | ITEMENABLED | HIGHBOX | CHECKIT | CHECKED;

/* initialize text for specific menu items */
RSText[0].IText = (UBYTE *)"   300";
RSText[1].IText = (UBYTE *)"   1200";
RSText[2].IText = (UBYTE *)"   2400";
RSText[3].IText = (UBYTE *)"   4800";
RSText[4].IText = (UBYTE *)"   9600";

return( 0 );
}


/***************************************************/
/*                Menu Definition                  */
/*                                                 */
/*      This section of code is where the simple   */
/*   menu definition goes.                         */
/***************************************************/

/* current number of available menu topics */
#define MAXMENU 2

/*   declaration of menu structure array for
 *   number of current topics.  Intuition
 *   will use the address of this array to
 *   set and clear the menus associated with
 *   the window.
 */
struct Menu menu[MAXMENU];

/**********************************************************************/
/*   The following function initializes the Menu structure array with */
/*  appropriate values for our simple menu strip.  Review the manual  */
/*  if you need to know what each value means.                        */
/**********************************************************************/
InitMenu()
{
menu[0].NextMenu = &menu[1];
menu[0].LeftEdge = 5;
menu[0].TopEdge = 0;
menu[0].Width = 50;
menu[0].Height = 10;
menu[0].Flags = MENUENABLED;
menu[0].MenuName = "File";           /* text for menu-bar display */
menu[0].FirstItem = &FileItem[0];    /* pointer to first item in list */

menu[1].NextMenu = NULL;
menu[1].LeftEdge = 65;
menu[1].TopEdge = 0;
menu[1].Width = 85;
menu[1].Height = 10;
menu[1].Flags = MENUENABLED;
menu[1].MenuName = "BaudRate";        /* text for menu-bar display */
menu[1].FirstItem = &RSItem[0];    /* pointer to first item in list */

return( 0 );
}

/* declarations for the serial stuff */
extern struct MsgPort *CreatePort();
struct IOExtSer *Read_Request;
static char rs_in[2];
struct IOExtSer *Write_Request;
static char rs_out[2];

/******************************************************/
/*                   Main Program                     */
/*                                                    */
/*      This is the main body of the program.         */
/******************************************************/

main()
{
ULONG class;
USHORT code,menunum,itemnum;
int KeepGoing,capture,send;
char c,name[32];
FILE *tranr = NULL;
FILE *trans = NULL;

IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", INTUITION_REV);
if( IntuitionBase == NULL )
   {
   puts("can't open intuition\n");
   exit(TRUE);
   }

GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",GRAPHICS_REV);
if( GfxBase == NULL )
   {
   puts("can't open graphics library\n");
   exit(TRUE);
   }

if(( mywindow = (struct Window *)OpenWindow(&NewWindow) ) == NULL)
   {
   puts("cant open window\n");
   exit(TRUE);
   }

Read_Request = (struct IOExtSer *)AllocMem(sizeof(*Read_Request),MEMF_PUBLIC|MEMF_CLEAR);
Read_Request->io_SerFlags = SERF_SHARED | SERF_XDISABLED;
Read_Request->IOSer.io_Message.mn_Length = sizeof(*Read_Request);
Read_Request->IOSer.io_Message.mn_ReplyPort = CreatePort("Read_RS",0);

if(OpenDevice(SERIALNAME,NULL,Read_Request,NULL))
   {
   puts("Cant open Read device\n");
   CloseWindow( mywindow );
   DeletePort(Read_Request->IOSer.io_Message.mn_ReplyPort);
   FreeMem(Read_Request,sizeof(*Read_Request));
   exit(TRUE);
   }
Read_Request->IOSer.io_Command = CMD_READ;
Read_Request->IOSer.io_Length = 1;
Read_Request->IOSer.io_Data = (APTR) &rs_in[0];

Write_Request = (struct IOExtSer *)AllocMem(sizeof(*Write_Request),MEMF_PUBLIC|MEMF_CLEAR);
Write_Request->io_SerFlags = SERF_SHARED | SERF_XDISABLED;
Write_Request->IOSer.io_Message.mn_Length = sizeof(*Write_Request);
Write_Request->IOSer.io_Message.mn_ReplyPort = CreatePort("Write_RS",0);
if(OpenDevice(SERIALNAME,NULL,Write_Request,NULL))
   {
   puts("Cant open Write device\n");
   CloseWindow( mywindow );
   DeletePort(Write_Request->IOSer.io_Message.mn_ReplyPort);
   FreeMem(Write_Request,sizeof(*Write_Request));
   DeletePort(Read_Request->IOSer.io_Message.mn_ReplyPort);
   FreeMem(Read_Request,sizeof(*Read_Request));
   exit(TRUE);
   }
Write_Request->IOSer.io_Command = CMD_WRITE;
Write_Request->IOSer.io_Length = 1;
Write_Request->IOSer.io_Data = (APTR) &rs_out[0];

Read_Request->io_SerFlags = SERF_SHARED | SERF_XDISABLED;
Read_Request->io_Baud = 300;
Read_Request->io_ReadLen = 8;
Read_Request->io_WriteLen = 8;
Read_Request->io_CtlChar = 1L;
Read_Request->IOSer.io_Command = SDCMD_SETPARAMS;
DoIO(Read_Request);
Read_Request->IOSer.io_Command = CMD_READ;

InitFileItems();
InitRSItems();
InitMenu();
SetMenuStrip(mywindow,&menu[0]);

KeepGoing = TRUE;
capture=FALSE;
send=FALSE;
SetAPen(mywindow->RPort,1);
emit(12);
BeginIO(Read_Request);

while( KeepGoing )
     {
     /* wait for window message or serial port message */
     Wait((1 << Read_Request->IOSer.io_Message.mn_ReplyPort->mp_SigBit) | ( 1 << mywindow->UserPort->mp_SigBit));

     if (send)
         {
         if ((c=getc(trans)) != EOF)
             {
             sendchar(c);
             }
         else
             {
             fclose(trans);
             emits("\nFile Sent\n");
             send=FALSE;
             }
         }
     if(CheckIO(Read_Request))
        {
        WaitIO(Read_Request);
        c=rs_in[0] & 0x7f;
        BeginIO(Read_Request);
        emit(c);
        if (capture)
            if (c > 31 && c < 127 || c == 10)  /* trash them mangy ctl chars */
                putc(c , tranr);
        }

     while( NewMessage=(struct IntuiMessage *)GetMsg(mywindow->UserPort) )
          {
          class = NewMessage->Class;
          code = NewMessage->Code;
          ReplyMsg( NewMessage );
          switch( class )
                {
                case CLOSEWINDOW:
                   /*   User is ready to quit, so indicate
                   *   that execution should terminate
                   *   with next iteration of the loop.
                   */
                   KeepGoing = FALSE;
                break;

                case RAWKEY:
                /*  User has touched the keyboard */
                switch( code )
                      {
                      case 95: /* help key */
                         emits("ESC Aborts Xmodem Xfer\n");
                         emits("AMIGA Term 1.0\n");
                         emits("Copyright 1985 by Michael Mounier\n");
                      break;
                      default:   
                         c = toasc(code); /* get in into ascii */
                         if (c != 0)
                             {
                             rs_out[0] = c;
                             DoIO(Write_Request);
                             }
                      break;
                      }
                break;

                case NEWSIZE:
                   emit(12);
                break;

                case MENUPICK:
                   if ( code != MENUNULL )
                       {
                       menunum = MENUNUM( code );
                       itemnum = ITEMNUM( code );
                       switch( menunum )
                             {
                             case 0:
                                switch( itemnum )
                                      {
                                      case 0:
                                         if (capture == TRUE)
                                             {
                                             capture=FALSE;
                                             fclose(tranr);
                                             emits("\nEnd File Capture\n");
                                             }
                                         else
                                             {
                                             emits("\nAscii Capture:");
                                             filename(name);
                                             if ((tranr=fopen(name,"w")) == 0)
                                                 {
                                                 capture=FALSE;
                                                 emits("\nError Opening File\n");
                                                 break;
                                                 }
                                             capture=TRUE;
                                             }
                                      break;
                                      case 1:
                                         if (send == TRUE)
                                             { 
                                             send=FALSE;
                                             fclose(trans);
                                             emits("\nFile Send Cancelled\n");
                                             }
                                         else
                                             {
                                             emits("\nAscii Send:");
                                             filename(name);
                                             if ((trans=fopen(name,"r")) == 0)
                                                 {
                                                 send=FALSE;
                                                 emits("\nError Opening File\n");
                                                 break;
                                                 }
                                             send=TRUE;
                                             }
                                      break;
                                      case 2:
                                         emits("\nXmodem Receive:");
                                         filename(name);
                                         if (XMODEM_Read_File(name))
                                            {
                                            emits("\nRecieved File\n");
                                            emit(8);
                                            }
                                         else
                                            {
                                            close(fd);
                                            emits("Xmodem Receive Failed\n");
                                            emit(8);
                                            }
                                      break;
                                      case 3:
                                         emits("\nXmodem Send:");
                                         filename(name);
                                         if (XMODEM_Send_File(name))
                                             {
                                             emits("\nSent File\n");
                                             emit(8);
                                             }
                                         else
                                             {
                                             close(fd);
                                             emits("\nXmodem Send Failed\n");
                                             emit(8);
                                             }
                                      break;
                                      }
                             break;

                             case 1:
                                AbortIO(Read_Request);
                                switch( itemnum )
                                      {
                                      case 0:
                                         Read_Request->io_Baud = 300;
                                      break;
                                      case 1:
                                         Read_Request->io_Baud = 1200;
                                      break;
                                      case 2:
                                         Read_Request->io_Baud = 2400;
                                      break;
                                      case 3:
                                         Read_Request->io_Baud = 4800;
                                      break;
                                      case 4:
                                         Read_Request->io_Baud = 9600;
                                      break;
                                      }
                                Read_Request->IOSer.io_Command = SDCMD_SETPARAMS;
                                DoIO(Read_Request);
                                Read_Request->IOSer.io_Command = CMD_READ;
                                BeginIO(Read_Request);
                             break;
                             } /* end of switch ( menunum ) */
                       }    /*  end of if ( not null ) */
                }   /* end of switch (class) */
          }   /* end of while ( newmessage )*/
     }  /* end while ( keepgoing ) */

/*   It must be time to quit, so we have to clean
*   up and exit.
*/
       
CloseDevice(Read_Request);
DeletePort(Read_Request->IOSer.io_Message.mn_ReplyPort);
FreeMem(Read_Request,sizeof(*Read_Request));
CloseDevice(Write_Request);
DeletePort(Write_Request->IOSer.io_Message.mn_ReplyPort);
FreeMem(Write_Request,sizeof(*Write_Request));
ClearMenuStrip( mywindow );
CloseWindow( mywindow );
exit(FALSE);
} /* end of main */

/*************************************************
*  function to get file name
*************************************************/
filename(name)
char name[];
{
char c;
ULONG class;
USHORT code;
int keepgoing,i;
keepgoing = TRUE;
i=0;
while (keepgoing) {
      while( NewMessage=(struct IntuiMessage *)GetMsg(mywindow->UserPort) )
           {
           class = NewMessage->Class;
           code = NewMessage->Code;
           ReplyMsg( NewMessage );
           if (class=RAWKEY)
              {
              c = toasc(code);
              name[i]=c;
              if (name[i] != 0)
                 {
                 if (name[i] == 13)
                    {
                    name[i]=0;
                    keepgoing = FALSE;
                    }
                 else
                    {
                    if (name[i] == 8)
                       {
                       i -=2;
                       if (i < -1)
                          i = -1;
                       else
                          {
                          emit(8);
                          emit(32);
                          emit(8);
                          }
                       }
                    else
                       emit(c);
                    }
                 i += 1;
                 }
              }
          } /* end of new message loop */
      }   /* end of god knows what */
      emit(13);
} /* end of function */


/*************************************************
*  function to print a string
*************************************************/
emits(string)
char string[];
{
int i;
char c;

i=0;
while (string[i] != 0)
      {
      c=string[i];
      if (c == 10)
         c = 13;
      emit(c);
      i += 1;
      }
}
/**************************************************************/
/* send char and read char functions for the xmodem function */
/************************************************************/
sendchar(ch)
int ch;
{
rs_out[0] = ch;
DoIO(Write_Request);
}

static unsigned char readchar()
{
unsigned char c;
int rd,ch;

rd = FALSE;

while (rd == FALSE)
      {
      Wait((1 << Read_Request->IOSer.io_Message.mn_ReplyPort->mp_SigBit) | ( 1 << mywindow->UserPort->mp_SigBit));
      if(CheckIO(Read_Request))
        {
        WaitIO(Read_Request);
        ch=rs_in[0];
        rd = TRUE;
        BeginIO(Read_Request);
        }
      if (NewMessage=(struct IntuiMessage *)GetMsg(mywindow->UserPort))
         if ((NewMessage->Class) == RAWKEY)
            if ((NewMessage->Code) == 69)
               {
               emits("\nUser Cancelled Transfer");
               break;
               }
      }
if (rd == FALSE)
   {
   timeout = TRUE;
   emits("\nTimeout Waiting For Character\n");
   }
c = ch;
return(c);
}

/**************************************/
/* xmodem send and recieve functions */
/************************************/

XMODEM_Read_File(file)
    char *file;
{
    int firstchar, sectnum, sectcurr, sectcomp, errors, errorflag;
    unsigned int checksum, j, bufptr;
    unsigned int i = 0;
    char numb[10];
    bytes_xferred = 0L;

    if ((fd = creat(file, 0)) < 0)
        {
        emits("Cannot Open File\n");
        return FALSE;
        }
    else
        emits("Receiving File\n");

    timeout=FALSE;
    sectnum = errors = bufptr = 0;
    sendchar(NAK);
    firstchar = 0;
    while (firstchar != EOT && errors != ERRORMAX)
        {
        errorflag = FALSE;

        do                                    /* get sync char */
           {
           firstchar = readchar();
           if (timeout == TRUE)
              return FALSE;
           }
        while (firstchar != SOH && firstchar != EOT);

        if  (firstchar == SOH)
            {
            emits("Getting Block ");
            sprintf(numb,"%i",sectnum);
//            stci_d(numb,sectnum,i);
            emit(numb);
            emits("...");
            sectcurr = readchar();
            if (timeout == TRUE)
               return FALSE;
            sectcomp = readchar();
            if (timeout == TRUE)
               return FALSE;
            if ((sectcurr + sectcomp) == 255)
                {
                if (sectcurr == (sectnum + 1 & 0xff))
                    {
                    checksum = 0;
                    for (j = bufptr; j < (bufptr + SECSIZ); j++)
                        {
                        bufr[j] = readchar();
                        if (timeout == TRUE)
                           return FALSE;
                        checksum = (checksum + bufr[j]) & 0xff;
                        }
                    if (checksum == readchar())
                        {
                        errors = 0;
                        sectnum++;
                        bufptr += SECSIZ;
                        bytes_xferred += SECSIZ;
                        emits("verified\n");
                        if (bufptr == BufSize)
                            {
                            bufptr = 0;
                            if (write(fd, bufr, BufSize) == EOF)
                                {
                                emits("\nError Writing File\n");
                                return FALSE;
                                };
                            };
                        sendchar(ACK);
                        }
                    else
                        {
                        errorflag = TRUE;
                        if (timeout == TRUE)
                           return FALSE;
                        }
                    }
                else
                    {
                    if (sectcurr == (sectnum & 0xff))
                        {
                        emits("\nReceived Duplicate Sector\n");
                        sendchar(ACK);
                        }
                    else
                        errorflag = TRUE;
                    }
                }
            else
                errorflag = TRUE;
            }
        if (errorflag == TRUE)
            {
            errors++;
            emits("\nError\n");
            sendchar(NAK);
            }
        };        /* end while */
            
    if ((firstchar == EOT) && (errors < ERRORMAX))
        {
        sendchar(ACK);
        write(fd, bufr, bufptr);
        close(fd);
        return TRUE;
        }
    return FALSE;
}

XMODEM_Send_File(file)
    char *file;
{
    int sectnum, bytes_to_send, size, attempts, c;
    int i = 0;
    unsigned checksum, j, bufptr;
    char numb[10];

    timeout=FALSE;
    bytes_xferred = 0;
    if ((fd = open(file, 1)) < 0)
        {
        emits("Cannot Open Send File\n");
        return FALSE;
        }
    else
        emits("Sending File\n");
    attempts = 0;
    sectnum = 1;
/* wait for sync char */
    j=1;
    while (((c = readchar()) != NAK) && (j++ < ERRORMAX));
    if (j >= (ERRORMAX))
        {
        emits("\nReceiver not sending NAKs\n");
        return FALSE;
        };

    while ((bytes_to_send = read(fd, bufr, BufSize)) && attempts != RETRYMAX)
        {
        if (bytes_to_send == EOF)
            {
            emits("\nError Reading File\n");
            return FALSE;
            };

        bufptr = 0;
        while (bytes_to_send > 0 && attempts != RETRYMAX)
            {
            attempts = 0;
            do
                {
                sendchar(SOH);
                sendchar(sectnum);
                sendchar(~sectnum);
                checksum = 0;
                size = SECSIZ <= bytes_to_send ? SECSIZ : bytes_to_send;
                bytes_to_send -= size;
                for (j = bufptr; j < (bufptr + SECSIZ); j++)
                    if (j < (bufptr + size))
                        {
                        sendchar(bufr[j]);
                        checksum += bufr[j];
                        }
                    else
                        {
                        sendchar(0);
                        }
                sendchar(checksum & 0xff);
                attempts++;
                c = readchar();
                if (timeout == TRUE)
                   return FALSE;
                }
            while ((c != ACK) && (attempts != RETRYMAX));
            bufptr += size;
            bytes_xferred += size;
            emits("Block ");
            sprintf(numb, "%i",sectnum);
//            stci_d(numb,sectnum,i);
            emits(numb);
            emits(" sent\n");
            sectnum++;
            }
        }
    close(fd);
    if (attempts == RETRYMAX)
        {
        emits("\nNo Acknowledgment Of Sector, Aborting\n");
        return FALSE;
        }
    else
        {
        attempts = 0;
        do
            {
            sendchar(EOT);
            attempts++;
            }
        while ((readchar() != ACK) && (attempts != RETRYMAX) && (timeout == FALSE));
         if (attempts == RETRYMAX)
            emits("\nNo Acknowledgment Of End Of File\n");
        };
    return TRUE;
}

/*************************************************
*  function to output ascii chars to window
*************************************************/
emit(c)
char c;
{
static short x = 3;
static short y = 17;
short xmax,ymax,cx,cy;

xmax = mywindow->Width;
ymax = mywindow->Height;

/* cursor */
if (x > (xmax-31))
   {
   cx = 9;
   cy = y + 8;
   }
else
  {
  cx = x+8;
  cy = y;
  }
 
if (cy > (ymax-2))
   {
   cx = 9;
   cy -= 8;
   }

SetDrMd(mywindow->RPort,COMPLEMENT);
SetAPen(mywindow->RPort,3);
RectFill(mywindow->RPort,cx-7,cy-6,cx,cy+1);
SetAPen(mywindow->RPort,1);
#warning Changed draw mode to JAM1 (from JAM2)
SetDrMd(mywindow->RPort,JAM1);

if (x > (xmax-31))
   {
   x = 3;
   y += 8;
   }

if (y > (ymax-2))
   {
   x = 3;
   y -= 8;
   }

Move(mywindow->RPort,x,y);

switch( c )
      {
      case '\t':
         x += 60;
         break;
      case '\n':
         break;
      case 13:  /* newline */
         x = 3;
         y += 8;
         break;
      case 8:   /* backspace */
         x -= 8;
         if (x < 3)
            x = 3;
         break;
      case 12:     /* page */
         x = 3;
         y = 17;
         SetAPen(mywindow->RPort,0);
         RectFill(mywindow->RPort,2,10,xmax-19,ymax-7);
         SetAPen(mywindow->RPort,1);
         break;
      case 7:     /* bell */
         ClipBlit(mywindow->RPort,0,0,mywindow->RPort,0,0,xmax,ymax,0x50);
         ClipBlit(mywindow->RPort,0,0,mywindow->RPort,0,0,xmax,ymax,0x50);
         break;
      default:
         Text(mywindow->RPort,&c,1);
         x += 8;
      } /* end of switch */
/* cursor */
if (x > (xmax-31))
   {
   cx = 9;
   cy = y + 8;
   }
else
  {
  cx = x+8;
  cy = y;
  }

if (cy > (ymax-2))
   {
   cx = 9;
   cy -= 8;
   ScrollRaster(mywindow->RPort,0,8,2,10,xmax-20,ymax-2);
   }

SetAPen(mywindow->RPort,3);
RectFill(mywindow->RPort,cx-7,cy-6,cx,cy+1);
SetAPen(mywindow->RPort,1);
}
/*************************************************
*  function to take raw key data and convert it 
*  into ascii chars
**************************************************/
static char toasc(code)
USHORT code;
{
static int ctrl = FALSE;
static int shift = FALSE;
static int capsl = FALSE;
char c;
static char keys[75] = {
'`' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' , '0' , '-' ,
'=' , '\\' , 0 , '0' , 'q' , 'w' , 'e' , 'r' , 't' , 'y' , 'u' , 'i' , 'o' ,
'p' , '[' , ']' , 0 , '1' , '2' , '3' , 'a' , 's' , 'd' , 'f' , 'g' , 'h' ,
'j' , 'k' , 'l' , ';' , '\'' , 0 , 0 , '4' , '5' , '6' , 0 , 'z' , 'x' , 'c' , 'v' ,
'b' , 'n' , 'm' , 44 , '.' , '/' , 0 , '.' , '7' , '8' , '9' , ' ' , 8 ,
'\t' , 13 , 13 , 27 , 127 , 0 , 0 , 0 , '-' } ;

             switch ( code ) /* I didn't know about the Quilifier field when I write this */
                    {
                    case 98:
                       capsl = TRUE;
                       c = 0;
                       break;
                    case 226:
                       capsl = FALSE;
                       c = 0;
                       break;
                    case 99:
                       ctrl = TRUE;
                       c = 0;
                       break;
                    case 227:
                       ctrl = FALSE;
                       c = 0;
                       break;
                    case 96:
                    case 97:
                       shift = TRUE;
                       c = 0;
                       break;
                    case 224:
                    case 225:
                       shift = FALSE;
                       c = 0;
                       break;
                   default:
                       if (code < 75)
                          c = keys[code];
                       else
                          c = 0;
                       }
         
/* add modifiers to the keys */

if (c != 0)
   {

   if (ctrl && (c <= 'z') && (c >= 'a'))
      c -= 96;
   else if (shift)
           {
      if ((c <= 'z') && (c >= 'a'))
         c -= 32;
      else
         switch( c )
               {
          case '[':
             c = '{';
             break;
          case ']':
             c = '}';
             break;
          case '\\':
             c = '|';
             break;
          case '\'':
             c = '\"';
             break;
          case ';':
             c = ':';
             break;
          case '/':
             c = '?';
             break;
          case '.':
             c = '>';
             break;
          case ',':
             c = '<';
             break;
          case '`':
             c = '~';
             break;
          case '=':
             c = '+';
             break;
          case '-':
             c = '_';
             break;
          case '1':
             c = '!';
             break;
          case '2':
             c = '@';
             break;
          case '3':
             c = '#';
             break;
          case '4':
             c = '$';
             break;
          case '5':
             c = '%';
             break;
          case '6':
             c = '^';
             break;
          case '7':
             c = '&';
             break;
          case '8':
             c = '*';
             break;
          case '9':
             c = '(';
             break;
          case '0':
             c = ')';
             break;
          default:
          } /* end switch */
      } /* end shift */
   else if (capsl && (c <= 'z') && (c >= 'a'))
           c -= 32;
   } /* end modifiers */
   return(c);
} /* end of routine */
/* end of file */



