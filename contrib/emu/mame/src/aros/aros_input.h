#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

extern int use_mouse, joystick, use_hotrod;
extern BOOL keypressed[256];

void AROS_Input_Update(void);
