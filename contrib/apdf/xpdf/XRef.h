//========================================================================
//
// XRef.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef XREF_H
#define XREF_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "Object.h"

class Dict;
class Stream;
class Parser;
class ObjectStream;
class PageOffsetHintTable;
class SharedObjectHintTable;
class GenericHintTable;
class FormsHintTable;
class ObjectStreamCache;
class PDFDoc;

//------------------------------------------------------------------------
// XRef
//------------------------------------------------------------------------

enum XRefEntryType {
  xrefEntryFree,
  xrefEntryUncompressed,
  xrefEntryCompressed
};

struct XRefEntry {
  Guint offset;
  int gen;
  XRefEntryType type;
};

class XRef {
public:

  // Constructor.  Read xref table from stream.
  XRef(BaseStream *strA, PDFDoc *doc = NULL, GString *ownerPassword = NULL, GString *userPassword = NULL);

  // Destructor.
  ~XRef();

  // Is xref table valid?
  GBool isOk() { return ok; }

  // Get the error code (if isOk() returns false).
  int getErrorCode() { return errCode; }

  // Set the encryption parameters.
  void setEncryption(int permFlagsA, GBool ownerPasswordOkA,
		     Guchar *fileKeyA, int keyLengthA, int encVersionA);

  // Is the file encrypted?
  GBool isEncrypted() { return encrypted; }

  // Is the file linearized?
  GBool isLinearized() { return linearized; }

  // Check various permissions.
  GBool okToPrint(GBool ignoreOwnerPW = gFalse);
  GBool okToChange(GBool ignoreOwnerPW = gFalse);
  GBool okToCopy(GBool ignoreOwnerPW = gFalse);
  GBool okToAddNotes(GBool ignoreOwnerPW = gFalse);

  // Get catalog object.
  Object *getCatalog(Object *obj) { return fetch(rootNum, rootGen, obj); }

  // Fetch an indirect reference.
  Object *fetch(int num, int gen, Object *obj);

  // Return the document's Info dictionary (if any).
  Object *getDocInfo(Object *obj);
  Object *getDocInfoNF(Object *obj);

  // Return the number of objects in the xref table.
  int getNumObjects() { return size; }

  // Return the offset of the last xref table.
  Guint getLastXRefPos() { return lastXRefPos; }

  // Return the catalog object reference.
  int getRootNum() { return rootNum; }
  int getRootGen() { return rootGen; }

  // Return the number of pages, if the document is linearized.
  int getNumPages() { return numPages; }

  // Fetch a page of a linearized file.
  void fetchPage(int n, Object *);

  // Get the page offset hint table.
  PageOffsetHintTable *getPageOffsetHintTable() { return pageOffsetHintTable; }

  // Preload the outlines.
  void preloadOutlines();

  // Preload the named dests.
  void preloadNamedDests();

  // Preload the info dictionary.
  void preloadInfo();

  // Preload the Forms.
  void preloadForms();

  // Get end position for a stream in a damaged file.
  // Returns false if unknown or file is not damaged.
  GBool getStreamEnd(Guint streamStart, Guint *streamEnd);

  // Direct access.
  int getSize() { return size; }
  XRefEntry *getEntry(int i) { return &entries[i]; }
  Object *getTrailerDict() { return &trailerDict; }

private:

  BaseStream *str;		// input stream
  Guint start;			// offset in file (to allow for garbage
				//   at beginning of file)
  XRefEntry *entries;		// xref entries
  int size;			// size of <entries> array
  int rootNum, rootGen;		// catalog dict
  GBool ok;			// true if xref table is valid
  int errCode;			// error code (if <ok> is false)
  Object trailerDict;		// trailer dictionary
  Guint lastXRefPos;		// offset of last xref table
  Guint *streamEnds;		// 'endstream' positions - only used in
				//   damaged files
  int streamEndsLen;		// number of valid entries in streamEnds
  ObjectStreamCache *strCache;	// cached object streams
  GBool encrypted;		// true if file is encrypted
  int permFlags;		// permission bits
  GBool ownerPasswordOk;	// true if owner password is correct
  Guchar fileKey[16];		// file decryption key
  int keyLength;		// length of key, in bytes
  int encVersion;		// encryption algorithm

  Guint getStartXref();
  GBool readXRef(Guint *pos);
  GBool readXRefTable(Parser *parser, Guint *pos);
  GBool readXRefStreamSection(Stream *xrefStr, int *w, int first, int n);
  GBool readXRefStream(Stream *xrefStr, Guint *pos);
  GBool constructXRef();
  Guint strToUnsigned(char *s);
  GBool checkEncrypted(PDFDoc *doc, GString *ownerPassword, GString *userPassword);

  GBool linearized;		// true if the file is linearized
  int numFirstPageXRefs;	// size of the first page XRef table
  int firstPageObjectNum;	// object number of the first page
  int endOfFirstPageOffset;	// offset of the end of the first page data
  int numPages;			// total number of pages
  int mainXRefOffset;		// offset of the first entry of the main
				// XRef table
  int firstPageNum;		// page number of the first page
  int fileSize;			// total file size
  int hintTable1Offset;
  int hintTable1Length;
  int hintTable2Offset;
  int hintTable2Length;

  PageOffsetHintTable *pageOffsetHintTable;
  SharedObjectHintTable *sharedObjectHintTable;
  GenericHintTable *outlinesHintTable;
  GenericHintTable *namedDestsHintTable;
  GenericHintTable *infoHintTable;
  FormsHintTable *formsHintTable;

  GBool readXRefPart(Parser *, int first, int num);
  GBool readXRefPart2(Parser *, int first, int num);
  GBool readTrailerDict();
  GBool getLinearizationInfo();
  GBool skipTrailerLine();
};

#endif
