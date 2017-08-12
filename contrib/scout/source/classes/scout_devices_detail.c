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

struct DevicesDetailWinData {
    TEXT ddwd_Title[WINDOW_TITLE_LENGTH];
    APTR ddwd_Texts[14];
    APTR ddwd_NSDList;
    struct DeviceEntry *ddwd_Device;
    APTR ddwd_MainGroup;
};

STATIC CONST struct LongFlag devFlags[] = {
    { LIBF_SUMMING,   "LIBF_SUMMING" },
    { LIBF_CHANGED,   "LIBF_CHANGED" },
    { LIBF_SUMUSED,   "LIBF_SUMUSED" },
    { LIBF_DELEXP,    "LIBF_DELEXP" },
#if defined(__MORPHOS__)
    { LIBF_RAMLIB,    "LIBF_RAMLIB" },
    { LIBF_QUERYINFO, "LIBF_QUERYINFO" },
#elif defined(__amigaos4__)
    { LIBF_EXP0CNT,   "LIBF_EXP0CNT" },
    { 0x20,           "< ??? >" },
#else
    { 0x10,           "< ??? >" },
    { 0x20,           "< ??? >" },
#endif
    { 0x40,           "< ??? >" },
    { 0x80,           "< ??? >" },
    { 0,              NULL },
};

struct NSDCommandEntry {
    UWORD nce_Number;
    TEXT nce_DecNumber[16];
    TEXT nce_HexNumber[16];
    TEXT nce_CmdName[64];
};

HOOKPROTONHNO(nsdlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct NSDCommandEntry));
}
MakeStaticHook(nsdlist_con2hook, nsdlist_con2func);

HOOKPROTONHNO(nsdlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(nsdlist_des2hook, nsdlist_des2func);

HOOKPROTONHNO(nsdlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct NSDCommandEntry *nce = (struct NSDCommandEntry *)msg->entry;

    if (nce) {
        msg->strings[0] = nce->nce_DecNumber;
        msg->strings[1] = nce->nce_HexNumber;
        msg->strings[2] = nce->nce_CmdName;
    } else {
        msg->strings[0] = txtDeviceNSDCmdNumberDec;
        msg->strings[1] = txtDeviceNSDCmdNumberHex;
        msg->strings[2] = txtDeviceNSDCmdName;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
    }
}

MakeStaticHook(nsdlist_dsp2hook, nsdlist_dsp2func);

STATIC LONG nsdlist_cmp2colfunc( struct NSDCommandEntry *nce1,
                                 struct NSDCommandEntry *nce2,
                                 ULONG column )
{
    switch (column) {
        default:
        case 0:
        case 1: return nce1->nce_Number - nce2->nce_Number;
        case 2: return stricmp(nce1->nce_CmdName, nce2->nce_CmdName);
    }
}

HOOKPROTONHNO(nsdlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct NSDCommandEntry *nce1, *nce2;
    ULONG col1, col2;

    nce1 = (struct NSDCommandEntry *)msg->entry1;
    nce2 = (struct NSDCommandEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = nsdlist_cmp2colfunc(nce2, nce1, col1);
    } else {
        cmp = nsdlist_cmp2colfunc(nce1, nce2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = nsdlist_cmp2colfunc(nce2, nce1, col2);
    } else {
        cmp = nsdlist_cmp2colfunc(nce1, nce2, col2);
    }

    return cmp;
}

MakeStaticHook(nsdlist_cmp2hook, nsdlist_cmp2func);

STATIC LONG DoNSDQuery( struct IOStdReq *io,
                        struct NSDeviceQueryResult *nqr )
{
    nqr->DevQueryFormat = 0;
    nqr->SizeAvailable = 0;

    io->io_Command = NSCMD_DEVICEQUERY;
    io->io_Data = nqr;
    io->io_Length = sizeof(*nqr);

    return DoIO((struct IORequest *)io);
}

STATIC LONG NSDQuery( STRPTR dev,
                      ULONG unit,
                      struct NSDeviceQueryResult *nqr )
{
    LONG result = IOERR_OPENFAIL;
    struct MsgPort *mp;

    if (strstr(dev, ".handler"))
    {
        /*
            On AROS handlers appear in the device list.
            Doing OpenDevice on a handler causes troubles
        */
        return IOERR_OPENFAIL;
    }

    if ((mp = CreateMsgPort()) != NULL) {
        struct IOStdReq *io;

        if ((io = (struct IOStdReq *)CreateIORequest(mp, sizeof(struct IOStdReq) + 128)) != NULL) {
            if (strcmp(AHINAME, dev) == 0) {
                // ahi.device needs special treatment
                ((struct AHIRequest *)io)->ahir_Version = 4;
            }

            if (OpenDevice(dev, unit, (struct IORequest *)io, 0) == 0) {
            #if defined(__amigaos4__)
                // AmigaOS4's usbsys.device needs even more special treatment
                if (strcmp("usbsys.device", dev) == 0) {
                    struct Device *USBSysBase = io->io_Device;
                    struct USBSysIFace *IUSBSys;

                    if (GETINTERFACE(IUSBSys, USBSysBase)) {
                        struct USBIOReq *usbio;

                        if ((usbio = USBAllocRequestA((struct IORequest *)io, NULL)) != NULL) {
                            result = DoNSDQuery((struct IOStdReq *)usbio, nqr);

                            USBFreeRequest(usbio);
                        }

                        DROPINTERFACE(IUSBSys);
                    }
                } else {
            #endif

                result = DoNSDQuery(io, nqr);

            #if defined(__amigaos4__)
                }
            #endif
                CloseDevice((struct IORequest *)io);
            }

            DeleteIORequest((struct IORequest *)io);
        }

        DeleteMsgPort(mp);
    }

    return result;
}

STATIC CONST_STRPTR NSDTypeToCmdName( ULONG nsdType,
                                      UWORD cmd )
{
    CONST_STRPTR result;

    switch (cmd) {
        case CMD_INVALID      : result = "CMD_INVALID";       break;
        case CMD_RESET        : result = "CMD_RESET";         break;
        case CMD_READ         : result = "CMD_READ";          break;
        case CMD_WRITE        : result = "CMD_WRITE";         break;
        case CMD_UPDATE       : result = "CMD_UPDATE";        break;
        case CMD_CLEAR        : result = "CMD_CLEAR";         break;
        case CMD_STOP         : result = "CMD_STOP";          break;
        case CMD_START        : result = "CMD_START";         break;
        case CMD_FLUSH        : result = "CMD_FLUSH";         break;
        case NSCMD_DEVICEQUERY: result = "NSCMD_DEVICEQUERY"; break;
        default               : result = NULL;                break;
    }

    if (result) return result;

    switch (nsdType) {
        case NSDEVTYPE_GAMEPORT:
            switch (cmd) {
                case GPD_READEVENT : result = "GPD_READEVENT";                 break;
                case GPD_ASKCTYPE  : result = "GPD_ASKCTYPE";                  break;
                case GPD_SETCTYPE  : result = "GPD_SETCTYPE";                  break;
                case GPD_ASKTRIGGER: result = "GPD_ASKTRIGGER";                break;
                case GPD_SETTRIGGER: result = "GPD_SETTRIGGER";                break;
                default            : result = txtDeviceUnknownGameportCommand; break;
            }
            break;

        case NSDEVTYPE_TIMER:
            switch (cmd) {
                case TR_ADDREQUEST: result = "TR_ADDREQUEST";              break;
                case TR_GETSYSTIME: result = "TR_GETSYSTIME";              break;
                case TR_SETSYSTIME: result = "TR_SETSYSTIME";              break;
                default           : result = txtDeviceUnknownTimerCommand; break;
            }
            break;

        case NSDEVTYPE_KEYBOARD:
            switch (cmd) {
                case KBD_READEVENT       : result = "KBD_READEVENT";                 break;
                case KBD_READMATRIX      : result = "KBD_READMATRIX";                break;
                case KBD_ADDRESETHANDLER : result = "KBD_ADDRESETHANDLER";           break;
                case KBD_REMRESETHANDLER : result = "KBD_REMRESETHANDLER";           break;
                case KBD_RESETHANDLERDONE: result = "KBD_RESETHANDLERDONE";          break;
                default                  : result = txtDeviceUnknownKeyboardCommand; break;
            }
            break;

        case NSDEVTYPE_INPUT:
            switch (cmd) {
                case IND_ADDHANDLER        : result = "IND_ADDHANDLER";             break;
                case IND_REMHANDLER        : result = "IND_REMHANDLER";             break;
                case IND_WRITEEVENT        : result = "IND_WRITEEVENT";             break;
                case IND_SETTHRESH         : result = "IND_SETTHRESH";              break;
                case IND_SETPERIOD         : result = "IND_SETPERIOD";              break;
                case IND_SETMPORT          : result = "IND_SETMPORT";               break;
                case IND_SETMTYPE          : result = "IND_SETMTYPE";               break;
                case IND_SETMTRIG          : result = "IND_SETMTRIG";               break;
            #if defined(__amigaos4__)
                case IND_SETMDEVICE        : result = "IND_SETMDEVICE";             break;
                case IND_SETKDEVICE        : result = "IND_SETKDEVICE";             break;
                case IND_GETMDEVICE        : result = "IND_GETMDEVICE";             break;
                case IND_GETKDEVICE        : result = "IND_GETKDEVICE";             break;
                case IND_ADDNOTIFY         : result = "IND_ADDNOTIFY";              break;
                case IND_IMMEDIATEADDNOTIFY: result = "IND_IMMEDIATEADDNOTIFY";     break;
                case IND_REMOVENOTIFY      : result = "IND_REMOVENOTIFY";           break;
                case IND_ADDEVENT          : result = "IND_ADDEVENT";               break;
                case IND_GETTHRESH         : result = "IND_GETTHRESH";              break;
                case IND_GETPERIOD         : result = "IND_GETPERIOD";              break;
                case IND_GETHANDLERLIST    : result = "IND_GETHANDLERLIST";         break;
            #endif
                default                    : result = txtDeviceUnknownInputCommand; break;
            }
            break;

        case NSDEVTYPE_TRACKDISK:
            switch (cmd) {
                case TD_MOTOR          : result = "TD_MOTOR";                       break;
                case TD_SEEK           : result = "TD_SEEK";                        break;
                case TD_FORMAT         : result = "TD_FORMAT";                      break;
                case TD_REMOVE         : result = "TD_REMOVE";                      break;
                case TD_CHANGENUM      : result = "TD_CHANGENUM";                   break;
                case TD_CHANGESTATE    : result = "TD_CHANGESTATE";                 break;
                case TD_PROTSTATUS     : result = "TD_PROTSTATUS";                  break;
                case TD_RAWREAD        : result = "TD_RAWREAD";                     break;
                case TD_RAWWRITE       : result = "TD_RAWWRITE";                    break;
                case TD_GETDRIVETYPE   : result = "TD_GETDRIVETYPE";                break;
                case TD_GETNUMTRACKS   : result = "TD_GETNUMTRACKS";                break;
                case TD_ADDCHANGEINT   : result = "TD_ADDCHANGEINT";                break;
                case TD_REMCHANGEINT   : result = "TD_REMCHANGEINT";                break;
                case TD_GETGEOMETRY    : result = "TD_GETGEOMETRY";                 break;
                case TD_EJECT          : result = "TD_EJECT";                       break;
                case TD_READ64         : result = "TD_READ64";                      break;
                case TD_WRITE64        : result = "TD_WRITE64";                     break;
                case TD_SEEK64         : result = "TD_SEEK64";                      break;
                case TD_FORMAT64       : result = "TD_FORMAT64";                    break;
                case HD_SCSICMD        : result = "HD_SCSICMD";                     break;
                case ETD_WRITE         : result = "ETD_WRITE";                      break;
                case ETD_READ          : result = "ETD_READ";                       break;
                case ETD_MOTOR         : result = "ETD_MOTOR";                      break;
                case ETD_SEEK          : result = "ETD_SEEK";                       break;
                case ETD_FORMAT        : result = "ETD_FORMAT";                     break;
                case ETD_UPDATE        : result = "ETD_UPDATE";                     break;
                case ETD_CLEAR         : result = "ETD_CLEAR";                      break;
                case ETD_RAWREAD       : result = "ETD_RAWREAD";                    break;
                case ETD_RAWWRITE      : result = "ETD_RAWWRITE";                   break;
                case NSCMD_TD_READ64   : result = "NSCMD_TD_READ64";                break;
                case NSCMD_TD_WRITE64  : result = "NSCMD_TD_WRITE64";               break;
                case NSCMD_TD_SEEK64   : result = "NSCMD_TD_SEEK64";                break;
                case NSCMD_TD_FORMAT64 : result = "NSCMD_TD_FORMAT64";              break;
                case NSCMD_ETD_READ64  : result = "NSCMD_ETD_READ64";               break;
                case NSCMD_ETD_WRITE64 : result = "NSCMD_ETD_WRITE64";              break;
                case NSCMD_ETD_SEEK64  : result = "NSCMD_ETD_SEEK64";               break;
                case NSCMD_ETD_FORMAT64: result = "NSCMD_ETD_FORMAT64";             break;
#if !defined(__AROS__)
                case CD_INFO           : result = "CD_INFO";                        break;
                case CD_CONFIG         : result = "CD_CONFIG";                      break;
                case CD_TOCMSF         : result = "CD_TOCMSF";                      break;
                case CD_TOCLSN         : result = "CD_TOCLSN";                      break;
                case CD_READXL         : result = "CD_READXL";                      break;
                case CD_PLAYTRACK      : result = "CD_PLAYTRACK";                   break;
                case CD_PLAYMSF        : result = "CD_PLAYMSF";                     break;
                case CD_PLAYLSN        : result = "CD_PLAYLSN";                     break;
                case CD_PAUSE          : result = "CD_PAUSE";                       break;
                case CD_SEARCH         : result = "CD_SEARCH";                      break;
                case CD_QCODEMSF       : result = "CD_QCODEMSF";                    break;
                case CD_QCODELSN       : result = "CD_QCODELSN";                    break;
                case CD_ATTENUATE      : result = "CD_ATTENUATE";                   break;
                case CD_ADDFRAMEINT    : result = "CD_ADDFRAMEINT";                 break;
                case CD_REMFRAMEINT    : result = "CD_REMFRAMEINT";                 break;
#endif
                default                : result = txtDeviceUnknownTrackdiskCommand; break;
            }
            break;

        case NSDEVTYPE_CONSOLE:
            switch (cmd) {
                case CD_ASKKEYMAP         : result = "CD_ASKKEYMAP";                 break;
                case CD_SETKEYMAP         : result = "CD_SETKEYMAP";                 break;
                case CD_ASKDEFAULTKEYMAP  : result = "CD_ASKDEFAULTKEYMAP";          break;
                case CD_SETDEFAULTKEYMAP  : result = "CD_SETDEFAULTKEYMAP";          break;
            #if defined(__amigaos4__)
                case CD_ASKKEYMAP_POINTERS: result = "CD_ASKKEYMAP_POINTERS";        break;
                case CD_SETKEYMAP_POINTERS: result = "CD_SETKEYMAP_POINTERS";        break;
            #endif
                default                   : result = txtDeviceUnknownConsoleCommand; break;
            }
            break;

        case NSDEVTYPE_SANA2:
            switch (cmd) {
                case S2_DEVICEQUERY           : result = "S2_DEVICEQUERY";             break;
                case S2_GETSTATIONADDRESS     : result = "S2_GETSTATIONADDRESS";       break;
                case S2_CONFIGINTERFACE       : result = "S2_CONFIGINTERFACE";         break;
                case S2_ADDMULTICASTADDRESS   : result = "S2_ADDMULTICASTADDRESS";     break;
                case S2_DELMULTICASTADDRESS   : result = "S2_DELMULTICASTADDRESS";     break;
                case S2_MULTICAST             : result = "S2_MULTICAST";               break;
                case S2_BROADCAST             : result = "S2_BROADCAST";               break;
                case S2_TRACKTYPE             : result = "S2_TRACKTYPE";               break;
                case S2_UNTRACKTYPE           : result = "S2_UNTRACKTYPE";             break;
                case S2_GETTYPESTATS          : result = "S2_GETTYPESTATS";            break;
                case S2_GETSPECIALSTATS       : result = "S2_GETSPECIALSTATS";         break;
                case S2_GETGLOBALSTATS        : result = "S2_GETGLOBALSTATS";          break;
                case S2_ONEVENT               : result = "S2_ONEVENT";                 break;
                case S2_READORPHAN            : result = "S2_READORPHAN";              break;
                case S2_ONLINE                : result = "S2_ONLINE";                  break;
                case S2_OFFLINE               : result = "S2_OFFLINE";                 break;
                case S2_ADDMULTICASTADDRESSES : result = "S2_ADDMULTICASTADDRESSES";   break;
                case S2_DELMULTICASTADDRESSES : result = "S2_DELMULTICASTADDRESSES";   break;
                case S2_GETPEERADDRESS        : result = "S2_GETPEERADDRESS";          break;
                case S2_GETDNSADDRESS         : result = "S2_GETDNSADDRESS";           break;
                case S2_GETEXTENDEDGLOBALSTATS: result = "S2_GETEXTENDEDGLOBALSTATS";  break;
                case S2_CONNECT               : result = "S2_CONNECT";                 break;
                case S2_DISCONNECT            : result = "S2_DISCONNECT";              break;
                case S2_SAMPLE_THROUGHPUT     : result = "S2_SAMPLE_THROUGHPUT";       break;
                case S2_SANA2HOOK             : result = "S2_SANA2HOOK";               break;
                default                       : result = txtDeviceUnknownSANA2Command; break;
            }
            break;

        case NSDEVTYPE_AUDIO:
            switch (cmd) {
                case ADCMD_FREE     : result = "ADCMD_FREE";                 break;
                case ADCMD_SETPREC  : result = "ADCMD_SETPREC";              break;
                case ADCMD_FINISH   : result = "ADCMD_FINISH";               break;
                case ADCMD_PERVOL   : result = "ADCMD_PERVOL";               break;
                case ADCMD_LOCK     : result = "ADCMD_LOCK";                 break;
                case ADCMD_WAITCYCLE: result = "ADCMD_WAITCYCLE";            break;
                case ADCMD_ALLOCATE : result = "ADCMD_ALLOCATE";             break;
                default             : result = txtDeviceUnknownAudioCommand; break;
            }
            break;

        case NSDEVTYPE_CLIPBOARD:
            switch (cmd) {
                case CBD_POST          : result = "CBD_POST";                       break;
                case CBD_CURRENTREADID : result = "CBD_CURRENTREADID";              break;
                case CBD_CURRENTWRITEID: result = "CBD_CURRENTWRITEID";             break;
                case CBD_CHANGEHOOK    : result = "CBD_CHANGEHOOK";                 break;
                default                : result = txtDeviceUnknownClipboardCommand; break;
            }
            break;

        case NSDEVTYPE_PRINTER:
            switch (cmd) {
                case PRD_RAWWRITE      : result = "PRD_RAWWRITE";                 break;
                case PRD_PRTCOMMAND    : result = "PRD_PRTCOMMAND";               break;
                case PRD_DUMPRPORT     : result = "PRD_DUMPRPORT";                break;
                case PRD_QUERY         : result = "PRD_QUERY";                    break;
                case PRD_RESETPREFS    : result = "PRD_RESETPREFS";               break;
                case PRD_LOADPREFS     : result = "PRD_LOADPREFS";                break;
                case PRD_USEPREFS      : result = "PRD_USEPREFS";                 break;
                case PRD_SAVEPREFS     : result = "PRD_SAVEPREFS";                break;
                case PRD_READPREFS     : result = "PRD_READPREFS";                break;
                case PRD_WRITEPREFS    : result = "PRD_WRITEPREFS";               break;
                case PRD_EDITPREFS     : result = "PRD_EDITPREFS";                break;
                case PRD_SETERRHOOK    : result = "PRD_SETERRHOOK";               break;
                case PRD_DUMPRPORTTAGS : result = "PRD_DUMPRPORTTAGS";            break;
            #if defined(__amigaos4__)
                case PRD_PRIVATECMD    : result = "PRD_PRIVATECMD";               break;
                case PRD_LASTSAVEDPREFS: result = "PRD_LASTSAVEDPREFS";           break;
            #endif
                default                : result = txtDeviceUnknownPrinterCommand; break;
            }
            break;

        case NSDEVTYPE_SERIAL:
            switch (cmd) {
                case SDCMD_QUERY    : result = "SDCMD_QUERY";                 break;
                case SDCMD_BREAK    : result = "SDCMD_BREAK";                 break;
                case SDCMD_SETPARAMS: result = "SDCMD_SETPARAMS";             break;
                case CMD_NONSTD + 7 : result = "SIOCMD_SETCTRLLINES";         break;
                default             : result = txtDeviceUnknownSerialCommand; break;
            }
            break;

        case NSDEVTYPE_PARALLEL:
            switch (cmd) {
                case PDCMD_QUERY    : result = "PDCMD_QUERY";                   break;
                case PDCMD_SETPARAMS: result = "PDCMD_SETPARAMS";               break;
                default             : result = txtDeviceUnknownParallelCommand; break;
            }
            break;
    }

    if (result == NULL) result = txtDeviceUnknownCommand;

    return result;
}

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct DevicesDetailWinData *ddwd = INST_DATA(cl, obj);
    struct DeviceEntry *de = ddwd->ddwd_Device;
    struct Library *lib = &de->de_Addr->dd_Library;
    ULONG unit;
    LONG cnt, total;
    struct NSDeviceQueryResult *nqr;

    MySetContentsHealed(ddwd->ddwd_Texts[ 0], "%s", lib->lib_Node.ln_Name);
    MySetContents(ddwd->ddwd_Texts[ 1], ADDRESS_FORMAT, lib);
    MySetContentsHealed(ddwd->ddwd_Texts[ 2], "%s", nonetest(lib->lib_IdString));
    MySetContents(ddwd->ddwd_Texts[ 3], "%s", GetNodeType(lib->lib_Node.ln_Type));
    MySetContents(ddwd->ddwd_Texts[ 4], "%ld", lib->lib_Node.ln_Pri);
    MySetContents(ddwd->ddwd_Texts[ 5], "%ld.%ld", lib->lib_Version, lib->lib_Revision);
    MySetContents(ddwd->ddwd_Texts[ 6], "%lD", lib->lib_OpenCnt);
    set(ddwd->ddwd_Texts[ 7], MUIA_FlagsButton_Flags, lib->lib_Flags);
    MySetContents(ddwd->ddwd_Texts[ 8], "%lD", lib->lib_NegSize);
    MySetContents(ddwd->ddwd_Texts[ 9], "%lD", lib->lib_PosSize);
    MySetContents(ddwd->ddwd_Texts[10], "$%08lx", lib->lib_Sum);  // ULONG

    GetRamPointerCount(lib, &cnt, &total);
    MySetContents(ddwd->ddwd_Texts[11], txtLibraryRPCFormat, cnt, total);
    MySetContents(ddwd->ddwd_Texts[12], "%s", de->de_CodeType);

    if ((nqr = tbAllocVecPooled(globalPool, sizeof(*nqr))) != NULL) {
        ApplicationSleep(TRUE);

        for (unit = 0; unit < 16; unit++) {
            LONG ioErr;
            STRPTR nsdType;

            if ((ioErr = NSDQuery(lib->lib_Node.ln_Name, unit, nqr)) == 0) {
                UWORD *cmds;

                set(ddwd->ddwd_NSDList, MUIA_NList_Quiet, TRUE);
                DoMethod(ddwd->ddwd_NSDList, MUIM_NList_Clear);

                cmds = nqr->SupportedCommands;
                if (cmds)
                {
                    while (*cmds) {
                        struct NSDCommandEntry nce;

                        nce.nce_Number = *cmds;
                        _snprintf(nce.nce_DecNumber, sizeof(nce.nce_DecNumber), "%6lD", *cmds);
                        _snprintf(nce.nce_HexNumber, sizeof(nce.nce_HexNumber), "$%04lx", *cmds);
                        stccpy(nce.nce_CmdName, NSDTypeToCmdName(nqr->DeviceType, *cmds), sizeof(nce.nce_CmdName));

                        InsertSortedEntry(ddwd->ddwd_NSDList, &nce);

                        cmds++;
                    }
                }

                set(ddwd->ddwd_NSDList, MUIA_NList_Quiet, FALSE);

                switch (nqr->DeviceType) {
                   case NSDEVTYPE_GAMEPORT:  nsdType = txtDeviceNSDTypeGameport;  break;
                   case NSDEVTYPE_TIMER:     nsdType = txtDeviceNSDTypeTimer;     break;
                   case NSDEVTYPE_KEYBOARD:  nsdType = txtDeviceNSDTypeKeyboard;  break;
                   case NSDEVTYPE_INPUT:     nsdType = txtDeviceNSDTypeInput;     break;
                   case NSDEVTYPE_TRACKDISK: nsdType = txtDeviceNSDTypeTrackdisk; break;
                   case NSDEVTYPE_CONSOLE:   nsdType = txtDeviceNSDTypeConsole;   break;
                   case NSDEVTYPE_SANA2:     nsdType = txtDeviceNSDTypeSANA2;     break;
                   case NSDEVTYPE_AUDIO:     nsdType = txtDeviceNSDTypeAudio;     break;
                   case NSDEVTYPE_CLIPBOARD: nsdType = txtDeviceNSDTypeClipboard; break;
                   case NSDEVTYPE_PRINTER:   nsdType = txtDeviceNSDTypePrinter;   break;
                   case NSDEVTYPE_SERIAL:    nsdType = txtDeviceNSDTypeSerial;    break;
                   case NSDEVTYPE_PARALLEL:  nsdType = txtDeviceNSDTypeParallel;  break;
                   default:                  nsdType = txtDeviceNSDTypeUnknown;   break;
                }

                MySetContents(ddwd->ddwd_Texts[13], txtDeviceNSDCompleteType, nsdType, nqr->DeviceSubType);

                break;
            } else {
                switch (ioErr) {
                    default:
                    case IOERR_OPENFAIL: nsdType = txtDeviceCannotOpen; break;
                    case IOERR_NOCMD:    nsdType = txtDeviceNonNSD; break;
                }

                MySetContents(ddwd->ddwd_Texts[13], "%s", nsdType);

                if (ioErr == IOERR_NOCMD) {
                    break;
                }
            }
        }

        set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtDevicesDetailTitle, de->de_Name, ddwd->ddwd_Title, sizeof(ddwd->ddwd_Title)));

        ApplicationSleep(FALSE);

        tbFreeVecPooled(globalPool, nqr);

    }
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[14], nsdList, exitButton, mainGroup;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Devices",
        MUIA_Window_ID, MakeID('.','D','E','V'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)(mainGroup = (Object *)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtNodeName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryIdString),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodeType2),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodePri2),
                            Child, (IPTR)(texts[ 4] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryVersion2),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryOpenCount2),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryFlags2),
                            Child, (IPTR)(texts[ 7] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtLibraryFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)devFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtDeviceFlagsTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtLibraryNegSize),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryPosSize),
                            Child, (IPTR)(texts[ 9] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibrarySum),
                            Child, (IPTR)(texts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryRPC2),
                            Child, (IPTR)(texts[11] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryCodeType2),
                            Child, (IPTR)(texts[12] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtDeviceNSDType),
                            Child, (IPTR)(texts[13] = MyTextObject6()),
                        End,
                        Child, (IPTR)HGroup,
                            GroupFrameT(txtSupportedCommands),
                            Child, (IPTR)MyNListviewObject(&nsdList, MakeID('.','N','S','D'), "BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR", &nsdlist_con2hook, &nsdlist_des2hook, &nsdlist_dsp2hook, &nsdlist_cmp2hook, FALSE),
                        End,
                    End),
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct DevicesDetailWinData *ddwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, ddwd->ddwd_Texts, sizeof(ddwd->ddwd_Texts));
        ddwd->ddwd_NSDList = nsdList;
        ddwd->ddwd_MainGroup = mainGroup;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_DefaultObject, group);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(nsdList, MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct DevicesDetailWinData *ddwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_DevicesDetailWin_Device:
                ddwd->ddwd_Device = (struct DeviceEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(DevicesDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeDevicesDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct DevicesDetailWinData), ENTRY(DevicesDetailWinDispatcher));
}

