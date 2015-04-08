//========================================================================
//
// Action.cc
//
// Copyright 2000-2005 Emmanuel Lesueur
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <stddef.h>
#include <string.h>
#include "gmem.h"
#include "GString.h"
#include "Error.h"
#include "Object.h"
#include "Array.h"
#include "Dict.h"
#include "Catalog.h"
#include "Page.h"
#include "XRef.h"
#include "PDFDoc.h"
#include "Action.h"
#include "Annotations.h"
#include "AcroForm.h"

//------------------------------------------------------------------------

GString *parseFileSpecName(Object *fileSpecObj);


//------------------------------------------------------------------------
// Destination
//------------------------------------------------------------------------

Destination::Destination(Object *obj, GBool pageIsRef1) { DEBUG_INFO
  Object obj1, obj2;

  // initialize fields
  pageIsRef = pageIsRef1;
  left = bottom = right = top = zoom = 0;
  kind = destXYZ;
  ok = gFalse;

  if (obj->isName()) {

    kind = destNamed;
    name = new GString(obj->getName());
    ok = gTrue;

  } else if (obj->isString()) {

    kind = destNamed;
    name = obj->getString()->copy();
    ok = gTrue;

  } else if (obj->isArray()) {
    Array *a = obj->getArray();

    // get page
    if (pageIsRef) {
      if (!a->getNF(0, &obj1)->isRef()) {
	error(-1, "Bad annotation destination");
	goto err2;
      }
      pageRef.num = obj1.getRefNum();
      pageRef.gen = obj1.getRefGen();
      obj1.free();
    } else {
      if (!a->get(0, &obj1)->isInt()) {
	error(-1, "Bad annotation destination");
	goto err2;
      }
      pageNum = obj1.getInt() + 1;
      obj1.free();
    }

    // get destination type
    a->get(1, &obj1);

    // XYZ link
    if (obj1.isName("XYZ")) {
      kind = destXYZ;
      a->get(2, &obj2);
      if (obj2.isNull()) {
	changeLeft = gFalse;
      } else if (obj2.isNum()) {
	changeLeft = gTrue;
	left = obj2.getNum();
      } else {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      obj2.free();
      a->get(3, &obj2);
      if (obj2.isNull()) {
	changeTop = gFalse;
      } else if (obj2.isNum()) {
	changeTop = gTrue;
	top = obj2.getNum();
      } else {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      obj2.free();
      a->get(4, &obj2);
      if (obj2.isNull()) {
	changeZoom = gFalse;
      } else if (obj2.isNum()) {
	changeZoom = gTrue;
	zoom = obj2.getNum();
      } else {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      obj2.free();

    // Fit link
    } else if (obj1.isName("Fit")) {
      kind = destFit;

    // FitH link
    } else if (obj1.isName("FitH")) {
      kind = destFitH;
      if (!a->get(2, &obj2)->isNum()) {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      top = obj2.getNum();
      obj2.free();

    // FitV link
    } else if (obj1.isName("FitV")) {
      kind = destFitV;
      if (!a->get(2, &obj2)->isNum()) {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      left = obj2.getNum();
      obj2.free();

    // FitR link
    } else if (obj1.isName("FitR")) {
      kind = destFitR;
      if (!a->get(2, &obj2)->isNum()) {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      left = obj2.getNum();
      obj2.free();
      if (!a->get(3, &obj2)->isNum()) {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      bottom = obj2.getNum();
      obj2.free();
      if (!a->get(4, &obj2)->isNum()) {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      right = obj2.getNum();
      obj2.free();
      if (!a->get(5, &obj2)->isNum()) {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      top = obj2.getNum();
      obj2.free();

    // FitB link
    } else if (obj1.isName("FitB")) {
      kind = destFitB;

    // FitBH link
    } else if (obj1.isName("FitBH")) {
      kind = destFitBH;
      if (!a->get(2, &obj2)->isNum()) {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      top = obj2.getNum();
      obj2.free();

    // FitBV link
    } else if (obj1.isName("FitBV")) {
      kind = destFitBV;
      if (!a->get(2, &obj2)->isNum()) {
	error(-1, "Bad annotation destination position");
	goto err1;
      }
      left = obj2.getNum();
      obj2.free();

    // unknown link kind
    } else {
      error(-1, "Unknown annotation destination type");
      goto err2;
    }

    obj1.free();
    ok = gTrue;
    return;

   err1:
    obj2.free();
   err2:
    obj1.free();
  } else
    error(-1, "Bad link destination");
}

Destination::Destination(Destination *dest) { DEBUG_INFO
  kind = dest->kind;
  if (kind == destNamed)
    name = dest->name->copy();
  else {
    pageIsRef = dest->pageIsRef;
    if (pageIsRef)
      pageRef = dest->pageRef;
    else
      pageNum = dest->pageNum;
    left = dest->left;
    bottom = dest->bottom;
    right = dest->right;
    top = dest->top;
    zoom = dest->zoom;
    changeLeft = dest->changeLeft;
    changeTop = dest->changeTop;
    changeZoom = dest->changeZoom;
  }
  ok = gTrue;
}

Destination::~Destination() {
  if (kind == destNamed)
    delete name;
}


//------------------------------------------------------------------------
// ActionContext
//------------------------------------------------------------------------

ActionContext::~ActionContext() {
  if (actions)
    gfree(actions);
}

GString *ActionContext::getBaseURI(){
  return doc->getCatalog()->getBaseURI();
}

Annot *ActionContext::findAnnot(GString *name) {
  AcroForm *form = getDoc()->getCatalog()->getAcroForm();
  if (form) {
    Field *field = form->findField(name);
    if (field)
      return field->getAnnot();
  }
  return NULL;
}

void ActionContext::push(Action *action) {
  if (curAction = maxAction) {
    maxAction += 8;
    actions = (Action**)grealloc(actions, maxAction * sizeof(*actions));
  }
  actions[curAction++] = action;
}

Action *ActionContext::pop() {
  if (curAction)
    return actions[--curAction];
  else
    return NULL;
}


//------------------------------------------------------------------------
// Action
//------------------------------------------------------------------------

Action::Action(Dict *dict) { DEBUG_INFO

  next = NULL;

  if (dict) {
    Object obj;

    if (!dict->lookup("Next", &obj)->isNull())
      next = makeAction(&obj);
    obj.free();
  }
}

Action::~Action() {
  delete next;
}

void Action::add(Action *action) {
  Action *action1 = this;
  while (action1->next)
    action1 = action1->next;
  action1->next = action;
}

Action *Action::execute(ActionContext *context) {
  Action *action2 = doExecute(context);
  if (action2 && next)
    context->push(next);
  return action2 ? action2 : (next ? next : context->pop());
}

Action *Action::makeAction(Object *obj1) { DEBUG_INFO
  Object obj2, obj3, obj4, obj5;
  Action *action = NULL;

  if (!obj1->isDict()) {
    error(-1, "Invalid action");
    return NULL;
  }

  obj1->dictLookup("S", &obj2);

  // GoTo action
  if (obj2.isName("GoTo")) {
    action = new ActionGoTo(obj1->getDict());

    // GoToR action
  } else if (obj2.isName("GoToR")) {
    action = new ActionGoToR(obj1->getDict());

    // Launch action
  } else if (obj2.isName("Launch")) {
    action = new ActionLaunch(obj1->getDict());

    // URI action
  } else if (obj2.isName("URI")) {
    action = new ActionURI(obj1->getDict());

    // SetState action
  } else if (obj2.isName("SetState")) {
    action = new ActionSetState(obj1->getDict());

    // Hide action
  } else if (obj2.isName("Hide")) {
    action = new ActionHide(obj1->getDict());

    // Named action
  } else if (obj2.isName("Named")) {
    action = new ActionNamed(obj1->getDict());

    // SubmitForm action
  } else if (obj2.isName("SubmitForm")) {
    action = new ActionSubmitForm(obj1->getDict());

    // ResetForm action
  } else if (obj2.isName("ResetForm")) {
    action = new ActionResetForm(obj1->getDict());

    // ImportData action
  } else if (obj2.isName("ImportData")) {
    action = new ActionImportData(obj1->getDict());

    // JavaScript action
  } else if (obj2.isName("JavaScript")) {
    error(-1, "JavaScript not supported.");
    action = NULL;

    // unknown action
  } else if (obj2.isName()) {
    error(-1, "Unknown action type: %s.", obj2.getName());
    action = NULL;
    //action = new ActionUnknown(obj2.getName());

    // action is missing or wrong type
  } else {
    error(-1, "Bad action");
    action = NULL;
  }

  if (action && !action->isOk()) {
    delete action;
    action = NULL;
  }

  obj2.free();
  return action;
}


//------------------------------------------------------------------------
// ActionGoTo
//------------------------------------------------------------------------

ActionGoTo::ActionGoTo(Dict *dict) : Action(dict) { DEBUG_INFO
  Object obj;

  dest = NULL;
  if (dict->lookup("D", &obj))
    dest = new Destination(&obj);
  obj.free();
}

ActionGoTo::~ActionGoTo() {
  delete dest;
}

Action *ActionGoTo::doExecute(ActionContext *context) { DEBUG_INFO
  int num;
  struct PDFRectangle rect;
  FitMode mode;
  Page *page;
  Destination *dest1 = dest;

  if (dest1->getKind() == destNamed) {
//printf("named dest: %s\n",dest1->getName()->getCString());
    dest1 = context->getDoc()->findDest(dest1->getName());
    if (!dest1) {
      error(-1, "Invalid destination");
      return NULL;
    }
  }
//printf("dest kind=%d\n",dest1->getKind());

  if (dest1->isPageRef()) {
    Ref ref = dest1->getPageRef();
    num = context->getDoc()->findPage(ref.num, ref.gen);
  } else
    num = dest1->getPageNum();
  page = context->getDoc()->getCatalog()->getPage(num);

  switch (dest1->getKind()) {
  case destXYZ:
    mode = fitXYZ;
    rect.x1 = dest1->getChangeLeft() ? dest1->getLeft() : -10000;
    rect.y1 = dest1->getChangeTop() ? dest1->getTop() : -10000;
    rect.x2 = 0;
    rect.y2 = 0;
    break;
  case destFit:
    mode = fitPage;
    rect = *page->getMediaBox();
    break;
  case destFitH:
    mode = fitH;
    rect.y1 = dest1->getTop();
    rect.x1 = page->getMediaBox()->x1;
    rect.x2 = page->getMediaBox()->x2;
    rect.y2 = 0;
    break;
  case destFitV:
    mode = fitV;
    rect.x1 = dest1->getLeft();
    rect.y1 = page->getMediaBox()->y1;
    rect.y2 = page->getMediaBox()->y2;
    rect.x2 = 0;
    break;
  case destFitR:
    mode = fitPage;
    rect.x1 = dest1->getLeft();
    rect.y1 = dest1->getBottom();
    rect.x2 = dest1->getRight();
    rect.y2 = dest1->getTop();
    break;
  case destFitB:
    mode = fitPage;
    rect = *page->getCropBox();
    break;
  case destFitBH:
    mode = fitH;
    rect.y1 = dest1->getTop();
    rect.x1 = page->getCropBox()->x1;
    rect.x2 = page->getCropBox()->x2;
    rect.y2 = 0;
    break;
  case destFitBV:
    mode = fitV;
    rect.x1 = dest1->getLeft();
    rect.y1 = page->getCropBox()->y1;
    rect.y2 = page->getCropBox()->y2;
    rect.x2 = 0;
    break;
  default:
    if (dest != dest1)
      delete dest1;
    return NULL;
  }

  if (dest != dest1)
    delete dest1;
  context->goToPage(num, &rect, mode);
  return NULL;
}


//------------------------------------------------------------------------
// ActionGoToR
//------------------------------------------------------------------------

ActionGoToR::ActionGoToR(Dict *dict) : Action(dict) { DEBUG_INFO
  Object obj;

  goTo = NULL;
  fileName = NULL;
  newWindow = gFalse;

  if (dict->lookup("D", &obj))
    goTo = new ActionGoTo(new Destination(&obj, gFalse));
  obj.free();

  if (dict->lookup("F", &obj))
    fileName = parseFileSpecName(&obj);
  obj.free();

  if (dict->lookup("NewWindow", &obj)->isBool())
    newWindow = obj.getBool();
  obj.free();
}

ActionGoToR::~ActionGoToR() {
  delete goTo;
  delete fileName;
}

Action *ActionGoToR::doExecute(ActionContext *context) {
  context->openDoc(fileName, newWindow);
  return goTo;
}


//------------------------------------------------------------------------
// ActionLaunch
//------------------------------------------------------------------------

ActionLaunch::ActionLaunch(Dict *dict) : Action(dict) { DEBUG_INFO
  Object obj;

  file = NULL;
  newWindow = gFalse;

  if (dict->lookup("F", &obj))
    file = parseFileSpecName(&obj);
  obj.free();

  if (dict->lookup("NewWindow", &obj)->isBool())
    newWindow = obj.getBool();
  obj.free();
}

ActionLaunch::~ActionLaunch() {
  delete file;
}

Action *ActionLaunch::doExecute(ActionContext *context) {
  context->launch(file, newWindow);
  return NULL;
}


//------------------------------------------------------------------------
// ActionURI
//------------------------------------------------------------------------

ActionURI::ActionURI(Dict *dict) : Action(dict) { DEBUG_INFO
  Object obj;

  uri = NULL;
  isMap = gFalse;

  if (dict->lookup("URI", &obj)->isString())
    uri = obj.getString()->copy();
  obj.free();

  if (dict->lookup("IsMap", &obj)->isBool())
    isMap = obj.getBool();
  obj.free();
}

ActionURI::~ActionURI() {
  delete uri;
}

Action *ActionURI::doExecute(ActionContext *context) {
  context->URI(uri, isMap);
  return NULL;
}


//------------------------------------------------------------------------
// ActionSetState
//------------------------------------------------------------------------

ActionSetState::ActionSetState(Dict *dict) : Action(dict) { DEBUG_INFO
  Object obj;

  target = NULL;
  state = NULL;

  if (dict->lookup("T", &obj)->isString())
    target = obj.getString()->copy();
  obj.free();

  if (dict->lookup("AS", &obj)->isName())
    state = new GString(obj.getName());
  obj.free();
}

ActionSetState::~ActionSetState() {
  delete target;
  delete state;
}

Action *ActionSetState::doExecute(ActionContext *context) {
  Annot *annot;

  if (annot = context->findAnnot(target)) {
    annot->setAppearanceState(state->getCString());
    context->refresh(annot);
  }

  return NULL;
}


//------------------------------------------------------------------------
// ActionHide
//------------------------------------------------------------------------

ActionHide::ActionHide(Dict *dict) : Action(dict) { DEBUG_INFO
  Object obj;

  target = NULL;
  hide = gTrue;

  if (dict->lookup("T", &obj)->isString())
    target = obj.getString()->copy();
  obj.free();

  if (dict->lookup("H", &obj)->isBool())
    hide = obj.getBool();
  obj.free();
}

ActionHide::~ActionHide() {
  delete target;
}

Action *ActionHide::doExecute(ActionContext *context) {
  Annot *annot;

  if (annot = context->findAnnot(target)) {
    annot->hide(hide);
    context->refresh(annot);
    context->changeAnnotState(annot);
  }

  return NULL;
}


//------------------------------------------------------------------------
// ActionNamed
//------------------------------------------------------------------------

ActionNamed::ActionNamed(Dict *dict) : Action(dict) { DEBUG_INFO
  dict->lookup("N", &name);
}

ActionNamed::~ActionNamed() {
  name.free();
}

Action *ActionNamed::doExecute(ActionContext *context) {
  context->namedAction(name.getName());
  return NULL;
}


//------------------------------------------------------------------------
// ActionSubmitForm
//------------------------------------------------------------------------

ActionSubmitForm::ActionSubmitForm(Dict *dict) : Action(dict) { DEBUG_INFO
  Object obj;

  url = NULL;
  flags = 0;

  if (dict->lookup("F", &obj))
    url = parseFileSpecName(&obj);
  obj.free();

  dict->lookup("Fields", &fields);

  if (dict->lookup("Flags", &obj)->isInt())
    flags = obj.getInt();
  obj.free();
}

ActionSubmitForm::~ActionSubmitForm() {
  fields.free();
  delete url;
}

Action *ActionSubmitForm::doExecute(ActionContext *context) { DEBUG_INFO
  AcroForm *form = context->getDoc()->getCatalog()->getAcroForm();
  SubmitContext *submitContext;
  int k;

  if (fields.isArray()) {
    Array *array = fields.getArray();
    Object obj;
    Field *field;

    if (flags & submitExclude && !(flags & submitIncludeNoValue)) {
      for (k = 0; k < form->getNumRootFields(); ++k)
	form->getRootField(k)->setExportFlags(flags & submitIncludeNoValue);
    } else {
      GBool flag = (flags & submitExclude) != 0;
      for (k = 0; k < form->getNumFields(); ++k)
	form->getField(k)->setExportFlag(flag);
    }

    for (k = 0; k <array->getLength(); ++k) {
      array->getNF(k, &obj);
      if (obj.isRef())
	field = form->findField(obj.getRefNum(), obj.getRefGen());
      else if (obj.isString())
	field = form->findField(obj.getString());
      else {
	field = NULL;
	error(-1, "Invalid fields array");
      }
      if (field) {
	if (flags & submitExclude) {
	  field->setExportFlag(gFalse);

	} else {
	  field->setExportFlags(flags & submitIncludeNoValue);
	}
      }
      obj.free();
    }

  } else {
    for (k = 0; k < form->getNumRootFields(); ++k)
      form->getRootField(k)->setExportFlags((flags & submitIncludeNoValue) != 0);
  }

  if (flags & submitExportHTML) {
    if (flags & submitGetMethod) {
      submitContext = new SubmitHTMLGetContext;
    } else {
      submitContext = new SubmitHTMLPostContext("t:tmp.html"); //~
    }
  } else {
    submitContext = new SubmitFDFContext("t:tmp.fdf"); //~
  }

  for (k = 0; k < form->getNumRootFields(); ++k)
    form->getRootField(k)->submit(submitContext);

  if (flags & submitExportHTML) {
    //~ map
    if (flags & submitGetMethod) {
      GString *str = url->copy();
      str->append('?');
      str->append(((SubmitHTMLGetContext *)submitContext)->getContents());
      context->URI(str, gFalse);
      delete str;
    } else {
      context->post(url, "t:tmp.html");
    }
  } else {
    context->post(url, "t:tmp.fdf");
  }

  delete submitContext;

  return NULL;
}


//------------------------------------------------------------------------
// ActionResetForm
//------------------------------------------------------------------------

ActionResetForm::ActionResetForm(Dict *dict) : Action(dict) { DEBUG_INFO
  Object obj;

  flags = 0;

  dict->lookup("Fields", &fields);

  if (dict->lookup("Flags", &obj)->isInt())
    flags = obj.getInt();
  obj.free();
}

ActionResetForm::~ActionResetForm() {
  fields.free();
}

Action *ActionResetForm::doExecute(ActionContext *context) { DEBUG_INFO
  AcroForm *form = context->getDoc()->getCatalog()->getAcroForm();

  if (fields.isArray()) {
    Array *array = fields.getArray();
    int k, l;
    Object obj;
    Field *field;
    Field **fields;

    if (flags & 1) {
      fields = new Field * [form->getNumFields()];
      for (k = 0; k < form->getNumFields(); ++k)
	fields[k] = form->getField(k);
    }

    for (k = 0; k <array->getLength(); ++k) {
      array->getNF(k, &obj);
      if (obj.isRef())
	field = form->findField(obj.getRefNum(), obj.getRefGen());
      else if (obj.isString())
	field = form->findField(obj.getString());
      else {
	field = NULL;
	error(-1, "Invalid fields array");
      }
      if (field) {
	if (flags & 1) {
	  // reset all fields except those in the array.
	  for (l = 0; l < form->getNumFields(); ++l)
	    if (fields[l] == field) {
	      fields[l] = NULL;
	      break;
	    }
	} else {
	  // reset fields designated by the array, including kids.
	  field->resetFieldsTree();
	}
      }
      obj.free();
    }

    if (flags & 1) {
      for (l = 0; l < form->getNumFields(); ++l)
	if (fields[l])
	  fields[l]->reset();
      delete [] fields;
    }

  } else {
    int k;

    for (k = 0; k < form->getNumFields(); ++k) {
      form->getField(k)->reset();
    }
  }
  context->refresh();
  return NULL;
}


//------------------------------------------------------------------------
// ActionImportData
//------------------------------------------------------------------------

ActionImportData::ActionImportData(Dict *dict) : Action(dict) { DEBUG_INFO
  Object obj;

  file = NULL;

  if (dict->lookup("F", &obj))
    file = parseFileSpecName(&obj);
  obj.free();
}

ActionImportData::~ActionImportData() {
  delete file;
}

Action *ActionImportData::doExecute(ActionContext *context) { DEBUG_INFO
  AcroForm *form = context->getDoc()->getCatalog()->getAcroForm();

  if (form && form->importFDF(file->copy())) {
    context->refresh();
  }

  return NULL;
}


//------------------------------------------------------------------------
// AdditionalAction
//------------------------------------------------------------------------

AdditionalAction::AdditionalAction(Dict *dict) { DEBUG_INFO
  int k;

  for (k = 0; k < maxTrig; ++k)
    actions[k] = NULL;
  if (dict) {
    for (k = 0; k < maxTrig; ++k) {
      Object obj;
      char key[2];

      key[0] = "EXDUOCFVK" [k];
      key[1] = '\0';

      if (dict->lookup(key, &obj)->isDict())
	actions[k] = Action::makeAction(&obj);
      obj.free();
    }
  }
}

AdditionalAction::~AdditionalAction() {
  int k;
  for (k = 0; k < maxTrig; ++k)
    delete actions[k];
}

void AdditionalAction::setAction(ActionTrigger t, Action *action) {
  action->add(actions[t]);
  actions[t] = action;
}


//------------------------------------------------------------------------

// Extract a file name from a file specification (string or dictionary).
GString *parseFileSpecName(Object *fileSpecObj) { DEBUG_INFO
  GString *name;
  Object obj1;

  name = NULL;

  // string
  if (fileSpecObj->isString()) {
    name = fileSpecObj->getString()->copy();

  // dictionary
  } else if (fileSpecObj->isDict()) {
    if (!fileSpecObj->dictLookup("Unix", &obj1)->isString()) {
      obj1.free();
      fileSpecObj->dictLookup("F", &obj1);
    }
    if (obj1.isString())
      name = obj1.getString()->copy();
    else
      error(-1, "Illegal file spec in link");
    obj1.free();

  // error
  } else {
    error(-1, "Illegal file spec in link");
  }

  return name;
}

