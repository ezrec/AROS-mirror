#include <exec/memory.h>
#include <exec/semaphores.h>
#include <datatypes/pictureclass.h>
#include <datatypes/animationclass.h>
#include <graphics/gfx.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/dos_lib.h>
#else
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/datatypes.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#endif

#include <clib/alib_protos.h>

#include <string.h>
#include <stdio.h>

#include "global.h"
#include "myimage.h"
#include "config.h"
#include "imageloader.h"
#include "defimages.h"
#include "reqattack.h"

ULONG imageloadermask;

static struct MsgPort imageloaderport;
static struct List imagelist;
static APTR mempool;
static WORD loadedimages;
static BOOL initdone;

/**********************************************************/

static struct MyImageSpec *GetDefaultImageSpec(char *name)
{
	struct MyImageSpec *rc = 0;

	if (name)
	{
		if (name == defaultconfig.infologo)
		{
			rc = &warnimagespec;
		} else if (name == defaultconfig.asklogo)
		{
			rc = &askimagespec;
		} else if (name == defaultconfig.multiasklogo)
		{
			rc = &askimagespec;
		} else if (name == defaultconfig.okimage)
		{
			rc = &useimagespec;
		} else if (name == defaultconfig.cancelimage)
		{
			rc = &cancelimagespec;
		} else if (name == defaultconfig.thirdimage)
		{
			rc = &pointimagespec;
		}

	} /* if (name) */

	return rc;
}

/**********************************************************/

void InitImageLoader(void)
{
	NewList (&imagelist);

	if (!(mempool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR,
										5000,
										5000)))
	{
		Cleanup("InitImageLoader: Out of memory!");
	}

	imageloaderport.mp_Node.ln_Type = NT_MSGPORT;
	imageloaderport.mp_Flags = PA_SIGNAL;
	imageloaderport.mp_SigBit = AllocSignal(-1);
	imageloaderport.mp_SigTask = FindTask(0);
	NewList(&imageloaderport.mp_MsgList);

	imageloadermask = 1L << imageloaderport.mp_SigBit;

	initdone = TRUE;
}

void CleanupImageLoader(void)
{
	struct RAImageNode *node,*succ;

	if (initdone)
	{
		node = (struct RAImageNode *)imagelist.lh_Head;
		while ((succ = (struct RAImageNode *)node->node.ln_Succ))
		{
			FreePooled(mempool,node,node->allocsize);
			node = succ;
		}

		DeletePool(mempool);

		FreeSignal(imageloaderport.mp_SigBit);

	}
}

struct RAImageNode *LoadRAImage(char *name)
{
	static struct RAImage header;
	static struct FileRAImage fileheader;
	static struct gpLayout gpLay;
//	static struct adtFrame adtFra;
	struct RAImageNode *node = 0;
	static struct BitMapHeader *bmh;
	static struct BitMap *bm;
	LONG l,allocsize;
	BPTR MyHandle;
	ULONG *cregs,numcolors,*dto=0;
	UBYTE *ub,memdepth;
	ULONG wordwidth,bpr,bprbm,y;

	BOOL rc = FALSE; //animmode = FALSE;

	D(bug("LoadRAImage\n"));
	D(bug("Open\n"));
	if ((MyHandle = Open(name,MODE_OLDFILE)))
	{
		D(bug("Open done\n"));
		D(bug("Read header \n"));

		if (Read(MyHandle,&fileheader,sizeof(struct FileRAImage)) == sizeof(struct FileRAImage))
		{
			D(bug("Read header done\n"));
			
    	    	#ifdef __AROS__
		#define CONVLONG(x) header.x = (fileheader.x[0] << 24) + \
		    	    		       (fileheader.x[1] << 16) + \
					       (fileheader.x[2] << 8) + \
					       (fileheader.x[3])
		#define CONVWORD(x) header.x = (fileheader.x[0] << 8) + \
		    	    	    	       (fileheader.x[1])
				
		    	CONVLONG(id);
			CONVLONG(filelen);
			CONVWORD(version);
			CONVWORD(width);
			CONVWORD(height);
			CONVWORD(framewidth);
			CONVWORD(frameheight);
			CONVWORD(numcols);
			CONVWORD(frames);
			CONVWORD(animspeed);
			CONVLONG(flags);
			/* reserved */
			CONVLONG(pal);
			CONVLONG(data);
			
		#undef CONVWORD
		#undef CONVLONG
			
		#else
		    	header = (struct RAImage *)fileheader;
		#endif
		
			if (header.id == REQATTACK_IMAGEID)
			{
				l = header.filelen - sizeof(struct FileRAImage);
				allocsize = sizeof(struct RAImageNode) + l;

				if ((node = (struct RAImageNode *)AllocPooled(mempool,allocsize)))
				{
					node->allocsize = allocsize;

					D(bug("Read image\n"));

					if (Read(MyHandle,node->raimage,l) == l)
					{
						D(bug("Read image done\n"));

						node->spec.data = (ULONG *)((ULONG)header.data + (ULONG)node->raimage - sizeof(struct FileRAImage));
						node->spec.col  = (UBYTE *)((ULONG)header.pal + (ULONG)node->raimage - sizeof(struct FileRAImage));
						node->spec.width = header.width;
						node->spec.height = header.height;
						node->spec.numcols = header.numcols;
						node->spec.framewidth = header.framewidth;
						node->spec.frameheight = header.frameheight;
						node->spec.frames = header.frames;
						node->spec.animspeed = header.animspeed;
						node->spec.flags = MYIMSPF_EXTERNAL;
						if (header.flags & RAIMF_NOTRANSPARENCY) node->spec.flags |= MYIMSPF_NOTRANSP;
						if (header.flags & RAIMF_PINGPONGANIM) node->spec.flags |= MYIMSPF_PINGPONG;
						if (header.flags & RAIMF_PACKED) node->spec.flags |= MYIMSPF_PACKED;
						if (header.flags & RAIMF_CHUNKY) node->spec.flags |= MYIMSPF_CHUNKY;

						name = FilePart(name);

						l = strlen(name);
						if (l > 31) l = 31;

						memcpy(node->filename,name,l);

						node->usecount = 1;
						loadedimages++;

						AddTail(&imagelist,&node->node);

						rc = TRUE;
					}

				} /* if ((node = AllocPooled(mempool,sizeof(struct RAImageNode) + header.filelen))) */

			} else /* if (header.id == REQATTACK_IMAGEID) */
			{

				if (DataTypesBase)
				{
					//open and process the picture...
					dto = (ULONG *)NewDTObject(name,
											 DTA_GroupID,GID_PICTURE,
											 PDTA_Remap,FALSE,
											 TAG_DONE);
					if (dto)
					{

						gpLay.MethodID = DTM_PROCLAYOUT;
						gpLay.gpl_GInfo = NULL;
						gpLay.gpl_Initial = 1L;
					if (DoMethodA((Object *)dto,(Msg)&gpLay))
					{
						WaitBlit();

					if (GetDTAttrs((Object *)dto,PDTA_BitMapHeader,(ULONG)&bmh,PDTA_BitMap,(ULONG)&bm,
											 PDTA_NumColors,(ULONG)&numcolors,
							 				PDTA_CRegs,(ULONG)&cregs,
							 				TAG_DONE) > 3)
						{
							wordwidth = (bmh->bmh_Width + 15) & ~15;
							bpr = wordwidth / 8;
							//bpr = bm->BytesPerRow;
							memdepth = bm->Depth;
						if (memdepth < 9)
						{
							//while(numcolors > (1 << memdepth)) memdepth++;
								allocsize = sizeof(struct RAImageNode) + (3 * numcolors) + (bpr * bmh->bmh_Height * memdepth * 2) + 24;
						if ((node = (struct RAImageNode *)AllocPooled(mempool,allocsize)))
						{
							node->allocsize = allocsize;
							node->spec.data = (ULONG *)(((ULONG)sizeof(struct RAImageNode) + (ULONG)node + (3 * numcolors) + 7)& ~3);//palette goes 1st!
							node->spec.col  = (UBYTE *)(((ULONG)sizeof(struct RAImageNode) + (ULONG)node + 3) & ~3);
							node->spec.width = bmh->bmh_Width;
							node->spec.height = bmh->bmh_Height;
							node->spec.numcols = numcolors;
							node->spec.framewidth = bmh->bmh_Width;;
							node->spec.frameheight = bmh->bmh_Height;;
							node->spec.frames = 1;
							node->spec.animspeed = 0;
							node->spec.flags = MYIMSPF_EXTERNAL;

							ub = (UBYTE *)node->spec.col;

							for (l = 0;l < numcolors * 3;l++)
							{
								*ub++ = (UBYTE)(cregs[l] >> 24);
							} //color palette conversion from RAIM_Convert

							ub = (UBYTE *)node->spec.data;
							bprbm = GetBitMapAttr(bm,BMA_WIDTH) / 8;

							if (GetBitMapAttr(bm,BMA_FLAGS) & BMF_INTERLEAVED)
							{
								for (y = 0;y < bmh->bmh_Height * memdepth;y++)
								{
									CopyMem(bm->Planes[0] + y * bprbm,ub,bpr);
									ub += bpr;
								}
							} //if (GetBitMapAttr(bm2,BMA_FLAGS) & BMF_INTERLEAVED)
								else
							{
								for(y = 0;y < bmh->bmh_Height;y++)
								{
									for(l = 0;l < memdepth;l++)
									{
										CopyMem(bm->Planes[l] + y * bprbm,
												  ub,
												  bpr);
										ub += bpr;
									}
								}
							}

							name = FilePart(name);

							l = strlen(name);
							if (l > 31) l = 31;

							memcpy(node->filename,name,l);

							node->usecount = 1;
							loadedimages++;

							AddTail(&imagelist,&node->node);

							rc = TRUE;
						}
						} //memdepth
					}	//GetDTAttrs(dto,PDTA_BitMap,(ULONG)&bm,
					}	//DoDTMethod(dto,0,0,DTM_PROCLAYOUT,0,(LONG)TRUE)
					}
				}//DataTypesBase

			}

		} /* if (Read(MyHandle,&fileheader,sizeof(struct FileRAImage)) == sizeof(struct FileRAImage)) */

		D(bug("Close\n"));
		Close(MyHandle);MyHandle=0;
		D(bug("Close done\n"));
		if (dto)
		{
			DisposeDTObject((Object *)dto);dto=0;
		}

	} /* if ((MyHandle = Open(name,MODE_OLDFILE))) */

	if (!rc && node) FreePooled(mempool,node,node->allocsize);

	return rc ? node : 0;
}

struct MyImageSpec *GetImageSpec(char *name,char *def,char *internal)
{
	struct MyImageSpec *rc = NULL;

	/* try */

	if ((ULONG)name != (ULONG)IMAGE_OFF) //not proper!!! - def image then...
	{
	rc = GetDefaultImageSpec(name);

	if (!rc && name)
	{
		if (name[0])
		{
			rc = SendImageLoaderMsg(ILM_GETIMAGE,name,0);
		}
	}
	}

	/* try default */

	if (!rc && def)
	{
		rc = GetDefaultImageSpec(def);

		if (!rc)
		{
			rc = SendImageLoaderMsg(ILM_GETIMAGE,def,0);
		}
	}

	/* try internal */

	if (!rc && internal)
	{
		rc = GetDefaultImageSpec(internal);
	}

	return rc;
}

struct MyImageSpec *SendImageLoaderMsg(WORD code,char *filename,struct MyImageSpec *spec)
{
	struct ImageLoaderMsg msg;
	struct MsgPort replyport;

	replyport.mp_Node.ln_Type = NT_MSGPORT;
	replyport.mp_Flags = PA_SIGNAL;
	replyport.mp_SigBit = SIGB_SINGLE;
	replyport.mp_SigTask = FindTask(0);
	NewList(&replyport.mp_MsgList);

	SetSignal(0,SIGF_SINGLE);

	msg.msg.mn_ReplyPort = &replyport;
	msg.msg.mn_Length = sizeof(msg);

	msg.code = code;
	msg.filename = filename;
	msg.spec = spec;

	D(bug("Send %ld\n",code));
	PutMsg(&imageloaderport,&msg.msg);
	WaitPort(&replyport);

	D(bug("Send done %ld\n\n",code));

	return msg.spec;
}

void HandleImageLoader(void)
{
	struct ImageLoaderMsg *msg;
	struct RAImageNode *node,*succ;
	char *name;

	D(bug("\nHandleImageLoader:\n"));

	while ((msg = (struct ImageLoaderMsg *)GetMsg(&imageloaderport)))
	{
		D(bug("Got message\n"));

		switch(msg->code)
		{
			case ILM_GETIMAGE:
				D(bug("ILM_GETIMAGE\n"));

				msg->spec = 0;

				name = FilePart(msg->filename);

				node = (struct RAImageNode *)imagelist.lh_Head;
				while ((succ = (struct RAImageNode *)node->node.ln_Succ))
				{
					if (!stricmp(name,node->filename))
					{
						D(bug("Image in cache\n"));

						node->usecount++;
						msg->spec = &node->spec;
						break;
					}
					node = succ;
				}

				if (!succ)
				{
					if ((node = LoadRAImage(msg->filename)))
					{
						msg->spec = &node->spec;
					}
				}

				break;

			case ILM_FREEIMAGE:
				D(bug("ILM_FREEIMAGE\n"));

				node = (struct RAImageNode *)imagelist.lh_Head;
				while ((succ = (struct RAImageNode *)node->node.ln_Succ))
				{
					if (msg->spec == &node->spec)
					{
						node->usecount--;
						break;
					}

					node = succ;
				}
				break;

			case ILM_LOGOFROMMEM:

				node = (struct RAImageNode *)SpecificImageLoader((ULONG *)msg->filename);
				if (node) msg->spec = (struct MyImageSpec *)node;
				break;

			case ILM_FREELOGOMEM:

				if (msg->spec)
				FreePooled(mempool,msg->spec,sizeof (struct MyImageSpec));
				break;

		} /* switch(msg->code) */

		ReplyMsg(&msg->msg);

	} /* while ((msg = (struct ImageLoaderMsg *)GetMsg(&imageloaderport))) */

	D(bug("no more messages\n"));

	/* if there are more than <config_imagecaches> cached images
	   then try to free some if possible (usecount = 0) */

	D(bug("checking if some images can be freed\n"));

	node = (struct RAImageNode *)imagelist.lh_Head;
	while ((succ = (struct RAImageNode *)node->node.ln_Succ) &&
			 (loadedimages > defaultconfig.imagecaches))
	{
		if (node->usecount == 0)
		{
			D(bug("freeing image\n"));
			Remove(&node->node);
			FreePooled(mempool,node,node->allocsize);
			loadedimages--;
		}

		node = succ;
	}

	D(bug("\nHandleImageLoader done\n\n"));
}

struct MyImageSpec *SpecificImageLoader(ULONG *filememory)
{
	static struct RAImage *header;
	struct MyImageSpec *spec = 0;

//	BOOL animmode = FALSE;

	header = (struct RAImage *)filememory;
	if (!(header)) return (struct MyImageSpec *)0;
	if (header->id == REQATTACK_IMAGEID)
	{

		if ((spec = (struct MyImageSpec *)AllocPooled(mempool,sizeof (struct MyImageSpec))))
		{

			spec->data = (ULONG *)((ULONG)header->data + (ULONG)header);
			spec->col  = (UBYTE *)((ULONG)header->pal + (ULONG)header);
			spec->width = header->width;
			spec->height = header->height;
			spec->numcols = header->numcols;
			spec->framewidth = header->framewidth;
			spec->frameheight = header->frameheight;
			spec->frames = header->frames;
			spec->animspeed = header->animspeed;
			spec->flags = MYIMSPF_EXTERNAL;
			if (header->flags & RAIMF_NOTRANSPARENCY) spec->flags |= MYIMSPF_NOTRANSP;
			if (header->flags & RAIMF_PINGPONGANIM) spec->flags |= MYIMSPF_PINGPONG;
			if (header->flags & RAIMF_PACKED) spec->flags |= MYIMSPF_PACKED;
			if (header->flags & RAIMF_CHUNKY) spec->flags |= MYIMSPF_CHUNKY;

				return spec;

		} /* if ((node = AllocPooled(mempool,sizeof(struct RAImageNode) + header.filelen))) */

	}//RAIM

	return 0;
}


