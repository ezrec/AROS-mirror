/*
** file requestor
*/

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@umlautllama.com
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
// for stat
#include <sys/types.h>
#include <sys/stat.h>
// for opendir
#include <sys/types.h>
#include <dirent.h>

#include <string.h>
#include <strings.h>
#include <stdlib.h> // malloc and free


#include "jsui.h"


typedef struct dirlist
{
    char name[512];
    int parent;
    int directory;
} DIRLIST;

int filereq_value = -1;

int filereq_cb(JSUI_DIALOG * d, int object, int msg)
{
    if (!d) return( JSUI_R_O_K );

    if(msg != JSUI_MSG_LRELEASE) return( JSUI_R_O_K );

    switch(object)
    {
    case(6):
	filereq_value = 0;
	return( JSUI_R_CLOSE );
    case(7):
	filereq_value = 1;
	return( JSUI_R_CLOSE );
    }
    return JSUI_R_O_K;
}


extern JSUI_DIALOG filereq_dialog[];


int
list_cb(
	JSUI_DIALOG * dlg,
	int focusitem,
	int message,
	int indexselected,
	char * selectedtext
)
{
//    printf("list_cb %d %s\n", indexselected, selectedtext);
    strcpy((char *)filereq_dialog[5].dp, selectedtext);
    
    return( JSUI_R_O_K );
}


JSUI_DIALOG filereq_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   
		(key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */

    {jsui_widget_frame, 0, 0, 400, 300, 0, 
            JSUI_FRAME_DRAGGABLE | JSUI_FRAME_TITLEBAR | JSUI_FRAME_CLOSE, 
	    JSUI_FRAME_CENTER,
            0, "text1", NULL, NULL  },

    {jsui_widget_list, 10, 10, 380, 200, 
		0, 0, 0, 0, 
		NULL, NULL, (void *)list_cb},

    {jsui_widget_text, 20, 220, 85, 20, 0, JSUI_F_NOFRAME, JSUI_T_RIGHT, 0,
                "Directory:", NULL, NULL},

    {jsui_widget_text, 20, 245, 85, 20, 0, JSUI_F_NOFRAME, JSUI_T_RIGHT, 0,
                "File:", NULL, NULL},

    {jsui_widget_edit, 110, 220, 280, 20,  0, 0,  1024, 0, NULL, NULL, NULL},
    {jsui_widget_edit, 110, 245, 280, 20,  0, 0,  1024, 0, NULL, NULL, NULL},
                
    {jsui_widget_button,  10, 275, 70, 18, 0, 0, 0, 2,
		"Cancel", NULL, (void *)filereq_cb},

    {jsui_widget_button,  320, 275, 70, 18, 0, 0, 0, 2,
		"Okay", NULL, (void *)filereq_cb},

    JSUI_DLG_END
};

void
build_dirlist( char * directory )
{
    DIR * dirp;
    struct dirent *dp;
    struct stat statbuf;
    int nentries = 0;
    int ccc;
    char filepath[2048];
    char tempbuff[512];

    char **tc;
    char    **dirlist;
/*
    DIRLIST  *detailedlist;
*/

    printf("scanning %s\n", directory);

    dirp = opendir(directory);
    while( dirp )
    {
	if((dp = readdir( dirp )) != NULL)
	{
	    if ( strcmp(dp->d_name, ".") )  nentries++;
	} else {
	    break;
	}
    }
    closedir( dirp );

    printf("%d entries\n", nentries);

    dirlist = (char **) malloc( sizeof( char *) * (nentries+2) );

    dirp = opendir(directory);
    ccc = 0;
    while( dirp )
    {
        if((dp = readdir( dirp )) != NULL)
        {
            strcpy(filepath, directory);
            if ( filepath[strlen(filepath)-1] != '/' )
                strcpy(filepath, "/");
            strcpy(filepath, dp->d_name);
            stat(filepath, &statbuf);

            if ( strcmp(dp->d_name, ".") )
            {
                sprintf(tempbuff, "%s %s%s", dp->d_name,
                        (!strcmp(dp->d_name, ".."))?"parent ":"",
                        (S_ISDIR( statbuf.st_mode ))?"(dir)":""
                );
		dirlist[ccc++] = strdup( tempbuff );
            }
        } else {
            break;
        }
    }
    closedir( dirp );
    dirlist[ccc + 1] = NULL;

    tc = (char **)filereq_dialog[1].dp;
    if( tc )
    {
	for( ccc=0 ; tc[ccc] ; ccc++)
	    free( tc[ccc] );
	free( tc );
    }

    filereq_dialog[1].dp = dirlist;

    tc = (char **)filereq_dialog[1].dp;
}


int
do_filereq(
        SDL_Surface * screen,
	char * title,
        char * button,
	char * dir_buffer,
	char * file_buffer,
	int buffersize
)
{
    filereq_dialog[0].x = 0;
    filereq_dialog[0].y = 0;
    filereq_dialog[0].d1 = JSUI_FRAME_CENTER;
    filereq_dialog[0].dp = title;

    filereq_dialog[1].d1 = -1;
    filereq_dialog[1].d2 = 0;

    filereq_dialog[4].dp = dir_buffer;
    filereq_dialog[4].d1 = buffersize;
    filereq_dialog[5].dp = file_buffer;
    filereq_dialog[5].d1 = buffersize;

    filereq_dialog[7].dp = button;
    filereq_value = -1;

//    build_dirlist( dir_buffer );
    jsui_dialog_run_modal( filereq_dialog );

    while ( jsui_is_running( filereq_dialog ) ) 
    {
	SDL_Delay(10);
	jsui_poll(screen);
    }

    if ( (dir_buffer[0] != '\0')
	&&
         (dir_buffer[strlen(dir_buffer) -1] != '/')
       )
    {
	strcat(dir_buffer, "/");
    }

    return( filereq_value );
}
