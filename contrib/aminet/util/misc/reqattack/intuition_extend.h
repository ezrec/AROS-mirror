#ifndef INTUITION_EXTEND_H
#define INTUITION_EXTEND_H

#include <intuition/intuition.h>

#define WA_ExtraGadget_Iconify			(WA_Dummy + 153)
#define WA_ExtraGadget_PopUp			(WA_Dummy + 156)

#define ETI_Dummy		(0xFFD0) /* you can change this base with WA_ExtraGadgetsStartID! */

#define ETI_Iconify		(ETI_Dummy		)
#define ETI_PopUp		(ETI_Dummy + 3	)

#endif
