#ifndef MYGLYPHREQUEST_H
#define MYGLYPHREQUEST_H

struct MyGlyphReq
{
	WORD			gr_DeviceDPI_X;
	WORD			gr_DeviceDPI_Y;
	WORD			gr_GlyphCode;
	WORD			gr_GlyphCode2;
	FIXED			gr_PointHeight;
	FIXED			gr_GlyphWidth;
	FIXED			gr_SetFactor;
	FIXED			gr_RotateSin;
	FIXED 		gr_RotateCos;
	FIXED			gr_ShearSin;
	FIXED			gr_ShearCos;
};

#endif

