/****************************************************************
   This file was created automatically by `FlexCat V1.0'
   Do NOT edit by hand!
****************************************************************/

#include "amosaic_cat.h"
#include <proto/locale.h>
#include <proto/alib.h>

static LONG amosaic_Version = 1;
static const STRPTR amosaic_BuiltInLanguage = (STRPTR) "english";

struct FC_Type
{   LONG   ID;
    STRPTR Str;
};


static const struct FC_Type amosaic_Array [] =
{
    {MSG_MENU_PROJECT, (STRPTR) "Project"},
    {MSG_MENU_OPEN_URL, (STRPTR) "Open URL..."},
    {MSG_MENU_OPEN_URL_SC, (STRPTR) "O"},
    {MSG_MENU_OPEN_LOCAL, (STRPTR) "Open Local..."},
    {MSG_MENU_OPEN_LOCAL_SC, (STRPTR) "L"},
    {MSG_MENU_RELOAD_CURRENT, (STRPTR) "Reload Current"},
    {MSG_MENU_RELOAD_CURRENT_SC, (STRPTR) "R"},
    {MSG_MENU_REFRESH_CURRENT, (STRPTR) "Refresh Current"},
    {MSG_MENU_URL_TO_CLIPBOARD, (STRPTR) "Copy URL to clipboard"},
    {MSG_MENU_URL_TO_CLIPBOARD_SC, (STRPTR) "C"},
    {MSG_MENU_CLIPBOARD_TO_URL, (STRPTR) "Paste clipboard to URL"},
    {MSG_MENU_CLIPBOARD_TO_URL_SC, (STRPTR) "V"},
    {MSG_MENU_FIND_IN_CURRENT, (STRPTR) "Find in Current..."},
    {MSG_MENU_FIND_IN_CURRENT_SC, (STRPTR) "/"},
    {MSG_MENU_VIEW_SOURCE, (STRPTR) "View Source..."},
    {MSG_MENU_VIEW_SOURCE_SC, (STRPTR) "S"},
    {MSG_MENU_SAVE_AS, (STRPTR) "Save As..."},
    {MSG_MENU_SAVE_AS_SC, (STRPTR) "A"},
    {MSG_MENU_PRINT, (STRPTR) "Print..."},
    {MSG_MENU_PRINT_SC, (STRPTR) "P"},
    {MSG_MENU_ABOUT, (STRPTR) "About..."},
    {MSG_MENU_ABOUT_SC, (STRPTR) "?"},
    {MSG_MENU_QUIT, (STRPTR) "Quit"},
    {MSG_MENU_QUIT_SC, (STRPTR) "Q"},
    {MSG_MENU_EDIT, (STRPTR) "Edit"},
    {MSG_MENU_COPY, (STRPTR) "Copy"},
    {MSG_MENU_COPY_SC, (STRPTR) " "},
    {MSG_MENU_OPTIONS, (STRPTR) "Options"},
    {MSG_MENU_LOAD_TO_DISK, (STRPTR) "Load To Local Disk"},
    {MSG_MENU_LOAD_TO_DISK_SC, (STRPTR) "T"},
    {MSG_MENU_DELAY_IMAGE_LOADING, (STRPTR) "Delay Image Loading"},
    {MSG_MENU_DELAY_IMAGE_LOADING_SC, (STRPTR) "D"},
    {MSG_MENU_LOAD_IMAGES, (STRPTR) "Load Images In Current"},
    {MSG_MENU_LOAD_IMAGES_SC, (STRPTR) "I"},
    {MSG_MENU_ISINDEX_TO_FORM, (STRPTR) "Isindex -> Form"},
    {MSG_MENU_BUTTONS, (STRPTR) "Buttons"},
    {MSG_MENU_TEXT_ONLY, (STRPTR) "Text only"},
    {MSG_MENU_TEXT_ONLY_SC, (STRPTR) "-"},
    {MSG_MENU_IMAGES_ONLY, (STRPTR) "Images only"},
    {MSG_MENU_IMAGES_ONLY_SC, (STRPTR) "="},
    {MSG_MENU_IMAGES_AND_TEXT, (STRPTR) "Images & Text"},
    {MSG_MENU_IMAGES_AND_TEXT_SC, (STRPTR) "\\"},
    {MSG_MENU_FLUSH_IMAGE_CACHE, (STRPTR) "Flush Image Cache"},
    {MSG_MENU_FLUSH_IMAGE_CACHE_SC, (STRPTR) "X"},
    {MSG_MENU_CLEAR_GLOBAL_HISTORY, (STRPTR) "Clear Global History"},
    {MSG_MENU_CLEAR_GLOBAL_HISTORY_SC, (STRPTR) "G"},
    {MSG_MENU_NAVIGATE, (STRPTR) "Navigate"},
    {MSG_MENU_BACK, (STRPTR) "Back"},
    {MSG_MENU_BACK_SC, (STRPTR) "B"},
    {MSG_MENU_FORWARD, (STRPTR) "Forward"},
    {MSG_MENU_FORWARD_SC, (STRPTR) "F"},
    {MSG_MENU_HOME_DOCUMENT, (STRPTR) "Home Document"},
    {MSG_MENU_HOME_DOCUMENT_SC, (STRPTR) "H"},
    {MSG_MENU_WINDOW_HISTORY, (STRPTR) "Window History"},
    {MSG_MENU_WINDOW_HISTORY_SC, (STRPTR) "W"},
    {MSG_MENU_HOTLIST, (STRPTR) "Hotlist... (rexx)"},
    {MSG_MENU_HOTLIST_SC, (STRPTR) "J"},
    {MSG_MENU_ADD_HOTLIST, (STRPTR) "Add Current To Hotlist (rexx)"},
    {MSG_MENU_ADD_HOTLIST_SC, (STRPTR) "E"},
    {MSG_MENU_REXX, (STRPTR) "Rexx"},
    {MSG_MENU_MACRO, (STRPTR) "Macro"},
    {MSG_MENU_MACRO_SC, (STRPTR) "M"},
    {MSG_GADGET_TITLE, (STRPTR) "Title:"},
    {MSG_GADGET_URL, (STRPTR) "URL:"},
    {MSG_HOTLIST_ADD, (STRPTR) "Add"},
    {MSG_HOTLIST_NEW, (STRPTR) "New..."},
    {MSG_HOTLIST_DELETE, (STRPTR) "Delete"},
    {MSG_HOTLIST_EDIT, (STRPTR) "Edit..."},
    {MSG_HOTLIST_SORT, (STRPTR) "Sort"},
    {MSG_HOTLIST_MOVE, (STRPTR) "Move"},
    {MSG_HOTLIST_PARENT, (STRPTR) "Parent"},
    {MSG_HOTLIST_EDITGROUP_TITLE, (STRPTR) "Edit hotlist group entry"},
    {MSG_HOTLIST_GROUP_DEFAULTNAME, (STRPTR) "(Unnamed)"},
    {MSG_HOTLIST_EDIT_WINDOW_TITLE, (STRPTR) "Edit hotlist entry"},
    {MSG_HOTLIST_EDIT_TITLE, (STRPTR) "Title:"},
    {MSG_HOTLIST_EDIT_URL, (STRPTR) "URL:"},
    {MSG_HOTLIST_EDIT_OK, (STRPTR) "_Ok"},
    {MSG_HOTLIST_EDIT_CANCEL, (STRPTR) "_Cancel"},
    {MSG_BUTTON_BACK, (STRPTR) "Back"},
    {MSG_BUTTON_BACK_SC, (STRPTR) "b"},
    {MSG_BUTTON_FORWARD, (STRPTR) "Forward"},
    {MSG_BUTTON_FORWARD_SC, (STRPTR) "f"},
    {MSG_BUTTON_RELOAD, (STRPTR) "Reload"},
    {MSG_BUTTON_RELOAD_SC, (STRPTR) "r"},
    {MSG_BUTTON_HOME, (STRPTR) "Home"},
    {MSG_BUTTON_HOME_SC, (STRPTR) "h"},
    {MSG_BUTTON_OPEN, (STRPTR) "Open..."},
    {MSG_REQ_URL_TITLE, (STRPTR) "Open Document"},
    {MSG_REQ_URL_URL, (STRPTR) "URL To Open:"},
    {MSG_REQ_URL_OPEN, (STRPTR) "_Open"},
    {MSG_REQ_URL_CLEAR, (STRPTR) "C_lear"},
    {MSG_REQ_URL_CANCEL, (STRPTR) "_Cancel"},
    {MSG_WIN_SOURCE_TITLE, (STRPTR) "Document_Source"},
    {MSG_REQ_FINDRESULT_TITLE, (STRPTR) "Amiga Mosaic: Find Result"},
    {MSG_REQ_FINDRESULT_NO_MATCHES, (STRPTR) "Sorry, no matches in this document."},
    {MSG_REQ_FINDRESULT_NO_MORE_MATCHES, (STRPTR) "Sorry, no more matches in this document."},
    {MSG_REQ_FINDRESULT_OK, (STRPTR) "_Ok"},
    {MSG_REQ_FIND_TITLE, (STRPTR) "Find in Document"},
    {MSG_REQ_FIND_STRING, (STRPTR) "Find string in document:"},
    {MSG_REQ_FIND_CASELESS, (STRPTR) "Caseless search:"},
    {MSG_REQ_FIND_BACKWARDS, (STRPTR) "Backwards search:"},
    {MSG_REQ_FIND_FIND, (STRPTR) "_Find"},
    {MSG_REQ_FIND_CLEAR, (STRPTR) "C_lear"},
    {MSG_REQ_FIND_CANCEL, (STRPTR) "_Cancel"},
    {MSG_REQ_SAVE_FILEREQ_TITLE, (STRPTR) "Save as..."},
    {MSG_REQ_SAVE_FILEREQ_POSTEXT, (STRPTR) "Save"},
    {MSG_REQ_SAVE_TITLE, (STRPTR) "Save Document"},
    {MSG_REQ_SAVE_FILEFORMAT, (STRPTR) "File format:"},
    {MSG_REQ_SAVE_FILEFORMAT_PLAIN, (STRPTR) "Plain Text"},
    {MSG_REQ_SAVE_FILEFORMAT_FORMATTED, (STRPTR) "Formatted Text"},
    {MSG_REQ_SAVE_FILEFORMAT_HTML, (STRPTR) "HTML"},
    {MSG_REQ_SAVE_FILE, (STRPTR) "File to save:"},
    {MSG_REQ_SAVE_SAVE, (STRPTR) "_Save"},
    {MSG_REQ_SAVE_CLEAR, (STRPTR) "C_lear"},
    {MSG_REQ_SAVE_CANCEL, (STRPTR) "_Cancel"},
    {MSG_REQ_SAVEBINARY_TITLE, (STRPTR) "Save as..."},
    {MSG_REQ_SAVEBINARY_POSTEXT, (STRPTR) "Save"},
    {MSG_REQ_ABOUT_TITLE, (STRPTR) "About Amiga Mosaic"},
    {MSG_REQ_ABOUT_OK, (STRPTR) "_Ok"},
    {MSG_REQ_OPEN_LOCAL_TITLE, (STRPTR) "Select File"},
    {MSG_REQ_MACRO_TITLE, (STRPTR) "Rexx Macro"},
    {MSG_REQ_MACRO_MACRO, (STRPTR) "Macro:"},
    {MSG_REQ_MACRO_RUN, (STRPTR) "_Run"},
    {MSG_REQ_MACRO_CLEAR, (STRPTR) "C_lear"},
    {MSG_REQ_MACRO_CANCEL, (STRPTR) "_Cancel"},
    {MSG_REQ_MACRO_FILEREQ_TITLE, (STRPTR) "Select Script"},
    {MSG_REQ_FORM_READY, (STRPTR) "The images in the current form has been loaded"},
    {MSG_REQ_SHOW_THEM, (STRPTR) "Show them"},
    {MSG_REQ_CONSTRUCTING, (STRPTR) "Constructing form, please wait..."}
};


static struct Catalog *amosaic_Catalog = NULL;

void OpenamosaicCatalog(struct Locale *loc, STRPTR language)
{ LONG tag, tagarg;
  extern struct LocaleBase *LocaleBase;

  if (language == NULL)
  { tag = TAG_IGNORE;
    tagarg = 0;
  }
  else
  { tag = OC_Language;
    tagarg = (IPTR) language;
  }
  if (LocaleBase != NULL  &&  amosaic_Catalog == NULL)
  { amosaic_Catalog = OpenCatalog(loc, (STRPTR) "amosaic.catalog",
			     OC_BuiltInLanguage, amosaic_BuiltInLanguage,
			     tag, tagarg,
			     OC_Version, amosaic_Version,
			     TAG_DONE);
  }
}

void CloseamosaicCatalog(void)
{ if (LocaleBase != NULL)
  { CloseCatalog(amosaic_Catalog);
  }
  amosaic_Catalog = NULL;
}

CONST_STRPTR GetamosaicString(LONG strnum)
{ STRPTR defaultstr = NULL;
  LONG i;

  for (i = 0;  i < sizeof(amosaic_Array)/sizeof(struct FC_Type);  i++)
    { if (amosaic_Array[i].ID == strnum)
      { defaultstr = amosaic_Array[i].Str;
	break;
      }
    }
  if (amosaic_Catalog == NULL)
  { return(defaultstr);
  }
  return(GetCatalogStr(amosaic_Catalog, strnum, defaultstr));
}
