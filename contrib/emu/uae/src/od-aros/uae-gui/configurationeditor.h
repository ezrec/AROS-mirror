#ifndef _CONFIGURATIONEDITOR_H_
#define _CONFIGURATIONEDITOR_H_

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>

/*** Variables **************************************************************/
extern struct MUI_CustomClass *ConfigurationEditor_CLASS;

/*** Macros *****************************************************************/
#define ConfigurationEditorObject BOOPSIOBJMACRO_START(ConfigurationEditor_CLASS->mcc_Class)

#endif /* _CONFIGURATIONEDITOR_H_ */
