// ScalosMcpGfx.c
// $Date$
// $Revision$


#ifndef SCALOS_MCPGFX_H
#define	SCALOS_MCPGFX_H

#include <libraries/mcpgfx.h>

/* ------------------------------------------------------------------------- */

void McpGfxRectFill(struct RastPort *rp,
	UWORD MinX, WORD MinY, WORD MaxX, WORD MaxY, ULONG FirstTag, ...);
void McpGfxDrawFrame(struct RastPort *rp,
	UWORD MinX, WORD MinY, WORD MaxX, WORD MaxY, ULONG FirstTag, ...);
const struct FrameSize *McpGetFrameSize(UWORD frameType);

/* ------------------------------------------------------------------------- */

#endif /* SCALOS_MCPGFX_H */
