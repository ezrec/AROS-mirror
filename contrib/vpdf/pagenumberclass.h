#ifndef NUMBERGENERATOR_MCC_H
#define NUMBERGENERATOR_MCC_H

#include "mcc/classes.h"

/*
#define	MUIA_VPDFNumberGeneratorPageSkip               (MUIA_VPDFNumberGenerator_TagBase + 1 )

#define MUIV_VPDFNumberGeneratorPageSkip_All	0
#define MUIV_VPDFNumberGeneratorPageSkip_Odd    1
#define MUIV_VPDFNumberGeneratorPageSkip_Even   2
*/

#define	MUIA_VPDFNumberGeneratorPageStart              (MUIA_VPDFNumberGenerator_TagBase + 2 )
#define	MUIA_VPDFNumberGeneratorPageEnd                (MUIA_VPDFNumberGenerator_TagBase + 3 )
#define MUIA_VPDFNumberGeneratorPageMax 			   (MUIA_VPDFNumberGenerator_TagBase + 4 )

#define MUIM_VPDFNumberGeneratorFirst                  (MUIM_VPDFNumberGenerator_Dummy + 1)
#define MUIM_VPDFNumberGeneratorNext                   (MUIM_VPDFNumberGenerator_Dummy + 2)

struct MUIP_VPDFNumberGeneratorFirst{ULONG MethodID; LONG reverse;};
struct MUIP_VPDFNumberGeneratorNext{ULONG MethodID;};

#define	VPDFNumberGenerator   NewObject(getVPDFNumberGeneratorClass(), NULL



DEFCLASS(VPDFNumberGenerator);

#endif