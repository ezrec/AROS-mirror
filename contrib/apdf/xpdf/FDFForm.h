//========================================================================
//
// FDFForm.h
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef FDFFORM_H
#define FDFFORM_H

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include "server/mystdio.h"

class GString;
class BaseStream;
class XRef;

//------------------------------------------------------------------------
// FDFForm
//------------------------------------------------------------------------

class FDFForm {
public:

  FDFForm(GString *fileName1);
  FDFForm(BaseStream *str1, GString *fileName1, myFILE *file1);
  ~FDFForm();

  // Was FDF document successfully opened?
  GBool isOk() { return ok; }

  // Get file name.
  GString *getFileName() { return fileName; }

  // Get catalog.
  Object *getCatalog(Object *obj) { return xref->getCatalog(obj); }

  // Get base stream.
  BaseStream *getBaseStream() { return str; }

  // Return the FDF version specified by the file.
  double getFDFVersion() { return fdfVersion; }

  // Get the name of the base PDF file.
  GString *getPDFFileName();

private:

  GBool setup();
  void checkHeader();

  GString *fileName;
  myFILE *file;
  BaseStream *str;
  double fdfVersion;
  XRef *xref;

  GBool ok;
};

#endif

