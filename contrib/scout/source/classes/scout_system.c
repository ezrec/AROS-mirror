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

struct SystemWinData {
    TEXT swd_Title[WINDOW_TITLE_LENGTH];
    APTR swd_Texts[58];
};

struct SystemCallbackUserData {
    APTR ud_Texts[58];
};

STATIC CONST_STRPTR MyIdHardware( ULONG num )
{
    if (IdentifyBase) {
        return IdHardwareTags(num, IDTAG_Localize, FALSE, TAG_DONE);
    }
    return (STRPTR)txtNoIdentifyLibrary;
}

STATIC IPTR MyIdHardwareNum( ULONG num )
{
    if (IdentifyBase) {
        return IdHardwareNum(num, NULL);
    } else {
        return 0;
    }
}

STATIC int MyIdCheck( ULONG num )
{
    if (IdentifyBase) {
        return (IdHardwareTags(num, IDTAG_NULL4NA, TRUE, TAG_DONE) != NULL);
    } else {
        return 0;
    }
}

#if !defined(__amigaos4__)
STATIC void DateToInt( struct DateStamp *ds,
                       ULONG *u )
{
#if defined(__PPC__)
    *u  = ds->ds_Days * 86400 + ds->ds_Minute * 60 + ds->ds_Tick / TICKS_PER_SECOND;
#else
    *u = SMult32(ds->ds_Days, 86400) +
         SMult32(ds->ds_Minute, 60) +
         SDivMod32(ds->ds_Tick, TICKS_PER_SECOND);
#endif
}
#endif

#if !defined(__amigaos4__)
STATIC void IntToDate( ULONG r,
                       struct DateStamp *ds )
{
#if defined(__GNUC__)
    // FIXME: is this correct for AROS?
    ULONG d;

    d = r % 86400;

    ds->ds_Days   = r / 86400;
    ds->ds_Minute = d / 60;
    ds->ds_Tick   = (d % 60) * TICKS_PER_SECOND;
#else
    ds->ds_Days = UDivMod32(r, 86400);

    r = getreg(REG_D1);
    ds->ds_Minute = UDivMod32(r, 60);

    r = getreg(REG_D1);
    ds->ds_Tick = UMult32(r, TICKS_PER_SECOND);
#endif
}
#endif

STATIC void ReceiveList( void (* callback)( struct SystemEntry *se, void *userData ),
                         void *userData )
{
    struct SystemEntry *se;

    if ((se = tbAllocVecPooled(globalPool, sizeof(struct SystemEntry))) != NULL) {
        if (SendDaemon("GetSysList")) {
            while (ReceiveDecodedEntry(se, sizeof(struct SystemEntry))) {
                callback(se, userData);
            }
        }

        tbFreeVecPooled(globalPool, se);
    }
}

STATIC void IterateList( void (* callback)( struct SystemEntry *se, void *userData ),
                         void *userData )
{
    struct SystemEntry *se;

    if (IdentifyBase) {
        IdHardwareUpdate(); // Liste auf dem aktuellsten Stand
    }

    if ((se = tbAllocVecPooled(globalPool, sizeof(struct SystemEntry))) != NULL) {
        TEXT tmp[PATH_LENGTH];
        UWORD attn;
        ULONG cacr, pcr;
        ULONG lastalert = 0;
        IPTR  lastalerttask = 0;
        BOOL fblit;
    #if defined(__amigaos4__)
        struct MsgPort *mp;
        struct TimeRequest *tr;
        struct Device *TimerBase;
        struct TimerIFace *ITimer;
    #elif defined(__MORPHOS__)
        struct timerequest tr;
    #else
        BPTR lock;
        struct InfoData *id;
    #endif

        attn = SysBase->AttnFlags;
        if (FLAG_IS_SET(attn, AFF_68020)) {
            cacr = GetCACR(); /* 68020..68060 */
        } else {
            cacr = 0;
        }
        if (FLAG_IS_SET(attn, AFF_68060)) {
            pcr = GetPCR();  /* 68060 */
        } else {
            pcr = 0;
        }

        stccpy(se->se_System, MyIdHardware(IDHW_SYSTEM), sizeof(se->se_System));
        if (MyIdCheck(IDHW_CPUREV)) {
            _snprintf(se->se_CPU, sizeof(se->se_CPU), "%s / %s (ID $%04lx, Rev %s)", MyIdHardware(IDHW_CPU), MyIdHardware(IDHW_CPUCLOCK), pcr >> 16, MyIdHardware(IDHW_CPUREV));
        } else {
            _snprintf(se->se_CPU, sizeof(se->se_CPU), "%s / %s", MyIdHardware(IDHW_CPU), MyIdHardware(IDHW_CPUCLOCK));
        }
        _snprintf(se->se_FPU, sizeof(se->se_FPU), "%s / %s", MyIdHardware(IDHW_FPU), MyIdHardware(IDHW_FPUCLOCK));
        stccpy(se->se_MMU, MyIdHardware(IDHW_MMU), sizeof(se->se_MMU));
        _snprintf(se->se_PPC, sizeof(se->se_PPC), "%s / %s", MyIdHardware(IDHW_POWERPC), MyIdHardware(IDHW_PPCCLOCK));

        stccpy(se->se_Chipset, MyIdHardware(IDHW_CHIPSET), sizeof(se->se_Chipset));
        _snprintf(se->se_Ramsey, sizeof(se->se_Ramsey), "Rev. %s", MyIdHardware(IDHW_RAMSEY));
        stccpy(se->se_Gary, MyIdHardware(IDHW_GARY), sizeof(se->se_Gary));
        _snprintf(se->se_Agnus, sizeof(se->se_Agnus), "%s %s", MyIdHardware(IDHW_AGNUS), MyIdHardware(IDHW_AGNUSMODE));
        _snprintf(se->se_Denise, sizeof(se->se_Denise), "%s Rev. %s", MyIdHardware(IDHW_DENISE), MyIdHardware(IDHW_DENISEREV));
        stccpy(se->se_ChunkyPlanar, MyIdHardware(IDHW_CHUNKYPLANAR), sizeof(se->se_ChunkyPlanar));
        stccpy(se->se_BattClock, MyIdHardware(IDHW_BATTCLOCK), sizeof(se->se_BattClock));

        Forbid();
        fblit = (FindPort("FBlit") != NULL);
        Permit();
        stccpy(se->se_FBlit, (fblit) ? txtSystemRunning : txtSystemNotRunning, sizeof(se->se_BattClock));

        _snprintf(se->se_RAMWidth, sizeof(se->se_RAMWidth), "%s bit", MyIdHardware(IDHW_RAMWIDTH));
        stccpy(se->se_RAMAccess, MyIdHardware(IDHW_RAMACCESS), sizeof(se->se_RAMAccess));
        stccpy(se->se_RAMCAS, MyIdHardware(IDHW_RAMCAS), sizeof(se->se_RAMCAS));
        _snprintf(se->se_RAMBandWidth, sizeof(se->se_RAMBandWidth), "%sx", MyIdHardware(IDHW_RAMBANDWIDTH));

        stccpy(se->se_VBlankFreq, MyIdHardware(IDHW_VBLANKFREQ), sizeof(se->se_VBlankFreq));
        stccpy(se->se_PowerFreq, MyIdHardware(IDHW_POWERFREQ), sizeof(se->se_PowerFreq));
        stccpy(se->se_EClock, MyIdHardware(IDHW_ECLOCK), sizeof(se->se_EClock));

        _snprintf(se->se_SSP, sizeof(se->se_SSP), ADDRESS_FORMAT, SysBase->SysStkLower);
        stccpy(se->se_VBR, txtNotAvailable, sizeof(se->se_VBR));
        stccpy(se->se_CACR, txtNotAvailable, sizeof(se->se_CACR));
        stccpy(se->se_PCR, txtNotAvailable, sizeof(se->se_PCR));
        stccpy(se->se_InstCache, txtNotAvailable, sizeof(se->se_InstCache));
        stccpy(se->se_DataCache, txtNotAvailable, sizeof(se->se_DataCache));
        stccpy(se->se_BranchCache, txtNotAvailable, sizeof(se->se_DataCache));
        stccpy(se->se_68060, txtNotAvailable, sizeof(se->se_68060));

        if (FLAG_IS_SET(attn, AFF_68010)) {
            ULONG cache;

            cache = CacheControl(0, 0);

            _snprintf(se->se_VBR, sizeof(se->se_VBR), ADDRESS_FORMAT, GetVBR());

            if (FLAG_IS_SET(attn, AFF_68020)) {
                _snprintf(se->se_CACR, sizeof(se->se_CACR), "$%08lx", cacr);
            }

            if (FLAG_IS_SET(attn, AFF_68060)) {
                _snprintf(se->se_PCR, sizeof(se->se_PCR), "$%08lx", pcr);
            }

            if (FLAG_IS_SET(attn, AFF_68020)) {
                if (FLAG_IS_SET(cache, CACRF_EnableI)) {
                    stccpy(tmp, txtCacheEnabled, sizeof(tmp));
                    if (FLAG_IS_SET(attn, AFF_68030) && FLAG_IS_SET(cache, CACRF_IBE)) {
                        _strcatn(tmp, txtCacheBurst, PATH_LENGTH);
                    }
                    if (FLAG_IS_SET(cache, CACRF_FreezeI)) {
                        _strcatn(tmp, txtCacheFrozen, PATH_LENGTH);
                    }
                    if (FLAG_IS_SET(attn, AFF_68060) && FLAG_IS_SET(cacr, CACRF_WriteAllocate)) {
                        _strcatn(tmp, txtCacheHalf, PATH_LENGTH);
                    }
                } else {
                    stccpy(tmp, txtCacheDisabled, sizeof(tmp));
                }
                stccpy(se->se_InstCache, tmp, sizeof(se->se_InstCache));
            }

            tmp[0] = 0;
            if (FLAG_IS_SET(attn, AFF_68030)) {
                if (FLAG_IS_SET(cache, CACRF_EnableD)) {
                    stccpy(tmp, txtCacheEnabled, sizeof(tmp));
                    if (FLAG_IS_SET(attn, AFF_68030) && FLAG_IS_SET(cache, CACRF_DBE)) {
                        _strcatn(tmp, txtCacheBurst, PATH_LENGTH);
                    }
                    if (FLAG_IS_SET(cache, CACRF_FreezeD)) {
                        _strcatn(tmp, txtCacheFrozen, PATH_LENGTH);
                    }
                    if (FLAG_IS_SET(attn, AFF_68060) && BIT_IS_SET(cacr, 27)) {
                        _strcatn(tmp, txtCacheHalf, PATH_LENGTH);
                    }
                    if (FLAG_IS_SET(attn, AFF_68030) && FLAG_IS_CLEAR(attn, AFF_68040) && FLAG_IS_CLEAR(cache, CACRF_WriteAllocate)) {
                        _strcatn(tmp, txtCacheAllocate, PATH_LENGTH);
                    }
                } else {
                    stccpy(tmp, txtCacheDisabled, sizeof(tmp));
                }
            }

            if (FLAG_IS_SET(attn, AFF_68040) && FLAG_IS_CLEAR(attn, AFF_68060) && FLAG_IS_SET(cache, CACRF_CopyBack)) {
                _strcatn(tmp, txtCacheCopyBack, sizeof(tmp));
            }

            if (FLAG_IS_SET(cache, CACRF_EnableE)) {
                _strcatn(tmp, txtCacheEnabled, sizeof(tmp));
                _strcatn(tmp, txtCacheExternal, sizeof(tmp));
            }

            stccpy(se->se_DataCache, tmp, sizeof(se->se_DataCache));

            if (FLAG_IS_SET(attn, AFF_68060)) {
                tmp[0] = '\0';

                if (BIT_IS_SET(pcr, 0)) {
                    _strcatn(tmp, txtCacheSuperscalar, sizeof(tmp));
                }

                if (BIT_IS_SET(cacr, 30)) {
                    _strcatn(tmp, txtCacheAllocateOperandDisabled, sizeof(tmp));
                }

                if (BIT_IS_SET(cacr, 14)) {
                    _strcatn(tmp, txtCacheAllocateDisabled, sizeof(tmp));
                }

                if (BIT_IS_SET(cacr, 29)) {
                    _strcatn(tmp, txtCacheWriteBuffer, sizeof(tmp));
                }

                if (BIT_IS_SET(cacr, 28)) {
                    _strcatn(tmp, txtCacheCPUSHInvalidationDisabled, sizeof(tmp));
                }

                if (BIT_IS_SET(pcr, 1)) {
                    _strcatn(tmp, txtCacheFPUDisabled, sizeof(tmp));
                }

                if (BIT_IS_SET(pcr, 5)) {
                    _strcatn(tmp, txtCacheLoadStoreBypassDisabled, sizeof(tmp));
                } else {
                    _strcatn(tmp, txtCacheLoadStoreBypassEnabled, sizeof(tmp));
                }

                stccpy(se->se_68060, &tmp[2], sizeof(se->se_68060));
            }

            if (FLAG_IS_SET(attn, AFF_68060)) {
                stccpy(se->se_BranchCache, BIT_IS_SET(cacr, 23) ? txtCacheEnabled : txtCacheDisabled, sizeof(se->se_BranchCache));
            }
        }

        stccpy(se->se_CurrentTime, txtSystemTimeInvalid, sizeof(se->se_CurrentTime));
        stccpy(se->se_BootTime, txtSystemTimeInvalid, sizeof(se->se_BootTime));
        stccpy(se->se_UpTime, txtSystemTimeInvalid, sizeof(se->se_UpTime));
    #if defined(__amigaos4__)
        if ((mp = CreateMsgPort()) != NULL) {
            if ((tr = (struct TimeRequest *)CreateIORequest(mp, sizeof(*tr))) != NULL) {
                if (OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *)tr, 0) == 0) {
                    struct TimeVal currentTime, upTime, bootTime;
                    struct DateTime dt;
                    TEXT dateStr[LEN_DATSTRING], timeStr[LEN_DATSTRING];

                    TimerBase = tr->Request.io_Device;

                    if (GETINTERFACE(ITimer, TimerBase)) {
                        GetSysTime(TIMEVAL(&currentTime));
                        GetUpTime(TIMEVAL(&upTime));

                        bootTime = currentTime;
                        SubTime(TIMEVAL(&bootTime), TIMEVAL(&upTime));

                        SecondsToDateStamp(currentTime.Seconds, &dt.dat_Stamp);
                        dt.dat_Format = FORMAT_DOS;
                        dt.dat_Flags = 0;
                        dt.dat_StrDay = NULL;
                        dt.dat_StrDate = dateStr;
                        dt.dat_StrTime = timeStr;
                        DateToStr(&dt);
                        _snprintf(se->se_CurrentTime, sizeof(se->se_CurrentTime), "%s %s", dateStr, timeStr);

                        SecondsToDateStamp(bootTime.Seconds, &dt.dat_Stamp);
                        dt.dat_Format = FORMAT_DOS;
                        dt.dat_Flags = 0;
                        dt.dat_StrDay = NULL;
                        dt.dat_StrDate = dateStr;
                        dt.dat_StrTime = timeStr;
                        DateToStr(&dt);
                        _snprintf(se->se_BootTime, sizeof(se->se_BootTime), "%s %s", dateStr, timeStr);

                        SecondsToDateStamp(upTime.Seconds % 86400, &dt.dat_Stamp);
                        dt.dat_Format = FORMAT_DOS;
                        dt.dat_Flags = 0;
                        dt.dat_StrDay = NULL;
                        dt.dat_StrDate = dateStr;
                        dt.dat_StrTime = timeStr;
                        DateToStr(&dt);
                        _snprintf(se->se_UpTime, sizeof(se->se_UpTime), txtSystemUpTimeFormat, upTime.Seconds / 86400, timeStr);

                        DROPINTERFACE(ITimer);
                    }

                    CloseDevice((struct IORequest *)tr);
                }

                DeleteIORequest((struct IORequest *)tr);
            }

            DeleteMsgPort(mp);
        }
    #elif defined(__MORPHOS__)
        /* No need to initialise timerequest */

        if (OpenDevice(TIMERNAME, UNIT_VBLANK, &tr.tr_node, 0) == 0) {
            struct timeval CurrentTime;
            struct Library *TimerBase;
            struct EClockVal ecv;
            struct DateTime dt;
            UQUAD ev;
            ULONG base, uptimesecs, bootsecs;
            TEXT dateStr[LEN_DATSTRING], timeStr[LEN_DATSTRING];

            TimerBase = (APTR)tr.tr_node.io_Device;

            GetSysTime(&CurrentTime);
            base = ReadEClock(&ecv);
            ev = ((UQUAD) ecv.ev_hi << 32) | ecv.ev_lo;

            uptimesecs = (ULONG) (ev / (UQUAD) base);
				bootsecs = CurrentTime.tv_secs - uptimesecs;

            IntToDate(CurrentTime.tv_secs, &dt.dat_Stamp);
            dt.dat_Format = FORMAT_DOS;
            dt.dat_Flags = 0;
            dt.dat_StrDay = NULL;
            dt.dat_StrDate = dateStr;
            dt.dat_StrTime = timeStr;
            DateToStr(&dt);
            _snprintf(se->se_CurrentTime, sizeof(se->se_CurrentTime), "%s %s", dateStr, timeStr);

            IntToDate(bootsecs, &dt.dat_Stamp);
            dt.dat_Format = FORMAT_DOS;
            dt.dat_Flags = 0;
            dt.dat_StrDay = NULL;
            dt.dat_StrDate = dateStr;
            dt.dat_StrTime = timeStr;
            DateToStr(&dt);
            _snprintf(se->se_BootTime, sizeof(se->se_BootTime), "%s %s", dateStr, timeStr);

            IntToDate(uptimesecs, &dt.dat_Stamp);
            dt.dat_Format = FORMAT_DOS;
            dt.dat_Flags = 0;
            dt.dat_StrDay = NULL;
            dt.dat_StrDate = dateStr;
            dt.dat_StrTime = timeStr;
            DateToStr(&dt);
            _snprintf(se->se_UpTime, sizeof(se->se_UpTime), txtSystemUpTimeFormat, uptimesecs / 86400, timeStr);

            CloseDevice(&tr.tr_node);
        }
    #else
        if ((lock = Lock("RAM:", SHARED_LOCK)) != ZERO) {
            if ((id = tbAllocVecPooled(globalPool, sizeof(struct InfoData))) != NULL) {
                if (Info(lock, id)) {
                    struct DeviceList *ramDev;
                    ULONG currentTime, bootTime, upTime, days;
                    struct DateStamp ds;
                    struct DateTime dt;
                    TEXT dateStr[LEN_DATSTRING], timeStr[LEN_DATSTRING];

                    ramDev = BADDR(id->id_VolumeNode);
                    DateToInt(&ramDev->dl_VolumeDate, &bootTime);

                    DateStamp(&ds);
                    DateToInt(&ds, &currentTime);

                    upTime = currentTime - bootTime;

                    IntToDate(currentTime, &dt.dat_Stamp);
                    dt.dat_Format = FORMAT_DOS;
                    dt.dat_Flags = 0;
                    dt.dat_StrDay = NULL;
                    dt.dat_StrDate = dateStr;
                    dt.dat_StrTime = timeStr;
                    DateToStr(&dt);

                    _snprintf(se->se_CurrentTime, sizeof(se->se_CurrentTime), "%s %s", dateStr, timeStr);

                    IntToDate(bootTime, &dt.dat_Stamp);
                    dt.dat_Format = FORMAT_DOS;
                    dt.dat_Flags = 0;
                    dt.dat_StrDay = NULL;
                    dt.dat_StrDate = dateStr;
                    dt.dat_StrTime = timeStr;
                    DateToStr(&dt);

                    _snprintf(se->se_BootTime, sizeof(se->se_BootTime), "%s %s", dateStr, timeStr);

                    days = upTime / 86400;
                    upTime %= 86400;
                    IntToDate(upTime, &dt.dat_Stamp);
                    dt.dat_Format = FORMAT_DOS;
                    dt.dat_Flags = 0;
                    dt.dat_StrDay = NULL;
                    dt.dat_StrDate = NULL;
                    dt.dat_StrTime = timeStr;
                    DateToStr(&dt);

                    _snprintf(se->se_UpTime, sizeof(se->se_UpTime), txtSystemUpTimeFormat, days, timeStr);
                }

                tbFreeVecPooled(globalPool, id);
            }

            UnLock(lock);
        }
    #endif

        stccpy(se->se_AmigaOS, MyIdHardware(IDHW_OSNR), sizeof(se->se_AmigaOS));
        stccpy(se->se_BoingBag, MyIdHardware(IDHW_BOINGBAG), sizeof(se->se_BoingBag));
        stccpy(se->se_OSVersion, MyIdHardware(IDHW_OSVER), sizeof(se->se_OSVersion));
        stccpy(se->se_SetPatch, MyIdHardware(IDHW_SETPATCHVER), sizeof(se->se_SetPatch));
        stccpy(se->se_ROMSize, MyIdHardware(IDHW_ROMSIZE), sizeof(se->se_ROMSize));
        stccpy(se->se_Exec, MyIdHardware(IDHW_EXECVER), sizeof(se->se_Exec));
        stccpy(se->se_Workbench, MyIdHardware(IDHW_WBVER), sizeof(se->se_Workbench));
        stccpy(se->se_GraphicSystem, MyIdHardware(IDHW_GFXSYS), sizeof(se->se_GraphicSystem));
        stccpy(se->se_AudioSystem, MyIdHardware(IDHW_AUDIOSYS), sizeof(se->se_AudioSystem));
        stccpy(se->se_TCPIP, MyIdHardware(IDHW_TCPIP), sizeof(se->se_TCPIP));
        stccpy(se->se_PowerPCOS, MyIdHardware(IDHW_PPCOS), sizeof(se->se_PowerPCOS));

        stccpy(se->se_PlainChipRAM, MyIdHardware(IDHW_PLNCHIPRAM), sizeof(se->se_PlainChipRAM));
        stccpy(se->se_PlainFastRAM, MyIdHardware(IDHW_PLNFASTRAM), sizeof(se->se_PlainFastRAM));
        stccpy(se->se_PlainRAM, MyIdHardware(IDHW_PLNRAM), sizeof(se->se_PlainRAM));
        stccpy(se->se_VirtualChipRAM, MyIdHardware(IDHW_VMMCHIPRAM), sizeof(se->se_VirtualChipRAM));
        stccpy(se->se_VirtualFastRAM, MyIdHardware(IDHW_VMMFASTRAM), sizeof(se->se_VirtualFastRAM));
        stccpy(se->se_VirtualRAM, MyIdHardware(IDHW_VMMRAM), sizeof(se->se_VirtualRAM));
        stccpy(se->se_TotalChipRAM, MyIdHardware(IDHW_CHIPRAM), sizeof(se->se_TotalChipRAM));
        stccpy(se->se_TotalFastRAM, MyIdHardware(IDHW_FASTRAM), sizeof(se->se_TotalFastRAM));
        stccpy(se->se_TotalRAM, MyIdHardware(IDHW_RAM), sizeof(se->se_TotalRAM));
        stccpy(se->se_SlowRAM, MyIdHardware(IDHW_SLOWRAM), sizeof(se->se_SlowRAM));

        if (CheckLibVersion(IdentifyBase, 37, 0) == CLV_NEWER_OR_SAME) {
            // the rewritten identify.library features exactly the other way of getting the last alert, so we can savely use the library
            lastalert = MyIdHardwareNum(IDHW_LASTALERT);
            lastalerttask = MyIdHardwareNum(IDHW_LASTALERTTASK);
        } else {
        #if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
        	#if defined(__SASC)
            struct MMUBase *MMUBase;

            if ((MMUBase = (struct MMUBase *)OpenLibrary(MMU_NAME, 43)) != NULL) {
                // if we have THOR's mmu.library installed it is very simple to get the last Guru and the task
                // that caused it without causing an Enforcer hit ourself.
                struct MMUContext *ctx;
                ULONG zerobase;

                ctx = DefaultContext();
                LockMMUContext(ctx);

                zerobase = GetMMUContextData(ctx, MCXTAG_ZEROBASE);
                lastalert = 0;
                lastalerttask = 0;
                ReadZeroPage(zerobase, &lastalert, &lastalerttask);

                UnlockMMUContext(ctx);

                CloseLibrary((struct Library *)MMUBase);
            } else if (SysBase->LibNode.lib_Version <= 45) { // we are not running under AmigaOS4 or MorphOS
                // if you used some non-mmu.library tool to remap the ZeroPage this is the only way to get the correct values
                // Sorry, but this will definitely cause an Enforcer hit!! No way 'round...
                lastalert = *((ULONG *)0x100);
                lastalerttask = *((ULONG *)0x104);
            }
            #else
            // Sorry, but this will definitely cause an Enforcer hit!! No way 'round...
            lastalert = *((ULONG *)0x100);
            lastalerttask = *((ULONG *)0x104);
            #endif
        #endif
        }

        _snprintf(se->se_AlertCode, sizeof(se->se_AlertCode), "%08lx", 0);
        stccpy(se->se_AlertTask, txtNone, sizeof(se->se_AlertTask));
        stccpy(se->se_AlertDeadend, txtNone, sizeof(se->se_AlertDeadend));
        stccpy(se->se_AlertSubsystem, txtNone, sizeof(se->se_AlertSubsystem));
        stccpy(se->se_AlertGeneral, txtNone, sizeof(se->se_AlertGeneral));
        stccpy(se->se_AlertSpecific, txtNone, sizeof(se->se_AlertSpecific));
        if (lastalert != 0x00000000 && lastalert != 0xffffffff) {
            struct Task *alerttask;

            _snprintf(se->se_AlertCode, sizeof(se->se_AlertCode), "%08lx", lastalert);

            if ((alerttask = TaskExists((struct Task *)lastalerttask)) != NULL) {
                GetTaskName(alerttask, tmp, sizeof(tmp));
                _snprintf(se->se_AlertTask, sizeof(se->se_AlertTask), ADDRESS_FORMAT " (%s)", lastalerttask, tmp);
            } else {
                _snprintf(se->se_AlertTask, sizeof(se->se_AlertTask), ADDRESS_FORMAT, lastalerttask);
            }

            if (IdentifyBase) {
                IdAlertTags(lastalert, IDTAG_DeadStr, (IPTR)se->se_AlertDeadend, IDTAG_StrLength, sizeof(se->se_AlertDeadend), TAG_DONE);
                IdAlertTags(lastalert, IDTAG_SubsysStr, (IPTR)se->se_AlertSubsystem, IDTAG_StrLength, sizeof(se->se_AlertSubsystem), TAG_DONE);
                IdAlertTags(lastalert, IDTAG_GeneralStr, (IPTR)se->se_AlertGeneral, IDTAG_StrLength, sizeof(se->se_AlertGeneral), TAG_DONE);
                IdAlertTags(lastalert, IDTAG_SpecStr, (IPTR)se->se_AlertSpecific, IDTAG_StrLength, sizeof(se->se_AlertSpecific), TAG_DONE);
            }
        }

        callback(se, userData);

        tbFreeVecPooled(globalPool, se);
    }
}

STATIC void UpdateCallback( struct SystemEntry *se,
                            void *userData )
{
    struct SystemCallbackUserData *ud = userData;

    MySetContents(ud->ud_Texts[ 0], se->se_System);
    MySetContents(ud->ud_Texts[ 1], se->se_CPU);
    MySetContents(ud->ud_Texts[ 2], se->se_FPU);
    MySetContents(ud->ud_Texts[ 3], se->se_MMU);
    MySetContents(ud->ud_Texts[ 4], se->se_PPC);
    MySetContents(ud->ud_Texts[ 5], se->se_Chipset);
    MySetContents(ud->ud_Texts[ 6], se->se_Ramsey);
    MySetContents(ud->ud_Texts[ 7], se->se_Gary);
    MySetContents(ud->ud_Texts[ 8], se->se_Agnus);
    MySetContents(ud->ud_Texts[ 9], se->se_Denise);
    MySetContents(ud->ud_Texts[10], se->se_ChunkyPlanar);
    MySetContents(ud->ud_Texts[11], se->se_BattClock);
    MySetContents(ud->ud_Texts[12], se->se_FBlit);
    MySetContents(ud->ud_Texts[13], se->se_RAMWidth);
    MySetContents(ud->ud_Texts[14], se->se_RAMAccess);
    MySetContents(ud->ud_Texts[15], se->se_RAMCAS);
    MySetContents(ud->ud_Texts[16], se->se_RAMBandWidth);
    MySetContents(ud->ud_Texts[17], se->se_VBlankFreq);
    MySetContents(ud->ud_Texts[18], se->se_PowerFreq);
    MySetContents(ud->ud_Texts[19], se->se_EClock);
    MySetContents(ud->ud_Texts[20], se->se_SSP);
    MySetContents(ud->ud_Texts[21], se->se_VBR);
    MySetContents(ud->ud_Texts[22], se->se_CACR);
    MySetContents(ud->ud_Texts[23], se->se_InstCache);
    MySetContents(ud->ud_Texts[24], se->se_DataCache);
    MySetContents(ud->ud_Texts[25], se->se_PCR);
    MySetContents(ud->ud_Texts[26], se->se_BranchCache);
    MySetContents(ud->ud_Texts[27], se->se_68060);
    MySetContents(ud->ud_Texts[28], se->se_CurrentTime);
    MySetContents(ud->ud_Texts[29], se->se_BootTime);
    MySetContents(ud->ud_Texts[30], se->se_UpTime);
    MySetContents(ud->ud_Texts[31], se->se_AmigaOS);
    MySetContents(ud->ud_Texts[32], se->se_BoingBag);
    MySetContents(ud->ud_Texts[33], se->se_OSVersion);
    MySetContents(ud->ud_Texts[34], se->se_SetPatch);
    MySetContents(ud->ud_Texts[35], se->se_ROMSize);
    MySetContents(ud->ud_Texts[36], se->se_Exec);
    MySetContents(ud->ud_Texts[37], se->se_Workbench);
    MySetContents(ud->ud_Texts[38], se->se_GraphicSystem);
    MySetContents(ud->ud_Texts[39], se->se_AudioSystem);
    MySetContents(ud->ud_Texts[40], se->se_TCPIP);
    MySetContents(ud->ud_Texts[41], se->se_PowerPCOS);
    MySetContents(ud->ud_Texts[42], se->se_PlainChipRAM);
    MySetContents(ud->ud_Texts[43], se->se_PlainFastRAM);
    MySetContents(ud->ud_Texts[44], se->se_PlainRAM);
    MySetContents(ud->ud_Texts[45], se->se_VirtualChipRAM);
    MySetContents(ud->ud_Texts[46], se->se_VirtualFastRAM);
    MySetContents(ud->ud_Texts[47], se->se_VirtualRAM);
    MySetContents(ud->ud_Texts[48], se->se_TotalChipRAM);
    MySetContents(ud->ud_Texts[49], se->se_TotalFastRAM);
    MySetContents(ud->ud_Texts[50], se->se_TotalRAM);
    MySetContents(ud->ud_Texts[51], se->se_SlowRAM);
    MySetContents(ud->ud_Texts[52], se->se_AlertCode);
    MySetContents(ud->ud_Texts[53], se->se_AlertTask);
    MySetContents(ud->ud_Texts[54], se->se_AlertDeadend);
    MySetContents(ud->ud_Texts[55], se->se_AlertSubsystem);
    MySetContents(ud->ud_Texts[56], se->se_AlertGeneral);
    MySetContents(ud->ud_Texts[57], se->se_AlertSpecific);
}

STATIC void PrintCallback( struct SystemEntry *se,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtSystemPrintHeader, txtSystemPageHardware);
    PrintFOneLine((BPTR)userData, txtSystemPrintTitleLine, txtSystemProcessor);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemCPU, se->se_CPU);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemFPU, se->se_FPU);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemMMU, se->se_MMU);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemPPC, se->se_PPC);
    PrintFOneLine((BPTR)userData, txtSystemPrintTitleLine, txtSystemChipset);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemChipset2, se->se_Chipset);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemRamsey, se->se_Ramsey);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemGary, se->se_Gary);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemAgnus, se->se_Agnus);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemDenise, se->se_Denise);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemChunkyPlanar, se->se_ChunkyPlanar);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemBattClock, se->se_BattClock);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemFBlit, se->se_FBlit);
    PrintFOneLine((BPTR)userData, txtSystemPrintTitleLine, txtSystemMotherboardRAM);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemRAMWidth, se->se_RAMWidth);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemRAMAccess, se->se_RAMAccess);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemRAMCAS, se->se_RAMCAS);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemRAMBandwidth, se->se_RAMBandWidth);
    PrintFOneLine((BPTR)userData, txtSystemPrintTitleLine, txtSystemFrequencies);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemVBlank, se->se_VBlankFreq);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemPower, se->se_PowerFreq);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemEClock, se->se_EClock);
    PrintFOneLine((BPTR)userData, "\n");
    PrintFOneLine((BPTR)userData, txtSystemPrintHeader, txtSystemPageRegisters);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemSSP, se->se_SSP);
    PrintFOneLine((BPTR)userData, txtSystemPrintTitleLine, txtSystem680x0Specific);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemVBR, se->se_VBR);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemCACR, se->se_CACR);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemInstCache, se->se_InstCache);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemDataCache, se->se_DataCache);
    PrintFOneLine((BPTR)userData, txtSystemPrintTitleLine, txtSystem68060Specific);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemPCR, se->se_PCR);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemBranchCache, se->se_BranchCache);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystem68060, se->se_68060);
    PrintFOneLine((BPTR)userData, "\n");
    PrintFOneLine((BPTR)userData, txtSystemPrintHeader, txtSystemPageUptime);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemCurrentTime, se->se_CurrentTime);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemBootTime, se->se_BootTime);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemUpTime, se->se_UpTime);
    PrintFOneLine((BPTR)userData, "\n");
    PrintFOneLine((BPTR)userData, txtSystemPrintHeader, txtSystemPageVersions);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemAmigaOSVersion, se->se_AmigaOS);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemBoingBagVersion, se->se_BoingBag);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemOSVersion, se->se_OSVersion);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemSetPatchVersion, se->se_SetPatch);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemROMSize, se->se_ROMSize);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemExecVersion, se->se_Exec);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemWorkbenchVersion, se->se_Workbench);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemGraphics, se->se_GraphicSystem);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemAudio, se->se_AudioSystem);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemTCPIP, se->se_TCPIP);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemPowerPCOS, se->se_PowerPCOS);
    PrintFOneLine((BPTR)userData, "\n");
    PrintFOneLine((BPTR)userData, txtSystemPrintHeader, txtSystemPageMemory);
    PrintFOneLine((BPTR)userData, txtSystemPrintRAMLine, txtSystemPlain, se->se_PlainChipRAM, se->se_PlainFastRAM, se->se_PlainRAM);
    PrintFOneLine((BPTR)userData, txtSystemPrintRAMLine, txtSystemVirtual, se->se_VirtualChipRAM, se->se_VirtualFastRAM, se->se_VirtualRAM);
    PrintFOneLine((BPTR)userData, txtSystemPrintRAMLine, txtSystemTotal2, se->se_TotalChipRAM, se->se_TotalFastRAM, se->se_TotalRAM);
    PrintFOneLine((BPTR)userData, txtSystemPrintSlowRAMLine, txtSystemSlowRAM, se->se_SlowRAM);
    PrintFOneLine((BPTR)userData, "\n");
    PrintFOneLine((BPTR)userData, txtSystemPrintHeader, txtSystemPageLastAlert);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemAlertCode, se->se_AlertCode);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemAlertTask, se->se_AlertTask);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemAlertDeadend, se->se_AlertDeadend);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemAlertSubsystem, se->se_AlertSubsystem);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemAlertGeneral, se->se_AlertGeneral);
    PrintFOneLine((BPTR)userData, txtSystemPrintLine, txtSystemAlertSpecific, se->se_AlertSpecific);
}

STATIC void SendCallback( struct SystemEntry *se,
                          UNUSED void *userData )
{
    SendEncodedEntry(se, sizeof(struct SystemEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    STATIC STRPTR systemPages[7] = { NULL };
    APTR pages, systemTexts[58], updateButton, printButton, exitButton;

    systemPages[0] = txtSystemPageHardware;
    systemPages[1] = txtSystemPageRegisters;
    systemPages[2] = txtSystemPageUptime;
    systemPages[3] = txtSystemPageVersions;
    systemPages[4] = txtSystemPageMemory;
    systemPages[5] = txtSystemPageLastAlert;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "System",
        MUIA_Window_ID, MakeID('S','Y','S','T'),
        WindowContents, VGroup,

            Child, (IPTR)(pages = (Object *)RegisterGroup(systemPages),
                MUIA_CycleChain, TRUE,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtSystemSystem),
                            Child, (IPTR)(systemTexts[ 0] = MyTextObject6()),
                        End,
                        Child, (IPTR)ColGroup(2), GroupFrameT(txtSystemProcessor),
                            Child, (IPTR)MyLabel2(txtSystemCPU),
                            Child, (IPTR)(systemTexts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemFPU),
                            Child, (IPTR)(systemTexts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemMMU),
                            Child, (IPTR)(systemTexts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemPPC),
                            Child, (IPTR)(systemTexts[ 4] = MyTextObject6()),
                        End,
                        Child, (IPTR)ColGroup(2), GroupFrameT(txtSystemChipset),
                            Child, (IPTR)MyLabel2(txtSystemChipset2),
                            Child, (IPTR)(systemTexts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemRamsey),
                            Child, (IPTR)(systemTexts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemGary),
                            Child, (IPTR)(systemTexts[ 7] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemAgnus),
                            Child, (IPTR)(systemTexts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemDenise),
                            Child, (IPTR)(systemTexts[ 9] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemChunkyPlanar),
                            Child, (IPTR)(systemTexts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemBattClock),
                            Child, (IPTR)(systemTexts[11] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemFBlit),
                            Child, (IPTR)(systemTexts[12] = MyTextObject6()),
                        End,
                        Child, (IPTR)ColGroup(2), GroupFrameT(txtSystemMotherboardRAM),
                            Child, (IPTR)MyLabel2(txtSystemRAMWidth),
                            Child, (IPTR)(systemTexts[13] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemRAMAccess),
                            Child, (IPTR)(systemTexts[14] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemRAMCAS),
                            Child, (IPTR)(systemTexts[15] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemRAMBandwidth),
                            Child, (IPTR)(systemTexts[16] = MyTextObject6()),
                        End,
                        Child, (IPTR)ColGroup(2), GroupFrameT(txtSystemFrequencies),
                            Child, (IPTR)MyLabel2(txtSystemVBlank),
                            Child, (IPTR)(systemTexts[17] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemPower),
                            Child, (IPTR)(systemTexts[18] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemEClock),
                            Child, (IPTR)(systemTexts[19] = MyTextObject6()),
                        End,
                    End,
                End,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtSystemSSP),
                            Child, (IPTR)(systemTexts[20] = MyTextObject6()),
                        End,
                        Child, (IPTR)ColGroup(2), GroupFrameT(txtSystem680x0Specific),
                            Child, (IPTR)MyLabel2(txtSystemVBR),
                            Child, (IPTR)(systemTexts[21] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemCACR),
                            Child, (IPTR)(systemTexts[22] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemInstCache),
                            Child, (IPTR)(systemTexts[23] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemDataCache),
                            Child, (IPTR)(systemTexts[24] = MyTextObject6()),
                        End,
                        Child, (IPTR)ColGroup(2), GroupFrameT(txtSystem68060Specific),
                            Child, (IPTR)MyLabel2(txtSystemPCR),
                            Child, (IPTR)(systemTexts[25] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemBranchCache),
                            Child, (IPTR)(systemTexts[26] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystem68060),
                            Child, (IPTR)(systemTexts[27] = MyTextObject6()),
                        End,
                    End,
                End,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtSystemCurrentTime),
                            Child, (IPTR)(systemTexts[28] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemBootTime),
                            Child, (IPTR)(systemTexts[29] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemUpTime),
                            Child, (IPTR)(systemTexts[30] = MyTextObject6()),
                        End,
                    End,
                End,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtSystemAmigaOSVersion),
                            Child, (IPTR)(systemTexts[31] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemBoingBagVersion),
                            Child, (IPTR)(systemTexts[32] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemOSVersion),
                            Child, (IPTR)(systemTexts[33] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemSetPatchVersion),
                            Child, (IPTR)(systemTexts[34] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemROMSize),
                            Child, (IPTR)(systemTexts[35] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemExecVersion),
                            Child, (IPTR)(systemTexts[36] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemWorkbenchVersion),
                            Child, (IPTR)(systemTexts[37] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemGraphics),
                            Child, (IPTR)(systemTexts[38] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemAudio),
                            Child, (IPTR)(systemTexts[39] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemTCPIP),
                            Child, (IPTR)(systemTexts[40] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemPowerPCOS),
                            Child, (IPTR)(systemTexts[41] = MyTextObject6()),
                        End,
                    End,
                End,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(4),
                            Child, (IPTR)HSpace(0),
                            Child, (IPTR)LabelC(txtSystemChip),
                            Child, (IPTR)LabelC(txtSystemFast),
                            Child, (IPTR)LabelC(txtSystemTotal),

                            Child, (IPTR)MyLabel2(txtSystemPlain),
                            Child, (IPTR)(systemTexts[42] = MyTextObject6()),
                            Child, (IPTR)(systemTexts[43] = MyTextObject6()),
                            Child, (IPTR)(systemTexts[44] = MyTextObject6()),

                            Child, (IPTR)MyLabel2(txtSystemVirtual),
                            Child, (IPTR)(systemTexts[45] = MyTextObject6()),
                            Child, (IPTR)(systemTexts[46] = MyTextObject6()),
                            Child, (IPTR)(systemTexts[47] = MyTextObject6()),

                            Child, (IPTR)MyLabel2(txtSystemTotal2),
                            Child, (IPTR)(systemTexts[48] = MyTextObject6()),
                            Child, (IPTR)(systemTexts[49] = MyTextObject6()),
                            Child, (IPTR)(systemTexts[50] = MyTextObject6()),

                            Child, (IPTR)MyLabel2(txtSystemSlowRAM),
                            Child, (IPTR)HSpace(0),
                            Child, (IPTR)HSpace(0),
                            Child, (IPTR)(systemTexts[51] = MyTextObject6()),
                        End,
                    End,
                End,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtSystemAlertCode),
                            Child, (IPTR)(systemTexts[52] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemAlertTask),
                            Child, (IPTR)(systemTexts[53] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemAlertDeadend),
                            Child, (IPTR)(systemTexts[54] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemAlertSubsystem),
                            Child, (IPTR)(systemTexts[55] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemAlertGeneral),
                            Child, (IPTR)(systemTexts[56] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtSystemAlertSpecific),
                            Child, (IPTR)(systemTexts[57] = MyTextObject6()),
                        End,
                    End,
                End,
            End),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton  = MakeButton(txtPrint)),
                Child, (IPTR)(exitButton   = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct SystemWinData *swd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(systemTexts, swd->swd_Texts, sizeof(swd->swd_Texts));

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtSystemTitle, swd->swd_Title, sizeof(swd->swd_Title)));
        set(obj, MUIA_Window_ActiveObject, pages);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     1, MUIM_SystemWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     1, MUIM_SystemWin_Print);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct SystemWinData *swd = INST_DATA(cl, obj);
    struct SystemCallbackUserData ud;

    ApplicationSleep(TRUE);

    CopyMemQuick(swd->swd_Texts, ud.ud_Texts, sizeof(swd->swd_Texts));

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintSystem(NULL);

    return 0;
}

DISPATCHER(SystemWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:            return (mDispose(cl, obj, (APTR)msg));
        case MUIM_SystemWin_Update: return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_SystemWin_Print:  return (mPrint(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintSystem( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendSystemList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeSystemWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct SystemWinData), ENTRY(SystemWinDispatcher));
}

