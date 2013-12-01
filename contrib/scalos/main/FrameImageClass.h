// FrameImageClass.h
// $Date$
// $Revision$


#ifndef	FrameImageCLASS_H
#define	FrameImageCLASS_H

//----------------------------------------------------------------------------

#define	FRAMEIMG_TagBase        	(TAG_USER + 0x91800)

#define	FRAMEIMG_ImageName		(FRAMEIMG_TagBase + 1)  // [I..] (CONST_STRPTR) required filename of datatypes image
#define	FRAMEIMG_OuterBackfillHook	(FRAMEIMG_TagBase + 2)  // [IS.] (struct Hook *) Backfill hook for area outside of frame
#define	FRAMEIMG_InnerBackfillHook	(FRAMEIMG_TagBase + 3)  // [IS.] (struct Hook *) Backfill hook for area inside of frame

//----------------------------------------------------------------------------

#endif	/* FrameImageCLASS_H */
