// McpFrameMCC.h
// $Date$
// $Revision$


#ifndef MCPFRAME_MCC_H
#define	MCPFRAME_MCC_H

/* ------------------------------------------------------------------------- */

#include <defs.h>
#include "MCPFrame_mcc.h"

/* ------------------------------------------------------------------------- */

struct MUI_CustomClass *InitMcpFrameClass(void);
void CleanupMcpFrameClass(struct MUI_CustomClass *mcc);

/* ------------------------------------------------------------------------- */

extern struct MUI_CustomClass *MccFrameClass;

/* ------------------------------------------------------------------------- */

#endif /* MCPFRAME_MCC_H */
