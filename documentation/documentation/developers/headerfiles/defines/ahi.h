/* Automatically generated header! Do not edit! */

#ifndef _INLINE_AHI_H
#define _INLINE_AHI_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef AHI_BASE_NAME
#define AHI_BASE_NAME AHIBase
#endif /* !AHI_BASE_NAME */

#define AHI_AllocAudioA(___tagList) __AHI_AllocAudioA_WB(AHI_BASE_NAME, ___tagList)
#define __AHI_AllocAudioA_WB(___base, ___tagList) \
	AROS_LC1(struct AHIAudioCtrl *, AHI_AllocAudioA, \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 7, Ahi)

#ifndef NO_INLINE_STDARG
#define AHI_AllocAudio(___tag1, ...) __AHI_AllocAudio_WB(AHI_BASE_NAME, ___tag1, ## __VA_ARGS__)
#define __AHI_AllocAudio_WB(___base, ___tag1, ...) \
	({IPTR _tags[] = { (IPTR) ___tag1, ## __VA_ARGS__ }; __AHI_AllocAudioA_WB((___base), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define AHI_FreeAudio(___AudioCtrl) __AHI_FreeAudio_WB(AHI_BASE_NAME, ___AudioCtrl)
#define __AHI_FreeAudio_WB(___base, ___AudioCtrl) \
	AROS_LC1NR(void, AHI_FreeAudio, \
	AROS_LCA(struct AHIAudioCtrl *, (___AudioCtrl), A2), \
	struct Library *, (___base), 8, Ahi)

#define AHI_ControlAudioA(___AudioCtrl, ___tagList) __AHI_ControlAudioA_WB(AHI_BASE_NAME, ___AudioCtrl, ___tagList)
#define __AHI_ControlAudioA_WB(___base, ___AudioCtrl, ___tagList) \
	AROS_LC2(ULONG, AHI_ControlAudioA, \
	AROS_LCA(struct AHIAudioCtrl *, (___AudioCtrl), A2), \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 10, Ahi)

#ifndef NO_INLINE_STDARG
#define AHI_ControlAudio(___AudioCtrl, ___tag1, ...) __AHI_ControlAudio_WB(AHI_BASE_NAME, ___AudioCtrl, ___tag1, ## __VA_ARGS__)
#define __AHI_ControlAudio_WB(___base, ___AudioCtrl, ___tag1, ...) \
	({IPTR _tags[] = { (IPTR) ___tag1, ## __VA_ARGS__ }; __AHI_ControlAudioA_WB((___base), (___AudioCtrl), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define AHI_SetVol(___Channel, ___Volume, ___Pan, ___AudioCtrl, ___Flags) __AHI_SetVol_WB(AHI_BASE_NAME, ___Channel, ___Volume, ___Pan, ___AudioCtrl, ___Flags)
#define __AHI_SetVol_WB(___base, ___Channel, ___Volume, ___Pan, ___AudioCtrl, ___Flags) \
	AROS_LC5NR(void, AHI_SetVol, \
	AROS_LCA(UWORD, (___Channel), D0), \
	AROS_LCA(Fixed, (___Volume), D1), \
	AROS_LCA(sposition, (___Pan), D2), \
	AROS_LCA(struct AHIAudioCtrl *, (___AudioCtrl), A2), \
	AROS_LCA(ULONG, (___Flags), D3), \
	struct Library *, (___base), 11, Ahi)

#define AHI_SetFreq(___Channel, ___Freq, ___AudioCtrl, ___Flags) __AHI_SetFreq_WB(AHI_BASE_NAME, ___Channel, ___Freq, ___AudioCtrl, ___Flags)
#define __AHI_SetFreq_WB(___base, ___Channel, ___Freq, ___AudioCtrl, ___Flags) \
	AROS_LC4NR(void, AHI_SetFreq, \
	AROS_LCA(UWORD, (___Channel), D0), \
	AROS_LCA(ULONG, (___Freq), D1), \
	AROS_LCA(struct AHIAudioCtrl *, (___AudioCtrl), A2), \
	AROS_LCA(ULONG, (___Flags), D2), \
	struct Library *, (___base), 12, Ahi)

#define AHI_SetSound(___Channel, ___Sound, ___Offset, ___Length, ___AudioCtrl, ___Flags) __AHI_SetSound_WB(AHI_BASE_NAME, ___Channel, ___Sound, ___Offset, ___Length, ___AudioCtrl, ___Flags)
#define __AHI_SetSound_WB(___base, ___Channel, ___Sound, ___Offset, ___Length, ___AudioCtrl, ___Flags) \
	AROS_LC6NR(void, AHI_SetSound, \
	AROS_LCA(UWORD, (___Channel), D0), \
	AROS_LCA(UWORD, (___Sound), D1), \
	AROS_LCA(ULONG, (___Offset), D2), \
	AROS_LCA(LONG, (___Length), D3), \
	AROS_LCA(struct AHIAudioCtrl *, (___AudioCtrl), A2), \
	AROS_LCA(ULONG, (___Flags), D4), \
	struct Library *, (___base), 13, Ahi)

#define AHI_SetEffect(___Effect, ___AudioCtrl) __AHI_SetEffect_WB(AHI_BASE_NAME, ___Effect, ___AudioCtrl)
#define __AHI_SetEffect_WB(___base, ___Effect, ___AudioCtrl) \
	AROS_LC2(ULONG, AHI_SetEffect, \
	AROS_LCA(APTR, (___Effect), A0), \
	AROS_LCA(struct AHIAudioCtrl *, (___AudioCtrl), A2), \
	struct Library *, (___base), 14, Ahi)

#define AHI_LoadSound(___Sound, ___Type, ___Info, ___AudioCtrl) __AHI_LoadSound_WB(AHI_BASE_NAME, ___Sound, ___Type, ___Info, ___AudioCtrl)
#define __AHI_LoadSound_WB(___base, ___Sound, ___Type, ___Info, ___AudioCtrl) \
	AROS_LC4(ULONG, AHI_LoadSound, \
	AROS_LCA(UWORD, (___Sound), D0), \
	AROS_LCA(ULONG, (___Type), D1), \
	AROS_LCA(APTR, (___Info), A0), \
	AROS_LCA(struct AHIAudioCtrl *, (___AudioCtrl), A2), \
	struct Library *, (___base), 15, Ahi)

#define AHI_UnloadSound(___Sound, ___Audioctrl) __AHI_UnloadSound_WB(AHI_BASE_NAME, ___Sound, ___Audioctrl)
#define __AHI_UnloadSound_WB(___base, ___Sound, ___Audioctrl) \
	AROS_LC2NR(void, AHI_UnloadSound, \
	AROS_LCA(UWORD, (___Sound), D0), \
	AROS_LCA(struct AHIAudioCtrl *, (___Audioctrl), A2), \
	struct Library *, (___base), 16, Ahi)

#define AHI_NextAudioID(___Last_ID) __AHI_NextAudioID_WB(AHI_BASE_NAME, ___Last_ID)
#define __AHI_NextAudioID_WB(___base, ___Last_ID) \
	AROS_LC1(ULONG, AHI_NextAudioID, \
	AROS_LCA(ULONG, (___Last_ID), D0), \
	struct Library *, (___base), 17, Ahi)

#define AHI_GetAudioAttrsA(___ID, ___Audioctrl, ___tagList) __AHI_GetAudioAttrsA_WB(AHI_BASE_NAME, ___ID, ___Audioctrl, ___tagList)
#define __AHI_GetAudioAttrsA_WB(___base, ___ID, ___Audioctrl, ___tagList) \
	AROS_LC3(BOOL, AHI_GetAudioAttrsA, \
	AROS_LCA(ULONG, (___ID), D0), \
	AROS_LCA(struct AHIAudioCtrl *, (___Audioctrl), A2), \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 18, Ahi)

#ifndef NO_INLINE_STDARG
#define AHI_GetAudioAttrs(___ID, ___Audioctrl, ___tag1, ...) __AHI_GetAudioAttrs_WB(AHI_BASE_NAME, ___ID, ___Audioctrl, ___tag1, ## __VA_ARGS__)
#define __AHI_GetAudioAttrs_WB(___base, ___ID, ___Audioctrl, ___tag1, ...) \
	({IPTR _tags[] = { (IPTR) ___tag1, ## __VA_ARGS__ }; __AHI_GetAudioAttrsA_WB((___base), (___ID), (___Audioctrl), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define AHI_BestAudioIDA(___tagList) __AHI_BestAudioIDA_WB(AHI_BASE_NAME, ___tagList)
#define __AHI_BestAudioIDA_WB(___base, ___tagList) \
	AROS_LC1(ULONG, AHI_BestAudioIDA, \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 19, Ahi)

#ifndef NO_INLINE_STDARG
#define AHI_BestAudioID(___tag1, ...) __AHI_BestAudioID_WB(AHI_BASE_NAME, ___tag1, ## __VA_ARGS__)
#define __AHI_BestAudioID_WB(___base, ___tag1, ...) \
	({IPTR _tags[] = { (IPTR) ___tag1, ## __VA_ARGS__ }; __AHI_BestAudioIDA_WB((___base), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define AHI_AllocAudioRequestA(___tagList) __AHI_AllocAudioRequestA_WB(AHI_BASE_NAME, ___tagList)
#define __AHI_AllocAudioRequestA_WB(___base, ___tagList) \
	AROS_LC1(struct AHIAudioModeRequester *, AHI_AllocAudioRequestA, \
	AROS_LCA(struct TagItem *, (___tagList), A0), \
	struct Library *, (___base), 20, Ahi)

#ifndef NO_INLINE_STDARG
#define AHI_AllocAudioRequest(___tag1, ...) __AHI_AllocAudioRequest_WB(AHI_BASE_NAME, ___tag1, ## __VA_ARGS__)
#define __AHI_AllocAudioRequest_WB(___base, ___tag1, ...) \
	({IPTR _tags[] = { (IPTR) ___tag1, ## __VA_ARGS__ }; __AHI_AllocAudioRequestA_WB((___base), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define AHI_AudioRequestA(___Requester, ___tagList) __AHI_AudioRequestA_WB(AHI_BASE_NAME, ___Requester, ___tagList)
#define __AHI_AudioRequestA_WB(___base, ___Requester, ___tagList) \
	AROS_LC2(BOOL, AHI_AudioRequestA, \
	AROS_LCA(struct AHIAudioModeRequester *, (___Requester), A0), \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 21, Ahi)

#ifndef NO_INLINE_STDARG
#define AHI_AudioRequest(___Requester, ___tag1, ...) __AHI_AudioRequest_WB(AHI_BASE_NAME, ___Requester, ___tag1, ## __VA_ARGS__)
#define __AHI_AudioRequest_WB(___base, ___Requester, ___tag1, ...) \
	({IPTR _tags[] = { (IPTR) ___tag1, ## __VA_ARGS__ }; __AHI_AudioRequestA_WB((___base), (___Requester), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define AHI_FreeAudioRequest(___Requester) __AHI_FreeAudioRequest_WB(AHI_BASE_NAME, ___Requester)
#define __AHI_FreeAudioRequest_WB(___base, ___Requester) \
	AROS_LC1NR(void, AHI_FreeAudioRequest, \
	AROS_LCA(struct AHIAudioModeRequester *, (___Requester), A0), \
	struct Library *, (___base), 22, Ahi)

#define AHI_PlayA(___Audioctrl, ___tagList) __AHI_PlayA_WB(AHI_BASE_NAME, ___Audioctrl, ___tagList)
#define __AHI_PlayA_WB(___base, ___Audioctrl, ___tagList) \
	AROS_LC2NR(void, AHI_PlayA, \
	AROS_LCA(struct AHIAudioCtrl *, (___Audioctrl), A2), \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 23, Ahi)

#ifndef NO_INLINE_STDARG
#define AHI_Play(___Audioctrl, ___tag1, ...) __AHI_Play_WB(AHI_BASE_NAME, ___Audioctrl, ___tag1, ## __VA_ARGS__)
#define __AHI_Play_WB(___base, ___Audioctrl, ___tag1, ...) \
	({IPTR _tags[] = { (IPTR) ___tag1, ## __VA_ARGS__ }; __AHI_PlayA_WB((___base), (___Audioctrl), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define AHI_SampleFrameSize(___SampleType) __AHI_SampleFrameSize_WB(AHI_BASE_NAME, ___SampleType)
#define __AHI_SampleFrameSize_WB(___base, ___SampleType) \
	AROS_LC1(ULONG, AHI_SampleFrameSize, \
	AROS_LCA(ULONG, (___SampleType), D0), \
	struct Library *, (___base), 24, Ahi)

#define AHI_AddAudioMode(___AHIPrivate) __AHI_AddAudioMode_WB(AHI_BASE_NAME, ___AHIPrivate)
#define __AHI_AddAudioMode_WB(___base, ___AHIPrivate) \
	AROS_LC1(ULONG, AHI_AddAudioMode, \
	AROS_LCA(struct TagItem *, (___AHIPrivate), A0), \
	struct Library *, (___base), 25, Ahi)

#define AHI_RemoveAudioMode(___AHIPrivate) __AHI_RemoveAudioMode_WB(AHI_BASE_NAME, ___AHIPrivate)
#define __AHI_RemoveAudioMode_WB(___base, ___AHIPrivate) \
	AROS_LC1(ULONG, AHI_RemoveAudioMode, \
	AROS_LCA(ULONG, (___AHIPrivate), D0), \
	struct Library *, (___base), 26, Ahi)

#define AHI_LoadModeFile(___AHIPrivate) __AHI_LoadModeFile_WB(AHI_BASE_NAME, ___AHIPrivate)
#define __AHI_LoadModeFile_WB(___base, ___AHIPrivate) \
	AROS_LC1(ULONG, AHI_LoadModeFile, \
	AROS_LCA(STRPTR, (___AHIPrivate), A0), \
	struct Library *, (___base), 27, Ahi)

#endif /* !_INLINE_AHI_H */
