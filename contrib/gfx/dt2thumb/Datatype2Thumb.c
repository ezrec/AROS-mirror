#include <dos/dos.h>
#include <dos/dosasl.h>
#include <dos/dosextens.h>
#include <dos/exall.h>
#include <dos/rdargs.h>
#include <exec/memory.h>
#include <exec/types.h>
#include <utility/utility.h>

#include <proto/arossupport.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/datatypes.h>
#include <proto/icon.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <datatypes/pictureclass.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CTRL_C      (SetSignal(0L,0L) & SIGBREAKF_CTRL_C)
#define  isDir(fib) ((fib)->fib_DirEntryType >= 0)

#define ARG_TEMPLATE    "FILE/A,ALL/S,QUIET/S,W=WIDTH/N,H=HEIGHT/N,F=FORCEASPECT/S,M=METHOD,DEFTOOL"

enum 
{
	ARG_FILE = 0,
	ARG_ALL,
	ARG_QUIET,
	ARG_WIDTH,
	ARG_HEIGHT,
	ARG_FORCEASPECT,
	ARG_METHOD,
	ARG_DEFTOOL,
	NOOFARGS
};

/* Maximum file path length */
#define MAX_PATH_LEN    2048

const TEXT version[] = "$VER: dt2thumb 1.1 (10.12.2010)\n";
static char cmdname[] = "dt2thumb";

typedef struct rgbImage
{
	UWORD Width;
	UWORD Height;
	UBYTE *Data;
}
RGBImage;

int doThumbs(struct AnchorPath *ap, STRPTR files, BOOL all, BOOL quiet,
		ULONG destwidth, ULONG destheight, BOOL keepaspect, STRPTR deftool, STRPTR method);
BOOL CreateThumb(STRPTR infile, ULONG destwidth, ULONG destheight, BOOL keepaspect, STRPTR deftool, STRPTR method);
BOOL readpic_by_datatype(RGBImage *pic, char *file_name);
BOOL savepic_by_datatype(RGBImage *pic, char *file_name);
RGBImage *resizeBilinear(RGBImage *pic, ULONG w2, ULONG h2);
RGBImage *resizeNearest(RGBImage *pic, ULONG w2, ULONG h2);
RGBImage *resizeAverage(RGBImage *pic, ULONG w2, ULONG h2);


int main(void)
{
	struct RDArgs      *rda = NULL;
	struct AnchorPath  *ap = NULL;
	int	   retval = RETURN_OK;
	
	STRPTR files = "#?";
	BOOL   all = FALSE;
	BOOL   quiet = FALSE;
	ULONG  destwidth = 128;
    ULONG  destheight = 128;
    BOOL   keepaspect = TRUE;
    STRPTR method = NULL;
    STRPTR deftool = NULL;
	IPTR   args[NOOFARGS] = { (IPTR)files, all, quiet, (IPTR)&destwidth, (IPTR)&destheight, !keepaspect ,(IPTR)method ,(IPTR)deftool};
	
	ap = AllocVec(sizeof(struct AnchorPath) + MAX_PATH_LEN, MEMF_ANY | MEMF_CLEAR);
	if (ap != NULL)
	{
		ap->ap_Strlen = MAX_PATH_LEN;
		
		rda = ReadArgs(ARG_TEMPLATE, args, NULL);

		if (rda != NULL)
		{
			/* Convert arguments into (less complex) variables */
			if (args[ARG_FILE])			files = (STRPTR)args[ARG_FILE];
			if (args[ARG_ALL])			all = TRUE;
			if (args[ARG_QUIET])		quiet = TRUE;
			if (args[ARG_WIDTH])		destwidth = (ULONG)*((IPTR *)args[ARG_WIDTH]);
			if (args[ARG_HEIGHT])		destheight = (ULONG)*((IPTR *)args[ARG_HEIGHT]);
			if (args[ARG_FORCEASPECT])	keepaspect = FALSE;
			if (args[ARG_METHOD])		method = (STRPTR)args[ARG_METHOD];
			if (args[ARG_DEFTOOL])		deftool = (STRPTR)args[ARG_DEFTOOL];

			if (!all &&IsDosEntryA(files, LDF_VOLUMES | LDF_DEVICES))
			{
				Printf("Can't create thumb for %s - ", files);
				SetIoErr(ERROR_OBJECT_WRONG_TYPE);
				PrintFault(IoErr(), NULL);

				retval = RETURN_FAIL;
			}
			else
				retval = doThumbs(ap, files, all, quiet, destwidth, destheight, keepaspect, deftool, method);
			
			FreeArgs(rda);
		}
		else
		{
			PrintFault(IoErr(), cmdname);
			retval = RETURN_FAIL;
		}
		
		if (ap!=NULL) FreeVec(ap);
	}
	else
	{
		retval = RETURN_FAIL;
	}
	
	return retval;
} /* main */

int doThumbs(struct AnchorPath *ap, STRPTR files, BOOL all, BOOL quiet,
			 ULONG destwidth, ULONG destheight, BOOL keepaspect, STRPTR deftool, STRPTR method)
{
    LONG  match;
    int   retval = RETURN_OK;
    LONG  indent = 0;
    int   i;			/* Loop variable */
    BOOL  error;

    for (match = MatchFirst(files, ap);
    	 match == 0 && retval == RETURN_OK;// && !CTRL_C;
	     match = MatchNext(ap))
    {
		if (isDir(&ap->ap_Info))
		{
			if (ap->ap_Flags & APF_DIDDIR)
			{
				indent--;
				ap->ap_Flags &= ~APF_DIDDIR; /* Should not be necessary */
				continue;
			}
			else if (all)
			{
				ap->ap_Flags |= APF_DODIR;
				indent++;
			}
		}


		error = CreateThumb(ap->ap_Buf, destwidth, destheight, keepaspect, deftool, method);

        if (!quiet)
        {
            /* Fix indentation level */
            for (i = 0; i < indent; i++)
            {
                PutStr("     ");
            }

            if (!isDir(&ap->ap_Info))
            {
                PutStr("   ");
            }

            PutStr(ap->ap_Info.fib_FileName);

            if (isDir(&ap->ap_Info))
            {
                PutStr(" (dir)");
            }

            if (error)
            {
                PutStr(" ..Not a known picture file\n");
            }
            else
            {
                PutStr(" ..Thumbnail created\n");
            }
        }
    }

    MatchEnd(ap);

    return retval;
}

STRPTR get_ext(char *filename)
{
	static char extension[32];
	int position=strlen((char *)filename)-1;

	strcpy(extension,"");
	
	while(position > -1 && filename[position] != '.') position--;

	if (position > -1)
	{
		strncpy(extension,&filename[position+1],32);
	}

	return extension;
}

BOOL CreateThumb(STRPTR infile, ULONG destwidth, ULONG destheight, BOOL keepaspect, STRPTR deftool, STRPTR method)
{
	RGBImage *in_pic = NULL, *out_pic = NULL;
	char outfile[MAX_PATH_LEN];
	BOOL retval = TRUE;

	// do not create thumb for info files
	if (strnicmp(get_ext(infile),"info",4)==0) return retval;

	sprintf(outfile,"%s.info",infile);

	if ((in_pic = (RGBImage *)AllocVec(sizeof(RGBImage),MEMF_ANY)))
	{
		in_pic->Data = NULL;
		if (readpic_by_datatype(in_pic, infile))
		{
			if (keepaspect)
			{
				int arw = in_pic->Width / destwidth;
				int arh = in_pic->Height / destheight;

				if (arw > arh) destheight = in_pic->Height / arw;
				else if (arh > arw) destwidth = in_pic->Width / arh;
			}
			
			if (method != NULL)
			{
				if (strnicmp(method,"BI",2)==0)
					out_pic = resizeBilinear(in_pic, destwidth, destheight);
				else if (strnicmp(method,"AV",2)==0)
					out_pic = resizeAverage(in_pic, destwidth, destheight);
				else
					out_pic = resizeNearest(in_pic, destwidth, destheight);
			}
			else
				out_pic = resizeNearest(in_pic, destwidth, destheight);
			
			if (out_pic)
			{
				if (savepic_by_datatype(out_pic, outfile))
				{
					// Write default tool
					struct DiskObject *icon = GetIconTags
					(
						infile, ICONGETA_FailIfUnavailable, FALSE, TAG_DONE
					);

					if (icon != NULL)
					{
						STRPTR oldDefaultTool = icon->do_DefaultTool;

						if (deftool)
							icon->do_DefaultTool = deftool;
						else
						{
							static STRPTR tool = "multiview";
							icon->do_DefaultTool = tool;
						}
						if (!PutIconTags(infile, icon, TAG_DONE))
						{
							Printf("ERROR: Failed to write icon.\n");
						}
						icon->do_DefaultTool = oldDefaultTool;
						
						FreeDiskObject(icon);
						
						retval = FALSE;
					}
					else
					{
						Printf("ERROR: Failed to open icon for file\n");
						retval = TRUE;;
					}			
				}
			}
		}
		if (in_pic)
		{
			if (in_pic->Data) FreeVec(in_pic->Data);
			FreeVec(in_pic);
		}
		if (out_pic)
		{
			if (out_pic->Data) FreeVec(out_pic->Data);
			FreeVec(out_pic);
		}
	}
		
	return retval;
}


BOOL readpic_by_datatype(RGBImage *pic, char *file_name)
{
	Object *DTImage = NULL;
	struct BitMapHeader *bmh;
	struct pdtBlitPixelArray bpa;

	DTImage = NewDTObject(	file_name,
							(DTA_SourceType),	DTST_FILE,
							(DTA_GroupID),		GID_PICTURE,
							(PDTA_Remap),		FALSE,
							(OBP_Precision),	PRECISION_EXACT,					
							TAG_DONE); 

	if (DTImage)
	{

		if (GetDTAttrs( DTImage,
					PDTA_BitMapHeader, &bmh,
					TAG_END ) == 1)

		{

			/* Picture struct and buffer mem allocation */
			pic->Data = (UBYTE *)AllocVec(bmh->bmh_Width * bmh->bmh_Height * 4, MEMF_ANY);
			if (pic->Data)
			{
				bpa.MethodID = PDTM_READPIXELARRAY;
				bpa.pbpa_PixelData = (APTR)pic->Data;
				bpa.pbpa_PixelFormat = PBPAFMT_ARGB;
				bpa.pbpa_PixelArrayMod = bmh->bmh_Width * 4;
				bpa.pbpa_Left = 0;
				bpa.pbpa_Top = 0;
				bpa.pbpa_Width = bmh->bmh_Width;
				bpa.pbpa_Height = bmh->bmh_Height;

				DoMethodA( DTImage, (Msg)&bpa );

				pic->Width  = bmh->bmh_Width;
				pic->Height = bmh->bmh_Height;
				
				DisposeDTObject( DTImage );

				return TRUE;
			}
		}
		DisposeDTObject( DTImage );
	}
	return FALSE;
}


/**
* Nearest Neighbor resizing algorithm
* In case of thumbnail generation the loss of quality 
* should be minimal vs the bilinear algorithm
*/
RGBImage *resizeNearest(RGBImage *pic, ULONG w2, ULONG h2) 
{
	ULONG *temp = NULL;
	RGBImage *outpic = NULL;
	ULONG *pixels = (ULONG *)pic->Data;
    ULONG x_ratio = (ULONG)((pic->Width<<16)/w2) +1;
    ULONG y_ratio = (ULONG)((pic->Height<<16)/h2) +1;
    ULONG x2,y2,i,j;
	
	if ((outpic = (RGBImage *)AllocVec(sizeof(RGBImage),MEMF_ANY)))
	{
		outpic->Data = NULL;
		if ((outpic->Data = (UBYTE *)AllocVec(w2*h2*4, MEMF_ANY)))
		{
			outpic->Width = w2;
			outpic->Height = h2;
			temp = (ULONG *)outpic->Data;
			
			for (i=0;i<h2;i++)
			{
				y2 = ((i*y_ratio)>>16) ;
				for (j=0;j<w2;j++)
				{
					x2 = ((j*x_ratio)>>16) ;
					temp[(i*w2)+j] = pixels[(y2*pic->Width)+x2] ;
				}                
			}
		}
	}
    return outpic;
}

#define max(x,y) (x)>(y)?(x):(y)
#define min(x,y) (x)<(y)?(x):(y)

/**
* Averaging resizing algorithm
*  
* 
*/
RGBImage *resizeAverage(RGBImage *pic, ULONG w2, ULONG h2) 
{
	ULONG *temp = NULL;
	RGBImage *outpic = NULL;
	ULONG *pixels = (ULONG *)pic->Data;
	ULONG xpixels = min(256,max((ULONG)(pic->Width/w2),1));
	ULONG ypixels = min(256,max((ULONG)(pic->Height/h2),1));
    ULONG x_ratio = (ULONG)((pic->Width<<16)/w2) +1;
    ULONG y_ratio = (ULONG)((pic->Height<<16)/h2) +1;
	ULONG r,g,b,a,index;
    ULONG x2,y2,i,j,x,y;
	
	if ((outpic = (RGBImage *)AllocVec(sizeof(RGBImage),MEMF_ANY)))
	{
		outpic->Data = NULL;
		if ((outpic->Data = (UBYTE *)AllocVec(w2*h2*4, MEMF_ANY)))
		{
			outpic->Width = w2;
			outpic->Height = h2;
			temp = (ULONG *)outpic->Data;
			
			for (i=0;i<h2;i++)
			{
				y2 = ((i*y_ratio)>>16) ;
				for (j=0;j<w2;j++)
				{
					x2 = ((j*x_ratio)>>16) ;
					
					r = 0;
					g = 0;
					b = 0;
					a = 0;
					
					for (y=0;y<ypixels;y++)
						for (x=0;x<xpixels;x++)
						{
							index = ((y2+y)*pic->Width+(x2+x));
							b += (pixels[index]&0xff);
							g += ((pixels[index]>>8)&0xff);
							r += ((pixels[index]>>16)&0xff);
							a += ((pixels[index]>>24)&0xff);
						}
						
					r /= (ypixels*xpixels);
					g /= (ypixels*xpixels);
					b /= (ypixels*xpixels);
					a /= (ypixels*xpixels);
						
					temp[(i*w2)+j] = 	((((ULONG)a)<<24) & 0xff000000) |
										((((ULONG)r)<<16) & 0x00ff0000) |
										((((ULONG)g)<<8)  & 0x0000ff00) |
										((ULONG)b) ;					
				}                
			}
		}
	}
    return outpic;
}

/**
* Bilinear resize ARGB image.
* pixels is an array of size w * h.
* Target dimension is w2 * h2.
* w2 * h2 cannot be zero.
*/
RGBImage *resizeBilinear(RGBImage *pic, ULONG w2, ULONG h2) 
{
	ULONG *temp = NULL;
	RGBImage *outpic = NULL;
	ULONG *pixels = (ULONG *)pic->Data;
	ULONG a, b, c, d, x, y, index ;
	float x_ratio = ((float)(pic->Width-1))/w2 ;
	float y_ratio = ((float)(pic->Height-1))/h2 ;
	float x_diff, y_diff, blue, red, green, alpha ;
	ULONG offset = 0 ;
	int i,j;
	
	if ((outpic = (RGBImage *)AllocVec(sizeof(RGBImage),MEMF_ANY)))
	{
		if ((outpic->Data = (UBYTE *)AllocVec(w2*h2*4, MEMF_ANY)))
		{
			outpic->Width = w2;
			outpic->Height = h2;
			temp = (ULONG *)outpic->Data;
	
			if ((pic->Width==w2) && (pic->Height=h2))
			{
				CopyMem(pixels, temp, pic->Width * pic->Height * 4);
				return outpic;
			}
			
			for (i=0;i<h2;i++) 
			{
				for (j=0;j<w2;j++)
				{
					x = (ULONG)(x_ratio * j) ;
					y = (ULONG)(y_ratio * i) ;
					x_diff = (x_ratio * j) - x ;
					y_diff = (y_ratio * i) - y ;
					index = (y*pic->Width+x) ;                
					a = pixels[index] ;
					b = pixels[index+1] ;
					c = pixels[index+pic->Width] ;
					d = pixels[index+pic->Width+1] ;

					// blue element
					// Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
					blue = (a&0xff)*(1-x_diff)*(1-y_diff) + (b&0xff)*(x_diff)*(1-y_diff) +
					(c&0xff)*(y_diff)*(1-x_diff)   + (d&0xff)*(x_diff*y_diff);

					// green element
					// Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
					green = ((a>>8)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>8)&0xff)*(x_diff)*(1-y_diff) +
					((c>>8)&0xff)*(y_diff)*(1-x_diff)   + ((d>>8)&0xff)*(x_diff*y_diff);

					// red element
					// Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
					red = ((a>>16)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>16)&0xff)*(x_diff)*(1-y_diff) +
					((c>>16)&0xff)*(y_diff)*(1-x_diff)   + ((d>>16)&0xff)*(x_diff*y_diff);

					// alpha element
					// Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
					alpha = ((a>>24)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>24)&0xff)*(x_diff)*(1-y_diff) +
					((c>>24)&0xff)*(y_diff)*(1-x_diff)   + ((d>>24)&0xff)*(x_diff*y_diff);

					
					temp[offset++] = 	((((ULONG)alpha)<<24) & 0xff000000) |
										((((ULONG)red)  <<16) & 0x00ff0000) |
										((((ULONG)green)<<8)  & 0x0000ff00) |
										((ULONG)blue) ;
				}
			}
		}
	}
	return outpic ;
}

BOOL savepic_by_datatype(RGBImage *pic, char *file_name)
{
	Object *DTImage = NULL;
	struct BitMapHeader *bmhd;
	struct dtWrite dtw;
	struct pdtBlitPixelArray dtb;
	BPTR *file = NULL;
	BOOL retval = FALSE;
	
	DTImage = NewDTObject(	(APTR)NULL,
							DTA_SourceType, DTST_RAM,
							DTA_BaseName, (IPTR)"png",
							PDTA_DestMode, PMODE_V43,
							TAG_DONE);
	if (!DTImage) return(FALSE);


	if (GetDTAttrs(DTImage,PDTA_BitMapHeader,(IPTR)&bmhd,TAG_DONE))
	{
		dtb.MethodID = PDTM_WRITEPIXELARRAY;
		dtb.pbpa_PixelData = pic->Data;
		dtb.pbpa_PixelFormat = PBPAFMT_ARGB;
		dtb.pbpa_PixelArrayMod = pic->Width*4;
		dtb.pbpa_Left = 0;
		dtb.pbpa_Top = 0;
		dtb.pbpa_Width = pic->Width;
		dtb.pbpa_Height = pic->Height;

		bmhd->bmh_Width = pic->Width;
		bmhd->bmh_Height = pic->Height;
		bmhd->bmh_Depth = 24;
		bmhd->bmh_PageWidth = 320;
		bmhd->bmh_PageHeight = 240;
		
		DoMethodA(DTImage, (Msg) &dtb);

		//write datatype object to file
		if ((file = Open (file_name,MODE_NEWFILE)))
		{
			dtw.MethodID = DTM_WRITE;
			dtw.dtw_GInfo = NULL;
			dtw.dtw_FileHandle = file;
			dtw.dtw_Mode = DTWM_RAW;
			dtw.dtw_AttrList = NULL;

			if (DoMethodA(DTImage, (Msg) &dtw)) retval = TRUE;
		}
	}

	if (file) Close (file);
	if (DTImage) DisposeDTObject(DTImage);
	return retval;
}

