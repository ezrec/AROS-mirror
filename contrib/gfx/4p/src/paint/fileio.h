/*
 * fileio
 *
 *  the basic routines to convert 'pages' out to 'images' for the jpnm library
 *  in the future any other file io routines will be in here as well.
 *  ie:
 *	loading / saving palettes
 *	loading / saving animations
 *	calling registered callbacks if another filetype is desired
 *	perhaps we also will use the Image Magick library here...
 */

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@absynth.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __FILEIO_H__
#define __FILEIO_H__

#include "jsui.h"

#define BACKUP_PIC_NAME "backup.pic"

IPAGE * file_load_page(char * filename);

void file_backup(char * filename);

void    file_save_page(IPAGE * page, char * filename);


#endif
