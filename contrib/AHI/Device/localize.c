/* $Id$
* $Log$
* Revision 1.4  2003/12/30 20:25:35  stegerg
* Updated to latest AHI sources.
*
* Revision 5.3  2003/04/22 18:19:05  martin
* Now creates localize.[ch] in srcdir.
*
* Revision 5.0  2000/11/28 00:15:17  lcs
* Bumped CVS revision to 5.0.
*
* Revision 4.3  1999/09/22 20:11:06  lcs
* Removed some "uninitialized variable" warnings.
*
* Revision 4.2  1999/01/15 22:40:25  lcs
* Fixed a couple of warnings.
*
* Revision 4.1  1997/04/02 22:29:53  lcs
* Bumped to version 4
*
* Revision 1.1  1997/02/03 16:22:45  lcs
* Initial revision
*
*/
/****************************************************************
   This file was created automatically by `FlexCat 2.4'
   from "../../ahisrc/Device/ahi.cd".

   Do NOT edit by hand!
****************************************************************/

#include "ahi_def.h"

#include <libraries/locale.h>
#include <proto/locale.h>


static LONG ahi_Version = 4;
static const STRPTR ahi_BuiltInLanguage = (STRPTR) "english";

struct FC_Type
{   LONG   ID;
    STRPTR Str;
};


const struct FC_Type _msgDefault = { 0, "Default" };
const struct FC_Type _msgMenuControl = { 1, "\000\000Control" };
const struct FC_Type _msgMenuLastMode = { 2, "L\000Last Mode" };
const struct FC_Type _msgMenuNextMode = { 3, "N\000Next Mode" };
const struct FC_Type _msgMenuPropertyList = { 4, "?\000Property List..." };
const struct FC_Type _msgMenuRestore = { 5, "R\000Restore" };
const struct FC_Type _msgMenuOK = { 6, "O\000OK" };
const struct FC_Type _msgMenuCancel = { 7, "C\000Cancel" };
const struct FC_Type _msgUnknown = { 8, "UNKNOWN:Audio ID 0x%08lx" };
const struct FC_Type _msgReqOK = { 9, "OK" };
const struct FC_Type _msgReqCancel = { 10, "Cancel" };
const struct FC_Type _msgReqFrequency = { 11, "Frequency" };
const struct FC_Type _msgDefaultMode = { 12, "Default audio mode" };
const struct FC_Type _msgReqInfoTitle = { 13, "Mode Properties" };
const struct FC_Type _msgReqInfoAudioID = { 14, "Audio mode ID: 0x%08lx" };
const struct FC_Type _msgReqInfoResolution = { 15, "Resolution: %ld bit %s" };
const struct FC_Type _msgReqInfoMono = { 16, "mono" };
const struct FC_Type _msgReqInfoStereo = { 17, "stereo" };
const struct FC_Type _msgReqInfoStereoPan = { 18, "stereo with panning" };
const struct FC_Type _msgReqInfoChannels = { 19, "Channels: %ld" };
const struct FC_Type _msgReqInfoMixrate = { 20, "Mixing rates: %ld-%ld Hz" };
const struct FC_Type _msgReqInfoHiFi = { 21, "HiFi mixing" };
const struct FC_Type _msgReqInfoRecordHalf = { 22, "Record in half duplex" };
const struct FC_Type _msgReqInfoRecordFull = { 23, "Record in full duplex" };


static struct Catalog *ahi_Catalog = NULL;

void OpenahiCatalog(struct Locale *loc, STRPTR language)
{
  LONG tag = 0, tagarg = 0;

  if(LocaleBase != NULL  &&  ahi_Catalog == NULL)
  {
    if(language == NULL)
    {
      tag = TAG_IGNORE;
    }
    else
    {
      tag = OC_Language;
      tagarg = (LONG) language;
    }

    ahi_Catalog = OpenCatalog(loc, (STRPTR) "ahi.catalog",
        OC_BuiltInLanguage, (ULONG) ahi_BuiltInLanguage,
        tag, tagarg,
        OC_Version, ahi_Version,
        TAG_DONE);
  }
}

struct Catalog *ExtOpenCatalog(struct Locale *loc, STRPTR language)
{
  LONG tag = 0, tagarg = 0;

  if(LocaleBase != NULL)
  {
    if(language == NULL)
    {
      tag = TAG_IGNORE;
    }
    else
    {
      tag = OC_Language;
      tagarg = (LONG) language;
    }

    return OpenCatalog(loc, (STRPTR) "ahi.catalog",
        OC_BuiltInLanguage, (ULONG) ahi_BuiltInLanguage,
        tag, tagarg,
        OC_Version, ahi_Version,
        TAG_DONE);
  }
  return NULL;
}

void CloseahiCatalog(void)
{
  if (LocaleBase != NULL)
  {
    CloseCatalog(ahi_Catalog);
  }
  ahi_Catalog = NULL;
}

void ExtCloseCatalog(struct Catalog *catalog)
{
  if (LocaleBase != NULL)
  {
    CloseCatalog(catalog);
  }
}

STRPTR GetahiString(APTR fcstr)
{
  STRPTR defaultstr;
  LONG strnum;

  strnum = ((struct FC_Type *) fcstr)->ID;
  defaultstr = ((struct FC_Type *) fcstr)->Str;

  return(ahi_Catalog ? GetCatalogStr(ahi_Catalog, strnum, defaultstr) :
		      defaultstr);
}

STRPTR GetString(APTR fcstr, struct Catalog *catalog)
{
  STRPTR defaultstr;
  LONG strnum;

  strnum = ((struct FC_Type *) fcstr)->ID;
  defaultstr = ((struct FC_Type *) fcstr)->Str;

  return(catalog ? GetCatalogStr(catalog, strnum, defaultstr) :
		      defaultstr);
}
