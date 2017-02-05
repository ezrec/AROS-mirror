#include "cdxlplay.h"

static GLuint textures[1] = {0};

static void glEnable2D () {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(viewport[0], viewport[0]+viewport[2],
		viewport[1]+viewport[3], viewport[1], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_LIGHTING_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
}

static void glDisable2D () {
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void GLInit (int w, int h) {
	glViewport(0, 0, w, h);
	glDisable(GL_LIGHTING);
	glDisable(GL_DITHER);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable2D();
	glGenTextures(1, textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glClear(GL_COLOR_BUFFER_BIT);
}

void GLExit () {
	glDeleteTextures(1, textures);
	glDisable2D();
}

void GLScaleSurface(SDL_Surface *source, SDL_Surface *dest, uint32_t pix_ratio) {
	uint32_t src_w = source->w;
	uint32_t src_h = source->h;
	uint32_t dst_x = 0;
	uint32_t dst_y = 0;
	uint32_t dst_w = dest->w;
	uint32_t dst_h = dest->h;
	uint32_t x_ratio, y_ratio;
	uint32_t x_factor, y_factor;
	
	if (pix_ratio < 0x10000UL) {
		x_factor = 1;
		y_factor = 0x10000UL / pix_ratio;
	} else {
		x_factor = pix_ratio / 0x10000UL;
		y_factor = 1;
	}
	
	x_ratio = (dst_w << 16) / (src_w * x_factor);
	y_ratio = (dst_h << 16) / (src_h * y_factor);
	if (x_ratio < y_ratio) {
		dst_h = (src_h * x_ratio * y_factor) >> 16;
		dst_y = (dest->h - dst_h) >> 1;
	} else {
		dst_w = (src_w * y_ratio * x_factor) >> 16;
		dst_x = (dest->w - dst_w) >> 1;
	}
	
	SDL_LockSurface(source);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, src_w, src_h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, source->pixels);
	SDL_UnlockSurface(source);
	
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);
	glVertex2i(dst_x, dst_y);
	glTexCoord2i(1, 0);
	glVertex2i(dst_x + dst_w, dst_y);
	glTexCoord2i(1, 1);
	glVertex2i(dst_x + dst_w, dst_y + dst_h);
	glTexCoord2i(0, 1);
	glVertex2i(dst_x, dst_y + dst_h);
	glEnd();
}
