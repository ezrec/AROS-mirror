#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include <string.h>

#include "pui.h"
#include "classmacros.h"
#include "coolimageclass.h"
#include "coolimagedata.h"

static struct lookup
{
	ULONG *data;
	ULONG *pal;
	WORD	width;
	WORD	height;
	WORD	depth;
} lookuptable[] =
{
	{diskdata,diskpal,DISKWIDTH,DISKHEIGHT,DISKDEPTH},
	{savedata,savepal,SAVEWIDTH,SAVEHEIGHT,SAVEDEPTH},
	{loaddata,loadpal,LOADWIDTH,LOADHEIGHT,LOADDEPTH},
	{canceldata,cancelpal,CANCELWIDTH,CANCELHEIGHT,CANCELDEPTH},
	{usedata,usepal,USEWIDTH,USEHEIGHT,USEDEPTH},
	{(ULONG *)monitordata,monitorpal,MONITORWIDTH,MONITORHEIGHT,MONITORDEPTH},
	{quotationmarkdata,quotationmarkpal,QUOTATIONMARKWIDTH,QUOTATIONMARKHEIGHT,QUOTATIONMARKDEPTH},
	{exclamationmarkdata,exclamationmarkpal,EXCLAMATIONMARKWIDTH,EXCLAMATIONMARKHEIGHT,EXCLAMATIONMARKDEPTH},
	{infodata,infopal,INFOWIDTH,INFOHEIGHT,INFODEPTH}
};

#ifdef _AROS

LONG _WritePixelLine8(struct RastPort *rp, LONG x1, LONG y1, LONG width,
    	    	      UBYTE *array, struct RastPort *temprp)
{
    WORD x;
    
    for(x = 0; x < width; x++)
    {
    	SetAPen(rp, *array++);
	WritePixel(rp, x1 + x, y1);
    }
    
    return 0;
}

#undef WritePixelLine8
#define WritePixelLine8(rp,x1,y1,width,array,temprp) _WritePixelLine8(rp, x1, y1, width, array, temprp)

#endif

static void MakeImage(struct IClass *cl,Object *obj)
{
	struct coolimagedata *cdata;
	struct PUI_AreaData *adata;

	UBYTE *chunky,*planar,*remaptable;
	ULONG *data,*col,*data2;
	struct RastPort temprp;
	struct BitMap tempbm;

	LONG reg,modulo;
	WORD width,height,depth,realdepth,i;

	cdata = INST_DATA(cl,obj);
	adata = INST_DATA(areaclass,obj);

	i = cdata->type;

	if (i >= 0 && i < NUM_COOLIMAGES)
	{
		data   = lookuptable[i].data;
		col    = lookuptable[i].pal;
		width  = lookuptable[i].width;
		height = lookuptable[i].height;
		depth  = lookuptable[i].depth;

		realdepth = adata->ri->dri->dri_Depth;
		if (realdepth > 8) realdepth = 8;
	
		modulo = (width + 15) & (~15);
		
		if ((cdata->chunkydata = AllocVec(modulo * height + 256,MEMF_PUBLIC|MEMF_REVERSE|MEMF_CLEAR)))
		{
			if ((cdata->coltable = AllocVec((1L << depth) * sizeof(ULONG),MEMF_PUBLIC)))
			{
				remaptable = (UBYTE *)(((ULONG)cdata->chunkydata) + modulo * height);
	
				planar = (UBYTE *)data;
				chunky = cdata->chunkydata;
				for(i = 0;i < height;i++)
				{
					Planar2Chunky(planar,chunky,modulo,depth);
					chunky += modulo;
					planar += (modulo / 8) * depth;
				}
	
				data = cdata->chunkydata;
			
				cdata->width = width;
				cdata->height = height;
				cdata->depth = depth;
				cdata->numcols = (1L << depth);
				cdata->pensalloced = TRUE;
	
				for(i = 0;i < cdata->numcols;i++)
				{
					reg = ObtainBestPen(adata->ri->cm,col[i*3],col[i*3+1],col[i*3+2],
											  OBP_Precision,PRECISION_EXACT,
							 				  OBP_FailIfBad,FALSE,
											  TAG_DONE);
			
					remaptable[i] = cdata->coltable[i] = reg;
				}
				
				if ((cdata->bm = AllocBitMap(width + 16,height + 1,realdepth,BMF_STANDARD|BMF_CLEAR,0)))
				{
					if ((data2 = AllocVec(modulo * height,MEMF_PUBLIC|MEMF_REVERSE)))
					{
						CopyMem(data,data2,modulo * height);
						chunky = (UBYTE *)data2;
					} else {
						chunky = (UBYTE *)data;
					}
				
					InitRastPort(&temprp);
					temprp.BitMap = cdata->bm;
					
					chunky = (UBYTE *)data;
					for (i = 0;i < height;i++)
					{
						WritePixelLine8(&temprp,0,i + 1,(width + 7) & (~7),chunky,&temprp);
						chunky += modulo;
					}
					
					WaitBlit();
					if (data2) FreeVec(data2);
				
					if (/*DoMask &&*/ (cdata->mask = AllocVec((GetBitMapAttr(cdata->bm,BMA_WIDTH) * (height + 1)) / 8,MEMF_CHIP|MEMF_CLEAR|MEMF_PUBLIC)))
					{
						InitBitMap(&tempbm,realdepth,GetBitMapAttr(cdata->bm,BMA_WIDTH),height + 1);
				
						for(i = 0;i < realdepth;i++)
						{
							tempbm.Planes[i] = cdata->mask;
						}
						
						BltBitMap(cdata->bm,0,1,&tempbm,0,1,width,height,0xEE,255,0);
					}
				
					RemapBytes(data,remaptable,modulo*height);
				
					WaitBlit();
		
					chunky = (UBYTE *)data;
					for (i = 0;i < height;i++)
					{
						WritePixelLine8(&temprp,0,i + 1,(width + 7) & (~7),chunky,&temprp);
						chunky += modulo;
					}
					WaitBlit();
				
					if (cdata->chunkydata)
					{
						FreeVec(cdata->chunkydata);
						cdata->chunkydata=0;
					}
		
					cdata->imageok = TRUE;
		
				} /* if ((cdata->bm=AllocBitMap(width+16,height+1,realdepth,BMF_STANDARD|BMF_CLEAR,0))) */

			} /* if ((cdata->coltable = AllocVec((1L << depth) * sizeof(ULONG),MEMF_PUBLIC))) */
				
		} /* if ((im->chunkydata = AllocVec(modulo*height,MEMF_PUBLIC|MEMF_REVERSE|MEMF_CLEAR))) */
		
	} /* if (i >= 0 && i < NUM_COOLIMAGES) */
}

ULONG CoolImageInstanceSize(void)
{
	return sizeof(struct coolimagedata);
}

ULONG CoolImageDispatcher(struct IClass *cl,Object *obj,Msg msg)
{
	struct coolimagedata *data;
	struct PUI_AreaData 		*adata;
	struct RastPort	 	*rp;
	WORD						i,x,y;
	
	ULONG						rc;
	
	rc = 0;
	
	if (msg->MethodID != OM_NEW)
	{
		data  = INST_DATA(cl,obj);
		adata = INST_DATA(areaclass,obj);
	}
	
	switch(msg->MethodID)
	{
		case OM_NEW:
			if ((obj = (Object *)DoSuperMethodA(cl,obj,msg)))
			{
				data = INST_DATA(cl,obj);
				memset(data,0,sizeof(*data));
				data->type = GetTagData(PUIA_CoolImage_Which,0,ATTRLIST);
				rc = (ULONG)obj;
			};
			break;
		
		case OM_DISPOSE:
			if (data->pensalloced)
			{
				for(i = 0;i < data->numcols;i++)
				{
					if (data->coltable[i] != -1)
					{
						ReleasePen(adata->ri->cm,data->coltable[i]);
					}
				}
				data->pensalloced = FALSE;
			}

			if (data->bm)
			{
				WaitBlit();
				FreeBitMap(data->bm);
				data->bm = 0;
			}

			if (data->mask)
			{
				FreeVec(data->mask);
				data->mask = 0;
			}

			if (data->chunkydata)
			{
				FreeVec(data->chunkydata);
				data->chunkydata = 0;
			}

			if (data->coltable)
			{
				FreeVec(data->coltable);
				data->coltable = 0;
			}

			DoSuperMethodA(cl,obj,msg);
			break;
		
		case PUIM_SETUP:
			DoSuperMethodA(cl,obj,msg);
			MakeImage(cl,obj);
			break;

		case PUIM_MINMAX:
			DoSuperMethodA(cl,obj,msg);
			x = data->width;
			y = data->height;

			MINMAXMSG->minmax->minwidth += x;
			MINMAXMSG->minmax->defwidth += x;
			MINMAXMSG->minmax->maxwidth += x;

			MINMAXMSG->minmax->minheight += y;
			MINMAXMSG->minmax->defheight += y;
			MINMAXMSG->minmax->maxheight += y;
			
			((struct Gadget *)obj)->Width = x;
			((struct Gadget *)obj)->Height = y;
			break;

		case GM_RENDER:
			if (data->imageok && (rp = RENDERMSG->gpr_RPort) && PUI_MayRender(obj))
			{
			#ifndef _AROS
				if (data->mask)
				{
					BltMaskBitMapRastPort(data->bm,0,1,rp,GAD->LeftEdge,GAD->TopEdge,data->width,data->height,ANBC|ABNC|ABC,data->mask);
				} else {
    	    	    	#endif
					BltBitMapRastPort(data->bm,0,1,rp,GAD->LeftEdge,GAD->TopEdge,data->width,data->height,192);
			#ifndef _AROS
				}
			#endif
			}
			break;
			
		default:
			rc = DoSuperMethodA(cl,obj,msg);
	}
	return rc;
}



