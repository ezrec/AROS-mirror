/*
 * Based on the code from the ttf.library by Richard Griffith.
 */
#include "ttfglyphengine.h"
#include "ttfdebug.h"

#include <diskfont/oterrors.h>

#include <string.h>

int OpenFace(TTF_GlyphEngine *ge, char *new_ttf_filename)
{
    TT_Error error;

    ge->instance_changed=TRUE;	/* instance needs to be changed */

    if(ge->face_established)
    {
	/* we have an existing face, if same file keep it */
	if(stricmp(ge->ttf_filename,new_ttf_filename)==0)
	    return set_last_error(ge,OTERR_Success);

	/* it is different, free the old one first */
	TT_Close_Face( ge->face );
	ge->KernPairs = -1;
    }
	
    ge->face_established=FALSE;
    strcpy(ge->ttf_filename,new_ttf_filename);
    D(bug("OpenFace %ls.\n",ge->ttf_filename));

    error = TT_Open_Face( ge->engine, ge->ttf_filename, &ge->face );
    if (error)
    {
	D(bug( "Error while opening %s, error code = %lx.\n",
	       ge->ttf_filename, (LONG)error ));
	return(set_last_error(ge,OTERR_BadFace));
    }

    ge->face_established=TRUE;
    TT_Get_Face_Properties( ge->face, &ge->properties );

    error = TT_New_Glyph( ge->face, &ge->glyph );
    if ( error )
    {
	D(bug( "Could not create glyph container.\n" ));
	return(set_last_error(ge,OTERR_Failure));
    }

    /* create instance */
    error = TT_New_Instance( ge->face, &ge->instance );
    if (error)
    {
	D(bug("Could not create instance.\n"));
	TT_Close_Face( ge->face );
	ge->face_established=FALSE;
	ge->KernPairs = -1;
	return set_last_error(ge,OTERR_Failure);
    }

    return set_last_error(ge,OTERR_Success);
}
