//========================================================================
//
// AROSPDFApp.cc
//
// Copyright 2005 Vijay Kumar B. <vijaykumar@bravegnu.org>
//
//========================================================================

#ifndef AROSPDFAPP_H
#define AROSPDFAPP_H

#define BITMAPX 600
#define BITMAPY 400
#define SCROLLSPEED 20
#include "AROSSplashOutputDev.h"
#include <libraries/asl.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include "gtypes.h"
#include "SplashTypes.h"
#include <string.h>
#include <devices/rawkeycodes.h>
#include <math.h>

class GString;
class PDFDoc;
class AROSSplashOutputDev;

class AROSPDFApp {
public:

  AROSPDFApp(GString *fileNameA, GString *ownerPWA, GString *userPWA);
  AROSPDFApp();
  ~AROSPDFApp();
  void OpenFile(GString *fileNameA, GString *ownerPWA, GString *userPWA);
  int run();
  void quit();
  GBool isOk() { return ok; }; 
  static void redraw(AROSPDFApp *data);
  void zoomIn(int n);
  void zoomOut(int n);
  bool isReady() { return ready; };
  void setVert(int n);
  void setHoriz(int n);
  Object * getBM() { return Bmp; };
  void ResizeBitMap(int width, int height);
  void AboutMenu(); 
  void RequestOpenFile();
  void ZoomInOut(bool In);
  void PageForwardBack(bool Forward);
  void JumpToPage(int pageno);
  void JumpToPageTextbox();
  void ZoomToFit(bool WidthOnly);
  void ToggleFitWidth() { FitWidth =!FitWidth; };
  void ToggleBestFit() { BestFit =!BestFit; };
  void ScrollUpDown(bool Down);
  void DisableBestFit(); 
  void DisableFitWidth();
  void EnableDisableGUI();
  void ToggleFullscreen();
  void BackFillFullScreen();
private:
  int initAROS();
  void exitAROS();

  PDFDoc *doc;  
  AROSSplashOutputDev *splashOut;
  struct RastPort * rp;
  struct RastPort * oldrp;
  int dispW, dispH;
  int resolution;
  GBool ok;
  int posX, posY;
  int page;
  bool ready;
  bool docLoaded;
  bool FitWidth;
  bool BestFit;
  bool FullScreen;
  GBool resLowLimit, resHiLimit;
  Object *wnd,  *muiapp, *but,*Bmp,*vslider,*hslider, *txt, *rct,
         *butFirst,*butPrev,*butNext,*butLast,*txtPage, *txtPageCount,
         *menustrip, *butZoomOut, *butZoomIn;
  struct BitMap *bm;
  struct DrawInfo *dri;
  struct Screen *fsscreen;
  struct Window *fswindow;
  struct Screen *origscreen;
  ULONG pen1;
  struct MUI_CustomClass *mcc;
};

#endif
