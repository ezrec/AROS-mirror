#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

/*------------------------------------------------------------------------
  Programmer-visible tags.   I=can initialize, S=can set, G=can get
------------------------------------------------------------------------*/

#define TAG(x)			(TAG_USER + x)
#define SPIN_state		TAG(1)		// -S-
#define SPIN_window             TAG(2)          // IS-
