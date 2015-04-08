/*************************************************************************\
 *                                                                       *
 * mui.h                                                                 *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef MUI_H
#define MUI_H

#ifdef MORPHOS
#   include <ppcinline/muimaster.h>
#else
#   include <proto/muimaster.h>
#endif

#undef Label
#undef Label1
#undef Label2
#undef LLabel
#undef LLabel1
#undef lLabel2
#undef KeyLabel
#undef KeyLabel1
#undef KeyLabel2
#undef KeyLLabel
#undef KeyLLabel1
#undef KeyLLabel2
#undef SimpleButton

#define Label(label)   MUI_MakeObject(MUIO_Label,(LONG)label,0)
#define Label1(label)  MUI_MakeObject(MUIO_Label,(LONG)label,MUIO_Label_SingleFrame)
#define Label2(label)  MUI_MakeObject(MUIO_Label,(LONG)label,MUIO_Label_DoubleFrame)
#define LLabel(label)  MUI_MakeObject(MUIO_Label,(LONG)label,MUIO_Label_LeftAligned)
#define LLabel1(label) MUI_MakeObject(MUIO_Label,(LONG)label,MUIO_Label_LeftAligned|MUIO_Label_SingleFrame)
#define LLabel2(label) MUI_MakeObject(MUIO_Label,(LONG)label,MUIO_Label_LeftAligned|MUIO_Label_DoubleFrame)
#define KeyLabel(label,key)   MUI_MakeObject(MUIO_Label,(LONG)label,key)
#define KeyLabel1(label,key)  MUI_MakeObject(MUIO_Label,(LONG)label,MUIO_Label_SingleFrame|(key))
#define KeyLabel2(label,key)  MUI_MakeObject(MUIO_Label,(LONG)label,MUIO_Label_DoubleFrame|(key))
#define KeyLLabel(label,key)  MUI_MakeObject(MUIO_Label,(LONG)label,MUIO_Label_LeftAligned|(key))
#define KeyLLabel1(label,key) MUI_MakeObject(MUIO_Label,(LONG)label,MUIO_Label_LeftAligned|MUIO_Label_SingleFrame|(key))
#define KeyLLabel2(label,key) MUI_MakeObject(MUIO_Label,(LONG)label,MUIO_Label_LeftAligned|MUIO_Label_DoubleFrame|(key))
#define SimpleButton(label)   MUI_MakeObject(MUIO_Button,(LONG)label)

#endif

