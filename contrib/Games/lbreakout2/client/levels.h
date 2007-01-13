/***************************************************************************
                          levels.h  -  description
                             -------------------
    begin                : Thu Sep 6 2001
    copyright            : (C) 2001 by Michael Speck
    email                : kulkanie@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
====================================================================
Loading and deleting of levels is managed here.
The according list is found in levels.c and accessed by
levels_get_first/get_next/get_current
====================================================================
*/

#include "lbreakout.h"

/*
====================================================================
Level resource structure.
====================================================================
*/
typedef struct Level {
    char *author, *name;
    /* extras and bricks are saved by their specific character and
    interpreted by bricks_create later */
    char bricks[EDIT_WIDTH][EDIT_HEIGHT];
    char extras[EDIT_WIDTH][EDIT_HEIGHT];
    int bkgnd_id; /* background for this level */
} Level;

/*
====================================================================
Open a levelset file by name.
====================================================================
*/
FILE *levelset_open( char *fname, char *mode );
/*
====================================================================
Free current levels if any and load all levels from either 
home directory (fname begins with ~) or installation directory.
Return Value: True if successful.
====================================================================
*/
int levels_load( char *fname );
/*
====================================================================
Save levelset to home directory (regardsless of ~ in front of it).
Return Value: True if successful.
====================================================================
*/
int levels_save( char *fname );
/*
====================================================================
Create an all empty levelset.
====================================================================
*/
void levels_create_empty_set( int count );
/*
====================================================================
Delete levels
====================================================================
*/
void levels_delete();
/*
====================================================================
Get a specific level from list. Either first one or the level that
follows the current level.
Return Value: Level pointer containing information of this level or
NULL if there is no level
====================================================================
*/
Level* levels_get_first();
Level* levels_get_next( Level *current_level );
/*
====================================================================
Return id of this level in list.
====================================================================
*/
int levels_get_id( Level *level );
/*
====================================================================
Load level from current file position.
====================================================================
*/
Level* level_load( FILE *file );
/*
====================================================================
Create an empty level
====================================================================
*/
Level* level_create_empty( char *author, char *name );
/*
====================================================================
Delete level pointer.
====================================================================
*/
void level_delete( void *level_ptr );

/*
====================================================================
Get/delete lists of all valid levelsets in data directory or
~/.lbreakout/levels.
====================================================================
*/
void levelsets_load_names();
void levelsets_delete_names();

/*
====================================================================
Get version and current update of levelset: x.x
Will reset the file pointer to beginning.
====================================================================
*/
void levelset_get_version( FILE *file, int *version, int *update );

/*
====================================================================
Get the name of the author of the first level.
====================================================================
*/
void levelset_get_first_author( FILE *file, char *author );

/*
====================================================================
Check if this levelset's name declare it as network set.
====================================================================
*/
int levelset_is_network( char *name );

