/***************************************************************************
                          levels.c  -  description
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

#include "lbreakout.h"
#include "config.h"
#include "file.h"
#include "levels.h"

extern Config config;
extern int bkgnd_count;

List *levels = 0; /* dynamic list containing the levels of current levelset */
int levelset_version, levelset_update; /* current version: x.x */

/* names of all valid levelsets */
List *levelset_names = 0; 

/* static lists with pointers to levelset_names */
char create_name[] = "<CREATE SET>"; /* the first entry in levelset_names_home 
                                        is to create a new set */
char tournament_name[] = "!FREAKOUT!"; /* special game type were all levels 
                                           are played in random order */
char **levelset_names_home = 0; /* home directory sets */
int levelset_count_home = 0;
char **levelset_names_local = 0; /* non-network levelsets */
int levelset_count_local = 0;

/*
====================================================================
Locals
====================================================================
*/

/*
====================================================================
Read in next line.
====================================================================
*/
static int next_line( FILE *file, char *buffer )
{
    /* lines with an # are comments: ignore them */
    if ( !fgets( buffer, 1023, file ) ) return 0;
    if ( buffer[strlen(buffer) - 1] == 10 )
        buffer[strlen(buffer) - 1] = 0;
    return 1;
}

/*
====================================================================
Publics
====================================================================
*/

/*
====================================================================
Open a levelset file by name.
====================================================================
*/
FILE *levelset_open( char *fname, char *mode )
{
    FILE *file;
    char path[512];
    if ( fname[0] == '~' ) {
        snprintf( path, sizeof(path)-1, "%s/%s/lbreakout2-levels/%s", 
                 (getenv( "HOME" )?getenv( "HOME" ):"."), 
                 CONFIG_DIR_NAME, fname + 1 );
    }
    else
        snprintf( path, sizeof(path)-1, "%s/levels/%s", SRC_DIR, fname );
    if ( ( file = fopen( path, mode ) ) == 0 ) {
        fprintf( stderr, "couldn't open %s\n", path );
        return 0;
    }
    return file;
}

/*
====================================================================
Free current levels if any and load all levels from either 
home directory (fname begins with ~) or installation directory.
Return Value: True if successful.
====================================================================
*/
int levels_load_set( char *fname, List *levels )
{
    /* load all levels from levelset 'fname' and put
       them to list 'levels' */
    FILE *file;
    Level *level;
    /* get file handle */
    if ( ( file = levelset_open( fname, "rb" ) ) == 0 )
        return 0;
    /* check version */
    levelset_get_version( file, &levelset_version, &levelset_update );
    /* read levels */
    while( ( level = level_load( file ) ) != 0 ) 
        list_add( levels, level );
    fclose( file );
    return 1;
}
int levels_load( char *fname )
{
    int i;
    ListEntry *entry;
    Level **pointers, *level;
    
    /* delete old levels */
    if ( levels ) levels_delete();
    /* init list */
    levels = list_create( LIST_AUTO_DELETE, level_delete );
    list_reset( levels );
    
    /* load either single set or all sets */
    if ( STRCMP( fname, tournament_name ) ) {
        for ( i = 1; i < levelset_count_local; i++ )
            levels_load_set( levelset_names_local[i], levels );
        levelset_version = 1; levelset_update = 0;
        /* shake the levels a bit */
        list_reset( levels ); i = 0;
        pointers = calloc( levels->count, sizeof( Level* ) );
        while ( ( level = list_next( levels ) ) ) {
            i = rand() % levels->count;
            while ( pointers[i] ) {
                i++;
                if ( i == levels->count )
                    i = 0;
            }
            pointers[i] = level;
        }
        entry = levels->head->next;
        for ( i = 0; i < levels->count; i++ ) {
            entry->item = pointers[i];
            entry = entry->next;
        }
        free( pointers );
    }
    else
        levels_load_set( fname, levels );
    
    /* if no levels loaded at all delete levels */
    if ( list_empty( levels ) ) {
        levels_delete();
        fprintf( stderr, "%s: no valid levels\n", fname );
        return 0;
    }
    printf( "%s v%i.%02i: %i levels\n", fname, levelset_version, levelset_update, levels->count );
    return 1;
}
/*
====================================================================
Create an all empty levelset.
====================================================================
*/
void levels_create_empty_set( int count )
{
    /* delete old levels */
    if ( levels ) levels_delete();
    /* init list */
    levels = list_create( LIST_AUTO_DELETE, level_delete );
    list_reset( levels );
    while ( count-- > 0 )
        list_add( levels, level_create_empty( "nobody", "none" ) );
}
/*
====================================================================
Delete levels
====================================================================
*/
void levels_delete()
{
    if ( !levels ) return;
    list_delete( levels );
    levels = 0;
}
/*
====================================================================
Get a specific level from list.
Return Value: Level pointer containing information of this level or
NULL if there is no level
====================================================================
*/
Level* levels_get_first()
{
    return list_first( levels );
}
Level* levels_get_next( Level *current_level )
{
    return list_get( levels, list_check( levels, current_level ) + 1 );
}
/*
====================================================================
Return id of this level in list.
====================================================================
*/
int levels_get_id( Level *level )
{
    return list_check( levels, level );
}
/*
====================================================================
Load level from current file position.
====================================================================
*/
Level* level_load( FILE *file )
{
    Level *level = 0;
    char buffer[1024];
    int i, j;

    /* file handle ok? */
    if ( !file ) 
        return 0;
    /* get level mem */
    level = calloc( 1, sizeof( Level ) );
    /* read entries */
    /* level: */
    if ( !next_line( file, buffer ) ) goto failure;
    if ( !strequal( "Level:", buffer ) ) goto failure;
    /* author */
    if ( !next_line( file, buffer ) ) goto failure;
    level->author = strdup( buffer );
    /* level name */
    if ( !next_line( file, buffer ) ) goto failure;
    level->name = strdup( buffer );
    /* bricks: */
    if ( !next_line( file, buffer ) ) goto failure;
    if ( !strequal( "Bricks:", buffer ) ) goto failure;
    /* load bricks */
    for ( i = 0; i < EDIT_HEIGHT; i++ ) {
        if ( !next_line( file, buffer ) ) goto failure;
        if ( strlen( buffer ) < EDIT_WIDTH ) goto failure;
        for ( j = 0; j < EDIT_WIDTH; j++ )
            level->bricks[j][i] = buffer[j];
    }
    /* extras: */
    if ( !next_line( file, buffer ) ) goto failure;
    if ( !strequal( "Bonus:", buffer ) ) goto failure;
    /* load extras */
    for ( i = 0; i < EDIT_HEIGHT; i++ ) {
        if ( !next_line( file, buffer ) ) goto failure;
        if ( strlen( buffer ) < EDIT_WIDTH ) goto failure;
        for ( j = 0; j < EDIT_WIDTH; j++ )
            level->extras[j][i] = buffer[j];
    }
    /* background */
    level->bkgnd_id = rand() % bkgnd_count;
    /* return level */
    return level;
failure:
    level_delete( level );
    return 0;
}
/*
====================================================================
Save levelset to home directory (regardsless of ~ in front of it).
Return Value: True if successful.
====================================================================
*/
int levels_save( char *fname )
{
    FILE *file;
    Level *level;
    char path[512];
    int i, j;
    if ( levels == 0 ) return 0;
    snprintf( path, sizeof(path)-1, "%s/%s/lbreakout2-levels/%s", 
             (getenv( "HOME" )?getenv( "HOME" ):"."), 
             CONFIG_DIR_NAME, (fname[0]=='~')?fname+1:fname );
    if ( ( file = fopen( path, "w" ) ) == 0 ) {
        fprintf( stderr, "couldn't open %s\n", path );
        return 0;
    }
    fprintf( file, "Version: %i.%02i\n", 
             levelset_version, levelset_update );
    list_reset( levels );
    while ( ( level = list_next( levels ) ) ) {
        fprintf( file, "Level:\n%s\n%s\nBricks:\n",
                 level->author, level->name );
        for ( j = 0; j < EDIT_HEIGHT; j++ ) {
            for ( i = 0; i < EDIT_WIDTH; i++ )
                fprintf( file, "%c", level->bricks[i][j] );
            fprintf( file, "\n" );
        }
        fprintf( file, "Bonus:\n" );
        for ( j = 0; j < EDIT_HEIGHT; j++ ) {
            for ( i = 0; i < EDIT_WIDTH; i++ )
                fprintf( file, "%c", level->extras[i][j] );
            fprintf( file, "\n" );
        }
    }
    fclose( file );
    return 1;
}
/*
====================================================================
Create an empty level
====================================================================
*/
Level* level_create_empty( char *author, char *name )
{
    int i, j;
    Level *level = calloc( 1, sizeof( Level ) );
    level->author = calloc( 32, sizeof( char ) ); 
    strcpy_lt( level->author, author, 31 );
    level->name = calloc( 32, sizeof( char ) );
    strcpy_lt( level->name, name, 31 );
    /* empty arena */
    for ( i = 0; i < EDIT_WIDTH; i++ )
        for ( j = 0; j < EDIT_HEIGHT; j++ ) {
            level->extras[i][j] = (char)EX_NONE;
            level->bricks[i][j] = (char)-1;
        }
    return level;
}
/*
====================================================================
Delete level pointer.
====================================================================
*/
void level_delete( void *level_ptr )
{
    Level *level = (Level*)level_ptr;
    if ( !level ) return;
    if ( level->author ) free( level->author );
    if ( level->name ) free( level->name );
    free( level );
}

/*
====================================================================
Get/delete list of all valid levelsets in data directory or
~/.lbreakout/levels.
====================================================================
*/
void levelsets_load_names()
{
    Text *text = 0;
    char level_dir[512];
    char file_name[64];
    char *name;
    int default_set_count = 4;
    char *default_sets[] = {
        "Original",
        "AddOn-1",
        "AddOn-2",
        "!FREAKOUT!"
    };
    int i, j;
    /* clear previous lists */
    levelsets_delete_names();
    /* gather all names in install&home directory to levelset_names */
    levelset_names = list_create( LIST_AUTO_DELETE, LIST_NO_CALLBACK );
    /* parse install directory */
    sprintf( level_dir, "%s/levels", SRC_DIR );
    if ( ( text = get_file_list( level_dir, 0, level_dir ) ) ) {
        for ( i = 0; i < text->count; i++ ) {
            /* filter stuff */
            if ( text->lines[i][0] == '*' ) continue;
            if ( strequal( text->lines[i], "Makefile.am" ) ) continue;
            if ( strequal( text->lines[i], "Makefile.in" ) ) continue;
            if ( strequal( text->lines[i], "Makefile" ) ) continue;
            /* default sets are at the beginning of the list thus
               ignored here */
            for ( j = 0; j < default_set_count; j++ )
                if ( STRCMP( default_sets[j], text->lines[i] ) )
                    continue;
            /* add */
            list_add( levelset_names, strdup( text->lines[i] ) );
        }
        delete_text( text );
    }
    /* parse home directory */
    snprintf( level_dir, sizeof(level_dir)-1, "%s/%s/lbreakout2-levels", (getenv( "HOME" )?getenv( "HOME" ):"."), CONFIG_DIR_NAME );
    if ( ( text = get_file_list( level_dir, 0, level_dir ) ) ) {
        for ( i = 0; i < text->count; i++ ) {
            /* filter stuff */
            if ( text->lines[i][0] == '*' ) continue;
            /* add */
            sprintf( file_name, "~%s", text->lines[i] );
            list_add( levelset_names, strdup( file_name ) );
        }
        delete_text( text );
    }
    /* create static home list */
    list_reset( levelset_names );
    levelset_count_home = 1;
    while ( ( name = list_next( levelset_names ) ) )
        if ( name[0] == '~' )
            levelset_count_home++;
    levelset_names_home = calloc( levelset_count_home, sizeof( char* ) );
    list_reset( levelset_names ); i = 1;
    while ( ( name = list_next( levelset_names ) ) )
        if ( name[0] == '~' )
            levelset_names_home[i++] = name + 1;
    levelset_names_home[0] = create_name;
    /* create static local set list */
    list_reset( levelset_names );
    levelset_count_local = 1;
    while ( ( name = list_next( levelset_names ) ) )
        if ( !levelset_is_network( name ) )
            levelset_count_local++;
    levelset_names_local = calloc( levelset_count_local, sizeof( char* ) );
    list_reset( levelset_names ); i = 1;
    while ( ( name = list_next( levelset_names ) ) )
        if ( !levelset_is_network( name ) )
            levelset_names_local[i++] = name;
    /* default names */
    for ( i = 0; i < default_set_count; i++ )
        levelset_names_local[i] = default_sets[i];
    /* adjust config */
    if ( config.levelset_count_local != levelset_count_local ) {
        config.levelset_id_local = 0;
        config.levelset_count_local = levelset_count_local;
    }
    if ( config.levelset_count_home != levelset_count_home ) {
        config.levelset_id_home = 0;
        config.levelset_count_home = levelset_count_home;
    }
}
void levelsets_delete_names()
{
    if ( levelset_names ) {
        list_delete( levelset_names );
        levelset_names = 0;
    }
    if ( levelset_names_home ) {
        free( levelset_names_home );
        levelset_names_home = 0;
    }
    if ( levelset_names_local ) {
        free( levelset_names_local );
        levelset_names_local = 0;
    }
}

/*
====================================================================
Get version and current update of levelset: x.x
Will reset the file pointer to beginning.
====================================================================
*/
void levelset_get_version( FILE *file, int *version, int *update )
{
    char buffer[1024];
    *version = 1; *update = 0;
    fseek( file, 0, SEEK_SET );
    next_line( file, buffer );
    if ( strlen( buffer ) > 8 && !strncmp( buffer, "Version:", 8 ) )
        parse_version( buffer + 8, version, update );
    else
        fseek( file, 0, SEEK_SET );
}

/*
====================================================================
Get the name of the author of the first level.
====================================================================
*/
void levelset_get_first_author( FILE *file, char *author )
{
    char buffer[1024];
    int dummy;
    levelset_get_version( file, &dummy, &dummy );
    next_line( file, buffer );
    next_line( file, buffer );
    strcpy_lt( author, buffer, 31 );
}
/*
====================================================================
Check if this levelset's name declare it as network set.
====================================================================
*/
int levelset_is_network( char *name )
{
    if ( name == 0 ) return 0;
    if ( strlen( name ) >= 4 )
        if ( !strncmp( name, "MP_", 3 ) || !strncmp( name, "~MP_", 4 ) )
            return 1;
    return 0;
}

