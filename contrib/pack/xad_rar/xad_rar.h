/* Header file for XAD RAR client
 * Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#ifndef XADMASTER_RAR_H
#define XADMASTER_RAR_H

#include "RAR_rev.h"
#include "Rar_Extractor-0.2.1/unrarlib/unrarlib.h"

struct xadrarprivate {
	ArchiveList_struct *List;
};

#endif
