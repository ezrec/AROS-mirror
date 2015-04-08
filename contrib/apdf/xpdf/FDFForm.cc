//========================================================================
//
// FDFForm.cc
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "GString.h"
#include "config.h"
#include "Object.h"
#include "Stream.h"
#include "XRef.h"
#include "Error.h"
#include "FDFForm.h"
#include "Catalog.h"

GString *parseFileSpecName(Object *fileSpecObj);

//------------------------------------------------------------------------

#define headerSearchSize 1024  // read this many bytes at beginning of
			       //   file to look for '%FDF'

//------------------------------------------------------------------------
// FDFForm
//------------------------------------------------------------------------

FDFForm::FDFForm(GString *fileName1) {
  Object obj;

  // setup
  ok = gFalse;

  file = NULL;
  str = NULL;
  xref = NULL;

  // try to open the file
  fileName = fileName1;
  if (!(file = myfopen(fileName->getCString(), "rb"))) {
    error(-1, "Couldn't open file '%s'", fileName->getCString());
    return;
  }

  // create stream
  obj.initNull();
  str = new FileStream(file, 0, gFalse, 0, &obj);

  ok = setup();
}

FDFForm::FDFForm(BaseStream *str1, GString *fileName1, myFILE *file1) {
  ok = gFalse;
  file = file1;
  str = str1;
  fileName = fileName1;
  xref = NULL;
  ok = setup();
}

GBool FDFForm::setup() {

  // check header
  checkHeader();

  // read xref table
  xref = new XRef(str);
  if (!xref->isOk()) {
    error(-1, "Couldn't read xref table");
    return gFalse;
  }

  // done
  return gTrue;
}

FDFForm::~FDFForm() {
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
}

// Check for a FDF header on this stream.  Skip past some garbage
// if necessary.
void FDFForm::checkHeader() {
  char hdrBuf[headerSearchSize+1];
  char *p;
  int i;

  fdfVersion = 0;
  for (i = 0; i < headerSearchSize; ++i) {
    hdrBuf[i] = str->getChar();
  }
  hdrBuf[headerSearchSize] = '\0';
  for (i = 0; i < headerSearchSize - 5; ++i) {
    if (!strncmp(&hdrBuf[i], "%FDF-", 5)) {
      break;
    }
  }
  if (i >= headerSearchSize - 5) {
    error(-1, "May not be a FDF file (continuing anyway)");
    return;
  }
  str->moveStart(i);
  p = strtok(&hdrBuf[i+5], " \t\n\r");
  fdfVersion = atof(p);
  if (!(hdrBuf[i+5] >= '0' && hdrBuf[i+5] <= '9') ||
      fdfVersion > supportedPDFVersionNum + 0.0001) {
    error(-1, "FDF version %s -- xpdf supports version %s"
	 " (continuing anyway)", p, supportedPDFVersionStr);
  }
}

GString *FDFForm::getPDFFileName() {
  Object catObj, obj;
  GString *str;

  if (getCatalog(&catObj)->isDict()) {
    if (!catObj.dictLookup("F", &obj)->isNull()) {
      str = parseFileSpecName(&obj);
      obj.free();
      catObj.free();
      return str;
    }
  }
  catObj.free();
  return NULL;
}

