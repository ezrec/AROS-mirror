/* Automatically generated header! Do not edit! */

#ifndef _INLINE_AHI_SUB_H
#define _INLINE_AHI_SUB_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef AHI_SUB_BASE_NAME
#define AHI_SUB_BASE_NAME AHIsubBase
#endif /* !AHI_SUB_BASE_NAME */

#define AHIsub_AllocAudio(___tagList, ___AudioCtrl) __AHIsub_AllocAudio_WB(AHI_SUB_BASE_NAME, ___tagList, ___AudioCtrl)
#define __AHIsub_AllocAudio_WB(___base, ___tagList, ___AudioCtrl) \
	AROS_LC2(ULONG, AHIsub_AllocAudio, \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 5, Ahi_sub)

#define AHIsub_FreeAudio(___AudioCtrl) __AHIsub_FreeAudio_WB(AHI_SUB_BASE_NAME, ___AudioCtrl)
#define __AHIsub_FreeAudio_WB(___base, ___AudioCtrl) \
	AROS_LC1NR(void, AHIsub_FreeAudio, \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 6, Ahi_sub)

#define AHIsub_Disable(___AudioCtrl) __AHIsub_Disable_WB(AHI_SUB_BASE_NAME, ___AudioCtrl)
#define __AHIsub_Disable_WB(___base, ___AudioCtrl) \
	AROS_LC1NR(void, AHIsub_Disable, \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 7, Ahi_sub)

#define AHIsub_Enable(___AudioCtrl) __AHIsub_Enable_WB(AHI_SUB_BASE_NAME, ___AudioCtrl)
#define __AHIsub_Enable_WB(___base, ___AudioCtrl) \
	AROS_LC1NR(void, AHIsub_Enable, \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 8, Ahi_sub)

#define AHIsub_Start(___Flags, ___AudioCtrl) __AHIsub_Start_WB(AHI_SUB_BASE_NAME, ___Flags, ___AudioCtrl)
#define __AHIsub_Start_WB(___base, ___Flags, ___AudioCtrl) \
	AROS_LC2(ULONG, AHIsub_Start, \
	AROS_LCA(ULONG, (___Flags), D0), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 9, Ahi_sub)

#define AHIsub_Update(___Flags, ___AudioCtrl) __AHIsub_Update_WB(AHI_SUB_BASE_NAME, ___Flags, ___AudioCtrl)
#define __AHIsub_Update_WB(___base, ___Flags, ___AudioCtrl) \
	AROS_LC2(ULONG, AHIsub_Update, \
	AROS_LCA(ULONG, (___Flags), D0), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 10, Ahi_sub)

#define AHIsub_Stop(___Flags, ___AudioCtrl) __AHIsub_Stop_WB(AHI_SUB_BASE_NAME, ___Flags, ___AudioCtrl)
#define __AHIsub_Stop_WB(___base, ___Flags, ___AudioCtrl) \
	AROS_LC2(ULONG, AHIsub_Stop, \
	AROS_LCA(ULONG, (___Flags), D0), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 11, Ahi_sub)

#define AHIsub_SetVol(___Channel, ___Volume, ___Pan, ___AudioCtrl, ___Flags) __AHIsub_SetVol_WB(AHI_SUB_BASE_NAME, ___Channel, ___Volume, ___Pan, ___AudioCtrl, ___Flags)
#define __AHIsub_SetVol_WB(___base, ___Channel, ___Volume, ___Pan, ___AudioCtrl, ___Flags) \
	AROS_LC5(ULONG, AHIsub_SetVol, \
	AROS_LCA(UWORD, (___Channel), D0), \
	AROS_LCA(Fixed, (___Volume), D1), \
	AROS_LCA(sposition, (___Pan), D2), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	AROS_LCA(ULONG, (___Flags), D3), \
	struct Library *, (___base), 12, Ahi_sub)

#define AHIsub_SetFreq(___Channel, ___Freq, ___AudioCtrl, ___Flags) __AHIsub_SetFreq_WB(AHI_SUB_BASE_NAME, ___Channel, ___Freq, ___AudioCtrl, ___Flags)
#define __AHIsub_SetFreq_WB(___base, ___Channel, ___Freq, ___AudioCtrl, ___Flags) \
	AROS_LC4(ULONG, AHIsub_SetFreq, \
	AROS_LCA(UWORD, (___Channel), D0), \
	AROS_LCA(ULONG, (___Freq), D1), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	AROS_LCA(ULONG, (___Flags), D2), \
	struct Library *, (___base), 13, Ahi_sub)

#define AHIsub_SetSound(___Channel, ___Sound, ___Offset, ___Length, ___AudioCtrl, ___Flags) __AHIsub_SetSound_WB(AHI_SUB_BASE_NAME, ___Channel, ___Sound, ___Offset, ___Length, ___AudioCtrl, ___Flags)
#define __AHIsub_SetSound_WB(___base, ___Channel, ___Sound, ___Offset, ___Length, ___AudioCtrl, ___Flags) \
	AROS_LC6(ULONG, AHIsub_SetSound, \
	AROS_LCA(UWORD, (___Channel), D0), \
	AROS_LCA(UWORD, (___Sound), D1), \
	AROS_LCA(ULONG, (___Offset), D2), \
	AROS_LCA(LONG, (___Length), D3), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	AROS_LCA(ULONG, (___Flags), D4), \
	struct Library *, (___base), 14, Ahi_sub)

#define AHIsub_SetEffect(___Effect, ___AudioCtrl) __AHIsub_SetEffect_WB(AHI_SUB_BASE_NAME, ___Effect, ___AudioCtrl)
#define __AHIsub_SetEffect_WB(___base, ___Effect, ___AudioCtrl) \
	AROS_LC2(ULONG, AHIsub_SetEffect, \
	AROS_LCA(APTR, (___Effect), A0), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 15, Ahi_sub)

#define AHIsub_LoadSound(___Sound, ___Type, ___Info, ___AudioCtrl) __AHIsub_LoadSound_WB(AHI_SUB_BASE_NAME, ___Sound, ___Type, ___Info, ___AudioCtrl)
#define __AHIsub_LoadSound_WB(___base, ___Sound, ___Type, ___Info, ___AudioCtrl) \
	AROS_LC4(ULONG, AHIsub_LoadSound, \
	AROS_LCA(UWORD, (___Sound), D0), \
	AROS_LCA(ULONG, (___Type), D1), \
	AROS_LCA(APTR, (___Info), A0), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 16, Ahi_sub)

#define AHIsub_UnloadSound(___Sound, ___Audioctrl) __AHIsub_UnloadSound_WB(AHI_SUB_BASE_NAME, ___Sound, ___Audioctrl)
#define __AHIsub_UnloadSound_WB(___base, ___Sound, ___Audioctrl) \
	AROS_LC2(ULONG, AHIsub_UnloadSound, \
	AROS_LCA(UWORD, (___Sound), D0), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___Audioctrl), A2), \
	struct Library *, (___base), 17, Ahi_sub)

#define AHIsub_GetAttr(___Attribute, ___Argument, ___DefValue, ___tagList, ___AudioCtrl) __AHIsub_GetAttr_WB(AHI_SUB_BASE_NAME, ___Attribute, ___Argument, ___DefValue, ___tagList, ___AudioCtrl)
#define __AHIsub_GetAttr_WB(___base, ___Attribute, ___Argument, ___DefValue, ___tagList, ___AudioCtrl) \
	AROS_LC5(LONG, AHIsub_GetAttr, \
	AROS_LCA(ULONG, (___Attribute), D0), \
	AROS_LCA(LONG, (___Argument), D1), \
	AROS_LCA(LONG, (___DefValue), D2), \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 18, Ahi_sub)

#define AHIsub_HardwareControl(___Attribute, ___Argument, ___AudioCtrl) __AHIsub_HardwareControl_WB(AHI_SUB_BASE_NAME, ___Attribute, ___Argument, ___AudioCtrl)
#define __AHIsub_HardwareControl_WB(___base, ___Attribute, ___Argument, ___AudioCtrl) \
	AROS_LC3(LONG, AHIsub_HardwareControl, \
	AROS_LCA(ULONG, (___Attribute), D0), \
	AROS_LCA(LONG, (___Argument), D1), \
	AROS_LCA(struct AHIAudioCtrlDrv *, (___AudioCtrl), A2), \
	struct Library *, (___base), 19, Ahi_sub)

#endif /* !_INLINE_AHI_SUB_H */
