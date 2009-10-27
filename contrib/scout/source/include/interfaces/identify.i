#ifndef IDENTIFY_INTERFACE_DEF_H
#define IDENTIFY_INTERFACE_DEF_H
/*
** This file is machine generated from idltool
** Do not edit
*/ 

#include <exec/types.i>
#include <exec/exec.i>
#include <exec/interfaces.i>

STRUCTURE IdentifyIFace, InterfaceData_SIZE
	    FPTR IIdentify_Obtain
	    FPTR IIdentify_Release
	    FPTR IIdentify_Expunge
	    FPTR IIdentify_Clone
	    FPTR IIdentify_IdExpansion
	    FPTR IIdentify_IdExpansionTags
	    FPTR IIdentify_IdHardware
	    FPTR IIdentify_IdHardwareTags
	    FPTR IIdentify_IdAlert
	    FPTR IIdentify_IdAlertTags
	    FPTR IIdentify_IdFunction
	    FPTR IIdentify_IdFunctionTags
	    FPTR IIdentify_IdHardwareNum
	    FPTR IIdentify_IdHardwareNumTags
	    FPTR IIdentify_IdHardwareUpdate
	    FPTR IIdentify_IdFormatString
	    FPTR IIdentify_IdFormatStringTags
	    FPTR IIdentify_IdEstimateFormatSize
	    FPTR IIdentify_IdEstimateFormatSizeTags
	LABEL IdentifyIFace_SIZE

#endif
