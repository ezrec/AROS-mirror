/****************************************************************************
*                                                                           *
* export_import.h -- Lunapaint,                                             *
*    http://developer.berlios.de/projects/lunapaintami/                     *
* Copyright (C) 2006, 2007, Hogne Titlestad <hogga@sub-ether.org>           *
* Copyright (C) 2009-2011 LunaPaint Development Team                        *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program; if not, write to the Free Software Foundation,   *
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.            *
*                                                                           *
****************************************************************************/

#ifndef _export_import_h_
#define _export_import_h_

#include <stdio.h>
#include <stdlib.h>

#ifndef __AROS__
#include "aros/aros.h"
#endif

#include <exec/types.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <cybergraphx/cybergraphics.h>

#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>

#include <dos/filehandler.h>

#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/muimaster.h>
#include <proto/cybergraphics.h>
#include <proto/datatypes.h>
#include <clib/alib_protos.h>
#include <png.h>
#include <string.h>

#include "parts.h"
#include "canvas.h"
#include "core/canvas.h"
#include "config.h"

#define DEBUG 1

extern Object *exportCycDT;
extern Object *exportBtnCancel;
extern Object *exportBtnExport;
extern Object *exportPopFilename;
extern Object *exportMode;

extern Object *exportAnimMethod;
extern Object *exportAnimMode;
extern Object *exportAnimRangeStart;
extern Object *exportAnimRangeEnd;
extern Object *exportAnimBtnExport;
extern Object *exportAnimBtnCancel;

extern Object *importCycDT;
extern Object *importBtnCancel;
extern Object *importBtnImport;
extern Object *importPopFilename;

extern Object *exportWindow;
extern Object *importWindow;

extern struct Hook export_hook;
extern struct Hook import_hook;
extern struct Hook exportanimation_hook;

/*
    Generate an exportable buffer
*/
unsigned int *generateExportableBuffer ( struct oCanvas *canvas, int mode, int datatype );

/*
    Generate the export window
*/
void makeExportWindow ( );
/*
    Generate the import window
*/
void makeImportWindow ( );

/*
    Create an image from an animation by the choices done in the
    gui
*/
void createImageFromAnimation ( struct oCanvas *canv, int datatype, char *filename );

/*
    Export a jpeg image (for now only jpeg)
*/
void exportDT ( int w, int h, unsigned int *buffer, unsigned char *filename, const char *format );

/*
    Export png
*/
void exportPNG ( int w, int h, unsigned int *buffer, unsigned char *filename );

/*
    Save a lunapaint project
*/
void saveProject ( );


#endif
