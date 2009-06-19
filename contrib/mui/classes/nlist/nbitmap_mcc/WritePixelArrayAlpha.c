STATIC LONG do_alpha(LONG a, LONG v) 
{
	LONG tmp  = (a*v);
	return ((tmp<<8) + tmp + 32768)>>16;
}

VOID WritePixelArrayAlpha(APTR src, UWORD srcx, UWORD srcy, UWORD srcmod, struct RastPort *rp, UWORD destx, UWORD desty, UWORD width, UWORD height, ULONG globalalpha)
{
	if (width > 0 && height > 0)
	{
		ULONG *buf = AllocMem(width * sizeof(ULONG), MEMF_ANY);

		if (buf)
		{
			ULONG x, y;

			/* Incorrect but cant bother with alpha channel math for now */
			globalalpha = 255 - (globalalpha >> 24);

			for (y = 0; y < height; y++)
			{
				ULONG *pix;

				ReadPixelArray(buf, 0, 0, width * sizeof(ULONG), rp, destx, desty + y, width, 1, RECTFMT_ARGB);

				pix = (ULONG *)((ULONG)src + (srcy + y) * srcmod + srcx * sizeof(ULONG));

				for (x = 0; x < width; x++)
				{
					ULONG srcpix, dstpix, a, r, g, b;

					srcpix = *pix++;
					dstpix = buf[x];

					a = (srcpix >> 24) & 0xff;
					r = (srcpix >> 16) & 0xff;
					g = (srcpix >> 8) & 0xff;
					b = (srcpix >> 0) & 0xff;

					a = a - globalalpha;  

					if (a > 0)
					{
						ULONG dest_r, dest_g, dest_b;

						dest_r = (dstpix >> 16) & 0xff;
						dest_g = (dstpix >> 8) & 0xff;
						dest_b = (dstpix >> 0) & 0xff;

						dest_r += do_alpha(a, r - dest_r);
						dest_g += do_alpha(a, g - dest_g);
						dest_b += do_alpha(a, b - dest_b);

						dstpix = 0xff000000 | r << 16 | g << 8 | b;
					}

					buf[x] = dstpix;
				}

				WritePixelArray(buf, 0, 0, width * sizeof(ULONG), rp, destx, desty + y, width, 1, RECTFMT_ARGB);
			}

			FreeMem(buf, width * sizeof(ULONG));
		}
	}
}      
