#ifndef PRAGMAS_GUIGFX_PRAGMAS_H
#define PRAGMAS_GUIGFX_PRAGMAS_H

#ifndef CLIB_GUIGFX_PROTOS_H
#include <clib/guigfx_protos.h>
#endif

#pragma  libcall GuiGFXBase MakePictureA           01e 910804
#pragma  libcall GuiGFXBase LoadPictureA           024 9802
#pragma  libcall GuiGFXBase ReadPictureA           02a a32109807
#pragma  libcall GuiGFXBase ClonePictureA          030 9802
#pragma  libcall GuiGFXBase DeletePicture          036 801
#pragma  libcall GuiGFXBase AddPictureA            042 a9803
#pragma  libcall GuiGFXBase AddPaletteA            048 a9803
#pragma  libcall GuiGFXBase AddPixelArrayA         04e a109805
#pragma  libcall GuiGFXBase RemColorHandle         054 801
#pragma  libcall GuiGFXBase CreatePenShareMapA     05a 801
#pragma  libcall GuiGFXBase DeletePenShareMap      060 801
#pragma  libcall GuiGFXBase ObtainDrawHandleA      066 ba9804
#pragma  libcall GuiGFXBase ReleaseDrawHandle      06c 801
#pragma  libcall GuiGFXBase DrawPictureA           072 a109805
#pragma  libcall GuiGFXBase MapPaletteA            078 ba9804
#pragma  libcall GuiGFXBase MapPenA                07e a9803
#pragma  libcall GuiGFXBase CreatePictureBitMapA   084 a9803
#pragma  libcall GuiGFXBase DoPictureMethodA       08a 90803
#pragma  libcall GuiGFXBase GetPictureAttrsA       090 9802
#pragma  libcall GuiGFXBase LockPictureA           096 90803
#pragma  libcall GuiGFXBase UnLockPicture          09c 0802
#pragma  libcall GuiGFXBase IsPictureA             0a2 9802
#pragma  libcall GuiGFXBase CreateDirectDrawHandleA 0a8 93210806
#pragma  libcall GuiGFXBase DeleteDirectDrawHandle 0ae 801
#pragma  libcall GuiGFXBase DirectDrawTrueColorA   0b4 a109805
#pragma  libcall GuiGFXBase CreatePictureMaskA     0ba a09804
#ifdef __SASC_60
#pragma  tagcall GuiGFXBase MakePicture            01e 910804
#pragma  tagcall GuiGFXBase LoadPicture            024 9802
#pragma  tagcall GuiGFXBase ReadPicture            02a a32109807
#pragma  tagcall GuiGFXBase ClonePicture           030 9802
#pragma  tagcall GuiGFXBase AddPicture             042 a9803
#pragma  tagcall GuiGFXBase AddPalette             048 a9803
#pragma  tagcall GuiGFXBase AddPixelArray          04e a109805
#pragma  tagcall GuiGFXBase CreatePenShareMap      05a 801
#pragma  tagcall GuiGFXBase ObtainDrawHandle       066 ba9804
#pragma  tagcall GuiGFXBase DrawPicture            072 a109805
#pragma  tagcall GuiGFXBase MapPalette             078 ba9804
#pragma  tagcall GuiGFXBase MapPen                 07e a9803
#pragma  tagcall GuiGFXBase CreatePictureBitMap    084 a9803
#pragma  tagcall GuiGFXBase DoPictureMethod        08a 90803
#pragma  tagcall GuiGFXBase GetPictureAttrs        090 9802
#pragma  tagcall GuiGFXBase LockPicture            096 90803
#pragma  tagcall GuiGFXBase IsPicture              0a2 9802
#pragma  tagcall GuiGFXBase CreateDirectDrawHandle 0a8 93210806
#pragma  tagcall GuiGFXBase DirectDrawTrueColor    0b4 a109805
#pragma  tagcall GuiGFXBase CreatePictureMask      0ba a09804
#endif

#endif	/*  PRAGMAS_GUIGFX_PRAGMA_H  */
