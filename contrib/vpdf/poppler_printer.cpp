
#include "Object.h"

#include <proto/charsets.h>
#define AROS_ALMOST_COMPATIBLE
#define _NO_PPCINLINE
#include <proto/keymap.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/iffparse.h>
#include <dos/stdio.h>
#include <clib/debug_protos.h>
#include <exec/semaphores.h>
#include <exec/lists.h>
#include <prefs/prefhdr.h>
#include <constructor.h>
#include <prefs/printertxt.h>

#define USE_FLOAT 1
#include <poppler-config.h>
#include <config.h>

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>

#include "PDFDoc.h"
#include "Outline.h"
#include "Link.h"
#include "GlobalParams.h"
#include "PDFDocEncoding.h"
#include "goo/GooList.h"

//#define USE_SPLASH
#ifdef USE_SPLASH
#include "splash/SplashBitmap.h"
#include "splash/Splash.h"
#include "SplashOutputDev.h"
#include "TextOutputDev.h"
#else
#include "CairoOutputDev.h"
#endif
#include "PSOutputDev.h"

#include "poppler.h"
#include "poppler_io.h"
#include "poppler_device.h"
#include "poppler_printer.h"

extern struct Library *LocaleBase;
#define LOCALE_BASE_NAME LocaleBase
#define D(x) x
#warning TODO: make the semaphore per-document
extern struct SignalSemaphore semaphore;

extern struct Library *CairoBase;


// turboprint includes
#include <devices/printer.h>
#include <devices/prtbase.h>
#include <datatypes/datatypesclass.h>
#include "turboprint.h"

#if 1
#define ENTER_SECTION \
{ ObtainSemaphore(&semaphore);}
#else // time delay needed to enter critical section
#define ENTER_SECTION \
{ clock_t t0 = clock(); ObtainSemaphore(&semaphore); t0 = clock() - t0; kprintf("section enter:%f:%s\n", (float)t0/CLOCKS_PER_SEC, __FUNCTION__);}
#endif

//#define ENTER_SECTION
#define LEAVE_SECTION ReleaseSemaphore(&semaphore);
//#define LEAVE_SECTION

#define OUTPUTBUFFERSIZE 262144

typedef struct
{
    int w;
    int h;
} PAGE_SIZES;
 
static const PAGE_SIZES page_sizes[]= {
{1,1 },  //PS_US_LETTER
{1,1 }, //PS_US_LEGAL
{1,1}, //PS_N_TRACTOR
{1,1 }, //PS_W_TRACTOR
{1,1 }, //PS_CUSTOM
{841,1189}, //PS_EURO_A0
{594,841 }, //PS_EURO_A1
{420,594}, //PS_EURO_A2
{297,420}, //PS_EURO_A3
{210,297 }, //PS_EURO_A4
{148,210 }, //PS_EURO_A5
{1,1, }, //PS_EURO_A6
{1,1}, //PS_EURO_A7
{1,1 }, //PS_EURO_A8
};

int get_pagesize()
{
    struct IFFHandle *iffhandle;
    struct StoredProperty *sp;
    int ifferror;
    struct PrinterTxtPrefs tmp;
    int ret = -1;
    
    if (iffhandle = AllocIFF()) 
    {
        if (iffhandle->iff_Stream = (LONG)Open("env:sys/printer.prefs", MODE_OLDFILE)) 
        {     
			D(kprintf("prefs file  opened\n"));
            InitIFFasDOS (iffhandle);
            if ((ifferror = OpenIFF (iffhandle, IFFF_READ)) == 0) 
            {
                D(kprintf("iff opened\n"));
                PropChunk(iffhandle, ID_PREF, ID_PTXT);
      
                for (;;) 
                {
                    ifferror = ParseIFF(iffhandle, IFFPARSE_STEP);

                    if (ifferror == IFFERR_EOC)
                        continue;
                    else if (ifferror)
                        break;
                  
       
                    if (sp = FindProp(iffhandle, ID_PREF, ID_PTXT)) 
                    {
                        ret = ((struct PrinterTxtPrefs *)sp->sp_Data)->pt_PaperSize;
                    }
                }
                CloseIFF(iffhandle);
                
            }
            Close (iffhandle->iff_Stream);
        }
        FreeIFF(iffhandle);
    }
    return ret;
}


struct printercontext
{
	ULONG format;
	PDFDoc *doc;

	PSOutputDev *dev;
	BPTR file;

	cairo_surface_t *surface;
	cairo_t *cairo;
	CairoOutputDev *cairo_dev;
	struct MsgPort *PrinterMP;
	union printerIO *PIO;
	struct PrinterData *PD;
	struct TPExtIODRP ExtIoDrp;
	BOOL TP_Installed;
	UWORD TPVersion;
	int last_page;
	int page_size;
	char buffer[OUTPUTBUFFERSIZE];
};


static void outputstreamfunc(void *stream, const char *data, int len)
{
	struct printercontext *pctx = (struct printercontext*)stream;
	FWrite(pctx->file, (void*)data, (LONG)len, 1);
}

struct printercontext *init_turboprint(struct printercontext *pctx)
{
    pctx->PrinterMP = NULL;
    pctx->PIO = NULL;

    D(kprintf("init_turboprint() started\n!"));
    if((pctx->PrinterMP = (struct MsgPort*)CreateMsgPort()))
    {
        if((pctx->PIO = (union printerIO *)CreateIORequest(pctx->PrinterMP, sizeof(union printerIO))))
        {
            if(!(OpenDevice((STRPTR)"printer.device", 0, (struct IORequest *)pctx->PIO, 0)))
            {
                pctx->PD = (struct PrinterData *)pctx->PIO->iodrp.io_Device;
                pctx->TP_Installed = (((ULONG *)(pctx->PD->pd_OldStk))[2] == TPMATCHWORD);
             
                pctx->TPVersion = pctx->PIO->iodrp.io_Device->dd_Library.lib_Version;
                //	pctx->TPIdString = (char*)(pctx->PIO->iodrp.io_Device->dd_Library.lib_IdString);

                if(pctx->TP_Installed && pctx->TPVersion >= 39)
                {
                    struct PrinterData *PD = pctx->PD;
                    
                    D(kprintf("PrinterName = '%s', Version=%u, Revision=%u\n",
                       PD->pd_SegmentData->ps_PED.ped_PrinterName, PD->pd_SegmentData->ps_Version,
                       PD->pd_SegmentData->ps_Revision));
                    D(kprintf("PrinterClass=%u, ColorClass=%u\n",
                       PD->pd_SegmentData->ps_PED.ped_PrinterClass, PD->pd_SegmentData->ps_PED.ped_ColorClass));
                    D(kprintf("MaxColumns=%u, NumCharSets=%u, NumRows=%u\n",
                       PD->pd_SegmentData->ps_PED.ped_MaxColumns, PD->pd_SegmentData->ps_PED.ped_NumCharSets, PD->pd_SegmentData->ps_PED.ped_NumRows));
                    D(kprintf("MaxXDots=%lu, MaxYDots=%lu, XDotsInch=%u, YDotsInch=%u\n",
                       PD->pd_SegmentData->ps_PED.ped_MaxXDots, PD->pd_SegmentData->ps_PED.ped_MaxYDots,
                       PD->pd_SegmentData->ps_PED.ped_XDotsInch, PD->pd_SegmentData->ps_PED.ped_YDotsInch));
                    D(kprintf("PrintLeftMargin=%d, PrintRightMargin=%d\n",
                       pctx->PD->pd_Preferences.PrintLeftMargin,pctx->PD->pd_Preferences.PrintRightMargin));
                       
                    D(kprintf("PrintAspect: %d\n",  PD->pd_Preferences.PrintAspect));
                    
                    pctx->PIO->iodrp.io_Command = PRD_TPEXTDUMPRPORT;
                    pctx->PIO->iodrp.io_SrcX = 0;  			// x offset in rastport to start printing from
                    pctx->PIO->iodrp.io_SrcY = 0;  			// y offset in rastpoty to start printing from
                    
                    // new: io.Modes must point to a new Structure (ExtIoDrp)
                    pctx->PIO->iodrp.io_Modes = (ULONG)&pctx->ExtIoDrp;

                    // fill in the new structure
                    pctx->ExtIoDrp.PixAspX = 1;   // for the correct aspect ratio
                    pctx->ExtIoDrp.PixAspY = 1;   // normally the values of the monitor-structure

                    pctx->PD->pd_Preferences.PrintFlags = (pctx->PD->pd_Preferences.PrintFlags & ~DIMENSIONS_MASK) | IGNORE_DIMENSIONS;
                    D(kprintf("init_turboprint() initiated\n!"));
                    
                    if (-1!=(pctx->page_size = get_pagesize()))      
						return pctx;
                }
				CloseDevice((struct IORequest *)pctx->PIO);
            }
            DeleteIORequest((struct IORequest *)pctx->PIO);
			pctx->PIO = NULL;
        }
        DeleteMsgPort(pctx->PrinterMP);
		pctx->PrinterMP = NULL;
    }
 
    D(kprintf("init_turboprint() bad!\n"));
    return NULL;
}

void *pdfPrintInit(void *_ctx, const char *path, int first, int last, struct printerjob *pj)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	struct printercontext *pctx = (struct printercontext*)calloc(1, sizeof(*pctx));

	if(pctx != NULL)
	{
		if(pj->mode == VPDF_PRINT_POSTSCRIPT_2 || pj->mode == VPDF_PRINT_POSTSCRIPT_3)
		{
			try
			{
				pctx->format = pj->mode;
				pctx->doc = ctx->doc;
				pctx->file = Open(path, MODE_NEWFILE);
				SetVBuf(pctx->file, pctx->buffer, BUF_FULL, OUTPUTBUFFERSIZE);

				if(pctx->file != NULL)
				{
					globalParams->setPSLevel((pctx->format == VPDF_PRINT_POSTSCRIPT_2)?psLevel2:psLevel3);
					pctx->dev = new PSOutputDev(outputstreamfunc, pctx, "vpdf ps document output", pctx->doc,
												first, last, psModePS);
				}
				else
				{
					free(pctx);
					return NULL;
				}
				
				//pctx->dev->startDoc(pctx->doc->getXRef());
			}
			catch(...)
			{
				if(pctx->file != NULL)
					Close(pctx->file);

				if(pctx->dev != NULL)
					delete pctx->dev;

				free(pctx);
				pctx = NULL;
			}
		}
		else  if (init_turboprint(pctx))
        {
            // Cairo device init 
            pctx->format = pj->mode;
            pctx->doc = ctx->doc;
            
            pctx->cairo_dev = new CairoOutputDev();
            pctx->cairo_dev->setPrinting(false);
            pctx->surface = NULL;
            pctx->last_page = -1; // indicate that we haven't printed anything yet               
            pctx->cairo = NULL;
            pctx->cairo_dev->startDoc(pctx->doc);
            return pctx;
        }
        else
        {
			free(pctx);
			return NULL;
        }
	}
	return pctx;
}



int pdfPrintPage(void *_pctx, int *_page, int center, int pages_per_sheet)
{
	int page = _page[0];
	struct printercontext *pctx = (struct printercontext*)_pctx;
	Page *pdfpage = pctx->doc->getCatalog()->getPage(page);

	ENTER_SECTION

	try
	{    
		if(pctx->format == VPDF_PRINT_POSTSCRIPT_2 || pctx->format == VPDF_PRINT_POSTSCRIPT_3 )
		{
			pctx->doc->displayPage(pctx->dev, page, 72, 72, 0, gTrue, gFalse, gTrue);
		}	
		else
		{
			struct RastPort MyRastPort;
			struct BitMap MyBitMap;
			char not_changed = FALSE;
			cairo_surface_t *surface_back=NULL;
            cairo_t *cairo_back = NULL;
	
            int slide2xmode = FALSE;
			int width_mm  = round((pdfpage->getMediaWidth()/72.0) *25.4);
			int height_mm = round((pdfpage->getMediaHeight()/72.0)*25.4); 
			
			int dpi_x = pctx->PD->pd_SegmentData->ps_PED.ped_XDotsInch;
			int dpi_y = pctx->PD->pd_SegmentData->ps_PED.ped_YDotsInch;      
			int printer_w_mm = round((pctx->PD->pd_SegmentData->ps_PED.ped_MaxXDots/dpi_y)*25.4);
			int printer_h_mm = round((pctx->PD->pd_SegmentData->ps_PED.ped_MaxYDots/dpi_y)*25.4);
			int landscape_mode = (width_mm>height_mm);
			float x_scale;
			float y_scale;

			if  (landscape_mode)
			{
				x_scale = (height_mm/(1.0*page_sizes[pctx->page_size].w));
				y_scale = (width_mm/(1.0*page_sizes[pctx->page_size].h));
			}
			else
			{  
				x_scale = (width_mm/(1.0*page_sizes[pctx->page_size].w));
				y_scale = (height_mm/(1.0*page_sizes[pctx->page_size].h));
			}
			KPrintF("X_SCALE: %f, Y_SCALE: %f\nPrinter W: %d, Printer H: %d, Page W: %d, Page H: %d\n",
			x_scale, y_scale, page_sizes[pctx->page_size].w, page_sizes[pctx->page_size].h, width_mm, height_mm);
			
			if (x_scale<1)
				x_scale=1;
			if (y_scale<1)
				y_scale=1;
			
			if (x_scale<y_scale)
				x_scale = y_scale;
			else
				y_scale = x_scale;
			D(kprintf("pdfPrintPage() turboprint mode: input w/h %d/%d\n!", width_mm,  height_mm));
			
			struct pdfBitmap bm;
		
			int width  = width_mm *dpi_x/25.4;
			int height = height_mm*dpi_y/25.4;
			
            if ((pages_per_sheet==2) && (height_mm<width_mm))
            {  
                   height *= 2;
                   height_mm *= 2;
                   D(kprintf(" slide mode 2x\n"));
                   slide2xmode = TRUE;
            }
        

			// check if we had to create new surface
			if(pctx->surface != NULL)
			{
				if(cairo_image_surface_get_width(pctx->surface) != width || cairo_image_surface_get_height(pctx->surface) != height)
				{
					cairo_surface_destroy(pctx->surface);
                    pctx->surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
					D(kprintf(" wrong sizes - new surface ok\n"));
				}
				else
				{
					not_changed = TRUE;
					D(kprintf(" not changed ok\n"));
				}
			}
			else
			{
				pctx->surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
				D(kprintf("new surface ok\n"));
			}
			
			
          	D(kprintf("pdfPrintPage() tuw\n"));
			if(pctx->surface == NULL)
			{
				LEAVE_SECTION
				return FALSE;
			}
		
			// check if we print the same page 2nd time
			if (pctx->last_page != page)
			{
				pctx->cairo_dev->setCairo(NULL);
				if (pctx->cairo)                    // if any context exist - delete context
					cairo_destroy(pctx->cairo);     
				pctx->cairo = cairo_create(pctx->surface); // create new cairo context from surface
				
				cairo_save(pctx->cairo);                   // save context on stack
				
				cairo_set_source_rgb(pctx->cairo,  1., 1., 1);  // change color to white
				cairo_paint(pctx->cairo);                  // paint all surface
				pctx->cairo_dev->setCairo(pctx->cairo);  
				D(kprintf("setCairo ok\n"));	
				D(kprintf("Display page: %d\n", page));
				
			    if (pages_per_sheet==2)
			    {  
                    surface_back = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height/2);
					if(surface_back == NULL)
					{
						LEAVE_SECTION
						return FALSE;
					}

			        cairo_back = cairo_create(surface_back);
			    	if (!slide2xmode)
			    	{
                        pctx->doc->displayPage(pctx->cairo_dev, _page[0], dpi_y/M_SQRT2, dpi_x/M_SQRT2, 90, gTrue, gFalse, gTrue);
                    }
                    else
                    {
                        pctx->doc->displayPage(pctx->cairo_dev, _page[0], dpi_x, dpi_y, 0, gTrue, gFalse, gTrue);
                    }
                    
                    cairo_restore(pctx->cairo);                // restore old cairo context
                    
                    if ((_page[1]>0))
                    {
                        cairo_save(cairo_back);                
                        cairo_set_source_rgb(cairo_back,  1., 1., 1);  // change color to white
                        cairo_paint(cairo_back);                  // paint all surface
                        pctx->cairo_dev->setCairo(cairo_back);
                        kprintf("Display page: %d\n", _page[1]);
                        
                        if (!slide2xmode)
                        {
                            pctx->doc->displayPage(pctx->cairo_dev, _page[1], dpi_y/M_SQRT2, dpi_x/M_SQRT2, 90, gTrue, gFalse, gTrue);
                        }
                        else
                        {
                            pctx->doc->displayPage(pctx->cairo_dev, _page[1], dpi_x, dpi_y, 0, gTrue, gFalse, gTrue);
                        } 
                       
                        cairo_restore(cairo_back);    
                        cairo_save(pctx->cairo);                   // save context on stack
                        cairo_set_source_surface(pctx->cairo, surface_back, 0, height/2);
                        cairo_paint (pctx->cairo);
                        cairo_restore(pctx->cairo);     
                    }
                }    
				else
                    pctx->doc->displayPage(pctx->cairo_dev, _page[0], dpi_x, dpi_y, 0, gTrue, gFalse, gTrue);
                    
               
				D(kprintf(" displayPage ok\n"));
			}
			
			InitRastPort(&MyRastPort);
			MyRastPort.BitMap = &MyBitMap;
			// we need only one BitPlane, because it's chunky format
			InitBitMap(&MyBitMap, 1, width, height);
			MyBitMap.BytesPerRow = width * 3;
			MyBitMap.Planes[0] = cairo_image_surface_get_data(pctx->surface);
			#ifdef _RZK_TEST
	     	cairo_surface_write_to_png(surface_back, "ram:image.png");  // only for testing
	     	cairo_surface_write_to_png(pctx->surface , "ram:image_back.png");  // only for testing
	     	#endif
	     	
	     	if (pages_per_sheet==2)
		    {
                cairo_destroy(cairo_back);
                cairo_surface_destroy(surface_back);
	     	}
	     	
			if (pctx->last_page != page)  // if we 
			{
				int i, j;
				LONG pixel, *ARGB_ptr;
				char *tmp2;

				ARGB_ptr = (LONG *)cairo_image_surface_get_data(pctx->surface);
				tmp2 = (char *)ARGB_ptr; 
			
				// RGBA to RGB inline conversion
				for(j = 0; j < height; j++)
				{
					for(i = 0; i < width; i++)
					{
						pixel = *ARGB_ptr++;
						*tmp2++ = (pixel & 0xFF0000) >> 16;
						*tmp2++ = (pixel & 0xFF00) >> 8;
						*tmp2++ = (pixel & 0xFF);
					}
				}
				D(kprintf(" conversion run ok\n"));
			}			
			
			if (landscape_mode && center)
                pctx->PD->pd_Preferences.PrintAspect=ASPECT_VERT;
                
			pctx->PIO->iodrp.io_RastPort = &MyRastPort;
			pctx->PIO->iodrp.io_SrcWidth = width;	// width of rastport
			pctx->PIO->iodrp.io_SrcHeight = height; // height of rastport
			pctx->PIO->iodrp.io_DestCols = 
                (LONG)(((width_mm)/25.4) * 1000.);  // width in printer pixels format
			pctx->PIO->iodrp.io_DestRows = 
                (LONG)(((height_mm)/25.4) * 1000.);	// height in printer pixel format
			pctx->PIO->iodrp.io_Special =  SPECIAL_MILROWS | SPECIAL_MILCOLS | (center)?SPECIAL_CENTER:0; 	
																				// save aspect ratio of picture, 
																				
			if ((pctx->PIO->iodrp.io_DestCols>pctx->PD->pd_SegmentData->ps_PED.ped_MaxXDots) && (pctx->PD->pd_Preferences.PrintAspect!=ASPECT_VERT))
			{
                double aspect = (height_mm*1.0)/width_mm;
                pctx->PIO->iodrp.io_DestCols = pctx->PD->pd_SegmentData->ps_PED.ped_MaxXDots;
                pctx->PIO->iodrp.io_DestRows = aspect*pctx->PIO->iodrp.io_DestCols;
            }															// turn on inches input for above fields

			//if(data->img.bpp == 24)
			{
				pctx->ExtIoDrp.Mode = TPFMT_RGB24;
				pctx->PIO->iodrp.io_ColorMap = 0;
			    #ifndef _RZK_TEST
			 	DoIO((struct IORequest *)pctx->PIO);
		        #endif
			}
			/*
			else
			{
				int i;
				ExtIoDrp.Mode = TPFMT_Chunky8;
				PD->pd_Preferences.PrintShade = SHADE_GREYSCALE;
				// PD->pd_Preferences.PrintAspect = ASPECT_HORIZ;
				PIO->iodrp.io_ColorMap = GetColorMap(256L);
				for(i = 0 ; i < 256 ; i++)
					SetRGB32CM(PIO->iodrp.io_ColorMap, i, i << 24, i << 24, i << 24);

				DoIO((struct IORequest *)PIO);
				FreeColorMap(PIO->iodrp.io_ColorMap);
			}*/
		}
	}
	catch(...)
	{
		LEAVE_SECTION
		return 0;
	}
	LEAVE_SECTION
	return 1;
}

void pdfPrintEnd(void *_pctx)
{
	struct printercontext *pctx = (struct printercontext*)_pctx;
	if(pctx != NULL)
	{
		if(pctx->format == VPDF_PRINT_POSTSCRIPT_2 || pctx->format == VPDF_PRINT_POSTSCRIPT_3)
		{
			if(pctx->dev)
				delete pctx->dev;

			if(pctx->file)
			{
				Close(pctx->file);	
			}		
		}
		else // turboprint
		{
			if (pctx->cairo)
			{
				cairo_destroy(pctx->cairo);
				pctx->cairo = NULL;	
			}
			if(pctx->surface)
			{
				cairo_surface_destroy(pctx->surface);
			}
			if(pctx->cairo_dev)
			{
				pctx->cairo_dev->setCairo(NULL);
				delete pctx->dev;
			}
		
			if (pctx->PIO)
			{
                CloseDevice((struct IORequest *)pctx->PIO);
                DeleteIORequest((struct IORequest *)pctx->PIO);
			}
			if (pctx->PrinterMP)
                DeleteMsgPort(pctx->PrinterMP);
		}

		free(pctx);
	}
}
