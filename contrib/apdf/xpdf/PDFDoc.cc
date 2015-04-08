//========================================================================
//
// PDFDoc.cc
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================
//
// EL (10/99): Added an activate() member function.
//
#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#ifdef WIN32
#  include <windows.h>
#endif
#include "GString.h"
#include "config.h"
#include "GlobalParams.h"
#include "Page.h"
#include "Catalog.h"
#include "Stream.h"
#include "XRef.h"
#include "Annotations.h"
#include "OutputDev.h"
#include "Error.h"
#include "ErrorCodes.h"
#include "Lexer.h"
#include "Parser.h"
#include "SecurityHandler.h"
#ifndef DISABLE_OUTLINE
#include "Outline2.h"
#endif
#include "PDFDoc.h"
#include "FDFForm.h"
#include "AcroForm.h"

//------------------------------------------------------------------------

#define headerSearchSize 1024  // read this many bytes at beginning of
			       //   file to look for '%PDF'

//------------------------------------------------------------------------
// PDFDoc
//------------------------------------------------------------------------

PDFDoc::PDFDoc(GString *fileNameA, GString *ownerPassword,
	       GString *userPassword, void *guiDataA) {
  DEBUG_INFO
  Object obj;
  GString *fileName1, *fileName2;

  // setup
  ok = gFalse;
  errCode = errNone;

  guiData = guiDataA;

  file = NULL;
  str = NULL;
  xref = NULL;
  catalog = NULL;
  fdfForm = NULL;
#ifndef DISABLE_OUTLINE
  outline = NULL;
#endif

  fileName = fileNameA;
  fileName1 = fileName;

  // try to open file
  fileName2 = NULL;
#ifdef VMS
  if (!(file = fopen(fileName1->getCString(), "rb", "ctx=stm"))) {
    error(-1, "Couldn't open file '%s'", fileName1->getCString());
    errCode = errOpenFile;
    return;
  }
#elif defined(__AMIGA__)
  if (!(file = myfopen(fileName1->getCString(), "rb"))) {
    error(-1, "Couldn't open file '%s'", fileName1->getCString());
    return;
  }
#else
  if (!(file = myfopen(fileName1->getCString(), "rb"))) {
    fileName2 = fileName->copy();
    fileName2->lowerCase();
    if (!(file = myfopen(fileName2->getCString(), "rb"))) {
      fileName2->upperCase();
      if (!(file = myfopen(fileName2->getCString(), "rb"))) {
	error(-1, "Couldn't open file '%s'", fileName->getCString());
	delete fileName2;
	errCode = errOpenFile;
	return;
      }
    }
    delete fileName2;
  }
#endif

  // create stream
  obj.initNull();
  str = new FileStream(file, 0, gFalse, 0, &obj);

  // prepare buffers
  file->add_parts();
  file->preread(0, 1024);

  ok = setup(ownerPassword, userPassword);
}

#ifdef WIN32
PDFDoc::PDFDoc(wchar_t *fileNameA, int fileNameLen, GString *ownerPassword,
	       GString *userPassword, void *guiDataA) {
  OSVERSIONINFO version;
  wchar_t fileName2[_MAX_PATH + 1];
  Object obj;
  int i;

  ok = gFalse;
  errCode = errNone;

  guiData = guiDataA;

  file = NULL;
  str = NULL;
  xref = NULL;
  catalog = NULL;
  links = NULL;
#ifndef DISABLE_OUTLINE
  outline = NULL;
#endif

  //~ file name should be stored in Unicode (?)
  fileName = new GString();
  for (i = 0; i < fileNameLen; ++i) {
    fileName->append((char)fileNameA[i]);
  }

  // zero-terminate the file name string
  for (i = 0; i < fileNameLen && i < _MAX_PATH; ++i) {
    fileName2[i] = fileNameA[i];
  }
  fileName2[i] = 0;

  // try to open file
  // NB: _wfopen is only available in NT
  version.dwOSVersionInfoSize = sizeof(version);
  GetVersionEx(&version);
  if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    file = _wfopen(fileName2, L"rb");
  } else {
    file = fopen(fileName->getCString(), "rb");
  }
  if (!file) {
    error(-1, "Couldn't open file '%s'", fileName->getCString());
    errCode = errOpenFile;
    return;
  }

  // create stream
  obj.initNull();
  str = new FileStream(file, 0, gFalse, 0, &obj);

  ok = setup(ownerPassword, userPassword);
}
#endif

PDFDoc::PDFDoc(BaseStream *strA, GString *ownerPassword,
	       GString *userPassword, void *guiDataA) {
  DEBUG_INFO
  ok = gFalse;
  errCode = errNone;
  guiData = guiDataA;
  fileName = NULL;
  file = NULL;
  str = strA;
  xref = NULL;
  catalog = NULL;
  fdfForm = NULL;
#ifndef DISABLE_OUTLINE
  outline = NULL;
#endif
  ok = setup(ownerPassword, userPassword);
}

GBool PDFDoc::setup(GString *ownerPassword, GString *userPassword) {
  DEBUG_INFO
  str->reset();

  // check header
  if (!checkHeader()) {
    return gFalse;
  }

  // read xref table
  xref = new XRef(str, this, ownerPassword, userPassword);
  if (!xref->isOk()) {
    error(-1, "Couldn't read xref table");
    errCode = xref->getErrorCode();
    return gFalse;
  }

  // read catalog
  catalog = new Catalog(xref);
  if (!catalog->isOk()) {
    error(-1, "Couldn't read page catalog");
    errCode = errBadCatalog;
    return gFalse;
  }

  // If we were loaded from an FDF file, import that file.
  if (fdfForm) {
    AcroForm *form = catalog->getAcroForm();
    Object catObj, obj;
    if (form) {
      if (fdfForm->getCatalog(&catObj)->isDict()) {
	if (catObj.dictLookup("FDF", &obj)->isDict()) {
	  form->importFDF(obj.getDict());
	}
	obj.free();
      }
      catObj.free();
      activate();
    }
  }

#ifndef DISABLE_OUTLINE
  // read outline
  outline = new Outline(catalog->getOutline(), xref);
#endif

  // done
  return gTrue;
}

PDFDoc::~PDFDoc() {
  DEBUG_INFO
#ifndef DISABLE_OUTLINE
  if (outline) {
    delete outline;
  }
#endif
  if (catalog) {
    delete catalog;
  }
  if (xref) {
    delete xref;
  }
  if (str) {
    delete str;
  }
  if (file) {
    myfclose(file);
  }
  if (fileName) {
    delete fileName;
  }
  if (fdfForm) {
    delete fdfForm;
  }
}

// Check for a PDF header on this stream.  Skip past some garbage
// if necessary.
GBool PDFDoc::checkHeader() {
  DEBUG_INFO
  char hdrBuf[headerSearchSize+1];
  char *p;
  int i;

  pdfVersion = 0;
  for (i = 0; i < headerSearchSize; ++i) {
    hdrBuf[i] = str->getChar();
  }
  hdrBuf[headerSearchSize] = '\0';
  for (i = 0; i < headerSearchSize - 5; ++i) {
    if (!strncmp(&hdrBuf[i], "%PDF-", 5)) {
      break;
    }
  }
  if (i >= headerSearchSize - 5) {
    Object obj;
    i = 0;

    if (!fdfForm) {
      // Check if it's an FDF file.
      for (i = 0; i < headerSearchSize - 5; ++i) {
	if (!strncmp(&hdrBuf[i], "%FDF-", 5)) {
	  break;
	}
      }
    }

    if (i >= headerSearchSize - 5) {
      error(-1, "May not be a PDF file (continuing anyway)");
      return gTrue;
    }

    // It's an FDF file. Load it and try to open the PDF file
    // from which it depends.
    str->reset();
    fdfForm = new FDFForm(str, fileName, file);
    str = NULL;
    fileName = NULL;
    file = NULL;
    if (!fdfForm->isOk()) {
      error(-1, "Bad FDF Form");
      return gFalse;
    }

    fileName = fdfForm->getPDFFileName();

    if (!fileName) {
      error(-1, "This FDF has no PDF base file.");
      return gFalse;
    }
    if (!(file = myfopen(fileName->getCString(), "rb"))) {
      error(-1, "Couldn't open file '%s'", fileName->getCString());
      return gFalse;
    }

    // create stream
    obj.initNull();
    str = new FileStream(file, 0, gFalse, 0, &obj);

    // prepare buffers
    file->add_parts();
    file->preread(0, 1024);

    return checkHeader();
  }
  str->moveStart(i);
  if (!(p = strtok(&hdrBuf[i+5], " \t\n\r"))) {
    error(-1, "May not be a PDF file (continuing anyway)");
    return gTrue;
  }
  pdfVersion = atof(p);
  if (!(hdrBuf[i+5] >= '0' && hdrBuf[i+5] <= '9') ||
      pdfVersion > supportedPDFVersionNum + 0.0001) {
    error(-1, "PDF version %s -- xpdf supports version %s"
	 " (continuing anyway)", p, supportedPDFVersionStr);
  }
  return gTrue;
}

#if 0
GBool PDFDoc::checkEncryption(GString *ownerPassword, GString *userPassword) {
  Object encrypt;
  GBool encrypted;
  SecurityHandler *secHdlr;
  GBool ret;

  xref->getTrailerDict()->dictLookup("Encrypt", &encrypt);
  if ((encrypted = encrypt.isDict())) {
    if ((secHdlr = SecurityHandler::make(this, &encrypt))) {
      if (secHdlr->checkEncryption(ownerPassword, userPassword)) {
	// authorization succeeded
       	xref->setEncryption(secHdlr->getPermissionFlags(),
			    secHdlr->getOwnerPasswordOk(),
			    secHdlr->getFileKey(),
			    secHdlr->getFileKeyLength(),
			    secHdlr->getEncVersion());
	ret = gTrue;
      } else {
	// authorization failed
	ret = gFalse;
      }
      delete secHdlr;
    } else {
      // couldn't find the matching security handler
      ret = gFalse;
    }
  } else {
    // document is not encrypted
    ret = gTrue;
  }
  encrypt.free();
  return ret;
}
#endif

void PDFDoc::displayPage(OutputDev *out, int page, double hDPI, double vDPI,
			 int rotate, GBool useMediaBox, GBool crop,
			 GBool doAnnots,
			 GBool (*abortCheckCbk)(void *data),
			 void *abortCheckCbkData) {
  DEBUG_INFO
  if (globalParams->getPrintCommands()) {
    printf("***** page %d *****\n", page);
  }
  getAnnots(page);
  catalog->getPage(page)->display(out, hDPI, vDPI, rotate, useMediaBox, crop, NULL, catalog,
      abortCheckCbk, abortCheckCbkData);
}

void PDFDoc::displayPages(OutputDev *out, int firstPage, int lastPage,
			  double hDPI, double vDPI, int rotate,
			  GBool useMediaBox, GBool crop, GBool doLinks,
			  GBool (*abortCheckCbk)(void *data),
			  void *abortCheckCbkData) {
  int page;

  for (page = firstPage; page <= lastPage; ++page) {
    displayPage(out, page, hDPI, vDPI, rotate, useMediaBox, crop, doLinks,
      abortCheckCbk, abortCheckCbkData);
  }
}

void PDFDoc::displayPageSlice(OutputDev *out, int page,
			      double hDPI, double vDPI, int rotate,
			      GBool useMediaBox, GBool crop, GBool doLinks,
			      int sliceX, int sliceY, int sliceW, int sliceH,
			      GBool (*abortCheckCbk)(void *data),
			      void *abortCheckCbkData) {
  Page *p;

  p = catalog->getPage(page);
  p->displaySlice(out, hDPI, vDPI, rotate, useMediaBox, crop,
		  sliceX, sliceY, sliceW, sliceH,
		  NULL, catalog, abortCheckCbk, abortCheckCbkData);
}

#if 0
Links *PDFDoc::takeLinks() {
  Links *ret;

  ret = links;
  links = NULL;
  return ret;
}

GBool PDFDoc::isLinearized() {
  Parser *parser;
  Object obj1, obj2, obj3, obj4, obj5;
  GBool lin;

  lin = gFalse;
  obj1.initNull();
  parser = new Parser(xref,
	   new Lexer(xref,
	     str->makeSubStream(str->getStart(), gFalse, 0, &obj1)));
  parser->getObj(&obj1);
  parser->getObj(&obj2);
  parser->getObj(&obj3);
  parser->getObj(&obj4);
  if (obj1.isInt() && obj2.isInt() && obj3.isCmd("obj") &&
      obj4.isDict()) {
    obj4.dictLookup("Linearized", &obj5);
    if (obj5.isNum() && obj5.getNum() > 0) {
      lin = gTrue;
    }
    obj5.free();
  }
  obj4.free();
  obj3.free();
  obj2.free();
  obj1.free();
  delete parser;
  return lin;
}
#endif

GBool PDFDoc::saveAs(GString *name) {
  FILE *f;
  int c;

  if (!(f = fopen(name->getCString(), "wb"))) {
    error(-1, "Couldn't open file '%s'", name->getCString());
    return gFalse;
  }
  str->reset();
  while ((c = str->getChar()) != EOF) {
    fputc(c, f);
  }
  str->close();
  fclose(f);
  return gTrue;
}

Annots *PDFDoc::getAnnots(int page) {
  DEBUG_INFO
  return catalog->getPage(page)->getAnnots(catalog);
}

void PDFDoc::activate() {
  ::baseFontCache = &baseFontCache;
  ::gfxFontCache = &gfxFontCache;
  ::docFontMap = &fontMap;
}
