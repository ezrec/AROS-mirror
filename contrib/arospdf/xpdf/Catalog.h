//========================================================================
//
// Catalog.h
//
// Copyright 1996-2007 Glyph & Cog, LLC
//
//========================================================================

#ifndef CATALOG_H
#define CATALOG_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

class XRef;
class xObject;
class Page;
class PageAttrs;
struct Ref;
class LinkDest;

//------------------------------------------------------------------------
// Catalog
//------------------------------------------------------------------------

class Catalog {
public:

  // Constructor.
  Catalog(XRef *xrefA);

  // Destructor.
  ~Catalog();

  // Is catalog valid?
  GBool isOk() { return ok; }

  // Get number of pages.
  int getNumPages() { return numPages; }

  // Get a page.
  Page *getPage(int i) { return pages[i-1]; }

  // Get the reference for a page xObject.
  Ref *getPageRef(int i) { return &pageRefs[i-1]; }

  // Return base URI, or NULL if none.
  GString *getBaseURI() { return baseURI; }

  // Return the contents of the metadata stream, or NULL if there is
  // no metadata.
  GString *readMetadata();

  // Return the structure tree root xObject.
  xObject *getStructTreeRoot() { return &structTreeRoot; }

  // Find a page, given its xObject ID.  Returns page number, or 0 if
  // not found.
  int findPage(int num, int gen);

  // Find a named destination.  Returns the link destination, or
  // NULL if <name> is not a destination.
  LinkDest *findDest(GString *name);

  xObject *getDests() { return &dests; }

  xObject *getNameTree() { return &nameTree; }

  xObject *getOutline() { return &outline; }

  xObject *getAcroForm() { return &acroForm; }

private:

  XRef *xref;			// the xref table for this PDF file
  Page **pages;			// array of pages
  Ref *pageRefs;		// xObject ID for each page
  int numPages;			// number of pages
  int pagesSize;		// size of pages array
  xObject dests;			// named destination dictionary
  xObject nameTree;		// name tree
  GString *baseURI;		// base URI for URI-type links
  xObject metadata;		// metadata stream
  xObject structTreeRoot;	// structure tree root dictionary
  xObject outline;		// outline dictionary
  xObject acroForm;		// AcroForm dictionary
  GBool ok;			// true if catalog is valid

  int readPageTree(Dict *pages, PageAttrs *attrs, int start,
		   char *alreadyRead);
  xObject *findDestInTree(xObject *tree, GString *name, xObject *obj);
};

#endif
