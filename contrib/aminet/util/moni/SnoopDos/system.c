/*
 *		Dummy to include SYSTEM.H for precompiling headers
 *
 *		We need to do it like this because SYSTEM.H does some #define's
 *		which won't take affect if we compile a GST from it directly
 *		(GST's only apply to #included files, not the main file.)
 */
#include "system.h"
