
#include <stdio.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <exec/exec.h>
#include <exec/memory.h>

void main (void)
{
	ULONG *colortable;
	if (colortable = AllocVec(sizeof(ULONG) * 6*6*6, MEMF_ANY))
	{
		BPTR fh;
		int y,u,v;
		DOUBLE Y, U, V;
		ULONG *p = colortable;
		
		for (y = 0; y < 6; ++y)
		{
			for (u = 0; u < 6; ++u)
			{
				for (v = 0; v < 6; ++v)
				{
						Y = (255 * y / 5);
						U = (222 * u / 5) - 111;
						V = (312 * v / 5) - 156;
						
						*p++ =
								((int)(Y + 1.140 * V) << 16) +
								((int)(Y - 0.396 * U - 0.581 * V) << 8) +
								(int)(Y + 2.029 * U);
				}
			}
		}

		fh = Open("ram:yuv", MODE_NEWFILE);
		if (fh)
		{
			int l;
			char buf[1000];
			p = colortable;
			for (y = 0; y < 6 * 6; ++y)
			{
				sprintf(buf, "	0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,\n", p[0],p[1],p[2],p[3],p[4],p[5]);
				l = strlen(buf);				
				p += 6;
				Write(fh, buf, l);
			}
		
			Close(fh);
		}
	}
}

