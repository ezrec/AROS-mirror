// ExifPicture_base.h
// $Date$
// $Revision$

#ifndef __EXIFPICTURE_BASE
#define __EXIFPICTURE_BASE

#include <libraries/locale.h>

#include "plugin.h"

struct ExifPictureBase
{
	struct PluginBase pdb_PluginBase;

	struct	Locale *pdb_Locale;
	struct	Catalog *pdb_Catalog;
};

#endif //__EXIFPICTURE_BASE

