/*
 * targa.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef TARGA_CLASS_H
#define TARGA_CLASS_H

#include <exec/exec.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <utility/utility.h>
#include <datatypes/pictureclass.h>
#include <graphics/gfx.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/graphics.h>

#include "targa_format.h"

#define OK (0)
#define NOTOK DTERROR_INVALID_DATA
#define ERROR_EOF DTERROR_NOT_ENOUGH_DATA

#define ReadError(C) ((C == -1) ? IoErr() : ERROR_EOF)
#define WriteError(C) IoErr()

#endif /* TARGA_CLASS_H */
