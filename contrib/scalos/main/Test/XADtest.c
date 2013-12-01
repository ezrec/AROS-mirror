// XADtest.c
// $Date$
// $Revision$

#define USE_INLINE_STDARG 1

#include <exec/types.h>
#include <libraries/xadmaster.h>
#include <proto/exec.h>
#include <proto/xadmaster.h>
#include <proto/utility.h>
#include <stdlib.h>
#include <stdio.h>

struct xadMasterBase *xadMasterBase;

int main(int argc, char *argv[])
{
	struct xadArchiveInfo *arcInfo = NULL;

	do	{
		LONG rc;
		struct xadFileInfo *fi;

		if (2 != argc)
			{
			fprintf(stderr, "Usage: xadtest archivefilename\n");
			break;
			}

		xadMasterBase = (struct xadMasterBase *) OpenLibrary(XADNAME, 0);
		if (NULL == xadMasterBase)
			{
			fprintf(stderr, "Failed to open <%s>\n", XADNAME);
			break;
			}

		arcInfo	= xadAllocObject(XADOBJ_ARCHIVEINFO, TAG_END);
		if (NULL == arcInfo)
			{
			fprintf(stderr, "Failed to create XADOBJ_ARCHIVEINFO\n");
			break;
			}

		rc = xadGetInfo(arcInfo,
			XAD_INFILENAME, (LONG) argv[1],
			TAG_END);
		if (0 != rc)
			{
			fprintf(stderr, "xadGetInfo returned error, rc=%ld\n", rc);
			break;
			}

		if (arcInfo->xai_Client)
			{
			printf("xc_ArchiverName=<%s>\n", arcInfo->xai_Client->xc_ArchiverName);
			}
		printf("xai_Flags=%08lx\n", arcInfo->xai_Flags);

		if (XADAIF_FILECORRUPT & arcInfo->xai_Flags)
			{
			fprintf(stderr, "XADAIF_FILECORRUPT\n");
			break;
			}

		fi = arcInfo->xai_FileInfo;
		while (fi)
			{
			printf("%ld <%s>  %ld  %08lx\n", fi->xfi_EntryNumber, fi->xfi_FileName, fi->xfi_Size, fi->xfi_Flags);
			fi = fi->xfi_Next;
			}
		} while (0);

	if (arcInfo)
		{
		xadFreeInfo(arcInfo);
		xadFreeObject(arcInfo, TAG_END);
		}
	if (xadMasterBase)
		{
		CloseLibrary((struct Library *) xadMasterBase);
		}
}

