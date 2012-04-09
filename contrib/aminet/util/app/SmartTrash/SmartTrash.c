//************************
//
// Name : SmartTrash
//
// Synopsis : An AppIcon trashcan, that moves files to the trashcan in the
//            root directory, creating one if necessary
//
// Usage : (Double click)
//
// Vers  : Date       : Ini : Comment
// 00.50 : 14.09.1995 : tst : First Version
// 01.00 : 16.09.1995 : tst : Fixed DRAWER and DISK bugs, added TOOLTYPES
// 02.00 : 18.09.1995 : tst : Added ToolTypes
// 02.30 : 20.09.1995 : tst : Modified to use a PATH
//
//************************


//************************ HEADER FILES



#include <exec/types.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <exec/libraries.h>

#if defined(__AROS__)

#define DEBUG 1
#include <aros/debug.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/workbench.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#else
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>

#include <clib/intuition_protos.h>
#include <clib/icon_protos.h>
#include <clib/wb_protos.h>
#endif

//#include <stdio.h> // using DOS now...
#include <string.h>
#include <stdlib.h>

#include "Do_Trash.h" // see note at end


//************************ FORWARD DECLARATIONS

BOOL Do_About(void);
struct DiskObject *GetAppIcon(int argc, char **argv);
BOOL Do_Trash(struct WBArg *WBArg);


//************************ DEFINITIONS

#ifdef LATTICE
int CHKBRK(void) { return(0); }
int chkabort(void) { return(0); }
#endif

#define VERSION  "2.3"
#define DATE     "(20.9.95)"
static  UBYTE VersTag[]="$VER: SmartTrash v" VERSION " " DATE "\n";

#define BUFSIZE 128
#define FNAMESIZE 32

#define DEFICON "WasteBasket"
#define DEFMENU "Trash"
#define DEFPATH ":Trashcan;:T"

#ifdef DEBUG
#if defined(__AROS__)
#define DEBUGMSG(S,V)	D(bug("[SmartTrash]" S,V))
#else
#define DEBUGMSG(S,V)	if(DebugIO)FPrintf(DebugIO,S,V)
#endif
#else
#define DEBUGMSG(S,V)	
#endif


//************************ STORAGE

//extern struct Library *SysBase;
struct Library *IconBase = NULL;
struct Library *WorkbenchBase=NULL;
#if defined(__AROS__)
struct IntuitionBase *IntuitionBase = NULL;
#else
struct Library *IntuitionBase = NULL;
#endif
char TrashPath[BUFSIZE];
char AppIconName[BUFSIZE];
char AppMenuItem[BUFSIZE];
ULONG apps=0;

#ifdef DEBUG
BPTR DebugIO=NULL;
#endif


//************************ MAIN

void main(int argc, char **argv) {
  struct DiskObject   *dobj;
  struct MsgPort      *myport;
  struct AppMessage   *appmsg;
  struct AppIcon     *appicon=NULL;
  struct AppMenuItem *appmenu=NULL;
  struct WBArg         *WBArg;
  BOOL quit;
  ULONG x;


  // Get the right version of the icon library, initialise IconBase
  if (IconBase = OpenLibrary("icon.library",37)) {
    // Get the right version of the Workbench library
    if (WorkbenchBase=OpenLibrary("workbench.library",37)) {
      if (IntuitionBase=OpenLibrary("intuition.library", 37)) {

        if (dobj=GetAppIcon(argc, argv)) {
          // The CreateMsgPort function is v37 and later
          if (myport=CreateMsgPort()) {
            // Put the appicon and menu up on the Workbench window
            if (apps<2) (appicon=AddAppIconA(0L,0L,AppIconName,myport,NULL,dobj,NULL));
            if (apps>0) (appmenu=AddAppMenuItemA(0L,0L,AppMenuItem,myport,NULL));

            if ( (appicon) || (appmenu) ) {
              for ( quit=FALSE ; !quit ; ) {
                WaitPort(myport);
            
                // Might be more than one at a time...
                while(appmsg=(struct AppMessage *)GetMsg(myport)) {
		    bug("[SmartTrash] %s: AppMessage @ 0x%p\n", __PRETTY_FUNCTION__, appmsg);
                  if (appmsg->am_NumArgs==0L) {
                    quit=Do_About();
                  } else if (appmsg->am_NumArgs>0L) {
                    WBArg=appmsg->am_ArgList;
                    for (x=0; x<appmsg->am_NumArgs; x++) {
                      Do_Trash(WBArg++);
                    } // for
                  } // if
                  // Let workbench know we're done with the message
                  ReplyMsg((struct Message *)appmsg);
                } // while GetMsg
              } // while !quit
            } // if AddApps
            if (appicon) RemoveAppIcon(appicon);
            if (appmenu) RemoveAppMenuItem(appmenu);

            // Clear away any messages that arrived at the last moment
            while (appmsg=(struct AppMessage *)GetMsg(myport)) ReplyMsg((struct Message *)appmsg);
            DeleteMsgPort(myport);
          } // if CreateMsgPort
          FreeDiskObject(dobj);
        } // if GetAppIcon
        CloseLibrary(IntuitionBase);
      } // if Open Intuition Library
      CloseLibrary(WorkbenchBase);
    } // if Open Workbench Library
    CloseLibrary(IconBase);
  } // if Open Icon Library
#ifdef DEBUG
  if (DebugIO) Close(DebugIO);
#endif
} // main


//************************ FUNCTIONS


struct DiskObject *GetAppIcon(int argc, char **argv) {
  BPTR oldp;
  char AppIconFile[BUFSIZE];
  LONG AppIconXPos=NO_ICON_POSITION;
  LONG AppIconYPos=NO_ICON_POSITION;
  struct DiskObject   *dobj;

  strcpy(TrashPath  ,DEFPATH);
  strcpy(AppIconName,DEFICON);
  strcpy(AppMenuItem,DEFMENU);
  AppIconFile[0]=0;


  if (argc==0) { // from Workbench
    struct WBStartup *wbs=(struct WBStartup *)argv;
    char **toolArray;
    char *value;

    // move to program dir, and try to get the icon
    oldp=CurrentDir(wbs->sm_ArgList->wa_Lock);
    if (dobj=GetDiskObject(wbs->sm_ArgList->wa_Name)) {
       strncpy(AppIconName,wbs->sm_ArgList->wa_Name,BUFSIZE);
       toolArray=dobj->do_ToolTypes;

#ifdef DEBUG
       if (value=FindToolType(toolArray,"DEBUG")) {
         DebugIO=Open(value,MODE_NEWFILE);
       } // if find DEBUG
#endif

       if (value=FindToolType(toolArray,"LEFTEDGE")) {
         StrToLong(value,&AppIconXPos);
         DEBUGMSG("ToolType LEFTEDGE=%ld\n",(ULONG)AppIconXPos);
       } // if find XPOS

       if (value=FindToolType(toolArray,"TOPEDGE")) {
         StrToLong(value,&AppIconYPos);
         DEBUGMSG("ToolType TOPEDGE =%ld\n",(ULONG)AppIconYPos);
       } // if find YPOS

       if (value=FindToolType(toolArray,"ICONNAME")) {
         strncpy(AppIconName,value,BUFSIZE);
         DEBUGMSG("ToolType ICONNAME=%s\n",(ULONG)&AppIconName);
       } // if find NAME

       if (value=FindToolType(toolArray,"ICONFILE")) {
         strncpy(AppIconFile,value,BUFSIZE);
         DEBUGMSG("ToolType ICONFILE=%s\n",(ULONG)&AppIconFile);
       } // if find ICON

       if (value=FindToolType(toolArray,"MENUITEM")) {
         strncpy(AppMenuItem,value,BUFSIZE);
         if (apps==0) apps=1;
         DEBUGMSG("ToolType MENUITEM=%s\n",(ULONG)&AppMenuItem);
       } // if find MENUITEM

       if (value=FindToolType(toolArray,"MENUONLY")) {
         if ((apps==1) && (MatchToolValue(value,"TRUE"))) {
           apps=2;
           DEBUGMSG("ToolType MENUONLY=TRUE\n",NULL);
         } else {
           DEBUGMSG("ToolType MENUONLY=TRUE (FAILED - NO MENU ITEM)\n",NULL);
         }
       } // if find MENUONLY

       if (value=FindToolType(toolArray,"TRASHPATH")) {
         strncpy(TrashPath,value,BUFSIZE);
         DEBUGMSG("ToolType TRASHPATH=%s\n",(ULONG)&TrashPath);
       } // if find TRASHPATH
    } // if GetDiskObject
  } // if from workbench

  // if user has requested a different icon, get that
  if (AppIconFile[0]) {
    if (dobj) FreeDiskObject(dobj);
    dobj=GetDiskObject(AppIconFile);
  }

  // if no icon, then use default WBGARBAGE
  if (NULL==dobj) {
    dobj=GetDefDiskObject(WBGARBAGE);
    DEBUGMSG("Using Default Garbage icon\n",NULL);
  }

  // Position the icon
  dobj->do_CurrentX=AppIconXPos;
  dobj->do_CurrentY=AppIconYPos;

  if (oldp) CurrentDir(oldp);
  return dobj;
} // GetAppIcon


//** Do_About is called when the user double clicks on the icon
//** and displays a message in a requester
BOOL Do_About(void) {
  struct EasyStruct ez = {
    sizeof(struct EasyStruct),
    0,
    "SmartTrash",
    "SmartTrash version " VERSION "\n"
    "The Smart Trashcan AppIcon\n\n"
    "Copyright © 1995 Tak Tang\n"
    "All Rights Reserved."
    ,"Quit|Hide"
  };
  DEBUGMSG("Displaying ABOUT\n",NULL);
  return ((BOOL)EasyRequest(NULL, &ez, NULL));
} // Do_About


//************************ DO_TRASH

BOOL Do_Trash(struct WBArg *WBArg) {
  struct FileInfoBlock fib;
  struct DiskObject *dobj;
  char buffer[BUFSIZE];
  char *name;
  BPTR oldp, parent, trash;
  BOOL retVal=FALSE;

  if (NULL==WBArg->wa_Lock) {
    DEBUGMSG("No lock - an appicon?\n",NULL);
    return FALSE;
  }

  name=WBArg->wa_Name;
  if (0==name[0]) {
    if (NULL==Examine(WBArg->wa_Lock,&fib)) {
      DEBUGMSG("Hmmm... a LOCK, but no name, and cant EXAMINE()it?\n",NULL);
      return FALSE;
    }
    parent=ParentDir(WBArg->wa_Lock);
    if (NULL==parent) {
      DEBUGMSG("Object is a Disk named '%s' - cannot move.\n",(ULONG)&fib.fib_FileName);
      return FALSE;
    }
    name=(char *)&fib.fib_FileName;
    DEBUGMSG("Name of DIR  is '%s'.\n",(ULONG)name);
  } else {
    parent=DupLock(WBArg->wa_Lock);
    DEBUGMSG("Name of FILE is '%s'.\n",(ULONG)name);
  }

  oldp=CurrentDir(parent);

  trash=FigureTrash(buffer); // this must come AFTER the CurrentDir
  if (NULL==trash) {
    DEBUGMSG("Eeek!  Cant access any named trashcans - aborting!\n",NULL);
    UnLock(parent);
    CurrentDir(oldp);
    return FALSE;
  }

  FindSlot(buffer, name, trash);
  DEBUGMSG("Target name of Object is '%s'.\n",(ULONG)&buffer);

  if (dobj=GetDiskObject(name)) {
    if (NULL==DeleteDiskObject(name)) {
      DEBUGMSG("Eeek!  Cant delete icon file.\n",NULL);
    } else {
      if ( (FALSE==ExistsFile(name)) || (Rename(name,buffer)) ) {
        dobj->do_CurrentX=NO_ICON_POSITION;
        dobj->do_CurrentY=NO_ICON_POSITION;
        PutDiskObject(buffer,dobj);
        retVal=TRUE;
      } else {
        DEBUGMSG("Move FAILED, replacing icon.\n",NULL);
        PutDiskObject(name,dobj);
      }
    } // if DeleteDiskObject
    FreeDiskObject(dobj);
  } else {
    if ( (FALSE==ExistsFile(name)) || (Rename(name,buffer)) ) {
      DeleteDiskObject(name); // get rid of icon if there is on SHOW ALL mode
      retVal=TRUE;
    } else {
      DEBUGMSG("Rename failed.\n",NULL);
    }
  } // if GetDiskObject

  UnLock(parent);
  UnLock(trash);
  CurrentDir(oldp);
  return retVal;
} // Do_Trash

//************************

// This has been broken out into a seperate file, because I'm thinking of
// makeing a version which doenst use an appicon (i.e. you shift click it)
// and a CLI based version, so this stuff will be shared.  I guess I
// could compile this seperately and link it...

#include "Do_Trash.c"

//************************ END OF FILE
