#if !defined(AROS_VISUAL_FUNCTIONS_H)
#define AROS_VISUAL_FUNCTIONS_H

#include <GL/gl.h>
#include "AROSMesa_intern.h"

AROSMesaVisual aros_new_visual(GLboolean db_flag);
void aros_delete_visual(AROSMesaVisual aros_vis);

#endif /* AROS_VISUAL_FUNCTIONS_H */
