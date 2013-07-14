/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#include "system_headers.h"

/*********************************************************************
                           Definitionen
*********************************************************************/
#define MYLIBVERSION    37
#define MUIVERSION      8

/*********************************************************************
                         Globale Variablen
*********************************************************************/
extern struct  ExecBase *SysBase;

struct Library          *MUIMasterBase;
struct Library          *IdentifyBase;
#if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
#if defined(__GNUC__)
struct Library          *MMUBase;
#else
struct MMUBase          *MMUBase;
#endif
#endif
#if defined(__SASC) || defined(__AROS__) || (defined(__GNUC__) && !defined(__amigaos4__) && !defined(__MORPHOS__))
struct RxsLib           *RexxSysBase;
struct LocaleBase       *LocaleBase;
#else
struct Library          *RexxSysBase;
struct Library          *LocaleBase;
#endif
struct Locale           *currentLocale;

#if defined(__amigaos4__)
struct MUIMasterIFace*  IMUIMaster;
struct GraphicsIFace*   IGraphics;
struct LocaleIFace      *ILocale;
struct RexxSysIFace     *IRexxSys;
struct IdentifyIFace    *IIdentify;
#endif

struct Task             *myprocess;
struct MsgPort          *myarexxport,*ScoutPort;

STRPTR portname;

APTR   globalPool;

CONST_STRPTR decimalSeparator;

BOOL amigaOS4;
BOOL morphOS;
BOOL arOS = FALSE;

/*
**  MUI
*/

APTR AP_Scout;

APTR WI_Main;

/*********************************************************************
                                 Fail
   Diese Routine wird zum Schluß des Programmes aufgerufen!
*********************************************************************/
STATIC void fail1( void )
{
    Forbid();
    if(FindPort("AMITCP") || FindPort("MIAMI.1") || FindPort("TCP/IP Control")) {
        Permit();
        failtcp();
    } else {
        Permit();
    }

    if (globalPool) {
        tbDeletePool(globalPool);
    }

    if (ScoutPort) {
        DeleteMsgPort(ScoutPort);
    }

    ClosescoutCatalog();

    if (RexxSysBase) {
        DROPINTERFACE(IRexxSys);
        CloseLibrary((struct Library *)RexxSysBase);
    }

    CleanupOOP();

#if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
    if (MMUBase) {
        CloseLibrary((struct Library *)MMUBase);
    }
#endif
    if (IdentifyBase) {
        DROPINTERFACE(IIdentify);
        CloseLibrary(IdentifyBase);
    }
    if (GfxBase) {
        DROPINTERFACE(IGraphics);
        CloseLibrary((struct Library *)GfxBase);
    }
    if (LocaleBase) {
        DROPINTERFACE(ILocale);
        CloseLibrary((struct Library *)LocaleBase);
    }
}

STATIC void fail( void )
{
    if (AP_Scout) {
        struct DiskObject *dob = NULL;

#if defined(__AROS__)
        GetAttr(MUIA_Application_DiskObject, AP_Scout, (APTR)&dob);
        /*
            Reason: Error: Variable oder Feld »__zune_val_storage«
            als void deklariert
        */
#else
        get(AP_Scout, MUIA_Application_DiskObject, (APTR)&dob);
#endif
        if (dob) FreeDiskObject(dob);

        MUI_DisposeObject(AP_Scout);
        AP_Scout = NULL;
    }

    if (MUIMasterBase) {
        DeleteCustomClasses();
        DROPINTERFACE(IMUIMaster);
        CloseLibrary(MUIMasterBase);
    }

    fail1();
}

/*********************************************************************
                                 Init
   Diese Routine wird am Beginn des Programmes aufgerufen!
*********************************************************************/
STATIC BOOL init1( void )
{
    if ((GfxBase = (struct GfxBase *)OpenLibrary(GRAPHICSNAME, MYLIBVERSION)) == NULL) {
        return FALSE;
    }
    if (!GETINTERFACE(IGraphics, GfxBase)) {
        return FALSE;
    }

    if ((RexxSysBase = (APTR)MyOpenLibrary(RXSNAME, 0)) == NULL) {
        return FALSE;
    }
    if (!GETINTERFACE(IRexxSys, RexxSysBase)) {
        return FALSE;
    }

    IdentifyBase = OpenLibrary("identify.library", IDENTIFYVERSION);
    if (GETINTERFACE(IIdentify, IdentifyBase)) {
        // just to keep SAS/C happy
    }

#if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
    MMUBase = (APTR)OpenLibrary(MMU_NAME, 43);
#endif

    if (!InitOOP())
        return FALSE;

    decimalSeparator = ","; // english separator for 1000s
    if ((LocaleBase = (APTR)OpenLibrary("locale.library", MYLIBVERSION)) != NULL) {
        if (GETINTERFACE(ILocale, LocaleBase)) {
            if ((currentLocale = OpenLocale(NULL)) != NULL) {
                decimalSeparator = currentLocale->loc_GroupSeparator;
            }
        }
    }

    OpenscoutCatalog();

    myprocess = FindTask(NULL);

    Forbid();
    morphOS = (FindResident("MorphOS") != NULL);
    arOS = (FindResident("kernel.resource") != NULL);
    Permit();
    amigaOS4 = !morphOS && !arOS && (SysBase->LibNode.lib_Version >= 50);

    if ((ScoutPort = CreateMsgPort()) == NULL) {
        return FALSE;
    }

    if ((globalPool = tbCreatePool(MEMF_CLEAR, 4096, 4096)) == NULL) {
        return FALSE;
    }

    return TRUE;
}

STATIC BOOL init2( void )
{
    if ((MUIMasterBase = MyOpenLibrary(MUIMASTER_NAME, MUIVERSION)) == NULL) {
        return FALSE;
    }
    if (!GETINTERFACE(IMUIMaster, MUIMasterBase)) {
        return FALSE;
    }

    if (!CheckMCCVersion("NList.mcc", 20, 106, TRUE)) {
        return FALSE;
    }
    if (!CheckMCCVersion("NListview.mcc", 19, 63, TRUE)) {
        return FALSE;
    }
    if (!CheckMCCVersion("NListtree.mcc", 18, 13, TRUE)) {
        return FALSE;
    }

    if (!CreateCustomClasses()) {
        return FALSE;
    }

    return TRUE;
}

/*********************************************************************
 BEGIN:                        MAIN
*********************************************************************/

ULONG scout_main( void )
{
    BOOL RETURN_FLAG = FALSE;
    ULONG RETURN_CODE = RETURN_OK;

    if (!init1()) {
        fail1();
        return RETURN_FAIL;
    }

    if ((opts.User || opts.Password) && (!opts.Host)) {
        PutStr(msgNoHostNameSpecified);
        RETURN_CODE = RETURN_FAIL;
        RETURN_FLAG = TRUE;
    } else if(opts.Host) {
        Forbid();
        if (FindPort("AMITCP") || FindPort("MIAMI.1") || FindPort("TCP/IP Control")) {
            Permit();

            if (opts.Command) {
                RETURN_CODE = netshellclient();
                RETURN_FLAG = TRUE;
            } else if (!(clientstate = ConnectToServer())) {
                RETURN_CODE = RETURN_FAIL;
                RETURN_FLAG = TRUE;
            }
        } else {
            Permit();

            PutStr(msgStartTCPIPStack);
            RETURN_CODE = RETURN_FAIL;
            RETURN_FLAG = TRUE;
        }
    } else if (opts.Command) {
        shellstate = TRUE;
        RETURN_CODE = ExecuteCommand(opts.Command);
        RETURN_FLAG = TRUE;
    } else {
        Forbid();
        if ((FindPort("AMITCP") || FindPort("MIAMI.1") || FindPort("TCP/IP Control")) && isNetCall()) {
            Permit();
            RETURN_CODE = netdaemon();
            RETURN_FLAG = TRUE;
        } else {
            Permit();
        }
    }

    if (RETURN_FLAG) {
        fail1();
        return RETURN_CODE;
    }

    if (!init2()) {
        fail();
        return RETURN_FAIL;
    }

    stccpy(updatetimetext, (opts.IntervalTime) ? opts.IntervalTime : (STRPTR)"1.0", sizeof(updatetimetext));

    if (!GetApplication()) {
        PutStr(msgCantCreateApplication);
        fail();
        return RETURN_FAIL;
    }

    Forbid();
    if ((portname = FindMyARexxPort("SCOUT")) != NULL) {
        myarexxport = FindPort(portname);
    } else {
        portname = (STRPTR)" < ERROR > ";
        myarexxport = NULL;
    }

    if ((opts.PortName) && (myarexxport)) {
        portname = opts.PortName;
        myarexxport->mp_Node.ln_Name = opts.PortName;
    }
    Permit();

    if (opts.Iconified) set(AP_Scout, MUIA_Application_Iconified, TRUE);

/*
** Everything's ready, lets launch the application. We will
** open the master window now at this old position.
*/

    DoMethod(AP_Scout, MUIM_Application_Load, MUIV_Application_Load_ENV);

    set(WI_Main, MUIA_Window_Open, TRUE);

/*
** Now the ARexx startup script will be started. Herewith it's
** possible to open window at the beginning.
*/

    if (opts.Startup) {
        BPTR startuplock;

        if ((startuplock = Lock(opts.Startup, SHARED_LOCK)) != ZERO) {
            UnLock (startuplock);

            if (!SendStartupMsg("REXX", opts.Startup, TRUE)) {
                Printf(msgCantSendARexxScript, opts.Startup, "REXX");
            }
        } else {
            if (!SendStartupMsg(myarexxport->mp_Node.ln_Name, opts.Startup, FALSE)) {
                Printf(msgCantSendARexxScript, opts.Startup, myarexxport->mp_Node.ln_Name);
            }
        }
    }

/*
** This is the main loop. As you can see, it does just nothing.
** Everything is handled by MUI, no work for the programmer.
*/

    {
        ULONG sigs = 0;

        while ((LONG)DoMethod(AP_Scout, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit) {
            if (sigs) {
                sigs = Wait(sigs | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_F);

                if (FLAG_IS_SET(sigs, SIGBREAKF_CTRL_C)) {
                    PutStr(msgBreak);
                    break;
                }
                if (FLAG_IS_SET(sigs, SIGBREAKF_CTRL_F)) {
                    set(AP_Scout, MUIA_Application_Iconified, FALSE);
                }
            }
        }
    }

    if (opts.Host) {
        SendDaemon(CMD_END);
    }

    // not just close the just, but trigger the CloseSubWindows method, too
    set(WI_Main, MUIA_Window_CloseRequest, TRUE);

    fail();
    return RETURN_OK;
}

/*********************************************************************
 END:                           MAIN
*********************************************************************/
