//========================================================================
//
// AcroForm.h
//
// Copyright 2000-2005 Emmanuel Lesueur
//
//========================================================================

#ifndef ACROFORM_H
#define ACROFORM_H

#ifdef __GNUC__
#pragma interface
#endif

#include "Object.h"
#include "Annotations.h"

class WidgetAnnot;
class RadioField;
class Action;
class ActionContext;
class SubmitContext;

enum FieldType {
  fieldNode,
  fieldPushButton,
  fieldRadio,
  fieldCheckbox,
  fieldText,
  fieldChoice,
  fieldSig
};

const int fldReadOnly    = 1 << 0;
const int fldRequired    = 1 << 1;
const int fldNoExport    = 1 << 2;
const int fldMultiline   = 1 << 12;
const int fldPassword    = 1 << 13;
const int fldNoToggleOff = 1 << 14;
const int fldRadio       = 1 << 15;
const int fldPushButton  = 1 << 16;
const int fldPopUp       = 1 << 17;
const int fldEdit        = 1 << 18;
const int fldSort        = 1 << 19;


//------------------------------------------------------------------------
// FieldAttrs
//------------------------------------------------------------------------

class FieldAttrs {
public:

  // Constructor.
  FieldAttrs(FieldAttrs *attrs, Dict *dict);

  // Destructor.
  ~FieldAttrs();

  // Accessors.
  Object *getValue(Object *obj) { return value.copy(obj); }
  Object *getDefaultValue(Object *obj) { return defaultValue.copy(obj); }
  int getFlags() { return flags; }
  Object *getDefaultResources(Object *obj) { return defaultResources.copy(obj); }
  GString *getDefaultAppearance() { return defaultAppearance; }
  int getQuadding() { return quadding; }
  GString *getTypeName() { return typeName; }
  Object *getOpt(Object *obj) { return opt.copy(obj); }
  int getTopIndex() { return topIndex; }
  int getMaxLen() { return maxLen; }
  GString *getName() { return name; }
  Ref getRef() { return ref; }

  void setName(GString *name1) { delete name; name = NULL; name = name1->copy(); }
  void setFlags(int flags1) { flags = flags1; }
  void setRef(Ref ref1) { ref = ref1; }

private:
  Object value;
  Object defaultValue;
  int flags;
  Object defaultResources;
  GString *defaultAppearance;
  int quadding;
  GString *typeName;
  Object opt;
  int topIndex;
  int maxLen;
  GString *name;
  Ref ref;
};


//------------------------------------------------------------------------
// SubmitContext
//------------------------------------------------------------------------

class SubmitContext {
public:

  // Destructor.
  virtual ~SubmitContext() {}

  // write data.
  virtual void beginField(GString *name) =0;
  virtual void endField() {}
  virtual void addValue(GString *value) =0;
  virtual void addNameValue(GString *value) =0;
  virtual void beginKids() {}
  virtual void endKids() {}
};

//------------------------------------------------------------------------
// SubmitFDFContext
//------------------------------------------------------------------------

class SubmitFDFContext : public SubmitContext {
public:

  // Constructor.
  SubmitFDFContext(const char *fileName);

  // Destructor.
  virtual ~SubmitFDFContext();

  // write data.
  virtual void beginField(GString *name);
  virtual void endField();
  virtual void addValue(GString *value);
  virtual void addNameValue(GString *value);
  virtual void beginKids();
  virtual void endKids();

private:
  FILE *file;
};


//------------------------------------------------------------------------
// SubmitHTMLGetContext
//------------------------------------------------------------------------

class SubmitHTMLGetContext : public SubmitContext {
public:

  // Constructor.
  SubmitHTMLGetContext();

  // Destructor.
  virtual ~SubmitHTMLGetContext();

  // Write data.
  virtual void beginField(GString *name);
  virtual void addValue(GString *value);
  virtual void addNameValue(GString *value);

  // Get the result.
  GString *getContents() const { return str; }

private:
  GString *str;
};


//------------------------------------------------------------------------
// SubmitHTMLPostContext
//------------------------------------------------------------------------

class SubmitHTMLPostContext : public SubmitContext {
public:

  // Constructor.
  SubmitHTMLPostContext(const char *fileName);

  // Destructor.
  virtual ~SubmitHTMLPostContext();

  // Write data.
  virtual void beginField(GString *name);
  virtual void addValue(GString *value);
  virtual void addNameValue(GString *value);

private:
  FILE *file;
};


//------------------------------------------------------------------------
// Field
//------------------------------------------------------------------------

class Field {
public:

  // Constructor.
  Field(XRef *xref, Dict *dict, FieldAttrs *attrs);

  // Destructor.
  virtual ~Field();

  // Was the field constructed successfully?
  virtual GBool isOk() { return gTrue; }

  // Get the field type.
  virtual FieldType getType() =0;

  // Reset to default value.
  virtual void reset() {}

  // Has the field a value?
  virtual GBool hasValue() { return gFalse; }

  // Build the field appearance stream.
  virtual Object *buildAppearance(Object *obj) { return obj->initNull(); }

  // The widget is activated.
  virtual Action *down(ActionContext *) { return NULL; }

  // Import an FDF field
  virtual GBool import(Dict *);

  // Associate an annotation.
  virtual void setAnnot(WidgetAnnot *annot1);

  // Get the associated annotation, if any.
  WidgetAnnot *getAnnot() { return annot; }

  // Get the partial field name.
  GString *getName() { return name; }

  // Is the field read-only?
  GBool isReadOnly() { return flags & fldReadOnly; }

  // Is the field required?
  GBool isRequired() { return flags & fldRequired; }

  // Is the field exportable?
  GBool noExport() { return flags & fldNoExport; }

  // Get the user name.
  GString *getUserName() { return userName; }

  // Get the mapping name.
  GString *getMappingName() { return mappingName; }

  // Get the ref.
  int getRefNum() { return ref.num; }
  int getRefGen() { return ref.gen; }

  // Reset this field and its chilren.
  void resetFieldsTree();

  // Submit the field.
  void submit(SubmitContext *);

  // Select fields to include in a submit operation.
  void setExportFlag(GBool f) { exportFlag = f; }
  void setExportFlags(GBool f);

  // Add a child.
  void addChild(Field *field) { field->next = kids; kids = field; }

protected:
  // Submit the field value.
  virtual GBool doSubmit(SubmitContext *);

  int flags;

private:
  Ref ref;
  GString *name;
  GString *userName;
  GString *mappingName;
  WidgetAnnot *annot;
  Field *next;                  // next field in the same level
  Field *kids;                  // first kid
  AnnotState annotState;
  GBool exportFlag;
};


//------------------------------------------------------------------------
// NodeField
//------------------------------------------------------------------------

class NodeField : public Field {
public:

  // Constructor.
  NodeField(XRef *xref, Dict *dict, FieldAttrs *attrs);

  // Get the field type.
  virtual FieldType getType() { return fieldNode; }
};


//------------------------------------------------------------------------
// PushButtonField
//------------------------------------------------------------------------

class PushButtonField : public Field {
public:

  // Constructor.
  PushButtonField(XRef *xref, Dict *dict, FieldAttrs *attrs);

  // Get the field type.
  virtual FieldType getType() { return fieldPushButton; }
};


//------------------------------------------------------------------------
// CheckboxField
//------------------------------------------------------------------------

class CheckboxField : public Field {
public:

  // Constructor.
  CheckboxField(XRef *xref, Dict *dict, FieldAttrs *attrs);

  // Destructor.
  virtual ~CheckboxField();

  // Get the field type.
  virtual FieldType getType() { return fieldCheckbox; }

  // Reset to default value.
  virtual void reset();

  // The widget is activated.
  virtual Action *down(ActionContext *);

  // Has the field a value?
  virtual GBool hasValue() { return gTrue; }

  // Import an FDF field
  virtual GBool import(Dict *);

  // Get value.
  char *getValue() { return value.getName(); }

  // Set parent radio
  void setRadio(RadioField *radio1) { radio = radio1; }

  // Toggle the state
  void toggle();

protected:
  // Submit the field value.
  virtual GBool doSubmit(SubmitContext *);

private:
  Object value;
  Object defaultValue;
  RadioField *radio;
};


//------------------------------------------------------------------------
// RadioField
//------------------------------------------------------------------------

class RadioField : public Field {
public:

  // Constructor.
  RadioField(XRef *xref, Dict *dict, FieldAttrs *attrs);

  // Destructor.
  virtual ~RadioField();

  // Were the radio and its checkboxes constructed successfully?
  virtual GBool isOk() { return numChoices && numChoices == maxChoices; }

  // Get the field type.
  virtual FieldType getType() { return fieldRadio; }

  // Reset to default value.
  virtual void reset();

  // Has the field a value?
  virtual GBool hasValue() { return gTrue; }

  // Import an FDF field
  virtual GBool import(Dict *);

  // The widget is activated.
  Action *down(ActionContext *, CheckboxField *);

  // Get the current value.
  char *getValue() { return value.getName(); }

  // Can this radio be off?
  GBool isNoToggleToOff() const { return flags & fldNoToggleOff; }

  // Add a checkbox.
  void addChoice(CheckboxField *);

protected:
  // Submit the field value.
  virtual GBool doSubmit(SubmitContext *);

private:
  int numChoices;
  int maxChoices;
  CheckboxField **choices;
  Object value;
  Object defaultValue;
};


//------------------------------------------------------------------------
// TextBaseField
//------------------------------------------------------------------------

class TextBaseField : public Field {
public:

  // Constructor.
  TextBaseField(XRef *xref, Dict *dict, FieldAttrs *attrs);

  // Destructor.
  virtual ~TextBaseField();

  // Associate an annotation.
  virtual void setAnnot(WidgetAnnot *annot1);

  /*Dict *getDefaultResources();

  GString *getDefaultAppearance();

  getHighlight();*/

  int getQuadding() { return quadding; }

  int getOrigHeight();

protected:
  GString *makeAppStreamData();
  void buildAppearance();
  void rebuildAppearance();
  virtual GString *getValueAppearance() =0;

  Object defaultResources;
  GString *defaultAppearance;
  int origHeight;
  int quadding;
  int lines;
  GBool changed;
  XRef *xref;
};


//------------------------------------------------------------------------
// ChoiceField
//------------------------------------------------------------------------

class ChoiceField : public TextBaseField {
public:

  // Constructor.
  ChoiceField(XRef *xref, Dict *dict, FieldAttrs *attrs);

  // Destructor.
  virtual ~ChoiceField();

  // Was the field constructed successfully?
  virtual GBool isOk() { return opt.isArray(); }

  // Get the field type.
  virtual FieldType getType() { return fieldChoice; }

  // Reset to default value.
  virtual void reset();

  // The widget is activated.
  virtual Action *down(ActionContext *);

  // Has the field a value?
  virtual GBool hasValue();

  // Import an FDF field.
  virtual GBool import(Dict *);

  // Get the field's value.
  GString *getValue() const { return value; }

  // Get the number of entries.
  int getNumEntries();

  // Get a choice entry.
  GString *getEntry(int);

  // Set a new value.
  void setChoice(int n);
  void setChoice(GString *);

  GBool isPopUp() const { return flags & fldPopUp; }
  GBool isEditable() const { return flags & fldEdit; }

  void update(ActionContext *, GString *);

protected:
  // Submit the field value.
  virtual GBool doSubmit(SubmitContext *);

private:
  virtual GString *getValueAppearance();

  Object opt;
  GString *value;
  GString *defaultValue;
  int topIndex;
};


//------------------------------------------------------------------------
// TextField
//------------------------------------------------------------------------

class TextField : public TextBaseField {
public:

  // Constructor.
  TextField(XRef *xref, Dict *dict, FieldAttrs *attrs);

  // Destructor.
  virtual ~TextField();

  // Get the field type.
  virtual FieldType getType() { return fieldText; }

  // Reset to default value.
  virtual void reset();

  // The widget is activated.
  virtual Action *down(ActionContext *);

  // Has the field a value?
  virtual GBool hasValue() { return value.isString(); }

  // Import an FDF field
  virtual GBool import(Dict *);

  /*Dict *getDefaultResources();

  GString *getDefaultAppearance();

  getHighlight();*/

  void update(ActionContext *, GString *);

  GBool isMultiline() { return flags & fldMultiline; }
  GBool isPassword() { return flags & fldPassword; }

  int getMaxLen() { return maxLen; }
  GString *getValue() { return value.isString() ? value.getString() : NULL; }

protected:
  // Submit the field value.
  virtual GBool doSubmit(SubmitContext *);

private:
  virtual GString *getValueAppearance();

  Object value;
  Object defaultValue;
  int maxLen;
};


//------------------------------------------------------------------------
// AcroForm
//------------------------------------------------------------------------

class AcroForm {
public:

  // Constructor.
  AcroForm(XRef *, Object *);

  // Destructor.
  ~AcroForm();

  // Was the AcroForm created successfully?
  GBool isOk() { return ok; }

  // Get the number of fields.
  int getNumFields() { return numFields; }

  // Get a field.
  Field *getField(int n) { return fields[n]; }

  // Get the number of root fields.
  int getNumRootFields() { return numRootFields; }

  // Get a root field.
  Field *getRootField(int n) { return rootFields[n]; }

  // Find a particular field.
  Field *findField(int num, int gen);
  Field *findField(GString *);

  // Import a FDF dictionary.
  GBool importFDF(Dict *dict);

  // Import a FDF file.
  GBool importFDF(GString *fileName);

  // Export a FDF file.
  GBool exportFDF(GString *fileName);

private:
  Field *readFieldTree(XRef *, Object *, FieldAttrs *);
  void addField(Field *);
  GBool importField(GString *name, Object *dict);

  Field **fields;
  Field **rootFields;
  int numFields;
  int numRootFields;
  int maxFields;
  GBool needAppearances;
  GBool ok;
};

#endif

