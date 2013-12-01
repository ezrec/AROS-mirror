// scalos/PictureDimensions.h
// $Date$
// $Revision$

#ifndef __PICTUREDIMENSIONS_BASE_H
#define __PICTUREDIMENSIONS_BASE_H

#include <libraries/locale.h>

#include "plugin.h"

struct PictureDimensionsBase
{
	struct PluginBase pdb_PluginBase;

	struct	Locale *pdb_Locale;
	struct	Catalog *pdb_Catalog;
};

#endif // __PICTUREDIMENSIONS_BASE_H

