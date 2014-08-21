#ifndef SETTINGS_MCC_H
#define SETTINGS_MCC_H

#include "mcc/classes.h"

#define MUIA_VPDFSettings_Updated     (MUIA_VPDF_TagBase + 1)
#define MUIA_VPDFSettings_File        (MUIA_VPDF_TagBase + 2)

#define MUIM_VPDFSettings_Use         (MUIM_VPDFSettings_Dummy + 1)
#define MUIM_VPDFSettings_Save        (MUIM_VPDFSettings_Dummy + 2)
#define MUIM_VPDFSettings_Test        (MUIM_VPDFSettings_Dummy + 3)
#define MUIM_VPDFSettings_Cancel      (MUIM_VPDFSettings_Dummy + 4)

#define MUIM_VPDFSettings_FromObjects (MUIM_VPDFSettings_Dummy + 5)
#define MUIM_VPDFSettings_ToObjects   (MUIM_VPDFSettings_Dummy + 6)

#define	VPDFSettingsObject NewObject(getVPDFSettingsClass(), NULL

#define MUIA_VPDFSettings_First       (MUIA_VPDF_TagBase + 32)
#define MUIA_VPDFSettings_Layout      (MUIA_VPDF_TagBase + 33)
#define MUIA_VPDFSettings_Scaling     (MUIA_VPDF_TagBase + 34)
#define MUIA_VPDFSettings_Outline     (MUIA_VPDF_TagBase + 35)
#define MUIA_VPDFSettings_PSName      (MUIA_VPDF_TagBase + 36)
#define MUIA_VPDFSettings_OpenLog     (MUIA_VPDF_TagBase + 37)
#define MUIA_VPDFSettings_PrintingMode (MUIA_VPDF_TagBase + 38)
#define MUIA_VPDFSettings_PSMode      (MUIA_VPDF_TagBase + 39)
#define MUIA_VPDFSettings_Last        (MUIA_VPDF_TagBase + 40)

struct MUIP_VPDFSettings_FromObjects{ULONG MethodID;};
struct MUIP_VPDFSettings_ToObjects{ULONG MethodID;};
struct MUIP_VPDFSettings_Save{ULONG MethodID;};
struct MUIP_VPDFSettings_Use{ULONG MethodID;};
struct MUIP_VPDFSettings_Cancel{ULONG MethodID;};

DEFCLASS(VPDFSettings);

enum {
	ETYPE_NUMERIC = 0,
	ETYPE_STRING,
};


enum {
	VPDF_SGROUP_LAYOUT = 1,
	VPDF_SGROUP_SCALING = 2,
	VPDF_SGROUP_GUI = 3,
	VPDF_SGROUP_PRINTER = 4,
};

#endif
