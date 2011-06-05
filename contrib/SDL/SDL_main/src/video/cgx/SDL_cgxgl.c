/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/* AROSMesa implementation of SDL OpenGL support */

#include "SDL_cgxgl_c.h"
#include "SDL_cgxvideo.h"

#if SDL_VIDEO_OPENGL
struct Library *MesaBase = NULL;
#endif

/* Create OpenGL context */
int CGX_GL_CreateContext(_THIS)
{
#if SDL_VIDEO_OPENGL
	LONG i = 0;
	struct TagItem attributes [10];
	struct Window *win = (struct Window *)SDL_Window;

	if ( this->gl_config.red_size   == 0 &&	this->gl_config.blue_size  == 0 &&
			this->gl_config.green_size == 0 ) {
		SDL_SetError("AROSMesa does not work with indexed color");
		return(-1);
	}

	if( this->gl_config.stereo ) {
		SDL_SetError("AROSMesa does not support stereo buffer");
		return(-1);		
	}
	
	if ( ! this->gl_config.driver_loaded ) {
		if ( CGX_GL_LoadLibrary(this, NULL) < 0 ) {
			return(-1);
		}
	}

	/* Required window pointer  */
	attributes[i].ti_Tag = AMA_Window;	attributes[i++].ti_Data = (IPTR)win;

	/* this->gl_config.double_buffer - skipped, AROSMesa always double-buffer */
	/* this->gl_config.multisamplebuffers - currently not supported by Mesa */
	/* this->gl_config.multisamplesample - currently not supported by Mesa */

	/* no depth buffer ? */
	if ( this->gl_config.depth_size == 0 ) 
	{
		attributes[i].ti_Tag = AMA_NoDepth;
		attributes[i++].ti_Data = GL_TRUE;
	}
	/* no stencil buffer ? */
	if ( this->gl_config.stencil_size == 0 )
	{
		attributes[i].ti_Tag = AMA_NoStencil;
		attributes[i++].ti_Data = GL_TRUE;
	}
	/* no accum buffer ? */
	if ( this->gl_config.accum_red_size   == 0 && 
			this->gl_config.accum_blue_size  == 0 &&
		 	this->gl_config.accum_green_size == 0 )
	{
		attributes[i].ti_Tag = AMA_NoAccum;
		attributes[i++].ti_Data = GL_TRUE;
	}

	/* done */
	attributes[i].ti_Tag = TAG_DONE;

	this->gl_data->glctx = AROSMesaCreateContext(attributes);
	if ( this->gl_data->glctx == NULL ) {
		SDL_SetError("Couldn't create OpenGL context");
		return(-1);
	}

	return(0);

#else
	SDL_SetError("OpenGL support not configured");
	return(-1);
#endif
}

/* Destroy OpenGL context */
void CGX_GL_DestroyContext(_THIS)
{
#if SDL_VIDEO_OPENGL
	if ( this->gl_data->glctx != NULL ) {
		AROSMesaDestroyContext(this->gl_data->glctx);
		this->gl_data->glctx = NULL;
	}
#endif
}

/* Quit OpenGL */
void CGX_GL_Quit(_THIS)
{
#if SDL_VIDEO_OPENGL
	CGX_GL_DestroyContext(this);
	if ( MesaBase != NULL ) {
		CloseLibrary(MesaBase);
		this->gl_config.driver_loaded = 0;
		MesaBase = NULL;
	}
#endif
}

/* Attach context to another window */
int CGX_GL_Update(_THIS)
{
#if SDL_VIDEO_OPENGL
	struct TagItem tags[2];
	struct Window *win = (struct Window*)SDL_Window;
	if( this->gl_data->glctx == NULL ) {
		return -1; //should never happen
	}
	tags[0].ti_Tag = AMA_Window;
	tags[0].ti_Data = (IPTR)win;
	tags[1].ti_Tag = TAG_DONE;

	AROSMesaSetRast(this->gl_data->glctx, tags);

	return 0;
#else
	SDL_SetError("OpenGL support not configured");
	return -1;
#endif
}

#if SDL_VIDEO_OPENGL

/* Make the current context active */
int CGX_GL_MakeCurrent(_THIS)
{
	if(this->gl_data->glctx == NULL)
		return -1;

	AROSMesaMakeCurrent(this->gl_data->glctx);
	return 0;
}

void CGX_GL_SwapBuffers(_THIS)
{
	AROSMesaSwapBuffers(this->gl_data->glctx);
}

int CGX_GL_GetAttribute(_THIS, SDL_GLattr attrib, int* value) {
	GLenum mesa_attrib;

	switch(attrib) {
		case SDL_GL_RED_SIZE:
			mesa_attrib = GL_RED_BITS;
			break;
		case SDL_GL_GREEN_SIZE:
			mesa_attrib = GL_GREEN_BITS;
			break;
		case SDL_GL_BLUE_SIZE:
			mesa_attrib = GL_BLUE_BITS;
			break;
		case SDL_GL_ALPHA_SIZE:
			mesa_attrib = GL_ALPHA_BITS;
			break;
		case SDL_GL_DOUBLEBUFFER:
			mesa_attrib = GL_DOUBLEBUFFER;
			break;
		case SDL_GL_DEPTH_SIZE:
			mesa_attrib = GL_DEPTH_BITS;
			break;
		case SDL_GL_STENCIL_SIZE:
			mesa_attrib = GL_STENCIL_BITS;
			break;
		case SDL_GL_ACCUM_RED_SIZE:
			mesa_attrib = GL_ACCUM_RED_BITS;
			break;
		case SDL_GL_ACCUM_GREEN_SIZE:
			mesa_attrib = GL_ACCUM_GREEN_BITS;
			break;
		case SDL_GL_ACCUM_BLUE_SIZE:
			mesa_attrib = GL_ACCUM_BLUE_BITS;
			break;
		case SDL_GL_ACCUM_ALPHA_SIZE:
			mesa_attrib = GL_ACCUM_ALPHA_BITS;
			break;
		case SDL_GL_STEREO:
			mesa_attrib = GL_STEREO;
			break;
		case SDL_GL_MULTISAMPLEBUFFERS:
			return 0;
		case SDL_GL_MULTISAMPLESAMPLES:
			return 0;
		default:
			return -1;
	}

	AROSMesaGetConfig(this->gl_data->glctx, mesa_attrib, value);
	return 0;
}

void *CGX_GL_GetProcAddress(_THIS, const char *proc) {
	void *func = NULL;
	func = AROSMesaGetProcAddress(proc);
	return func;
}

int CGX_GL_LoadLibrary(_THIS, const char *path) {

	if ( MesaBase == NULL ) {
		if ((MesaBase = OpenLibrary("mesa.library", 17)) != NULL) {
			this->gl_config.driver_loaded = 1;
		}
		else {
			this->gl_config.driver_loaded = 0;
			SDL_SetError("Could not open mesa.library");
			return(-1);
		}
		this->gl_data->glctx = NULL;
	}
	
	return 0;
}

#endif /* SDL_VIDEO_OPENGL */

