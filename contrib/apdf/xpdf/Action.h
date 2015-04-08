//========================================================================
//
// Action.h
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef ACTION_H
#define ACTION_H

#ifdef __GNUC__
#pragma interface
#endif

#include "Object.h"

class Annot;
class PDFDoc;
class Action;
class TextField;
class ChoiceField;
struct PDFRectangle;

//------------------------------------------------------------------------
// EventHandler
//------------------------------------------------------------------------

enum FitMode {
  fitXYZ, fitPage, fitH, fitV
};

class EventHandler {
public:

  virtual ~EventHandler() {}

  virtual void openDoc(GString *file, GBool newWindow) {}
  virtual void goToPage(int num, PDFRectangle *, FitMode) {}
  virtual void refresh() {}
  virtual void refresh(Annot *annot) {}
  virtual void changeAnnotState(Annot *annot) {}
  virtual void showText(GString *text) {}
  virtual void launch(GString *cmd, GBool newWindow) {}
  virtual void URI(GString *uri, GBool isMap) {}
  virtual void namedAction(const char *action) {}
  virtual void editText(TextField *field) {}
  virtual void editChoice(ChoiceField *field) {}
  virtual void post(GString *url, const char *fileName) {}
};


//------------------------------------------------------------------------
// ActionContext
//------------------------------------------------------------------------

class ActionContext {
public:

  ActionContext(EventHandler *handler1)
  : doc(NULL), handler(handler1), actions(NULL), curAction(0), maxAction(0) {}

  ~ActionContext();

  void openDoc(GString *file, GBool newWindow) { handler->openDoc(file, newWindow); }
  void goToPage(int num, PDFRectangle *rect, FitMode m) { handler->goToPage(num, rect, m); }
  void refresh() { handler->refresh(); }
  void refresh(Annot *annot) { handler->refresh(annot); }
  void changeAnnotState(Annot *annot) { handler->changeAnnotState(annot); }
  void showText(GString *txt) { handler->showText(txt); }
  void launch(GString *cmd, GBool newWindow) { handler->launch(cmd, newWindow); }
  void URI(GString *uri, GBool isMap) { handler->URI(uri, isMap); }
  void namedAction(const char *action) { handler->namedAction(action); }
  void editText(TextField *field) { handler->editText(field); }
  void editChoice(ChoiceField *field) { handler->editChoice(field); }
  void post(GString *url, const char *fileName) { handler->post(url, fileName); }

  PDFDoc *getDoc() { return doc; }
  GString *getBaseURI();
  Annot *findAnnot(GString *);

  void setDoc(PDFDoc *doc1) { doc = doc1; }

  void push(Action *);
  Action *pop();

private:
  PDFDoc *doc;
  EventHandler *handler;
  Action **actions;
  int curAction;
  int maxAction;
};


//------------------------------------------------------------------------
// Destination
//------------------------------------------------------------------------

enum DestKind {
  destXYZ,
  destFit,
  destFitH,
  destFitV,
  destFitR,
  destFitB,
  destFitBH,
  destFitBV,
  destNamed
};

class Destination {
public:

  // Build a LinkDest from an array or a name. If <pageIsRef> is true, the
  // page is specified by an object reference; otherwise the page is
  // specified by a (zero-relative) page number.
  Destination(Object *obj, GBool pageIsRef1 = gTrue);

  // Destructor.
  ~Destination();

  // Copy a Destination.
  Destination *copy() { return new Destination(this); }

  // Was the Destination created successfully?
  GBool isOk() { return ok; }

  // Accessors.
  DestKind getKind() { return kind; }
  GBool isPageRef() { return pageIsRef; }
  int getPageNum() { return pageNum; }
  Ref getPageRef() { return pageRef; }
  double getLeft() { return left; }
  double getBottom() { return bottom; }
  double getRight() { return right; }
  double getTop() { return top; }
  double getZoom() { return zoom; }
  GBool getChangeLeft() { return changeLeft; }
  GBool getChangeTop() { return changeTop; }
  GBool getChangeZoom() { return changeZoom; }
  GString *getName() { return name; }

private:

  DestKind kind;                // destination type
  GBool pageIsRef;              // is the page a reference or number?
  union {
    Ref pageRef;                // reference to page
    int pageNum;                // one-relative page number
    GString *name;              // named destination
  };
  double left, bottom;          // position
  double right, top;
  double zoom;                  // zoom factor
  GBool changeLeft, changeTop;  // for destXYZ links, which position
  GBool changeZoom;             //   components to change
  GBool ok;                     // set if created successfully
  Destination(Destination *dest);
};


//------------------------------------------------------------------------
// Action
//------------------------------------------------------------------------

enum ActionKind {
  actionGoTo,                   // go to destination
  actionGoToR,                  // go to destination in new file
  actionLaunch,                 // launch app (or open document)
  actionThread,                 // enter a thread
  actionURI,                    // URI
  actionSound,                  // start a sound
  actionMovie,                  // start a movie
  actionSetState,               // set an annotation state
  actionHide,                   // show/hide an annotation
  actionNamed,                  // named action
  actionSubmitForm,             // submit a form
  actionResetForm,              // reset a form
  actionImportData,             // import a FDF file
  actionJavascript,             // execute a Javascript program
  actionUnknown                 // anything else
};

class Action {
protected:

  // Constructor
  Action(Dict *);

public:

  // Destructor.
  virtual ~Action();

  // Was the Action created successfully?
  virtual GBool isOk() = 0;

  // Check link action type.
  virtual ActionKind getKind() = 0;

  // Get the description string.
  virtual char *getDescr() { return NULL; }

  // Execute an action.
  Action *execute(ActionContext *);

  // Get the next action to execute.
  Action *getNext() { return next; }

  // Chain another action.
  void add(Action *);

  // Make an action from a dictionary.
  static Action *makeAction(Object *obj);

protected:

  virtual Action *doExecute(ActionContext *) = 0;

private:
  Action *next;
};


//------------------------------------------------------------------------
// ActionGoTo
//------------------------------------------------------------------------

class ActionGoTo: public Action {
public:

  // Constructor.
  ActionGoTo(Dict *dict);
  ActionGoTo(Destination *dest1) : Action(NULL), dest(dest1) {}

  // Destructor.
  virtual ~ActionGoTo();

  // Was the ActionGoTo created successfully?
  virtual GBool isOk() { return dest && dest->isOk(); }

  // Accessors.
  virtual ActionKind getKind() { return actionGoTo; }

  // Get the description string.
  virtual char *getDescr() { return "[internal link]"; }

protected:
  virtual Action *doExecute(ActionContext *);

private:
  Destination *dest;            // action destination
};

//------------------------------------------------------------------------
// ActionGoToR
//------------------------------------------------------------------------

class ActionGoToR: public Action {
public:

  // Constructor.
  ActionGoToR(Dict *);

  // Destructor.
  virtual ~ActionGoToR();

  // Was the ActionGoToR created successfully?
  virtual GBool isOk() { return fileName && goTo && goTo->isOk(); }

  // Accessors.
  virtual ActionKind getKind() { return actionGoToR; }

  // Get the description string.
  virtual char *getDescr() { return fileName->getCString(); }

protected:
  virtual Action *doExecute(ActionContext *);

private:

  GString *fileName;            // file name
  //Destination *dest;            // action destination
  ActionGoTo *goTo;             // GoTo action
  GBool newWindow;              // window in which to open the file
};


//------------------------------------------------------------------------
// ActionLaunch
//------------------------------------------------------------------------

class ActionLaunch: public Action {
public:

  // Constructor.
  ActionLaunch(Dict *dict);

  // Destructor.
  virtual ~ActionLaunch();

  // Was the action created successfully?
  virtual GBool isOk() { return file != NULL; }

  // Accessors.
  virtual ActionKind getKind() { return actionLaunch; }

  // Get the description string.
  virtual char *getDescr() { return file->getCString(); }

protected:
  virtual Action *doExecute(ActionContext *);

private:
  GString *file;                // file specification.
  GBool newWindow;              // should the file be loaded in a new window?
};


//------------------------------------------------------------------------
// ActionThread
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// ActionURI
//------------------------------------------------------------------------

class ActionURI: public Action {
public:

  // Constructor.
  ActionURI(Dict *dict);

  // Destructor.
  virtual ~ActionURI();

  // Was the action created successfully?
  virtual GBool isOk() { return uri != NULL; }

  // Accessors.
  virtual ActionKind getKind() { return actionURI; }

  // Get the description string.
  virtual char *getDescr() { return uri->getCString(); }

protected:
  virtual Action *doExecute(ActionContext *);

private:
  GString *uri;                 // URI.
  GBool isMap;                  // should the mouse position be tracked?
};


//------------------------------------------------------------------------
// ActionSound
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// ActionMovie
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// ActionSetState
//------------------------------------------------------------------------

class ActionSetState : public Action {
public:

  // Constructor.
  ActionSetState(Dict *);

  // Destructor.
  virtual ~ActionSetState();

  // Was the Action created successfully?
  virtual GBool isOk() { return target && state; }

  // Accessors.
  virtual ActionKind getKind() { return actionSetState; }

protected:
  virtual Action *doExecute(ActionContext *);

private:
  GString *target;
  GString *state;
};


//------------------------------------------------------------------------
// ActionHide
//------------------------------------------------------------------------

class ActionHide : public Action {
public:

  // Constructor.
  ActionHide(Dict *);

  // Destructor.
  virtual ~ActionHide();

  // Was the Action created successfully?
  virtual GBool isOk() { return target != NULL; }

  // Accessors.
  virtual ActionKind getKind() { return actionHide; }

protected:
  virtual Action *doExecute(ActionContext *);

private:
  GString *target;
  GBool hide;
};


//------------------------------------------------------------------------
// ActionNamed
//------------------------------------------------------------------------

class ActionNamed : public Action {
public:

  // Constructor.
  ActionNamed(Dict *);

  // Destructor.
  virtual ~ActionNamed();

  // Was the Action created successfully?
  virtual GBool isOk() { return name.isName(); }

  // Accessors.
  virtual ActionKind getKind() { return actionNamed; }

  // Get the description string.
  virtual char *getDescr() { return name.getName(); }

protected:
  virtual Action *doExecute(ActionContext *);

private:
  Object name;
};


//------------------------------------------------------------------------
// ActionSubmitForm
//------------------------------------------------------------------------

const int submitExclude        = 1 << 0;
const int submitIncludeNoValue = 1 << 1;
const int submitExportHTML     = 1 << 2;
const int submitGetMethod      = 1 << 3;
const int submitCoords         = 1 << 4;

class ActionSubmitForm : public Action {
public:

  // Constructor.
  ActionSubmitForm(Dict *);

  // Destructor.
  virtual ~ActionSubmitForm();

  // Was the Action created successfully?
  virtual GBool isOk() { return url != NULL; }

  // Accessors.
  virtual ActionKind getKind() { return actionSubmitForm; }

  // Get the description string.
  virtual char *getDescr() { return "Submit form"; }

protected:
  virtual Action *doExecute(ActionContext *);

private:
  GString *url;
  Object fields;
  int flags;
};


//------------------------------------------------------------------------
// ActionResetForm
//------------------------------------------------------------------------

class ActionResetForm : public Action {
public:

  // Constructor.
  ActionResetForm(Dict *);

  // Destructor.
  virtual ~ActionResetForm();

  // Was the Action created successfully?
  virtual GBool isOk() { return gTrue; }

  // Accessors.
  virtual ActionKind getKind() { return actionResetForm; }

  // Get the description string.
  virtual char *getDescr() { return "Reset form"; }

protected:
  virtual Action *doExecute(ActionContext *);

private:
  Object fields;
  int flags;
};


//------------------------------------------------------------------------
// ActionImportData
//------------------------------------------------------------------------

class ActionImportData : public Action {
public:

  // Constructor.
  ActionImportData(Dict *);

  // Destructor.
  virtual ~ActionImportData();

  // Was the Action created successfully?
  virtual GBool isOk() { return file != NULL; }

  // Accessors.
  virtual ActionKind getKind() { return actionImportData; }

  // Get the description string.
  virtual char *getDescr() { return "Import data"; }

protected:
  virtual Action *doExecute(ActionContext *);

private:
  GString *file;
};


//------------------------------------------------------------------------
// ActionJavascript
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// AdditionalAction
//------------------------------------------------------------------------

enum ActionTrigger {
  trigEnter,
  trigExit,
  trigDown,
  trigUp,
  trigOpen,
  trigClose,
  trigFormat,
  trigValidate,
  trigKeystroke,
  //~ trigCalculate, conflicts with close?
  maxTrig
};

class AdditionalAction {
public:

  // Constructor.
  AdditionalAction(Dict *);

  // Destructor.
  ~AdditionalAction();

  // Accessors.
  Action *getAction(ActionTrigger t) { return actions[t]; }
  void setAction(ActionTrigger t, Action *action);

private:
  Action *actions[maxTrig];
};

#endif

