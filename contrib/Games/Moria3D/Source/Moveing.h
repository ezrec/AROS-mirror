/****************************************************************************
*                                                                           *
* Moveing.h                                                          970319 *
*                                                                           *
* Move the Player around                                                    *
*                                                                           *
****************************************************************************/

#ifndef MOVEING_H
#define MOVEING_H

#include "MyTypes.h"
#include "Level.h"

#define DefSpeedUp   0.25
#define DefSpeedDown 0.75

extern int32 Button1,Button2;
extern struct MsgPort * WindowPort;

extern void MovePlayer(pBot MyPlayer);
extern void MoveBots(void);

#endif

