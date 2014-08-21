#ifndef PRINTER_MCC_H
#define PRINTER_MCC_H

#include "mcc/classes.h"


#define	MUIM_VPDFPrinter_PrintDocument	       (MUIM_VPDFPrinter_Dummy + 1)
#define	MUIM_VPDFPrinter_Start			       (MUIM_VPDFPrinter_Dummy + 2)
#define MUIM_VPDFPrinter_Stop                  (MUIM_VPDFPrinter_Dummy + 3)
#define MUIM_VPDFPrinter_Done                  (MUIM_VPDFPrinter_Dummy + 4)
#define MUIM_VPDFPrinter_StatusUpdate          (MUIM_VPDFPrinter_Dummy + 5)
#define MUIM_VPDFPrinter_Close                 (MUIM_VPDFPrinter_Dummy + 6)
#define MUIM_VPDFPrinter_PrinterPrefs          (MUIM_VPDFPrinter_Dummy + 7)
#define MUIM_VPDFPrinter_HideCollate  	       (MUIM_VPDFPrinter_Dummy + 8)
#define MUIM_VPDFPrinter_Error                 (MUIM_VPDFPrinter_Dummy + 9)


#define MUIA_VPDFPrinter_PSName  	           (MUIM_VPDFPrinter_Dummy + 20)
#define MUIA_VPDFPrinter_PrintingMode  	       (MUIM_VPDFPrinter_Dummy + 21)
#define MUIA_VPDFPrinter_PSMode  	           (MUIM_VPDFPrinter_Dummy + 22)

struct MUIP_VPDFPrinter_PrintDocument{ULONG MethodID; APTR doc;};
struct MUIP_VPDFPrinter_Start{ULONG MethodID;};
struct MUIP_VPDFPrinter_Stop{ULONG MethodID;};
struct MUIP_VPDFPrinter_Done{ULONG MethodID;};
struct MUIP_VPDFPrinter_StatusUpdate{ULONG MethodID; ULONG page_number; ULONG actual_page;};
struct MUIP_VPDFPrinter_Close{ULONG MethodID;};
struct MUIP_VPDFPrinter_PrinterPrefs{ULONG MethodID;};
struct MUIP_VPDFPrinter_HideCollate{ULONG MethodID; ULONG copies;};
struct MUIP_VPDFPrinter_Error{ULONG MethodID;};

#define	VPDFPrinterObject   NewObject(getVPDFPrinterClass(), NULL

DEFCLASS(VPDFPrinter);

#endif