// DtImageClass.h
// $Date$
// $Revision$


#ifndef	DTIMAGECLASS_H
#define	DTIMAGECLASS_H

//----------------------------------------------------------------------------

#define	DTIMG_TagBase	     (TAG_USER + 0x91700)

#define	DTIMG_ImageName		(DTIMG_TagBase + 1)  // [I..] (CONST_STRPTR) required filename of datatypes image
#define	DTIMG_SelImageName	(DTIMG_TagBase + 2)  // [I..] (CONST_STRPTR) optional Filename of selected state datatypes image
#define DTIMG_DisabledImageName (DTIMG_TagBase + 3)  // [I..] (CONST_STRPTR) optional Filename of selected state datatypes image

//----------------------------------------------------------------------------

#endif	/* DTIMAGECLASS_H */
