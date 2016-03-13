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

struct AudioModesDetailWinData {
    TEXT amdwd_Title[WINDOW_TITLE_LENGTH];
    APTR amdwd_Texts[30];
    struct AudioModeEntry *amdwd_AudioMode;
};

#if defined(__amigaos4__)
	#undef NEED_GLOBAL_AHIBASE
#elif defined(__MORPHOS__)
	#define NEED_GLOBAL_AHIBASE
#elif defined(__AROS__)
	#undef NEED_GLOBAL_AHIBASE
#else
	#if defined(__SASC)
		#undef NEED_GLOBAL_AHIBASE
	#else
		#define NEED_GLOBAL_AHIBASE
	#endif
#endif

#if defined(NEED_GLOBAL_AHIBASE)
struct Library *AHIBase;
#endif

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct AudioModesDetailWinData *amdwd = INST_DATA(cl, obj);
    struct AudioModeEntry *ame = amdwd->amdwd_AudioMode;
    STRPTR tmp;

    if ((tmp = tbAllocVecPooled(globalPool, PATH_LENGTH)) != NULL) {
        struct MsgPort *mp;

        if ((mp = CreateMsgPort()) != NULL) {
            struct AHIRequest *ahir;

            if ((ahir = (struct AHIRequest *)CreateIORequest(mp, sizeof(struct AHIRequest))) != NULL) {
                ahir->ahir_Version = 4;

                if (OpenDevice(AHINAME, 0, (struct IORequest *)ahir, 0) == 0) {
                #if !defined(NEED_GLOBAL_AHIBASE)
                    struct Library *AHIBase;
                #endif
                #if defined(__amigaos4__)
                    struct AHIIFace *IAHI;
                #endif
                    ULONG bits, maxchannels, minfreq, maxfreq, numfreqs, maxplaysamp, maxrecsamp;
                    Fixed minmonivol, maxmonivol;
                    Fixed mininputgain, maxinputgain;
                    Fixed minoutputvol, maxoutputvol;
                    ULONG volume, stereo, panning, hifi, pingpong, record, fullduplex, realtime;

                    AHIBase = (struct Library *)ahir->ahir_Std.io_Device;

                    if (GETINTERFACE(IAHI, AHIBase)) {
                        MySetContents(amdwd->amdwd_Texts[ 0], ame->ame_Name);
                        MySetContents(amdwd->amdwd_Texts[ 1], ame->ame_Id);

                        AHI_GetAudioAttrs(ame->ame_ModeID, NULL, AHIDB_Driver, (IPTR)tmp, AHIDB_BufferLen, PATH_LENGTH, TAG_DONE);
                        MySetContents(amdwd->amdwd_Texts[ 2], "DEVS:AHI/%s.audio", tmp);

                        AHI_GetAudioAttrs(ame->ame_ModeID, NULL, AHIDB_Version, (IPTR)tmp, AHIDB_BufferLen, PATH_LENGTH, TAG_DONE);
                        MySetContentsHealed(amdwd->amdwd_Texts[ 3], "%s", tmp);

                        AHI_GetAudioAttrs(ame->ame_ModeID, NULL, AHIDB_Author, (IPTR)tmp, AHIDB_BufferLen, PATH_LENGTH, TAG_DONE);
                        MySetContentsHealed(amdwd->amdwd_Texts[ 4], "%s", tmp);

                        AHI_GetAudioAttrs(ame->ame_ModeID, NULL, AHIDB_Copyright, (IPTR)tmp, AHIDB_BufferLen, PATH_LENGTH, TAG_DONE);
                        MySetContentsHealed(amdwd->amdwd_Texts[ 5], "%s", tmp);

                        AHI_GetAudioAttrs(ame->ame_ModeID, NULL, AHIDB_Annotation, (IPTR)tmp, AHIDB_BufferLen, PATH_LENGTH, TAG_DONE);
                        MySetContentsHealed(amdwd->amdwd_Texts[ 6], "%s", tmp);

                        AHI_GetAudioAttrs(ame->ame_ModeID, NULL, AHIDB_Bits, (IPTR)&bits,
                                                                 AHIDB_MaxChannels, (IPTR)&maxchannels,
                                                                 AHIDB_MinMixFreq, (IPTR)&minfreq,
                                                                 AHIDB_MaxMixFreq, (IPTR)&maxfreq,
                                                                 AHIDB_MaxMixFreq, (IPTR)&maxfreq,
                                                                 AHIDB_Frequencies, (IPTR)&numfreqs,
                                                                 AHIDB_MaxPlaySamples, (IPTR)&maxplaysamp,
                                                                 AHIDB_MaxRecordSamples, (IPTR)&maxrecsamp,
                                                                 AHIDB_Volume, (IPTR)&volume,
                                                                 AHIDB_Stereo, (IPTR)&stereo,
                                                                 AHIDB_Panning, (IPTR)&panning,
                                                                 AHIDB_HiFi, (IPTR)&hifi,
                                                                 AHIDB_PingPong, (IPTR)&pingpong,
                                                                 AHIDB_Record, (IPTR)&record,
                                                                 AHIDB_FullDuplex, (IPTR)&fullduplex,
                                                                 AHIDB_Realtime, (IPTR)&realtime,
                                                                 AHIDB_MinMonitorVolume, (IPTR)&minmonivol,
                                                                 AHIDB_MaxMonitorVolume, (IPTR)&maxmonivol,
                                                                 AHIDB_MinInputGain, (IPTR)&mininputgain,
                                                                 AHIDB_MaxInputGain, (IPTR)&maxinputgain,
                                                                 AHIDB_MinOutputVolume, (IPTR)&minoutputvol,
                                                                 AHIDB_MaxOutputVolume, (IPTR)&maxoutputvol,
                                                                 TAG_DONE);

                        MySetContents(amdwd->amdwd_Texts[ 7], "%lD", bits);
                        MySetContents(amdwd->amdwd_Texts[ 8], "%lD", maxchannels);
                        if (numfreqs > 1) {
                            MySetContents(amdwd->amdwd_Texts[ 9], "%6lD Hz..%6lD Hz", minfreq, maxfreq);
                        } else {
                            MySetContents(amdwd->amdwd_Texts[ 9], "---");
                        }
                        MySetContents(amdwd->amdwd_Texts[10], "%lD", maxplaysamp);
                        MySetContents(amdwd->amdwd_Texts[11], "%lD", maxrecsamp);

                        if (minmonivol == maxmonivol && minmonivol == 0) {
                            MySetContents(amdwd->amdwd_Texts[12], txtNone);
                        } else {
                            MySetContents(amdwd->amdwd_Texts[12], "%ld%%..%ld%%", minmonivol * 100 >> 16, maxmonivol * 100 >> 16);
                        }
                        if (mininputgain == maxinputgain) {
                            MySetContents(amdwd->amdwd_Texts[13], txtNone);
                        } else {
                            MySetContents(amdwd->amdwd_Texts[13], "%ld%%..%ld%%", mininputgain * 100 >> 16, maxinputgain * 100 >> 16);
                        }
                        if (minoutputvol == maxoutputvol) {
                            MySetContents(amdwd->amdwd_Texts[14], txtNone);
                        } else {
                            MySetContents(amdwd->amdwd_Texts[14], "%ld%%..%ld%%", minoutputvol * 100 >> 16, maxoutputvol * 100 >> 16);
                        }

                        MySetContents(amdwd->amdwd_Texts[ 15], "%s", (volume) ? msgYes : msgNo);
                        MySetContents(amdwd->amdwd_Texts[ 16], "%s", (stereo) ? msgYes : msgNo);
                        MySetContents(amdwd->amdwd_Texts[ 17], "%s", (panning) ? msgYes : msgNo);
                        MySetContents(amdwd->amdwd_Texts[ 18], "%s", (hifi) ? msgYes : msgNo);
                        MySetContents(amdwd->amdwd_Texts[ 19], "%s", (pingpong) ? msgYes : msgNo);
                        MySetContents(amdwd->amdwd_Texts[ 20], "%s", (record) ? msgYes : msgNo);
                        MySetContents(amdwd->amdwd_Texts[ 21], "%s", (fullduplex) ? msgYes : msgNo);
                        MySetContents(amdwd->amdwd_Texts[ 22], "%s", (realtime) ? msgYes : msgNo);

                        DROPINTERFACE(IAHI);
                    }

                    CloseDevice((struct IORequest *)ahir);
                #if defined(NEED_GLOBAL_AHIBASE)
                    AHIBase = NULL;
                #endif
                } else {
                    MyRequest(msgErrorContinue, msgCantOpenAHIDevice, AHINAME, ahir->ahir_Version);
                }

                DeleteIORequest((struct IORequest *)ahir);
            }

            DeleteMsgPort(mp);
        }

        tbFreeVecPooled(globalPool, tmp);
    }

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtAudioModesDetailTitle, ame->ame_Name, amdwd->amdwd_Title, sizeof(amdwd->amdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[30], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "AudioModes",
        MUIA_Window_ID, MakeID('.','A','M','D'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeID2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeDriver),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeVersion),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeAuthor),
                            Child, (IPTR)(texts[ 4] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeCopyright),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeAnnotation),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeBits2),
                            Child, (IPTR)(texts[ 7] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeMaxChannels),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeMixFrequency),
                            Child, (IPTR)(texts[ 9] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeMaxPlaySamples),
                            Child, (IPTR)(texts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeMaxRecordSamples),
                            Child, (IPTR)(texts[11] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeMonitorVolume),
                            Child, (IPTR)(texts[12] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeInputGain),
                            Child, (IPTR)(texts[13] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeOutputVolume),
                            Child, (IPTR)(texts[14] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeVolume),
                            Child, (IPTR)(texts[15] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeStereo),
                            Child, (IPTR)(texts[17] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModePanning),
                            Child, (IPTR)(texts[19] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeHiFi),
                            Child, (IPTR)(texts[21] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModePingPong),
                            Child, (IPTR)(texts[16] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeRecord),
                            Child, (IPTR)(texts[18] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeFullDuplex),
                            Child, (IPTR)(texts[20] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAudioModeRealTime),
                            Child, (IPTR)(texts[22] = MyTextObject6()),
                        End,
                    End,
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct AudioModesDetailWinData *amdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, amdwd->amdwd_Texts, sizeof(amdwd->amdwd_Texts));

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_DefaultObject, group);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
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

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct AudioModesDetailWinData *amdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_AudioModesDetailWin_AudioMode:
                amdwd->amdwd_AudioMode = (struct AudioModeEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return (DoSuperMethodA(cl, obj, (Msg)msg));
}

DISPATCHER(AudioModesDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

APTR MakeAudioModesDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct AudioModesDetailWinData), ENTRY(AudioModesDetailWinDispatcher));
}

