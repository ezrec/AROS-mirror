/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998, 1999, 2000, 2001  Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

/* wrapper functions for StormMesa */

#ifdef HAVE_OPENGL

#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>

/* The GL API */

/*
 * Miscellaneous
 */

 void AmiglClearIndex( GLfloat c ) {
    glClearIndex(c);
 }

 void AmiglClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
    glClearColor(red, green, blue, alpha);
 }

 void AmiglClear( GLbitfield mask ) {
    glClear(mask);
 }

 void AmiglIndexMask( GLuint mask ) {
    glIndexMask(mask);
 }

 void AmiglColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ) {
    glColorMask(red, green, blue, alpha);
 }

 void AmiglAlphaFunc( GLenum func, GLclampf ref ) {
    glAlphaFunc(func, ref);
 }

 void AmiglBlendFunc( GLenum sfactor, GLenum dfactor ) {
    glBlendFunc(sfactor, dfactor);
 }

 void AmiglLogicOp( GLenum opcode ) {
    glLogicOp(opcode);
 }

 void AmiglCullFace( GLenum mode ) {
    glCullFace(mode);
 }

 void AmiglFrontFace( GLenum mode ) {
    glFrontFace(mode);
 }

 void AmiglPointSize( GLfloat size ) {
    glPointSize(size);
 }

 void AmiglLineWidth( GLfloat width ) {
    glLineWidth(width);
 }

 void AmiglLineStipple( GLint factor, GLushort pattern ) {
    glLineStipple(factor, pattern);
 }

 void AmiglPolygonMode( GLenum face, GLenum mode ) {
    glPolygonMode(face, mode);
 }

 void AmiglPolygonOffset( GLfloat factor, GLfloat units ) {
    glPolygonOffset(factor, units);
 }

 void AmiglPolygonStipple( const GLubyte *mask ) {
    glPolygonStipple(mask);
 }

 void AmiglGetPolygonStipple( GLubyte *mask ) {
    glGetPolygonStipple(mask);
 }

 void AmiglEdgeFlag( GLboolean flag ) {
    glEdgeFlag(flag);
 }

 void AmiglEdgeFlagv( const GLboolean *flag ) {
    glEdgeFlagv(flag);
 }

 void AmiglScissor( GLint x, GLint y, GLsizei width, GLsizei height) {
    glScissor(x, y, width, height);
 }

 void AmiglClipPlane( GLenum plane, const GLdouble *equation ) {
    glClipPlane(plane, equation);
 }

 void AmiglGetClipPlane( GLenum plane, GLdouble *equation ) {
    glGetClipPlane(plane, equation);
 }

 void AmiglDrawBuffer( GLenum mode ) {
    glDrawBuffer(mode);
 }

 void AmiglReadBuffer( GLenum mode ) {
    glReadBuffer(mode);
 }

 void AmiglEnable( GLenum cap ) {
    glEnable(cap);
 }

 void AmiglDisable( GLenum cap ) {
    glDisable(cap);
 }

 GLboolean AmiglIsEnabled( GLenum cap ) {
    return glIsEnabled(cap);
 }

 void AmiglEnableClientState( GLenum cap ) {  /* 1.1 */
    glEnableClientState(cap);
 }

 void AmiglDisableClientState( GLenum cap ) {  /* 1.1 */
    glDisableClientState(cap);
 }

 void AmiglGetBooleanv( GLenum pname, GLboolean *params ) {
    glGetBooleanv(pname, params);
 }

 void AmiglGetDoublev( GLenum pname, GLdouble *params ) {
    glGetDoublev(pname, params);
 }

 void AmiglGetFloatv( GLenum pname, GLfloat *params ) {
    glGetFloatv(pname, params);
 }

 void AmiglGetIntegerv( GLenum pname, GLint *params ) {
    glGetIntegerv(pname, params);
 }

 void AmiglPushAttrib( GLbitfield mask ) {
    glPushAttrib(mask);
 }

 void AmiglPopAttrib( void ) {
    glPopAttrib();
 }

 void AmiglPushClientAttrib( GLbitfield mask ) {  /* 1.1 */
    glPushClientAttrib(mask);
 }

 void AmiglPopClientAttrib( void ) {  /* 1.1 */
    glPopClientAttrib();
 }

 GLint AmiglRenderMode( GLenum mode ) {
    return glRenderMode(mode);
 }

 GLenum AmiglGetError( void ) {
    return glGetError();
 }

 const GLubyte* AmiglGetString( GLenum name ) {
    return glGetString(name);
 }

 void AmiglFinish( void ) {
    glFinish();
 }

 void AmiglFlush( void ) {
    glFlush();
 }

 void AmiglHint( GLenum target, GLenum mode ) {
    glHint(target, mode);
 }

/*
 * Depth Buffer
 */

 void AmiglClearDepth( GLclampd depth ) {
    glClearDepth(depth);
 }

 void AmiglDepthFunc( GLenum func ) {
    glDepthFunc(func);
 }

 void AmiglDepthMask( GLboolean flag ) {
    glDepthMask(flag);
 }

 void AmiglDepthRange( GLclampd near_val, GLclampd far_val ) {
    glDepthRange(near_val, far_val);
 }

/*
 * Accumulation Buffer
 */

 void AmiglClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
    glClearAccum(red, green, blue, alpha);
 }

 void AmiglAccum( GLenum op, GLfloat value ) {
    glAccum(op, value);
 }

/*
 * Transformation
 */

 void AmiglMatrixMode( GLenum mode ) {
    glMatrixMode(mode);
 }

 void AmiglOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
    glOrtho(left, right, bottom, top, near_val, far_val);
 }

 void AmiglFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
    glFrustum(left, right, bottom, top, near_val, far_val);
 }

 void AmiglViewport( GLint x, GLint y, GLsizei width, GLsizei height ) {
    glViewport(x, y, width, height);
 }

 void AmiglPushMatrix( void ) {
    glPushMatrix();
 }

 void AmiglPopMatrix( void ) {
    glPopMatrix();
 }

 void AmiglLoadIdentity( void ) {
    glLoadIdentity();
 }

 void AmiglLoadMatrixd( const GLdouble *m ) {
    glLoadMatrixd(m);
 }

 void AmiglLoadMatrixf( const GLfloat *m ) {
    glLoadMatrixf(m);
 }

 void AmiglMultMatrixd( const GLdouble *m ) {
    glMultMatrixd(m);
 }

 void AmiglMultMatrixf( const GLfloat *m ) {
    glMultMatrixf(m);
 }

 void AmiglRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z ) {
    glRotated(angle, x, y, z);
 }

 void AmiglRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ) {
    glRotatef(angle, x, y, z);
 }

 void AmiglScaled( GLdouble x, GLdouble y, GLdouble z ) {
    glScaled(x, y, z);
 }

 void AmiglScalef( GLfloat x, GLfloat y, GLfloat z ) {
    glScalef(x, y, z);
 }

 void AmiglTranslated( GLdouble x, GLdouble y, GLdouble z ) {
    glTranslated(x, y, z);
 }

 void AmiglTranslatef( GLfloat x, GLfloat y, GLfloat z ) {
    glTranslatef(x, y, z);
 }

/*
 * Display Lists
 */

 GLboolean AmiglIsList( GLuint list ) {
    return glIsList(list);
 }

 void AmiglDeleteLists( GLuint list, GLsizei range ) {
    glDeleteLists(list, range);
 }

 GLuint AmiglGenLists( GLsizei range ) {
    return glGenLists(range);
 }

 void AmiglNewList( GLuint list, GLenum mode ) {
    glNewList(list, mode);
 }

 void AmiglEndList( void ) {
    glEndList();
 }

 void AmiglCallList( GLuint list ) {
    glCallList(list);
 }

 void AmiglCallLists( GLsizei n, GLenum type, const GLvoid *lists ) {
    glCallLists(n, type, lists);
 }

 void AmiglListBase( GLuint base ) {
    glListBase(base);
 }

/*
 * Drawing Functions
 */

 void AmiglBegin( GLenum mode ) {
    glBegin(mode);
 }

 void AmiglEnd( void ) {
    glEnd();
 }

 void AmiglVertex2d( GLdouble x, GLdouble y )   { glVertex2d(x, y); }
 void AmiglVertex2f( GLfloat x, GLfloat y )         { glVertex2f(x, y); }
 void AmiglVertex2i( GLint x, GLint y )             { glVertex2i(x, y); }
 void AmiglVertex2s( GLshort x, GLshort y )     { glVertex2s(x, y); }

 void AmiglVertex3d( GLdouble x, GLdouble y, GLdouble z )   { glVertex3d(x, y, z); }
 void AmiglVertex3f( GLfloat x, GLfloat y, GLfloat z )      { glVertex3f(x, y, z); }
 void AmiglVertex3i( GLint x, GLint y, GLint z )                { glVertex3i(x, y, z); }
 void AmiglVertex3s( GLshort x, GLshort y, GLshort z )      { glVertex3s(x, y, z); }

 void AmiglVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ){ glVertex4d(x, y, z, w); }
 void AmiglVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )       { glVertex4f(x, y, z, w); }
 void AmiglVertex4i( GLint x, GLint y, GLint z, GLint w )               { glVertex4i(x, y, z, w); }
 void AmiglVertex4s( GLshort x, GLshort y, GLshort z, GLshort w )       { glVertex4s(x, y, z, w); }

 void AmiglVertex2dv( const GLdouble *v )   { glVertex2dv(v); }
 void AmiglVertex2fv( const GLfloat *v )    { glVertex2fv(v); }
 void AmiglVertex2iv( const GLint *v )      { glVertex2iv(v); }
 void AmiglVertex2sv( const GLshort *v )    { glVertex2sv(v); }

 void AmiglVertex3dv( const GLdouble *v )   { glVertex3dv(v); }
 void AmiglVertex3fv( const GLfloat *v )    { glVertex3fv(v); }
 void AmiglVertex3iv( const GLint *v )      { glVertex3iv(v); }
 void AmiglVertex3sv( const GLshort *v )    { glVertex3sv(v); }

 void AmiglVertex4dv( const GLdouble *v )   { glVertex4dv(v); }
 void AmiglVertex4fv( const GLfloat *v )    { glVertex4fv(v); }
 void AmiglVertex4iv( const GLint *v )      { glVertex4iv(v); }
 void AmiglVertex4sv( const GLshort *v )    { glVertex4sv(v); }

 void AmiglNormal3b( GLbyte nx, GLbyte ny, GLbyte nz )      { glNormal3b(nx, ny, nz); }
 void AmiglNormal3d( GLdouble nx, GLdouble ny, GLdouble nz ){ glNormal3d(nx, ny, nz); }
 void AmiglNormal3f( GLfloat nx, GLfloat ny, GLfloat nz )   { glNormal3f(nx, ny, nz); }
 void AmiglNormal3i( GLint nx, GLint ny, GLint nz )         { glNormal3i(nx, ny, nz); }
 void AmiglNormal3s( GLshort nx, GLshort ny, GLshort nz )   { glNormal3s(nx, ny, nz); }

 void AmiglNormal3bv( const GLbyte *v ) { glNormal3bv(v); }
 void AmiglNormal3dv( const GLdouble *v )   { glNormal3dv(v); }
 void AmiglNormal3fv( const GLfloat *v )    { glNormal3fv(v); }
 void AmiglNormal3iv( const GLint *v )      { glNormal3iv(v); }
 void AmiglNormal3sv( const GLshort *v )    { glNormal3sv(v); }

 void AmiglIndexd( GLdouble c ) { glIndexd(c); }
 void AmiglIndexf( GLfloat c )  { glIndexf(c); }
 void AmiglIndexi( GLint c )        { glIndexi(c); }
 void AmiglIndexs( GLshort c )  { glIndexs(c); }
 void AmiglIndexub( GLubyte c ) { glIndexub(c); }

 void AmiglIndexdv( const GLdouble *c ) { glIndexdv(c); }
 void AmiglIndexfv( const GLfloat *c )      { glIndexfv(c); }
 void AmiglIndexiv( const GLint *c )        { glIndexiv(c); }
 void AmiglIndexsv( const GLshort *c )      { glIndexsv(c); }
 void AmiglIndexubv( const GLubyte *c ) { glIndexubv(c); }

 void AmiglColor3b( GLbyte red, GLbyte green, GLbyte blue )         { glColor3b(red, green, blue); }
 void AmiglColor3d( GLdouble red, GLdouble green, GLdouble blue )   { glColor3d(red, green, blue); }
 void AmiglColor3f( GLfloat red, GLfloat green, GLfloat blue )      { glColor3f(red, green, blue); }
 void AmiglColor3i( GLint red, GLint green, GLint blue )                { glColor3i(red, green, blue); }
 void AmiglColor3s( GLshort red, GLshort green, GLshort blue )      { glColor3s(red, green, blue); }
 void AmiglColor3ub( GLubyte red, GLubyte green, GLubyte blue ) { glColor3ub(red, green, blue); }
 void AmiglColor3ui( GLuint red, GLuint green, GLuint blue )        { glColor3ui(red, green, blue); }
 void AmiglColor3us( GLushort red, GLushort green, GLushort blue ){ glColor3us(red, green, blue); }

 void AmiglColor4b( GLbyte red, GLbyte green,
                   GLbyte blue, GLbyte alpha )      { glColor4b(red, green, blue, alpha); }
 void AmiglColor4d( GLdouble red, GLdouble green,
                   GLdouble blue, GLdouble alpha )  { glColor4d(red, green, blue, alpha); }
 void AmiglColor4f( GLfloat red, GLfloat green,
                   GLfloat blue, GLfloat alpha )    { glColor4f(red, green, blue, alpha); }
 void AmiglColor4i( GLint red, GLint green,
                   GLint blue, GLint alpha )            { glColor4i(red, green, blue, alpha); }
 void AmiglColor4s( GLshort red, GLshort green,
                   GLshort blue, GLshort alpha )    { glColor4s(red, green, blue, alpha); }
 void AmiglColor4ub( GLubyte red, GLubyte green,
                    GLubyte blue, GLubyte alpha )   { glColor4ub(red, green, blue, alpha); }
 void AmiglColor4ui( GLuint red, GLuint green,
                    GLuint blue, GLuint alpha )     { glColor4ui(red, green, blue, alpha); }
 void AmiglColor4us( GLushort red, GLushort green,
                    GLushort blue, GLushort alpha ){ glColor4us(red, green, blue, alpha); }

 void AmiglColor3bv( const GLbyte *v )      { glColor3bv(v); }
 void AmiglColor3dv( const GLdouble *v )    { glColor3dv(v); }
 void AmiglColor3fv( const GLfloat *v ) { glColor3fv(v); }
 void AmiglColor3iv( const GLint *v )       { glColor3iv(v); }
 void AmiglColor3sv( const GLshort *v ) { glColor3sv(v); }
 void AmiglColor3ubv( const GLubyte *v )    { glColor3ubv(v); }
 void AmiglColor3uiv( const GLuint *v ) { glColor3uiv(v); }
 void AmiglColor3usv( const GLushort *v )   { glColor3usv(v); }

 void AmiglColor4bv( const GLbyte *v )      { glColor4bv(v); }
 void AmiglColor4dv( const GLdouble *v )    { glColor4dv(v); }
 void AmiglColor4fv( const GLfloat *v ) { glColor4fv(v); }
 void AmiglColor4iv( const GLint *v )       { glColor4iv(v); }
 void AmiglColor4sv( const GLshort *v ) { glColor4sv(v); }
 void AmiglColor4ubv( const GLubyte *v )    { glColor4ubv(v); }
 void AmiglColor4uiv( const GLuint *v ) { glColor4uiv(v); }
 void AmiglColor4usv( const GLushort *v )   { glColor4usv(v); }

 void AmiglTexCoord1d( GLdouble s ) { glTexCoord1d(s); }
 void AmiglTexCoord1f( GLfloat s )  { glTexCoord1f(s); }
 void AmiglTexCoord1i( GLint s )        { glTexCoord1i(s); }
 void AmiglTexCoord1s( GLshort s )  { glTexCoord1s(s); }

 void AmiglTexCoord2d( GLdouble s, GLdouble t ) { glTexCoord2d(s, t); }
 void AmiglTexCoord2f( GLfloat s, GLfloat t )   { glTexCoord2f(s, t); }
 void AmiglTexCoord2i( GLint s, GLint t )           { glTexCoord2i(s, t); }
 void AmiglTexCoord2s( GLshort s, GLshort t )   { glTexCoord2s(s, t); }

 void AmiglTexCoord3d( GLdouble s, GLdouble t, GLdouble r ) { glTexCoord3d(s, t, r); }
 void AmiglTexCoord3f( GLfloat s, GLfloat t, GLfloat r )        { glTexCoord3f(s, t, r); }
 void AmiglTexCoord3i( GLint s, GLint t, GLint r )              { glTexCoord3i(s, t, r); }
 void AmiglTexCoord3s( GLshort s, GLshort t, GLshort r )        { glTexCoord3s(s, t, r); }

 void AmiglTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q ) { glTexCoord4d(s, t, r, q); }
 void AmiglTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q )     { glTexCoord4f(s, t, r, q); }
 void AmiglTexCoord4i( GLint s, GLint t, GLint r, GLint q )                 { glTexCoord4i(s, t, r, q); }
 void AmiglTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q )     { glTexCoord4s(s, t, r, q); }

 void AmiglTexCoord1dv( const GLdouble *v ) { glTexCoord1dv(v); }
 void AmiglTexCoord1fv( const GLfloat *v )  { glTexCoord1fv(v); }
 void AmiglTexCoord1iv( const GLint *v )        { glTexCoord1iv(v); }
 void AmiglTexCoord1sv( const GLshort *v )  { glTexCoord1sv(v); }

 void AmiglTexCoord2dv( const GLdouble *v ) { glTexCoord2dv(v); }
 void AmiglTexCoord2fv( const GLfloat *v )  { glTexCoord2fv(v); }
 void AmiglTexCoord2iv( const GLint *v )        { glTexCoord2iv(v); }
 void AmiglTexCoord2sv( const GLshort *v )  { glTexCoord2sv(v); }

 void AmiglTexCoord3dv( const GLdouble *v ) { glTexCoord3dv(v); }
 void AmiglTexCoord3fv( const GLfloat *v )  { glTexCoord3fv(v); }
 void AmiglTexCoord3iv( const GLint *v )        { glTexCoord3iv(v); }
 void AmiglTexCoord3sv( const GLshort *v )  { glTexCoord3sv(v); }

 void AmiglTexCoord4dv( const GLdouble *v ) { glTexCoord4dv(v); }
 void AmiglTexCoord4fv( const GLfloat *v )  { glTexCoord4fv(v); }
 void AmiglTexCoord4iv( const GLint *v )        { glTexCoord4iv(v); }
 void AmiglTexCoord4sv( const GLshort *v )  { glTexCoord4sv(v); }

 void AmiglRasterPos2d( GLdouble x, GLdouble y )    { glRasterPos2d(x, y); }
 void AmiglRasterPos2f( GLfloat x, GLfloat y )      { glRasterPos2f(x, y); }
 void AmiglRasterPos2i( GLint x, GLint y )          { glRasterPos2i(x, y); }
 void AmiglRasterPos2s( GLshort x, GLshort y )      { glRasterPos2s(x, y); }

 void AmiglRasterPos3d( GLdouble x, GLdouble y, GLdouble z )    { glRasterPos3d(x, y, z); }
 void AmiglRasterPos3f( GLfloat x, GLfloat y, GLfloat z )       { glRasterPos3f(x, y, z); }
 void AmiglRasterPos3i( GLint x, GLint y, GLint z )             { glRasterPos3i(x, y, z); }
 void AmiglRasterPos3s( GLshort x, GLshort y, GLshort z )       { glRasterPos3s(x, y, z); }

 void AmiglRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )    { glRasterPos4d(x, y, z, w); }
 void AmiglRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )            { glRasterPos4f(x, y, z, w); }
 void AmiglRasterPos4i( GLint x, GLint y, GLint z, GLint w )                    { glRasterPos4i(x, y, z, w); }
 void AmiglRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w )            { glRasterPos4s(x, y, z, w); }

 void AmiglRasterPos2dv( const GLdouble *v )    { glRasterPos2dv(v); }
 void AmiglRasterPos2fv( const GLfloat *v ) { glRasterPos2fv(v); }
 void AmiglRasterPos2iv( const GLint *v )       { glRasterPos2iv(v); }
 void AmiglRasterPos2sv( const GLshort *v ) { glRasterPos2sv(v); }

 void AmiglRasterPos3dv( const GLdouble *v )    { glRasterPos3dv(v); }
 void AmiglRasterPos3fv( const GLfloat *v ) { glRasterPos3fv(v); }
 void AmiglRasterPos3iv( const GLint *v )       { glRasterPos3iv(v); }
 void AmiglRasterPos3sv( const GLshort *v ) { glRasterPos3sv(v); }

 void AmiglRasterPos4dv( const GLdouble *v )    { glRasterPos4dv(v); }
 void AmiglRasterPos4fv( const GLfloat *v ) { glRasterPos4fv(v); }
 void AmiglRasterPos4iv( const GLint *v )       { glRasterPos4iv(v); }
 void AmiglRasterPos4sv( const GLshort *v ) { glRasterPos4sv(v); }

 void AmiglRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 )  { glRectd(x1, y1, x2, y2); }
 void AmiglRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )      { glRectf(x1, y1, x2, y2); }
 void AmiglRecti( GLint x1, GLint y1, GLint x2, GLint y2 )                  { glRecti(x1, y1, x2, y2); }
 void AmiglRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 )      { glRects(x1, y1, x2, y2); }

 void AmiglRectdv( const GLdouble *v1, const GLdouble *v2 ) { glRectdv(v1, v2); }
 void AmiglRectfv( const GLfloat *v1, const GLfloat *v2 )   { glRectfv(v1, v2); }
 void AmiglRectiv( const GLint *v1, const GLint *v2 )           { glRectiv(v1, v2); }
 void AmiglRectsv( const GLshort *v1, const GLshort *v2 )   { glRectsv(v1, v2); }

/*
 * Vertex Arrays  (1.1)
 */

 void AmiglVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
        glVertexPointer(size, type, stride, ptr);
 }

 void AmiglNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr )  {
        glNormalPointer(type, stride, ptr);
 }

 void AmiglColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
        glColorPointer(size, type, stride, ptr);
 }

 void AmiglIndexPointer( GLenum type, GLsizei stride, const GLvoid *ptr )   {
        glIndexPointer(type, stride, ptr);
 }

 void AmiglTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr )    {
        glTexCoordPointer(size, type, stride, ptr);
 }

 void AmiglEdgeFlagPointer( GLsizei stride, const GLboolean *ptr )  {
        glEdgeFlagPointer(stride, ptr); }

 void AmiglGetPointerv( GLenum pname, void **params )   {
        glGetPointerv(pname, params);
 }

 void AmiglArrayElement( GLint i )  { glArrayElement(i); }

 void AmiglDrawArrays( GLenum mode, GLint first, GLsizei count )    {
        glDrawArrays(mode, first, count);
 }

 void AmiglDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices ) {
        glDrawElements(mode, count, type, indices);
 }

 void AmiglInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer ) {
        glInterleavedArrays(format, stride, pointer);
 }

/*
 * Lighting
 */

 void AmiglShadeModel( GLenum mode )    { glShadeModel(mode); }

 void AmiglLightf( GLenum light, GLenum pname, GLfloat param )  { glLightf(light, pname, param); }
 void AmiglLighti( GLenum light, GLenum pname, GLint param )    { glLighti(light, pname, param); }
 void AmiglLightfv( GLenum light, GLenum pname,
                   const GLfloat *params )          { glLightfv(light, pname, params); }
 void AmiglLightiv( GLenum light, GLenum pname,
                   const GLint *params )                { glLightiv(light, pname, params); }

 void AmiglGetLightfv( GLenum light, GLenum pname,
                      GLfloat *params )             { glGetLightfv(light, pname, params); }
 void AmiglGetLightiv( GLenum light, GLenum pname,
                      GLint *params )                   { glGetLightiv(light, pname, params); }

 void AmiglLightModelf( GLenum pname, GLfloat param )               { glLightModelf(pname, param); }
 void AmiglLightModeli( GLenum pname, GLint param )             { glLightModeli(pname, param); }
 void AmiglLightModelfv( GLenum pname, const GLfloat *params )  { glLightModelfv(pname, params); }
 void AmiglLightModeliv( GLenum pname, const GLint *params )    { glLightModeliv(pname, params); }

 void AmiglMaterialf( GLenum face, GLenum pname, GLfloat param )        { glMaterialf(face, pname, param); }
 void AmiglMateriali( GLenum face, GLenum pname, GLint param )          { glMateriali(face, pname, param); }
 void AmiglMaterialfv( GLenum face, GLenum pname, const GLfloat *params )   { glMaterialfv(face, pname, params); }
 void AmiglMaterialiv( GLenum face, GLenum pname, const GLint *params )     { glMaterialiv(face, pname, params); }

 void AmiglGetMaterialfv( GLenum face, GLenum pname, GLfloat *params )  { glGetMaterialfv(face, pname, params); }
 void AmiglGetMaterialiv( GLenum face, GLenum pname, GLint *params )        { glGetMaterialiv(face, pname, params); }

 void AmiglColorMaterial( GLenum face, GLenum mode )    { glColorMaterial(face, mode); }

/*
 * Raster functions
 */

 void AmiglPixelZoom( GLfloat xfactor, GLfloat yfactor )    { glPixelZoom(xfactor, yfactor); }

 void AmiglPixelStoref( GLenum pname, GLfloat param )   { glPixelStoref(pname, param); }
 void AmiglPixelStorei( GLenum pname, GLint param ) { glPixelStorei(pname, param); }

 void AmiglPixelTransferf( GLenum pname, GLfloat param )    { glPixelTransferf(pname, param); }
 void AmiglPixelTransferi( GLenum pname, GLint param )  { glPixelTransferi(pname, param); }

 void AmiglPixelMapfv( GLenum map, GLint mapsize,
                      const GLfloat *values )       { glPixelMapfv(map, mapsize, values); }
 void AmiglPixelMapuiv( GLenum map, GLint mapsize,
                       const GLuint *values )       { glPixelMapuiv(map, mapsize, values); }
 void AmiglPixelMapusv( GLenum map, GLint mapsize,
                       const GLushort *values )     { glPixelMapusv(map, mapsize, values); }

 void AmiglGetPixelMapfv( GLenum map, GLfloat *values ) { glGetPixelMapfv(map, values); }
 void AmiglGetPixelMapuiv( GLenum map, GLuint *values ) { glGetPixelMapuiv(map, values); }
 void AmiglGetPixelMapusv( GLenum map, GLushort *values ){ glGetPixelMapusv(map, values); }

 void AmiglBitmap( GLsizei width, GLsizei height,
                  GLfloat xorig, GLfloat yorig,
                  GLfloat xmove, GLfloat ymove,
                  const GLubyte *bitmap )   {

        glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);
 }

 void AmiglReadPixels( GLint x, GLint y,
                      GLsizei width, GLsizei height,
                      GLenum format, GLenum type,
                      GLvoid *pixels )  {

        glReadPixels(x, y, width, height, format, type, pixels);
 }

 void AmiglDrawPixels( GLsizei width, GLsizei height,
                      GLenum format, GLenum type,
                      const GLvoid *pixels )    {
        glDrawPixels(width, height, format, type, pixels);
 }

 void AmiglCopyPixels( GLint x, GLint y,
                      GLsizei width, GLsizei height,
                      GLenum type ) {
        glCopyPixels(x, y, width, height, type);
 }

/*
 * Stenciling
 */

 void AmiglStencilFunc( GLenum func, GLint ref, GLuint mask )   {
        glStencilFunc(func, ref, mask);
 }

 void AmiglStencilMask( GLuint mask ) {
        glStencilMask(mask);
 }

 void AmiglStencilOp( GLenum fail, GLenum zfail, GLenum zpass ) {
        glStencilOp(fail, zfail, zpass);
 }

 void AmiglClearStencil( GLint s ) {
        glClearStencil(s);
 }

/*
 * Texture mapping
 */

 void AmiglTexGend( GLenum coord, GLenum pname, GLdouble param )    { glTexGend(coord, pname, param); }
 void AmiglTexGenf( GLenum coord, GLenum pname, GLfloat param ) { glTexGenf(coord, pname, param); }
 void AmiglTexGeni( GLenum coord, GLenum pname, GLint param )       { glTexGeni(coord, pname, param); }

 void AmiglTexGendv( GLenum coord, GLenum pname, const GLdouble *params )   { glTexGendv(coord, pname, params); }
 void AmiglTexGenfv( GLenum coord, GLenum pname, const GLfloat *params )    { glTexGenfv(coord, pname, params); }
 void AmiglTexGeniv( GLenum coord, GLenum pname, const GLint *params )      { glTexGeniv(coord, pname, params); }

 void AmiglGetTexGendv( GLenum coord, GLenum pname, GLdouble *params )  { glGetTexGendv(coord, pname, params); }
 void AmiglGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params )   { glGetTexGenfv(coord, pname, params); }
 void AmiglGetTexGeniv( GLenum coord, GLenum pname, GLint *params )     { glGetTexGeniv(coord, pname, params); }

 void AmiglTexEnvf( GLenum target, GLenum pname, GLfloat param )    { glTexEnvf(target, pname, param); }
 void AmiglTexEnvi( GLenum target, GLenum pname, GLint param )      { glTexEnvi(target, pname, param); }

 void AmiglTexEnvfv( GLenum target, GLenum pname, const GLfloat *params )   { glTexEnvfv(target, pname, params); }
 void AmiglTexEnviv( GLenum target, GLenum pname, const GLint *params )     { glTexEnviv(target, pname, params); }

 void AmiglGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params )  { glGetTexEnvfv(target, pname, params); }
 void AmiglGetTexEnviv( GLenum target, GLenum pname, GLint *params )        { glGetTexEnviv(target, pname, params); }

 void AmiglTexParameterf( GLenum target, GLenum pname, GLfloat param )  { glTexParameterf(target, pname, param); }
 void AmiglTexParameteri( GLenum target, GLenum pname, GLint param )        { glTexParameteri(target, pname, param); }

 void AmiglTexParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
        glTexParameterfv(target, pname, params);
 }
 void AmiglTexParameteriv( GLenum target, GLenum pname, const GLint *params ) {
        glTexParameteriv(target, pname, params);
 }

 void AmiglGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params) {
        glGetTexParameterfv(target, pname, params);
 }
 void AmiglGetTexParameteriv( GLenum target, GLenum pname, GLint *params ) {
        glGetTexParameteriv(target, pname, params);
 }

 void AmiglGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params ) {
        glGetTexLevelParameterfv(target, level, pname, params);
 }
 void AmiglGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params ) {
        glGetTexLevelParameteriv(target, level, pname, params);
 }

 void AmiglTexImage1D( GLenum target, GLint level,
                      GLint internalFormat,
                      GLsizei width, GLint border,
                      GLenum format, GLenum type,
                      const GLvoid *pixels )    {

        glTexImage1D(target, level, internalFormat,
                      width, border, format, type, pixels);
 }

 void AmiglTexImage2D( GLenum target, GLint level,
                      GLint internalFormat,
                      GLsizei width, GLsizei height,
                      GLint border, GLenum format, GLenum type,
                      const GLvoid *pixels )    {

        glTexImage2D(target, level, internalFormat,
                      width, height, border, format, type, pixels);
 }

 void AmiglGetTexImage( GLenum target, GLint level,
                       GLenum format, GLenum type,
                       GLvoid *pixels ) {

        glGetTexImage(target, level, format, type, pixels);
 }

/* 1.1 functions */

 void AmiglGenTextures( GLsizei n, GLuint *textures )   {
        glGenTextures(n, textures);
 }

 void AmiglDeleteTextures( GLsizei n, const GLuint *textures)   {
        glDeleteTextures(n, textures);
 }

 void AmiglBindTexture( GLenum target, GLuint texture ) {
        glBindTexture(target, texture);
 }

 void AmiglPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities )  {
        glPrioritizeTextures(n, textures, priorities);
 }

 GLboolean AmiglAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences ) {
        return glAreTexturesResident(n, textures, residences);
 }

 GLboolean AmiglIsTexture( GLuint texture ) {
        return glIsTexture(texture);
 }

 void AmiglTexSubImage1D( GLenum target, GLint level,
                     GLint xoffset,
                     GLsizei width, GLenum format,
                     GLenum type, const GLvoid *pixels ) {

        glTexSubImage1D(target, level, xoffset, width, format, type, pixels);
 }

 void AmiglTexSubImage2D( GLenum target, GLint level,
                     GLint xoffset, GLint yoffset,
                     GLsizei width, GLsizei height,
                     GLenum format, GLenum type,
                     const GLvoid *pixels ) {

        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
 }

 void AmiglCopyTexImage1D( GLenum target, GLint level,
                      GLenum internalformat,
                      GLint x, GLint y,
                      GLsizei width, GLint border ) {

        glCopyTexImage1D(target, level, internalformat, x, y, width, border);
 }

 void AmiglCopyTexImage2D( GLenum target, GLint level,
                      GLenum internalformat,
                      GLint x, GLint y,
                      GLsizei width, GLsizei height,
                      GLint border ){

        glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
 }

 void AmiglCopyTexSubImage1D( GLenum target, GLint level,
                         GLint xoffset, GLint x, GLint y,
                         GLsizei width ) {

        glCopyTexSubImage1D(target, level, xoffset, x, y, width);
 }

 void AmiglCopyTexSubImage2D( GLenum target, GLint level,
                         GLint xoffset, GLint yoffset,
                         GLint x, GLint y,
                         GLsizei width, GLsizei height ) {

        glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
 }

/*
 * Evaluators
 */

 void AmiglMap1d( GLenum target, GLdouble u1, GLdouble u2,
                 GLint stride,
                 GLint order, const GLdouble *points ) {
        glMap1d(target, u1, u2, stride, order, points);
 }
 void AmiglMap1f( GLenum target, GLfloat u1, GLfloat u2,
                 GLint stride,
                 GLint order, const GLfloat *points ) {
        glMap1f(target, u1, u2, stride, order, points);
 }

 void AmiglMap2d( GLenum target,
             GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
             GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
             const GLdouble *points ) {
        glMap2d(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
 }
 void AmiglMap2f( GLenum target,
             GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
             GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
             const GLfloat *points ) {
        glMap2f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
 }

 void AmiglGetMapdv( GLenum target, GLenum query, GLdouble *v ) {
        glGetMapdv(target, query, v);
 }
 void AmiglGetMapfv( GLenum target, GLenum query, GLfloat *v ) {
        glGetMapfv(target, query, v);
 }
 void AmiglGetMapiv( GLenum target, GLenum query, GLint *v ) {
        glGetMapiv(target, query, v);
 }

 void AmiglEvalCoord1d( GLdouble u ) {
        glEvalCoord1d(u);
 }
 void AmiglEvalCoord1f( GLfloat u ) {
        glEvalCoord1f(u);
 }

 void AmiglEvalCoord1dv( const GLdouble *u ) {
        glEvalCoord1dv(u);
 }
 void AmiglEvalCoord1fv( const GLfloat *u ) {
        glEvalCoord1fv(u);
 }

 void AmiglEvalCoord2d( GLdouble u, GLdouble v ) {
        glEvalCoord2d(u, v);
 }
 void AmiglEvalCoord2f( GLfloat u, GLfloat v ) {
        glEvalCoord2f(u, v);
 }

 void AmiglEvalCoord2dv( const GLdouble *u ) {
        glEvalCoord2dv(u);
 }
 void AmiglEvalCoord2fv( const GLfloat *u ) {
        glEvalCoord2fv(u);
 }

 void AmiglMapGrid1d( GLint un, GLdouble u1, GLdouble u2 ) {
        glMapGrid1d(un, u1, u2);
 }
 void AmiglMapGrid1f( GLint un, GLfloat u1, GLfloat u2 ) {
        glMapGrid1f(un, u1, u2);
 }

 void AmiglMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 ) {
        glMapGrid2d(un, u1, u2, vn, v1, v2);
 }
 void AmiglMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 ) {
        glMapGrid2f(un, u1, u2, vn, v1, v2);
 }

 void AmiglEvalPoint1( GLint i ) {
        glEvalPoint1(i);
 }

 void AmiglEvalPoint2( GLint i, GLint j ) {
        glEvalPoint2(i, j);
 }

 void AmiglEvalMesh1( GLenum mode, GLint i1, GLint i2 ) {
        glEvalMesh1(mode, i1, i2);
 }

 void AmiglEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 ) {
        glEvalMesh2(mode, i1, i2, j1, j2);
 }

/*
 * Fog
 */

 void AmiglFogf( GLenum pname, GLfloat param ) {
        glFogf(pname, param);
 }

 void AmiglFogi( GLenum pname, GLint param ) {
        glFogi(pname, param);
 }

 void AmiglFogfv( GLenum pname, const GLfloat *params ) {
        glFogfv(pname, params);
 }

 void AmiglFogiv( GLenum pname, const GLint *params ) {
        glFogiv(pname, params);
 }

/*
 * Selection and Feedback
 */

 void AmiglFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer ) {
        glFeedbackBuffer(size, type, buffer);
 }

 void AmiglPassThrough( GLfloat token ) {
        glPassThrough(token);
 }

 void AmiglSelectBuffer( GLsizei size, GLuint *buffer ) {
        glSelectBuffer(size, buffer);
 }

 void AmiglInitNames( void ) {
        glInitNames();
 }

 void AmiglLoadName( GLuint name ) {
        glLoadName(name);
 }

 void AmiglPushName( GLuint name ) {
        glPushName(name);
 }

 void AmiglPopName( void ) {
        glPopName();
 }

/*
 * 1.0 Extensions
 */

/* GL_EXT_blend_minmax */
 void AmiglBlendEquationEXT( GLenum mode ) {
        glBlendEquationEXT(mode);
 }

/* GL_EXT_blend_color */
 void AmiglBlendColorEXT( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
        glBlendColorEXT(red, green, blue, alpha);
 }

/* GL_EXT_polygon_offset */
 void AmiglPolygonOffsetEXT( GLfloat factor, GLfloat bias ) {
        glPolygonOffsetEXT(factor, bias);
 }

/* GL_EXT_vertex_array */

 void AmiglVertexPointerEXT( GLint size, GLenum type,
                        GLsizei stride,
                        GLsizei count, const GLvoid *ptr ) {
        glVertexPointerEXT(size, type, stride, count, ptr);
 }

 void AmiglNormalPointerEXT( GLenum type, GLsizei stride,
                        GLsizei count, const GLvoid *ptr ) {
        glNormalPointerEXT(type, stride, count, ptr);
 }

 void AmiglColorPointerEXT( GLint size, GLenum type,
                       GLsizei stride,
                       GLsizei count, const GLvoid *ptr ) {
        glColorPointerEXT(size, type, stride, count, ptr);
 }

 void AmiglIndexPointerEXT( GLenum type, GLsizei stride,
                       GLsizei count, const GLvoid *ptr ) {
        glIndexPointerEXT(type, stride, count, ptr);
 }

 void AmiglTexCoordPointerEXT( GLint size, GLenum type,
                          GLsizei stride, GLsizei count,
                          const GLvoid *ptr ) {
        glTexCoordPointerEXT(size, type, stride, count, ptr);
 }

 void AmiglEdgeFlagPointerEXT( GLsizei stride, GLsizei count, const GLboolean *ptr ) {
        glEdgeFlagPointerEXT(stride, count, ptr);
 }

 void AmiglGetPointervEXT( GLenum pname, void **params ) {
        glGetPointervEXT(pname, params);
 }

 void AmiglArrayElementEXT( GLint i ) {
        glArrayElementEXT(i);
 }

 void AmiglDrawArraysEXT( GLenum mode, GLint first, GLsizei count ) {
        glDrawArraysEXT(mode, first, count);
 }

/* GL_EXT_texture_object */

 void AmiglGenTexturesEXT( GLsizei n, GLuint *textures ) {
        glGenTexturesEXT(n, textures);
 }

 void AmiglDeleteTexturesEXT( GLsizei n, const GLuint *textures) {
        glDeleteTexturesEXT(n, textures);
 }

 void AmiglBindTextureEXT( GLenum target, GLuint texture ) {
        glBindTextureEXT(target, texture);
 }

 void AmiglPrioritizeTexturesEXT( GLsizei n, const GLuint *textures, const GLclampf *priorities ) {
        glPrioritizeTexturesEXT(n, textures, priorities);
 }

 GLboolean AmiglAreTexturesResidentEXT( GLsizei n, const GLuint *textures, GLboolean *residences ) {
        return glAreTexturesResidentEXT(n, textures, residences);
 }

 GLboolean AmiglIsTextureEXT( GLuint texture ) {
        return glIsTextureEXT(texture);
 }

/* GL_EXT_texture3D */

 void AmiglTexImage3DEXT( GLenum target, GLint level,
                     GLenum internalFormat,
                     GLsizei width, GLsizei height,
                     GLsizei depth, GLint border,
                     GLenum format, GLenum type,
                     const GLvoid *pixels ) {

        glTexImage3DEXT(target, level, internalFormat, width, height, depth, border,
                             format, type, pixels);
 }

 void AmiglTexSubImage3DEXT( GLenum target, GLint level,
                        GLint xoffset, GLint yoffset,
                        GLint zoffset, GLsizei width,
                        GLsizei height, GLsizei depth,
                        GLenum format,
                        GLenum type, const GLvoid *pixels) {

        glTexSubImage3DEXT(target, level, xoffset, yoffset, zoffset,
                                 width, height, depth, format, type, pixels);
 }

 void AmiglCopyTexSubImage3DEXT( GLenum target, GLint level,
                        GLint xoffset, GLint yoffset,
                        GLint zoffset, GLint x,
                        GLint y, GLsizei width,
                        GLsizei height ) {

        glCopyTexSubImage3DEXT(target, level, xoffset, yoffset, zoffset,
                                      x, y, width, height);
 }

/* GL_EXT_color_table */

 void AmiglColorTableEXT( GLenum target, GLenum internalformat,
                     GLsizei width, GLenum format,
                     GLenum type, const GLvoid *table ) {
        glColorTableEXT(target, internalformat, width, format, type, table);
 }

 void AmiglColorSubTableEXT( GLenum target,
                        GLsizei start, GLsizei count,
                        GLenum format, GLenum type,
                        const GLvoid *data ) {
        glColorSubTableEXT(target, start, count, format, type, data);
 }

 void AmiglGetColorTableEXT( GLenum target, GLenum format,
                        GLenum type, GLvoid *table ) {
        glGetColorTableEXT(target, format, type, table);
 }

 void AmiglGetColorTableParameterfvEXT( GLenum target,
                               GLenum pname,
                               GLfloat *params ) {
        glGetColorTableParameterfvEXT(target, pname, params);
 }

 void AmiglGetColorTableParameterivEXT( GLenum target,
                               GLenum pname,
                               GLint *params ) {
        glGetColorTableParameterivEXT(target, pname, params);
 }

/* GL_SGIS_multitexture */

 void AmiglMultiTexCoord1dSGIS(GLenum target, GLdouble s) {
        glMultiTexCoord1dSGIS(target, s);
 }
 void AmiglMultiTexCoord1dvSGIS(GLenum target, const GLdouble *v) {
        glMultiTexCoord1dvSGIS(target, v);
 }
 void AmiglMultiTexCoord1fSGIS(GLenum target, GLfloat s) {
        glMultiTexCoord1fSGIS(target, s);
 }
 void AmiglMultiTexCoord1fvSGIS(GLenum target, const GLfloat *v) {
        glMultiTexCoord1fvSGIS(target, v);
 }
 void AmiglMultiTexCoord1iSGIS(GLenum target, GLint s) {
        glMultiTexCoord1iSGIS(target, s);
 }
 void AmiglMultiTexCoord1ivSGIS(GLenum target, const GLint *v) {
        glMultiTexCoord1ivSGIS(target, v);
 }
 void AmiglMultiTexCoord1sSGIS(GLenum target, GLshort s) {
        glMultiTexCoord1sSGIS(target, s);
 }
 void AmiglMultiTexCoord1svSGIS(GLenum target, const GLshort *v) {
        glMultiTexCoord1svSGIS(target, v);
 }
 void AmiglMultiTexCoord2dSGIS(GLenum target, GLdouble s, GLdouble t) {
        glMultiTexCoord2dSGIS(target, s, t);
 }
 void AmiglMultiTexCoord2dvSGIS(GLenum target, const GLdouble *v) {
        glMultiTexCoord2dvSGIS(target, v);
 }
 void AmiglMultiTexCoord2fSGIS(GLenum target, GLfloat s, GLfloat t) {
        glMultiTexCoord2fSGIS(target, s, t);
 }
 void AmiglMultiTexCoord2fvSGIS(GLenum target, const GLfloat *v) {
        glMultiTexCoord2fvSGIS(target, v);
 }
 void AmiglMultiTexCoord2iSGIS(GLenum target, GLint s, GLint t) {
        glMultiTexCoord2iSGIS(target, s, t);
 }
 void AmiglMultiTexCoord2ivSGIS(GLenum target, const GLint *v) {
        glMultiTexCoord2ivSGIS(target, v);
 }
 void AmiglMultiTexCoord2sSGIS(GLenum target, GLshort s, GLshort t) {
        glMultiTexCoord2sSGIS(target, s, t);
 }
 void AmiglMultiTexCoord2svSGIS(GLenum target, const GLshort *v) {
        glMultiTexCoord2svSGIS(target, v);
 }
 void AmiglMultiTexCoord3dSGIS(GLenum target, GLdouble s, GLdouble t, GLdouble r) {
        glMultiTexCoord3dSGIS(target, s, t, r);
 }
 void AmiglMultiTexCoord3dvSGIS(GLenum target, const GLdouble *v) {
        glMultiTexCoord3dvSGIS(target, v);
 }
 void AmiglMultiTexCoord3fSGIS(GLenum target, GLfloat s, GLfloat t, GLfloat r) {
        glMultiTexCoord3fSGIS(target, s, t, r);
 }
 void AmiglMultiTexCoord3fvSGIS(GLenum target, const GLfloat *v) {
        glMultiTexCoord3fvSGIS(target, v);
 }
 void AmiglMultiTexCoord3iSGIS(GLenum target, GLint s, GLint t, GLint r) {
        glMultiTexCoord3iSGIS(target, s, t, r);
 }
 void AmiglMultiTexCoord3ivSGIS(GLenum target, const GLint *v) {
        glMultiTexCoord3ivSGIS(target, v);
 }
 void AmiglMultiTexCoord3sSGIS(GLenum target, GLshort s, GLshort t, GLshort r) {
        glMultiTexCoord3sSGIS(target, s, t, r);
 }
 void AmiglMultiTexCoord3svSGIS(GLenum target, const GLshort *v) {
        glMultiTexCoord3svSGIS(target, v);
 }
 void AmiglMultiTexCoord4dSGIS(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q) {
        glMultiTexCoord4dSGIS(target, s, t, r, q);
 }
 void AmiglMultiTexCoord4dvSGIS(GLenum target, const GLdouble *v) {
        glMultiTexCoord4dvSGIS(target, v);
 }
 void AmiglMultiTexCoord4fSGIS(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) {
        glMultiTexCoord4fSGIS(target, s, t, r, q);
 }
 void AmiglMultiTexCoord4fvSGIS(GLenum target, const GLfloat *v) {
        glMultiTexCoord4fvSGIS(target, v);
 }
 void AmiglMultiTexCoord4iSGIS(GLenum target, GLint s, GLint t, GLint r, GLint q) {
        glMultiTexCoord4iSGIS(target, s, t, r, q);
 }
 void AmiglMultiTexCoord4ivSGIS(GLenum target, const GLint *v) {
        glMultiTexCoord4ivSGIS(target, v);
 }
 void AmiglMultiTexCoord4sSGIS(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q) {
        glMultiTexCoord4sSGIS(target, s, t, r, q);
 }
 void AmiglMultiTexCoord4svSGIS(GLenum target, const GLshort *v) {
        glMultiTexCoord4svSGIS(target, v);
 }

 void AmiglMultiTexCoordPointerSGIS(GLenum target, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
        glMultiTexCoordPointerSGIS(target, size, type, stride, pointer);
 }

 void AmiglSelectTextureSGIS(GLenum target) {
        glSelectTextureSGIS(target);
 }

 void AmiglSelectTextureCoordSetSGIS(GLenum target) {
        glSelectTextureCoordSetSGIS(target);
 }

/* GL_EXT_multitexture */

 void AmiglMultiTexCoord1dEXT(GLenum target, GLdouble s) {
        glMultiTexCoord1dEXT(target, s);
 }
 void AmiglMultiTexCoord1dvEXT(GLenum target, const GLdouble *v) {
        glMultiTexCoord1dvEXT(target, v);
 }
 void AmiglMultiTexCoord1fEXT(GLenum target, GLfloat s) {
        glMultiTexCoord1fEXT(target, s);
 }
 void AmiglMultiTexCoord1fvEXT(GLenum target, const GLfloat *v) {
        glMultiTexCoord1fvEXT(target, v);
 }
 void AmiglMultiTexCoord1iEXT(GLenum target, GLint s) {
        glMultiTexCoord1iEXT(target, s);
 }
 void AmiglMultiTexCoord1ivEXT(GLenum target, const GLint *v) {
        glMultiTexCoord1ivEXT(target, v);
 }
 void AmiglMultiTexCoord1sEXT(GLenum target, GLshort s) {
        glMultiTexCoord1sEXT(target, s);
 }
 void AmiglMultiTexCoord1svEXT(GLenum target, const GLshort *v) {
        glMultiTexCoord1svEXT(target, v);
 }
 void AmiglMultiTexCoord2dEXT(GLenum target, GLdouble s, GLdouble t) {
        glMultiTexCoord2dEXT(target, s, t);
 }
 void AmiglMultiTexCoord2dvEXT(GLenum target, const GLdouble *v) {
        glMultiTexCoord2dvEXT(target, v);
 }
 void AmiglMultiTexCoord2fEXT(GLenum target, GLfloat s, GLfloat t) {
        glMultiTexCoord2fEXT(target, s, t);
 }
 void AmiglMultiTexCoord2fvEXT(GLenum target, const GLfloat *v) {
        glMultiTexCoord2fvEXT(target, v);
 }
 void AmiglMultiTexCoord2iEXT(GLenum target, GLint s, GLint t) {
        glMultiTexCoord2iEXT(target, s, t);
 }
 void AmiglMultiTexCoord2ivEXT(GLenum target, const GLint *v) {
        glMultiTexCoord2ivEXT(target, v);
 }
 void AmiglMultiTexCoord2sEXT(GLenum target, GLshort s, GLshort t) {
        glMultiTexCoord2sEXT(target, s, t);
 }
 void AmiglMultiTexCoord2svEXT(GLenum target, const GLshort *v) {
        glMultiTexCoord2svEXT(target, v);
 }
 void AmiglMultiTexCoord3dEXT(GLenum target, GLdouble s, GLdouble t, GLdouble r) {
        glMultiTexCoord3dEXT(target, s, t, r);
 }
 void AmiglMultiTexCoord3dvEXT(GLenum target, const GLdouble *v) {
        glMultiTexCoord3dvEXT(target, v);
 }
 void AmiglMultiTexCoord3fEXT(GLenum target, GLfloat s, GLfloat t, GLfloat r) {
        glMultiTexCoord3fEXT(target, s, t, r);
 }
 void AmiglMultiTexCoord3fvEXT(GLenum target, const GLfloat *v) {
        glMultiTexCoord3fvEXT(target, v);
 }
 void AmiglMultiTexCoord3iEXT(GLenum target, GLint s, GLint t, GLint r) {
        glMultiTexCoord3iEXT(target, s, t, r);
 }
 void AmiglMultiTexCoord3ivEXT(GLenum target, const GLint *v) {
        glMultiTexCoord3ivEXT(target, v);
 }
 void AmiglMultiTexCoord3sEXT(GLenum target, GLshort s, GLshort t, GLshort r) {
        glMultiTexCoord3sEXT(target, s, t, r);
 }
 void AmiglMultiTexCoord3svEXT(GLenum target, const GLshort *v) {
        glMultiTexCoord3svEXT(target, v);
 }
 void AmiglMultiTexCoord4dEXT(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q) {
        glMultiTexCoord4dEXT(target, s, t, r, q);
 }
 void AmiglMultiTexCoord4dvEXT(GLenum target, const GLdouble *v) {
        glMultiTexCoord4dvEXT(target, v);
 }
 void AmiglMultiTexCoord4fEXT(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) {
        glMultiTexCoord4fEXT(target, s, t, r, q);
 }
 void AmiglMultiTexCoord4fvEXT(GLenum target, const GLfloat *v) {
        glMultiTexCoord4fvEXT(target, v);
 }
 void AmiglMultiTexCoord4iEXT(GLenum target, GLint s, GLint t, GLint r, GLint q) {
        glMultiTexCoord4iEXT(target, s, t, r, q);
 }
 void AmiglMultiTexCoord4ivEXT(GLenum target, const GLint *v) {
        glMultiTexCoord4ivEXT(target, v);
 }
 void AmiglMultiTexCoord4sEXT(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q) {
        glMultiTexCoord4sEXT(target, s, t, r, q);
 }
 void AmiglMultiTexCoord4svEXT(GLenum target, const GLshort *v) {
        glMultiTexCoord4svEXT(target, v);
 }
 void AmiglInterleavedTextureCoordSetsEXT( GLint factor ) {
        glInterleavedTextureCoordSetsEXT(factor);
 }

 void AmiglSelectTextureEXT( GLenum target ) {
        glSelectTextureEXT(target);
 }

 void AmiglSelectTextureCoordSetEXT( GLenum target ) {
        glSelectTextureCoordSetEXT(target);
 }

 void AmiglSelectTextureTransformEXT( GLenum target ) {
        glSelectTextureTransformEXT(target);
 }

/* GL_EXT_point_parameters */
 void AmiglPointParameterfEXT( GLenum pname, GLfloat param ) {
        glPointParameterfEXT(pname, param);
 }
 void AmiglPointParameterfvEXT( GLenum pname, const GLfloat *params ) {
        glPointParameterfvEXT(pname, params);
 }

/* GL_MESA_window_pos */

 void AmiglWindowPos2iMESA( GLint x, GLint y ) {
        glWindowPos2iMESA(x, y);
 }
 void AmiglWindowPos2sMESA( GLshort x, GLshort y ) {
        glWindowPos2sMESA(x, y);
 }
 void AmiglWindowPos2fMESA( GLfloat x, GLfloat y ) {
        glWindowPos2fMESA(x, y);
 }
 void AmiglWindowPos2dMESA( GLdouble x, GLdouble y ) {
        glWindowPos2dMESA(x, y);
 }
 void AmiglWindowPos2ivMESA( const GLint *p ) {
        glWindowPos2ivMESA(p);
 }
 void AmiglWindowPos2svMESA( const GLshort *p ) {
        glWindowPos2svMESA(p);
 }
 void AmiglWindowPos2fvMESA( const GLfloat *p ) {
        glWindowPos2fvMESA(p);
 }
 void AmiglWindowPos2dvMESA( const GLdouble *p ) {
        glWindowPos2dvMESA(p);
 }
 void AmiglWindowPos3iMESA( GLint x, GLint y, GLint z ) {
        glWindowPos3iMESA(x, y, z);
 }
 void AmiglWindowPos3sMESA( GLshort x, GLshort y, GLshort z ) {
        glWindowPos3sMESA(x, y, z);
 }
 void AmiglWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z ) {
        glWindowPos3fMESA(x, y, z);
 }
 void AmiglWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z ) {
        glWindowPos3dMESA(x, y, z);
 }
 void AmiglWindowPos3ivMESA( const GLint *p ) {
        glWindowPos3ivMESA(p);
 }
 void AmiglWindowPos3svMESA( const GLshort *p ) {
        glWindowPos3svMESA(p);
 }
 void AmiglWindowPos3fvMESA( const GLfloat *p ) {
        glWindowPos3fvMESA(p);
 }
 void AmiglWindowPos3dvMESA( const GLdouble *p ) {
        glWindowPos3dvMESA(p);
 }
 void AmiglWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w ) {
        glWindowPos4iMESA(x, y, z, w);
 }
 void AmiglWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w ) {
        glWindowPos4sMESA(x, y, z, w);
 }
 void AmiglWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
        glWindowPos4fMESA(x, y, z, w);
 }
 void AmiglWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w) {
        glWindowPos4dMESA(x, y, z, w);
 }
 void AmiglWindowPos4ivMESA( const GLint *p ) {
        glWindowPos4ivMESA(p);
 }
 void AmiglWindowPos4svMESA( const GLshort *p ) {
        glWindowPos4svMESA(p);
 }
 void AmiglWindowPos4fvMESA( const GLfloat *p ) {
        glWindowPos4fvMESA(p);
 }
 void AmiglWindowPos4dvMESA( const GLdouble *p ) {
        glWindowPos4dvMESA(p);
 }

/* GL_MESA_resize_buffers */

 void AmiglResizeBuffersMESA( void ) {
        glResizeBuffersMESA();
 }

/* 1.2 functions */
 void AmiglDrawRangeElements( GLenum mode, GLuint start,
    GLuint end, GLsizei count, GLenum type, const GLvoid *indices ) {

        glDrawRangeElements(mode, start, end, count, type, indices);
 }

 void AmiglTexImage3D( GLenum target, GLint level,
                      GLenum internalFormat,
                      GLsizei width, GLsizei height,
                      GLsizei depth, GLint border,
                      GLenum format, GLenum type,
                      const GLvoid *pixels ) {

        glTexImage3D(target, level, internalFormat, width, height, depth,
                         border, format, type, pixels);
 }

 void AmiglTexSubImage3D( GLenum target, GLint level,
                     GLint xoffset, GLint yoffset,
                     GLint zoffset, GLsizei width,
                     GLsizei height, GLsizei depth,
                     GLenum format,
                     GLenum type, const GLvoid *pixels) {

        glTexSubImage3D(target, level, xoffset, yoffset, zoffset,
                             width, height, depth, format, type, pixels);
 }

 void AmiglCopyTexSubImage3D( GLenum target, GLint level,
                         GLint xoffset, GLint yoffset,
                         GLint zoffset, GLint x,
                         GLint y, GLsizei width,
                         GLsizei height ) {

        glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
 }

/* The GLU API */

/*
 *
 * Miscellaneous functions
 *
 */

 void AmigluLookAt( GLdouble eyex, GLdouble eyey, GLdouble eyez,
                                GLdouble centerx, GLdouble centery,
                                GLdouble centerz,
                                GLdouble upx, GLdouble upy, GLdouble upz ) {

        gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
 }

 void AmigluOrtho2D( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top ) {
        gluOrtho2D(left, right, bottom, top);
 }

 void AmigluPerspective( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar ) {
        gluPerspective(fovy, aspect, zNear, zFar);
 }

 void AmigluPickMatrix( GLdouble x, GLdouble y, GLdouble width, GLdouble height, const GLint viewport[4] ) {
        gluPickMatrix(x, y, width, height, viewport);
 }

 GLint AmigluProject( GLdouble objx, GLdouble objy, GLdouble objz,
                                  const GLdouble modelMatrix[16],
                                  const GLdouble projMatrix[16],
                                  const GLint viewport[4],
                                  GLdouble *winx, GLdouble *winy,
                                  GLdouble *winz ) {

        return gluProject(objx, objy, objz, modelMatrix, projMatrix, viewport, winx, winy, winz);
 }

 GLint AmigluUnProject( GLdouble winx, GLdouble winy,
                                    GLdouble winz,
                                    const GLdouble modelMatrix[16],
                                    const GLdouble projMatrix[16],
                                    const GLint viewport[4],
                                    GLdouble *objx, GLdouble *objy,
                                    GLdouble *objz ) {

        return gluUnProject(winx, winy, winz, modelMatrix, projMatrix, viewport, objx, objy, objz);
 }

 const GLubyte* AmigluErrorString( GLenum errorCode ) {
        return gluErrorString(errorCode);
 }

/*
 *
 * Mipmapping and image scaling
 *
 */

 GLint AmigluScaleImage( GLenum format,
                                     GLint widthin, GLint heightin,
                                     GLenum typein, const void *datain,
                                     GLint widthout, GLint heightout,
                                     GLenum typeout, void *dataout ) {

        return gluScaleImage(format, widthin, heightin, typein, datain, widthout, heightout, typeout, dataout);
 }

 GLint AmigluBuild1DMipmaps( GLenum target, GLint components,
                                         GLint width, GLenum format,
                                         GLenum type, const void *data ) {

        return gluBuild1DMipmaps(target, components, width, format, type, data);
 }

 GLint AmigluBuild2DMipmaps( GLenum target, GLint components,
                                         GLint width, GLint height,
                                         GLenum format,
                                         GLenum type, const void *data ) {

        return gluBuild2DMipmaps(target, components, width, height, format, type, data);
 }

/*
 *
 * Quadrics
 *
 */

 GLUquadricObj* AmigluNewQuadric( void ) {
        return gluNewQuadric();
 }

 void AmigluDeleteQuadric( GLUquadricObj *state ) {
        gluDeleteQuadric(state);
 }

 void AmigluQuadricDrawStyle( GLUquadricObj *quadObject, GLenum drawStyle ) {
        gluQuadricDrawStyle(quadObject, drawStyle);
 }

 void AmigluQuadricOrientation( GLUquadricObj *quadObject, GLenum orientation ) {
        gluQuadricOrientation(quadObject, orientation);
 }

 void AmigluQuadricNormals( GLUquadricObj *quadObject, GLenum normals ) {
        gluQuadricNormals(quadObject, normals);
 }

 void AmigluQuadricTexture( GLUquadricObj *quadObject, GLboolean textureCoords ) {
        gluQuadricTexture(quadObject, textureCoords);
 }

 void AmigluQuadricCallback( GLUquadricObj *qobj, GLenum which, void (CALLBACK *fn)() ) {
        gluQuadricCallback(qobj, which, fn);
 }

 void AmigluCylinder( GLUquadricObj *qobj,
                                  GLdouble baseRadius,
                                  GLdouble topRadius,
                                  GLdouble height,
                                  GLint slices, GLint stacks ) {

        gluCylinder(qobj, baseRadius, topRadius, height, slices, stacks);
 }

 void AmigluSphere( GLUquadricObj *qobj, GLdouble radius, GLint slices, GLint stacks ) {
        gluSphere(qobj, radius, slices, stacks);
 }

 void AmigluDisk( GLUquadricObj *qobj,
                              GLdouble innerRadius, GLdouble outerRadius,
                              GLint slices, GLint loops ) {

        gluDisk(qobj, innerRadius, outerRadius, slices, loops);
 }

 void AmigluPartialDisk( GLUquadricObj *qobj, GLdouble innerRadius,
                                     GLdouble outerRadius, GLint slices,
                                     GLint loops, GLdouble startAngle,
                                     GLdouble sweepAngle ) {

        gluPartialDisk(qobj, innerRadius, outerRadius, slices, loops, startAngle, sweepAngle);
 }

/*
 *
 * Nurbs
 *
 */

 GLUnurbsObj* AmigluNewNurbsRenderer( void ) {
        return gluNewNurbsRenderer();
 }

 void AmigluDeleteNurbsRenderer( GLUnurbsObj *nobj ) {
        gluDeleteNurbsRenderer(nobj);
 }

 void AmigluLoadSamplingMatrices( GLUnurbsObj *nobj,
                                              const GLfloat modelMatrix[16],
                                              const GLfloat projMatrix[16],
                                              const GLint viewport[4] ) {

        gluLoadSamplingMatrices(nobj, modelMatrix, projMatrix, viewport);
 }

 void AmigluNurbsProperty( GLUnurbsObj *nobj, GLenum property, GLfloat value ) {
        gluNurbsProperty(nobj, property, value);
 }

 void AmigluGetNurbsProperty( GLUnurbsObj *nobj, GLenum property, GLfloat *value ) {
        gluGetNurbsProperty(nobj, property, value);
 }

 void AmigluBeginCurve( GLUnurbsObj *nobj ) {
        gluBeginCurve(nobj);
 }

 void AmigluEndCurve( GLUnurbsObj * nobj ) {
        gluEndCurve(nobj);
 }

 void AmigluNurbsCurve( GLUnurbsObj *nobj, GLint nknots,
                                    GLfloat *knot, GLint stride,
                                    GLfloat *ctlarray, GLint order,
                                    GLenum type ) {

        gluNurbsCurve(nobj, nknots, knot, stride, ctlarray, order, type);
 }

 void AmigluBeginSurface( GLUnurbsObj *nobj ) {
        gluBeginSurface(nobj);
 }

 void AmigluEndSurface( GLUnurbsObj * nobj ) {
        gluEndSurface(nobj);
 }

 void AmigluNurbsSurface( GLUnurbsObj *nobj,
                                      GLint sknot_count, GLfloat *sknot,
                                      GLint tknot_count, GLfloat *tknot,
                                      GLint s_stride, GLint t_stride,
                                      GLfloat *ctlarray,
                                      GLint sorder, GLint torder,
                                      GLenum type ) {

        gluNurbsSurface(nobj, sknot_count, sknot, tknot_count, tknot,
                             s_stride, t_stride, ctlarray, sorder, torder, type);
 }

 void AmigluBeginTrim( GLUnurbsObj *nobj ) {
        gluBeginTrim(nobj);
 }

 void AmigluEndTrim( GLUnurbsObj *nobj ) {
        gluEndTrim(nobj);
 }

 void AmigluPwlCurve( GLUnurbsObj *nobj, GLint count, GLfloat *array, GLint stride, GLenum type ) {
        gluPwlCurve(nobj, count, array, stride, type);
 }

 void AmigluNurbsCallback( GLUnurbsObj *nobj, GLenum which, void (CALLBACK *fn)() ) {
        gluNurbsCallback(nobj, which, fn);
 }

/*
 *
 * Polygon tesselation
 *
 */

 GLUtriangulatorObj* AmigluNewTess( void ) {
        return gluNewTess();
 }

 void AmigluTessCallback( GLUtriangulatorObj *tobj, GLenum which, void (CALLBACK *fn)() ) {
        gluTessCallback(tobj, which, fn);
 }

 void AmigluDeleteTess( GLUtriangulatorObj *tobj ) {
        gluDeleteTess(tobj);
 }

 void AmigluBeginPolygon( GLUtriangulatorObj *tobj ) {
        gluBeginPolygon(tobj);
 }

 void AmigluEndPolygon( GLUtriangulatorObj *tobj ) {
        gluEndPolygon(tobj);
 }

 void AmigluNextContour( GLUtriangulatorObj *tobj, GLenum type ) {
        gluNextContour(tobj, type);
 }

 void AmigluTessVertex( GLUtriangulatorObj *tobj, GLdouble v[3], void *data ) {
        gluTessVertex(tobj, v, data);
 }

/*
 *
 * New functions in GLU 1.1
 *
 */

 const GLubyte* AmigluGetString( GLenum name ) {
        return gluGetString(name);
 }


/* Find the functionAddress when the name is given */
void *AmiGetGLProc(const char *proc) {
     void *func = NULL;

     if(strcmp(proc, "glClearIndex") == 0) func =  AmiglClearIndex;
     else if(strcmp(proc, "glClearColor") == 0) func =  AmiglClearColor;
     else if(strcmp(proc, "glClear") == 0) func =  AmiglClear;
     else if(strcmp(proc, "glIndexMask") == 0) func =  AmiglIndexMask;
     else if(strcmp(proc, "glColorMask") == 0) func =  AmiglColorMask;
     else if(strcmp(proc, "glAlphaFunc") == 0) func =  AmiglAlphaFunc;
     else if(strcmp(proc, "glBlendFunc") == 0) func =  AmiglBlendFunc;
     else if(strcmp(proc, "glLogicOp") == 0) func =  AmiglLogicOp;
     else if(strcmp(proc, "glCullFace") == 0) func =  AmiglCullFace;
     else if(strcmp(proc, "glFrontFace") == 0) func =  AmiglFrontFace;
     else if(strcmp(proc, "glPointSize") == 0) func =  AmiglPointSize;
     else if(strcmp(proc, "glLineWidth") == 0) func =  AmiglLineWidth;
     else if(strcmp(proc, "glLineStipple") == 0) func =  AmiglLineStipple;
     else if(strcmp(proc, "glPolygonMode") == 0) func =  AmiglPolygonMode;
     else if(strcmp(proc, "glPolygonOffset") == 0) func =  AmiglPolygonOffset;
     else if(strcmp(proc, "glPolygonStipple") == 0) func =  AmiglPolygonStipple;
     else if(strcmp(proc, "glGetPolygonStipple") == 0) func =  AmiglGetPolygonStipple;
     else if(strcmp(proc, "glEdgeFlag") == 0) func =  AmiglEdgeFlag;
     else if(strcmp(proc, "glEdgeFlagv") == 0) func =  AmiglEdgeFlagv;
     else if(strcmp(proc, "glScissor") == 0) func =  AmiglScissor;
     else if(strcmp(proc, "glClipPlane") == 0) func =  AmiglClipPlane;
     else if(strcmp(proc, "glGetClipPlane") == 0) func =  AmiglGetClipPlane;
     else if(strcmp(proc, "glDrawBuffer") == 0) func =  AmiglDrawBuffer;
     else if(strcmp(proc, "glReadBuffer") == 0) func =  AmiglReadBuffer;
     else if(strcmp(proc, "glEnable") == 0) func =  AmiglEnable;
     else if(strcmp(proc, "glDisable") == 0) func =  AmiglDisable;
     else if(strcmp(proc, "glIsEnabled") == 0) func =  AmiglIsEnabled;
     else if(strcmp(proc, "glEnableClientState") == 0) func =  AmiglEnableClientState;
     else if(strcmp(proc, "glDisableClientState") == 0) func =  AmiglDisableClientState;
     else if(strcmp(proc, "glGetBooleanv") == 0) func =  AmiglGetBooleanv;
     else if(strcmp(proc, "glGetDoublev") == 0) func =  AmiglGetDoublev;
     else if(strcmp(proc, "glGetFloatv") == 0) func =  AmiglGetFloatv;
     else if(strcmp(proc, "glGetIntegerv") == 0) func =  AmiglGetIntegerv;
     else if(strcmp(proc, "glPushAttrib") == 0) func =  glPushAttrib;
     else if(strcmp(proc, "glPopAttrib") == 0) func =  AmiglPopAttrib;
     else if(strcmp(proc, "glPushClientAttrib") == 0) func =  AmiglPushClientAttrib;
     else if(strcmp(proc, "glPopClientAttrib") == 0) func =  AmiglPopClientAttrib;
     else if(strcmp(proc, "glRenderMode") == 0) func =  AmiglRenderMode;
     else if(strcmp(proc, "glGetError") == 0) func =  AmiglGetError;
     else if(strcmp(proc, "glGetString") == 0) func =  AmiglGetString;
     else if(strcmp(proc, "glFinish") == 0) func =  AmiglFinish;
     else if(strcmp(proc, "glFlush") == 0) func =  AmiglFlush;
     else if(strcmp(proc, "glHint") == 0) func =  glHint;
     else if(strcmp(proc, "glClearDepth") == 0) func =  AmiglClearDepth;
     else if(strcmp(proc, "glDepthFunc") == 0) func =  AmiglDepthFunc;
     else if(strcmp(proc, "glDepthMask") == 0) func =  AmiglDepthMask;
     else if(strcmp(proc, "glDepthRange") == 0) func =  AmiglDepthRange;
     else if(strcmp(proc, "glClearAccum") == 0) func =  AmiglClearAccum;
     else if(strcmp(proc, "glAccum") == 0) func =  AmiglAccum;
     else if(strcmp(proc, "glMatrixMode") == 0) func =  AmiglMatrixMode;
     else if(strcmp(proc, "glOrtho") == 0) func =  AmiglOrtho;
     else if(strcmp(proc, "glFrustum") == 0) func =  AmiglFrustum;
     else if(strcmp(proc, "glViewport") == 0) func =  AmiglViewport;
     else if(strcmp(proc, "glPushMatrix") == 0) func =  AmiglPushMatrix;
     else if(strcmp(proc, "glPopMatrix") == 0) func =  AmiglPopMatrix;
     else if(strcmp(proc, "glLoadIdentity") == 0) func =  AmiglLoadIdentity;
     else if(strcmp(proc, "glLoadMatrixd") == 0) func =  AmiglLoadMatrixd;
     else if(strcmp(proc, "glLoadMatrixf") == 0) func =  AmiglLoadMatrixf;
     else if(strcmp(proc, "glMultMatrixd") == 0) func =  AmiglMultMatrixd;
     else if(strcmp(proc, "glMultMatrixf") == 0) func =  AmiglMultMatrixf;
     else if(strcmp(proc, "glRotated") == 0) func =  AmiglRotated;
     else if(strcmp(proc, "glRotatef") == 0) func =  AmiglRotatef;
     else if(strcmp(proc, "glScaled") == 0) func =  AmiglScaled;
     else if(strcmp(proc, "glScalef") == 0) func =  AmiglScalef;
     else if(strcmp(proc, "glTranslated") == 0) func =  AmiglTranslated;
     else if(strcmp(proc, "glTranslatef") == 0) func =  AmiglTranslatef;
     else if(strcmp(proc, "glIsList") == 0) func =  AmiglIsList;
     else if(strcmp(proc, "glDeleteLists") == 0) func =  AmiglDeleteLists;
     else if(strcmp(proc, "glGenLists") == 0) func =  AmiglGenLists;
     else if(strcmp(proc, "glNewList") == 0) func =  AmiglNewList;
     else if(strcmp(proc, "glEndList") == 0) func =  AmiglEndList;
     else if(strcmp(proc, "glCallList") == 0) func =  AmiglCallList;
     else if(strcmp(proc, "glCallLists") == 0) func =  AmiglCallLists;
     else if(strcmp(proc, "glListBase") == 0) func =  AmiglListBase;
     else if(strcmp(proc, "glBegin") == 0) func =  AmiglBegin;
     else if(strcmp(proc, "glEnd") == 0) func =  AmiglEnd;
     else if(strcmp(proc, "glVertex2d") == 0) func =  AmiglVertex2d;
     else if(strcmp(proc, "glVertex2f") == 0) func =  AmiglVertex2f;
     else if(strcmp(proc, "glVertex2i") == 0) func =  AmiglVertex2i;
     else if(strcmp(proc, "glVertex2s") == 0) func =  AmiglVertex2s;
     else if(strcmp(proc, "glVertex3d") == 0) func =  AmiglVertex3d;
     else if(strcmp(proc, "glVertex3f") == 0) func =  AmiglVertex3f;
     else if(strcmp(proc, "glVertex3i") == 0) func =  AmiglVertex3i;
     else if(strcmp(proc, "glVertex3s") == 0) func =  AmiglVertex3s;
     else if(strcmp(proc, "glVertex4d") == 0) func =  AmiglVertex4d;
     else if(strcmp(proc, "glVertex4f") == 0) func =  AmiglVertex4f;
     else if(strcmp(proc, "glVertex4i") == 0) func =  AmiglVertex4i;
     else if(strcmp(proc, "glVertex4s") == 0) func =  AmiglVertex4s;
     else if(strcmp(proc, "glVertex2dv") == 0) func =  AmiglVertex2dv;
     else if(strcmp(proc, "glVertex2fv") == 0) func =  AmiglVertex2fv;
     else if(strcmp(proc, "glVertex2iv") == 0) func =  AmiglVertex2iv;
     else if(strcmp(proc, "glVertex2sv") == 0) func =  AmiglVertex2sv;
     else if(strcmp(proc, "glVertex3dv") == 0) func =  AmiglVertex3dv;
     else if(strcmp(proc, "glVertex3fv") == 0) func =  AmiglVertex3fv;
     else if(strcmp(proc, "glVertex3iv") == 0) func =  AmiglVertex3iv;
     else if(strcmp(proc, "glVertex3sv") == 0) func =  AmiglVertex3sv;
     else if(strcmp(proc, "glVertex4dv") == 0) func =  AmiglVertex4dv;
     else if(strcmp(proc, "glVertex4fv") == 0) func =  AmiglVertex4fv;
     else if(strcmp(proc, "glVertex4iv") == 0) func =  AmiglVertex4iv;
     else if(strcmp(proc, "glVertex4sv") == 0) func =  AmiglVertex4sv;
     else if(strcmp(proc, "glNormal3b") == 0) func =  AmiglNormal3b;
     else if(strcmp(proc, "glNormal3d") == 0) func =  AmiglNormal3d;
     else if(strcmp(proc, "glNormal3f") == 0) func =  AmiglNormal3f;
     else if(strcmp(proc, "glNormal3i") == 0) func =  AmiglNormal3i;
     else if(strcmp(proc, "glNormal3s") == 0) func =  AmiglNormal3s;
     else if(strcmp(proc, "glNormal3bv") == 0) func =  AmiglNormal3bv;
     else if(strcmp(proc, "glNormal3dv") == 0) func =  AmiglNormal3dv;
     else if(strcmp(proc, "glNormal3fv") == 0) func =  AmiglNormal3fv;
     else if(strcmp(proc, "glNormal3iv") == 0) func =  AmiglNormal3iv;
     else if(strcmp(proc, "glNormal3sv") == 0) func =  AmiglNormal3sv;
     else if(strcmp(proc, "glIndexd") == 0) func =  AmiglIndexd;
     else if(strcmp(proc, "glIndexf") == 0) func =  AmiglIndexf;
     else if(strcmp(proc, "glIndexi") == 0) func =  AmiglIndexi;
     else if(strcmp(proc, "glIndexs") == 0) func =  AmiglIndexs;
     else if(strcmp(proc, "glIndexub") == 0) func =  AmiglIndexub;
     else if(strcmp(proc, "glIndexdv") == 0) func =  AmiglIndexdv;
     else if(strcmp(proc, "glIndexfv") == 0) func =  AmiglIndexfv;
     else if(strcmp(proc, "glIndexiv") == 0) func =  AmiglIndexiv;
     else if(strcmp(proc, "glIndexsv") == 0) func =  AmiglIndexsv;
     else if(strcmp(proc, "glIndexubv") == 0) func =  AmiglIndexubv;
     else if(strcmp(proc, "glColor3b") == 0) func =  AmiglColor3b;
     else if(strcmp(proc, "glColor3d") == 0) func =  AmiglColor3d;
     else if(strcmp(proc, "glColor3f") == 0) func =  AmiglColor3f;
     else if(strcmp(proc, "glColor3i") == 0) func =  AmiglColor3i;
     else if(strcmp(proc, "glColor3s") == 0) func =  AmiglColor3s;
     else if(strcmp(proc, "glColor3ub") == 0) func =  AmiglColor3ub;
     else if(strcmp(proc, "glColor3ui") == 0) func =  AmiglColor3ui;
     else if(strcmp(proc, "glColor3us") == 0) func =  AmiglColor3us;
     else if(strcmp(proc, "glColor4b") == 0) func =  AmiglColor4b;
     else if(strcmp(proc, "glColor4d") == 0) func =  AmiglColor4d;
     else if(strcmp(proc, "glColor4f") == 0) func =  AmiglColor4f;
     else if(strcmp(proc, "glColor4i") == 0) func =  AmiglColor4i;
     else if(strcmp(proc, "glColor4s") == 0) func =  AmiglColor4s;
     else if(strcmp(proc, "glColor4ub") == 0) func =  AmiglColor4ub;
     else if(strcmp(proc, "glColor4ui") == 0) func =  AmiglColor4ui;
     else if(strcmp(proc, "glColor4us") == 0) func =  AmiglColor4us;
     else if(strcmp(proc, "glColor3bv") == 0) func =  AmiglColor3bv;
     else if(strcmp(proc, "glColor3dv") == 0) func =  AmiglColor3dv;
     else if(strcmp(proc, "glColor3fv") == 0) func =  AmiglColor3fv;
     else if(strcmp(proc, "glColor3iv") == 0) func =  AmiglColor3iv;
     else if(strcmp(proc, "glColor3sv") == 0) func =  AmiglColor3sv;
     else if(strcmp(proc, "glColor3ubv") == 0) func =  AmiglColor3ubv;
     else if(strcmp(proc, "glColor3uiv") == 0) func =  AmiglColor3uiv;
     else if(strcmp(proc, "glColor3usv") == 0) func =  AmiglColor3usv;
     else if(strcmp(proc, "glColor4bv") == 0) func =  AmiglColor4bv;
     else if(strcmp(proc, "glColor4dv") == 0) func =  AmiglColor4dv;
     else if(strcmp(proc, "glColor4fv") == 0) func =  AmiglColor4fv;
     else if(strcmp(proc, "glColor4iv") == 0) func =  AmiglColor4iv;
     else if(strcmp(proc, "glColor4sv") == 0) func =  AmiglColor4sv;
     else if(strcmp(proc, "glColor4ubv") == 0) func =  AmiglColor4ubv;
     else if(strcmp(proc, "glColor4uiv") == 0) func =  AmiglColor4uiv;
     else if(strcmp(proc, "glColor4usv") == 0) func =  AmiglColor4usv;
     else if(strcmp(proc, "glTexCoord1d") == 0) func =  AmiglTexCoord1d;
     else if(strcmp(proc, "glTexCoord1f") == 0) func =  AmiglTexCoord1f;
     else if(strcmp(proc, "glTexCoord1i") == 0) func =  AmiglTexCoord1i;
     else if(strcmp(proc, "glTexCoord1s") == 0) func =  AmiglTexCoord1s;
     else if(strcmp(proc, "glTexCoord2d") == 0) func =  AmiglTexCoord2d;
     else if(strcmp(proc, "glTexCoord2f") == 0) func =  AmiglTexCoord2f;
     else if(strcmp(proc, "glTexCoord2i") == 0) func =  AmiglTexCoord2i;
     else if(strcmp(proc, "glTexCoord2s") == 0) func =  AmiglTexCoord2s;
     else if(strcmp(proc, "glTexCoord3d") == 0) func =  AmiglTexCoord3d;
     else if(strcmp(proc, "glTexCoord3f") == 0) func =  AmiglTexCoord3f;
     else if(strcmp(proc, "glTexCoord3i") == 0) func =  AmiglTexCoord3i;
     else if(strcmp(proc, "glTexCoord3s") == 0) func =  AmiglTexCoord3s;
     else if(strcmp(proc, "glTexCoord4d") == 0) func =  AmiglTexCoord4d;
     else if(strcmp(proc, "glTexCoord4f") == 0) func =  AmiglTexCoord4f;
     else if(strcmp(proc, "glTexCoord4i") == 0) func =  AmiglTexCoord4i;
     else if(strcmp(proc, "glTexCoord4s") == 0) func =  AmiglTexCoord4s;
     else if(strcmp(proc, "glTexCoord1dv") == 0) func =  AmiglTexCoord1dv;
     else if(strcmp(proc, "glTexCoord1fv") == 0) func =  AmiglTexCoord1fv;
     else if(strcmp(proc, "glTexCoord1iv") == 0) func =  AmiglTexCoord1iv;
     else if(strcmp(proc, "glTexCoord1sv") == 0) func =  AmiglTexCoord1sv;
     else if(strcmp(proc, "glTexCoord2dv") == 0) func =  AmiglTexCoord2dv;
     else if(strcmp(proc, "glTexCoord2fv") == 0) func =  AmiglTexCoord2fv;
     else if(strcmp(proc, "glTexCoord2iv") == 0) func =  AmiglTexCoord2iv;
     else if(strcmp(proc, "glTexCoord2sv") == 0) func =  AmiglTexCoord2sv;
     else if(strcmp(proc, "glTexCoord3dv") == 0) func =  AmiglTexCoord3dv;
     else if(strcmp(proc, "glTexCoord3fv") == 0) func =  AmiglTexCoord3fv;
     else if(strcmp(proc, "glTexCoord3iv") == 0) func =  AmiglTexCoord3iv;
     else if(strcmp(proc, "glTexCoord3sv") == 0) func =  AmiglTexCoord3sv;
     else if(strcmp(proc, "glTexCoord4dv") == 0) func =  AmiglTexCoord4dv;
     else if(strcmp(proc, "glTexCoord4fv") == 0) func =  AmiglTexCoord4fv;
     else if(strcmp(proc, "glTexCoord4iv") == 0) func =  AmiglTexCoord4iv;
     else if(strcmp(proc, "glTexCoord4sv") == 0) func =  AmiglTexCoord4sv;
     else if(strcmp(proc, "glRasterPos2d") == 0) func =  AmiglRasterPos2d;
     else if(strcmp(proc, "glRasterPos2f") == 0) func =  AmiglRasterPos2f;
     else if(strcmp(proc, "glRasterPos2i") == 0) func =  AmiglRasterPos2i;
     else if(strcmp(proc, "glRasterPos2s") == 0) func =  AmiglRasterPos2s;
     else if(strcmp(proc, "glRasterPos3d") == 0) func =  AmiglRasterPos3d;
     else if(strcmp(proc, "glRasterPos3f") == 0) func =  AmiglRasterPos3f;
     else if(strcmp(proc, "glRasterPos3i") == 0) func =  AmiglRasterPos3i;
     else if(strcmp(proc, "glRasterPos3s") == 0) func =  AmiglRasterPos3s;
     else if(strcmp(proc, "glRasterPos4d") == 0) func =  AmiglRasterPos4d;
     else if(strcmp(proc, "glRasterPos4f") == 0) func =  AmiglRasterPos4f;
     else if(strcmp(proc, "glRasterPos4i") == 0) func =  AmiglRasterPos4i;
     else if(strcmp(proc, "glRasterPos4s") == 0) func =  AmiglRasterPos4s;
     else if(strcmp(proc, "glRasterPos2dv") == 0) func =  AmiglRasterPos2dv;
     else if(strcmp(proc, "glRasterPos2fv") == 0) func =  AmiglRasterPos2fv;
     else if(strcmp(proc, "glRasterPos2iv") == 0) func =  AmiglRasterPos2iv;
     else if(strcmp(proc, "glRasterPos2sv") == 0) func =  AmiglRasterPos2sv;
     else if(strcmp(proc, "glRasterPos3dv") == 0) func =  AmiglRasterPos3dv;
     else if(strcmp(proc, "glRasterPos3fv") == 0) func =  AmiglRasterPos3fv;
     else if(strcmp(proc, "glRasterPos3iv") == 0) func =  AmiglRasterPos3iv;
     else if(strcmp(proc, "glRasterPos3sv") == 0) func =  AmiglRasterPos3sv;
     else if(strcmp(proc, "glRasterPos4dv") == 0) func =  AmiglRasterPos4dv;
     else if(strcmp(proc, "glRasterPos4fv") == 0) func =  AmiglRasterPos4fv;
     else if(strcmp(proc, "glRasterPos4iv") == 0) func =  AmiglRasterPos4iv;
     else if(strcmp(proc, "glRasterPos4sv") == 0) func =  AmiglRasterPos4sv;
     else if(strcmp(proc, "glRectd") == 0) func =  AmiglRectd;
     else if(strcmp(proc, "glRectf") == 0) func =  AmiglRectf;
     else if(strcmp(proc, "glRecti") == 0) func =  AmiglRecti;
     else if(strcmp(proc, "glRects") == 0) func =  AmiglRects;
     else if(strcmp(proc, "glRectdv") == 0) func =  AmiglRectdv;
     else if(strcmp(proc, "glRectfv") == 0) func =  AmiglRectfv;
     else if(strcmp(proc, "glRectiv") == 0) func =  AmiglRectiv;
     else if(strcmp(proc, "glRectsv") == 0) func =  AmiglRectsv;
     else if(strcmp(proc, "glVertexPointer") == 0) func =  AmiglVertexPointer;
     else if(strcmp(proc, "glNormalPointer") == 0) func =  AmiglNormalPointer;
     else if(strcmp(proc, "glColorPointer") == 0) func =  AmiglColorPointer;
     else if(strcmp(proc, "glIndexPointer") == 0) func =  AmiglIndexPointer;
     else if(strcmp(proc, "glTexCoordPointer") == 0) func =  AmiglTexCoordPointer;
     else if(strcmp(proc, "glEdgeFlagPointer") == 0) func =  AmiglEdgeFlagPointer;
     else if(strcmp(proc, "glGetPointerv") == 0) func =  AmiglGetPointerv;
     else if(strcmp(proc, "glArrayElement") == 0) func =  AmiglArrayElement;
     else if(strcmp(proc, "glDrawArrays") == 0) func =  AmiglDrawArrays;
     else if(strcmp(proc, "glDrawElements") == 0) func =  AmiglDrawElements;
     else if(strcmp(proc, "glInterleavedArrays") == 0) func =  AmiglInterleavedArrays;
     else if(strcmp(proc, "glShadeModel") == 0) func =  AmiglShadeModel;
     else if(strcmp(proc, "glLightf") == 0) func =  AmiglLightf;
     else if(strcmp(proc, "glLighti") == 0) func =  AmiglLighti;
     else if(strcmp(proc, "glLightfv") == 0) func =  AmiglLightfv;
     else if(strcmp(proc, "glLightiv") == 0) func =  AmiglLightiv;
     else if(strcmp(proc, "glGetLightfv") == 0) func =  AmiglGetLightfv;
     else if(strcmp(proc, "glGetLightiv") == 0) func =  AmiglGetLightiv;
     else if(strcmp(proc, "glLightModelf") == 0) func =  AmiglLightModelf;
     else if(strcmp(proc, "glLightModeli") == 0) func =  AmiglLightModeli;
     else if(strcmp(proc, "glLightModelfv") == 0) func =  AmiglLightModelfv;
     else if(strcmp(proc, "glLightModeliv") == 0) func =  AmiglLightModeliv;
     else if(strcmp(proc, "glMaterialf") == 0) func =  AmiglMaterialf;
     else if(strcmp(proc, "glMateriali") == 0) func =  AmiglMateriali;
     else if(strcmp(proc, "glMaterialfv") == 0) func =  AmiglMaterialfv;
     else if(strcmp(proc, "glMaterialiv") == 0) func =  AmiglMaterialiv;
     else if(strcmp(proc, "glGetMaterialfv") == 0) func =  AmiglGetMaterialfv;
     else if(strcmp(proc, "glGetMaterialiv") == 0) func =  AmiglGetMaterialiv;
     else if(strcmp(proc, "glColorMaterial") == 0) func =  AmiglColorMaterial;
     else if(strcmp(proc, "glPixelZoom") == 0) func =  AmiglPixelZoom;
     else if(strcmp(proc, "glPixelStoref") == 0) func =  AmiglPixelStoref;
     else if(strcmp(proc, "glPixelStorei") == 0) func =  AmiglPixelStorei;
     else if(strcmp(proc, "glPixelTransferf") == 0) func =  AmiglPixelTransferf;
     else if(strcmp(proc, "glPixelTransferi") == 0) func =  AmiglPixelTransferi;
     else if(strcmp(proc, "glPixelMapfv") == 0) func =  AmiglPixelMapfv;
     else if(strcmp(proc, "glPixelMapuiv") == 0) func =  AmiglPixelMapuiv;
     else if(strcmp(proc, "glPixelMapusv") == 0) func =  AmiglPixelMapusv;
     else if(strcmp(proc, "glGetPixelMapfv") == 0) func =  AmiglGetPixelMapfv;
     else if(strcmp(proc, "glGetPixelMapuiv") == 0) func =  AmiglGetPixelMapuiv;
     else if(strcmp(proc, "glGetPixelMapusv") == 0) func =  AmiglGetPixelMapusv;
     else if(strcmp(proc, "glBitmap") == 0) func =  AmiglBitmap;
     else if(strcmp(proc, "glReadPixels") == 0) func =  AmiglReadPixels;
     else if(strcmp(proc, "glDrawPixels") == 0) func =  AmiglDrawPixels;
     else if(strcmp(proc, "glCopyPixels") == 0) func =  AmiglCopyPixels;
     else if(strcmp(proc, "glStencilFunc") == 0) func =  AmiglStencilFunc;
     else if(strcmp(proc, "glStencilMask") == 0) func =  AmiglStencilMask;
     else if(strcmp(proc, "glStencilOp") == 0) func =  AmiglStencilOp;
     else if(strcmp(proc, "glClearStencil") == 0) func =  AmiglClearStencil;
     else if(strcmp(proc, "glTexGend") == 0) func =  AmiglTexGend;
     else if(strcmp(proc, "glTexGenf") == 0) func =  AmiglTexGenf;
     else if(strcmp(proc, "glTexGeni") == 0) func =  AmiglTexGeni;
     else if(strcmp(proc, "glTexGendv") == 0) func =  AmiglTexGendv;
     else if(strcmp(proc, "glTexGenfv") == 0) func =  AmiglTexGenfv;
     else if(strcmp(proc, "glTexGeniv") == 0) func =  AmiglTexGeniv;
     else if(strcmp(proc, "glGetTexGendv") == 0) func =  AmiglGetTexGendv;
     else if(strcmp(proc, "glGetTexGenfv") == 0) func =  AmiglGetTexGenfv;
     else if(strcmp(proc, "glGetTexGeniv") == 0) func =  AmiglGetTexGeniv;
     else if(strcmp(proc, "glTexEnvf") == 0) func =  AmiglTexEnvf;
     else if(strcmp(proc, "glTexEnvi") == 0) func =  AmiglTexEnvi;
     else if(strcmp(proc, "glTexEnvfv") == 0) func =  AmiglTexEnvfv;
     else if(strcmp(proc, "glTexEnviv") == 0) func =  AmiglTexEnviv;
     else if(strcmp(proc, "glGetTexEnvfv") == 0) func =  AmiglGetTexEnvfv;
     else if(strcmp(proc, "glGetTexEnviv") == 0) func =  AmiglGetTexEnviv;
     else if(strcmp(proc, "glTexParameterf") == 0) func =  AmiglTexParameterf;
     else if(strcmp(proc, "glTexParameteri") == 0) func =  AmiglTexParameteri;
     else if(strcmp(proc, "glTexParameterfv") == 0) func =  AmiglTexParameterfv;
     else if(strcmp(proc, "glTexParameteriv") == 0) func =  AmiglTexParameteriv;
     else if(strcmp(proc, "glGetTexParameterfv") == 0) func =  AmiglGetTexParameterfv;
     else if(strcmp(proc, "glGetTexParameteriv") == 0) func =  AmiglGetTexParameteriv;
     else if(strcmp(proc, "glGetTexLevelParameterfv") == 0) func =  AmiglGetTexLevelParameterfv;
     else if(strcmp(proc, "glGetTexLevelParameteriv") == 0) func =  AmiglGetTexLevelParameteriv;
     else if(strcmp(proc, "glTexImage1D") == 0) func =  AmiglTexImage1D;
     else if(strcmp(proc, "glTexImage2D") == 0) func =  AmiglTexImage2D;
     else if(strcmp(proc, "glGetTexImage") == 0) func =  AmiglGetTexImage;
     else if(strcmp(proc, "glGenTextures") == 0) func =  AmiglGenTextures;
     else if(strcmp(proc, "glDeleteTextures") == 0) func =  AmiglDeleteTextures;
     else if(strcmp(proc, "glBindTexture") == 0) func =  AmiglBindTexture;
     else if(strcmp(proc, "glPrioritizeTextures") == 0) func =  AmiglPrioritizeTextures;
     else if(strcmp(proc, "glAreTexturesResident") == 0) func =  AmiglAreTexturesResident;
     else if(strcmp(proc, "glIsTexture") == 0) func =  AmiglIsTexture;
     else if(strcmp(proc, "glTexSubImage1D") == 0) func =  AmiglTexSubImage1D;
     else if(strcmp(proc, "glTexSubImage2D") == 0) func =  AmiglTexSubImage2D;
     else if(strcmp(proc, "glCopyTexImage1D") == 0) func =  AmiglCopyTexImage1D;
     else if(strcmp(proc, "glCopyTexImage2D") == 0) func =  AmiglCopyTexImage2D;
     else if(strcmp(proc, "glCopyTexSubImage1D") == 0) func =  AmiglCopyTexSubImage1D;
     else if(strcmp(proc, "glCopyTexSubImage2D") == 0) func =  AmiglCopyTexSubImage2D;
     else if(strcmp(proc, "glMap1d") == 0) func =  AmiglMap1d;
     else if(strcmp(proc, "glMap1f") == 0) func =  AmiglMap1f;
     else if(strcmp(proc, "glMap2d") == 0) func =  AmiglMap2d;
     else if(strcmp(proc, "glMap2f") == 0) func =  AmiglMap2f;
     else if(strcmp(proc, "glGetMapdv") == 0) func =  AmiglGetMapdv;
     else if(strcmp(proc, "glGetMapfv") == 0) func =  AmiglGetMapfv;
     else if(strcmp(proc, "glGetMapiv") == 0) func =  AmiglGetMapiv;
     else if(strcmp(proc, "glEvalCoord1d") == 0) func =  AmiglEvalCoord1d;
     else if(strcmp(proc, "glEvalCoord1f") == 0) func =  AmiglEvalCoord1f;
     else if(strcmp(proc, "glEvalCoord1dv") == 0) func =  AmiglEvalCoord1dv;
     else if(strcmp(proc, "glEvalCoord1fv") == 0) func =  AmiglEvalCoord1fv;
     else if(strcmp(proc, "glEvalCoord2d") == 0) func =  AmiglEvalCoord2d;
     else if(strcmp(proc, "glEvalCoord2f") == 0) func =  AmiglEvalCoord2f;
     else if(strcmp(proc, "glEvalCoord2dv") == 0) func =  AmiglEvalCoord2dv;
     else if(strcmp(proc, "glEvalCoord2fv") == 0) func =  AmiglEvalCoord2fv;
     else if(strcmp(proc, "glMapGrid1d") == 0) func =  AmiglMapGrid1d;
     else if(strcmp(proc, "glMapGrid1f") == 0) func =  AmiglMapGrid1f;
     else if(strcmp(proc, "glMapGrid2d") == 0) func =  AmiglMapGrid2d;
     else if(strcmp(proc, "glMapGrid2f") == 0) func =  AmiglMapGrid2f;
     else if(strcmp(proc, "glEvalPoint1") == 0) func =  AmiglEvalPoint1;
     else if(strcmp(proc, "glEvalPoint2") == 0) func =  AmiglEvalPoint2;
     else if(strcmp(proc, "glEvalMesh1") == 0) func =  AmiglEvalMesh1;
     else if(strcmp(proc, "glEvalMesh2") == 0) func =  AmiglEvalMesh2;
     else if(strcmp(proc, "glFogf") == 0) func =  AmiglFogf;
     else if(strcmp(proc, "glFogi") == 0) func =  AmiglFogi;
     else if(strcmp(proc, "glFogfv") == 0) func =  AmiglFogfv;
     else if(strcmp(proc, "glFogiv") == 0) func =  AmiglFogiv;
     else if(strcmp(proc, "glFeedbackBuffer") == 0) func =  AmiglFeedbackBuffer;
     else if(strcmp(proc, "glPassThrough") == 0) func =  AmiglPassThrough;
     else if(strcmp(proc, "glSelectBuffer") == 0) func =  AmiglSelectBuffer;
     else if(strcmp(proc, "glInitNames") == 0) func =  AmiglInitNames;
     else if(strcmp(proc, "glLoadName") == 0) func =  AmiglLoadName;
     else if(strcmp(proc, "glPushName") == 0) func =  AmiglPushName;
     else if(strcmp(proc, "glPopName") == 0) func =  AmiglPopName;
     else if(strcmp(proc, "glBlendEquationEXT") == 0) func =  AmiglBlendEquationEXT;
     else if(strcmp(proc, "glBlendColorEXT") == 0) func =  AmiglBlendColorEXT;
     else if(strcmp(proc, "glPolygonOffsetEXT") == 0) func =  AmiglPolygonOffsetEXT;
     else if(strcmp(proc, "glVertexPointerEXT") == 0) func =  AmiglVertexPointerEXT;
     else if(strcmp(proc, "glNormalPointerEXT") == 0) func =  AmiglNormalPointerEXT;
     else if(strcmp(proc, "glColorPointerEXT") == 0) func =  AmiglColorPointerEXT;
     else if(strcmp(proc, "glIndexPointerEXT") == 0) func =  AmiglIndexPointerEXT;
     else if(strcmp(proc, "glTexCoordPointerEXT") == 0) func =  AmiglTexCoordPointerEXT;
     else if(strcmp(proc, "glEdgeFlagPointerEXT") == 0) func =  AmiglEdgeFlagPointerEXT;
     else if(strcmp(proc, "glGetPointervEXT") == 0) func =  AmiglGetPointervEXT;
     else if(strcmp(proc, "glArrayElementEXT") == 0) func =  AmiglArrayElementEXT;
     else if(strcmp(proc, "glDrawArraysEXT") == 0) func =  AmiglDrawArraysEXT;
     else if(strcmp(proc, "glGenTexturesEXT") == 0) func =  AmiglGenTexturesEXT;
     else if(strcmp(proc, "glDeleteTexturesEXT") == 0) func =  AmiglDeleteTexturesEXT;
     else if(strcmp(proc, "glBindTextureEXT") == 0) func =  AmiglBindTextureEXT;
     else if(strcmp(proc, "glPrioritizeTexturesEXT") == 0) func =  AmiglPrioritizeTexturesEXT;
     else if(strcmp(proc, "glAreTexturesResidentEXT") == 0) func =  AmiglAreTexturesResidentEXT;
     else if(strcmp(proc, "glIsTextureEXT") == 0) func =  AmiglIsTextureEXT;
     else if(strcmp(proc, "glTexImage3DEXT") == 0) func =  AmiglTexImage3DEXT;
     else if(strcmp(proc, "glTexSubImage3DEXT") == 0) func =  AmiglTexSubImage3DEXT;
     else if(strcmp(proc, "glCopyTexSubImage3DEXT") == 0) func =  AmiglCopyTexSubImage3DEXT;
     else if(strcmp(proc, "glColorTableEXT") == 0) func =  AmiglColorTableEXT;
     else if(strcmp(proc, "glColorSubTableEXT") == 0) func =  AmiglColorSubTableEXT;
     else if(strcmp(proc, "glGetColorTableEXT") == 0) func =  AmiglGetColorTableEXT;
     else if(strcmp(proc, "glGetColorTableParameterfvEXT") == 0) func =  AmiglGetColorTableParameterfvEXT;
     else if(strcmp(proc, "glGetColorTableParameterivEXT") == 0) func =  AmiglGetColorTableParameterivEXT;
     else if(strcmp(proc, "glMultiTexCoord1dSGIS") == 0) func =  AmiglMultiTexCoord1dSGIS;
     else if(strcmp(proc, "glMultiTexCoord1dvSGIS") == 0) func =  AmiglMultiTexCoord1dvSGIS;
     else if(strcmp(proc, "glMultiTexCoord1fSGIS") == 0) func =  AmiglMultiTexCoord1fSGIS;
     else if(strcmp(proc, "glMultiTexCoord1fvSGIS") == 0) func =  AmiglMultiTexCoord1fvSGIS;
     else if(strcmp(proc, "glMultiTexCoord1iSGIS") == 0) func =  AmiglMultiTexCoord1iSGIS;
     else if(strcmp(proc, "glMultiTexCoord1ivSGIS") == 0) func =  AmiglMultiTexCoord1ivSGIS;
     else if(strcmp(proc, "glMultiTexCoord1sSGIS") == 0) func =  AmiglMultiTexCoord1sSGIS;
     else if(strcmp(proc, "glMultiTexCoord1svSGIS") == 0) func =  AmiglMultiTexCoord1svSGIS;
     else if(strcmp(proc, "glMultiTexCoord2dSGIS") == 0) func =  AmiglMultiTexCoord2dSGIS;
     else if(strcmp(proc, "glMultiTexCoord2dvSGIS") == 0) func =  AmiglMultiTexCoord2dvSGIS;
     else if(strcmp(proc, "glMultiTexCoord2fSGIS") == 0) func =  AmiglMultiTexCoord2fSGIS;
     else if(strcmp(proc, "glMultiTexCoord2fvSGIS") == 0) func =  AmiglMultiTexCoord2fvSGIS;
     else if(strcmp(proc, "glMultiTexCoord2iSGIS") == 0) func =  AmiglMultiTexCoord2iSGIS;
     else if(strcmp(proc, "glMultiTexCoord2ivSGIS") == 0) func =  AmiglMultiTexCoord2ivSGIS;
     else if(strcmp(proc, "glMultiTexCoord2sSGIS") == 0) func =  AmiglMultiTexCoord2sSGIS;
     else if(strcmp(proc, "glMultiTexCoord2svSGIS") == 0) func =  AmiglMultiTexCoord2svSGIS;
     else if(strcmp(proc, "glMultiTexCoord3dSGIS") == 0) func =  AmiglMultiTexCoord3dSGIS;
     else if(strcmp(proc, "glMultiTexCoord3dvSGIS") == 0) func =  AmiglMultiTexCoord3dvSGIS;
     else if(strcmp(proc, "glMultiTexCoord3fSGIS") == 0) func =  AmiglMultiTexCoord3fSGIS;
     else if(strcmp(proc, "glMultiTexCoord3fvSGIS") == 0) func =  AmiglMultiTexCoord3fvSGIS;
     else if(strcmp(proc, "glMultiTexCoord3iSGIS") == 0) func =  AmiglMultiTexCoord3iSGIS;
     else if(strcmp(proc, "glMultiTexCoord3ivSGIS") == 0) func =  AmiglMultiTexCoord3ivSGIS;
     else if(strcmp(proc, "glMultiTexCoord3sSGIS") == 0) func =  AmiglMultiTexCoord3sSGIS;
     else if(strcmp(proc, "glMultiTexCoord3svSGIS") == 0) func =  AmiglMultiTexCoord3svSGIS;
     else if(strcmp(proc, "glMultiTexCoord4dSGIS") == 0) func =  AmiglMultiTexCoord4dSGIS;
     else if(strcmp(proc, "glMultiTexCoord4dvSGIS") == 0) func =  AmiglMultiTexCoord4dvSGIS;
     else if(strcmp(proc, "glMultiTexCoord4fSGIS") == 0) func =  AmiglMultiTexCoord4fSGIS;
     else if(strcmp(proc, "glMultiTexCoord4fvSGIS") == 0) func =  AmiglMultiTexCoord4fvSGIS;
     else if(strcmp(proc, "glMultiTexCoord4iSGIS") == 0) func =  AmiglMultiTexCoord4iSGIS;
     else if(strcmp(proc, "glMultiTexCoord4ivSGIS") == 0) func =  AmiglMultiTexCoord4ivSGIS;
     else if(strcmp(proc, "glMultiTexCoord4sSGIS") == 0) func =  AmiglMultiTexCoord4sSGIS;
     else if(strcmp(proc, "glMultiTexCoord4svSGIS") == 0) func=  AmiglMultiTexCoord4svSGIS;
     else if(strcmp(proc, "glMultiTexCoordPointerSGIS") == 0) func =  AmiglMultiTexCoordPointerSGIS;
     else if(strcmp(proc, "glSelectTextureSGIS") == 0) func =  AmiglSelectTextureSGIS;
     else if(strcmp(proc, "glSelectTextureCoordSetSGIS") == 0) func =  AmiglSelectTextureCoordSetSGIS;
     else if(strcmp(proc, "glMultiTexCoord1dEXT") == 0) func =  AmiglMultiTexCoord1dEXT;
     else if(strcmp(proc, "glMultiTexCoord1dvEXT") == 0) func =  AmiglMultiTexCoord1dvEXT;
     else if(strcmp(proc, "glMultiTexCoord1fEXT") == 0) func =  AmiglMultiTexCoord1fEXT;
     else if(strcmp(proc, "glMultiTexCoord1fvEXT") == 0) func =  AmiglMultiTexCoord1fvEXT;
     else if(strcmp(proc, "glMultiTexCoord1iEXT") == 0) func =  AmiglMultiTexCoord1iEXT;
     else if(strcmp(proc, "glMultiTexCoord1ivEXT") == 0) func =  AmiglMultiTexCoord1ivEXT;
     else if(strcmp(proc, "glMultiTexCoord1sEXT") == 0) func =  AmiglMultiTexCoord1sEXT;
     else if(strcmp(proc, "glMultiTexCoord1svEXT") == 0) func =  AmiglMultiTexCoord1svEXT;
     else if(strcmp(proc, "glMultiTexCoord2dEXT") == 0) func =  AmiglMultiTexCoord2dEXT;
     else if(strcmp(proc, "glMultiTexCoord2dvEXT") == 0) func =  AmiglMultiTexCoord2dvEXT;
     else if(strcmp(proc, "glMultiTexCoord2fEXT") == 0) func =  AmiglMultiTexCoord2fEXT;
     else if(strcmp(proc, "glMultiTexCoord2fvEXT") == 0) func =  AmiglMultiTexCoord2fvEXT;
     else if(strcmp(proc, "glMultiTexCoord2iEXT") == 0) func =  AmiglMultiTexCoord2iEXT;
     else if(strcmp(proc, "glMultiTexCoord2ivEXT") == 0) func =  AmiglMultiTexCoord2ivEXT;
     else if(strcmp(proc, "glMultiTexCoord2sEXT") == 0) func =  AmiglMultiTexCoord2sEXT;
     else if(strcmp(proc, "glMultiTexCoord2svEXT") == 0) func =  AmiglMultiTexCoord2svEXT;
     else if(strcmp(proc, "glMultiTexCoord3dEXT") == 0) func =  AmiglMultiTexCoord3dEXT;
     else if(strcmp(proc, "glMultiTexCoord3dvEXT") == 0) func =  AmiglMultiTexCoord3dvEXT;
     else if(strcmp(proc, "glMultiTexCoord3fEXT") == 0) func =  AmiglMultiTexCoord3fEXT;
     else if(strcmp(proc, "glMultiTexCoord3fvEXT") == 0) func =  AmiglMultiTexCoord3fvEXT;
     else if(strcmp(proc, "glMultiTexCoord3iEXT") == 0) func =  AmiglMultiTexCoord3iEXT;
     else if(strcmp(proc, "glMultiTexCoord3ivEXT") == 0) func =  AmiglMultiTexCoord3ivEXT;
     else if(strcmp(proc, "glMultiTexCoord3sEXT") == 0) func =  AmiglMultiTexCoord3sEXT;
     else if(strcmp(proc, "glMultiTexCoord3svEXT") == 0) func =  AmiglMultiTexCoord3svEXT;
     else if(strcmp(proc, "glMultiTexCoord4dEXT") == 0) func =  AmiglMultiTexCoord4dEXT;
     else if(strcmp(proc, "glMultiTexCoord4dvEXT") == 0) func =  AmiglMultiTexCoord4dvEXT;
     else if(strcmp(proc, "glMultiTexCoord4fEXT") == 0) func =  AmiglMultiTexCoord4fEXT;
     else if(strcmp(proc, "glMultiTexCoord4fvEXT") == 0) func =  AmiglMultiTexCoord4fvEXT;
     else if(strcmp(proc, "glMultiTexCoord4iEXT") == 0) func =  AmiglMultiTexCoord4iEXT;
     else if(strcmp(proc, "glMultiTexCoord4ivEXT") == 0) func =  AmiglMultiTexCoord4ivEXT;
     else if(strcmp(proc, "glMultiTexCoord4sEXT") == 0) func =  AmiglMultiTexCoord4sEXT;
     else if(strcmp(proc, "glMultiTexCoord4svEXT") == 0) func =  AmiglMultiTexCoord4svEXT;
     else if(strcmp(proc, "glInterleavedTextureCoordSetsEXT") == 0) func =  AmiglInterleavedTextureCoordSetsEXT;
     else if(strcmp(proc, "glSelectTextureEXT") == 0) func =  AmiglSelectTextureEXT;
     else if(strcmp(proc, "glSelectTextureCoordSetEXT") == 0) func =  AmiglSelectTextureCoordSetEXT;
     else if(strcmp(proc, "glSelectTextureTransformEXT") == 0) func =  AmiglSelectTextureTransformEXT;
     else if(strcmp(proc, "glPointParameterfEXT") == 0) func =  AmiglPointParameterfEXT;
     else if(strcmp(proc, "glPointParameterfvEXT") == 0) func =  AmiglPointParameterfvEXT;
     else if(strcmp(proc, "glWindowPos2iMESA") == 0) func =  AmiglWindowPos2iMESA;
     else if(strcmp(proc, "glWindowPos2sMESA") == 0) func =  AmiglWindowPos2sMESA;
     else if(strcmp(proc, "glWindowPos2fMESA") == 0) func =  AmiglWindowPos2fMESA;
     else if(strcmp(proc, "glWindowPos2dMESA") == 0) func =  AmiglWindowPos2dMESA;
     else if(strcmp(proc, "glWindowPos2ivMESA") == 0) func =  AmiglWindowPos2ivMESA;
     else if(strcmp(proc, "glWindowPos2svMESA") == 0) func =  AmiglWindowPos2svMESA;
     else if(strcmp(proc, "glWindowPos2fvMESA") == 0) func =  AmiglWindowPos2fvMESA;
     else if(strcmp(proc, "glWindowPos2dvMESA") == 0) func =  AmiglWindowPos2dvMESA;
     else if(strcmp(proc, "glWindowPos3iMESA") == 0) func =  AmiglWindowPos3iMESA;
     else if(strcmp(proc, "glWindowPos3sMESA") == 0) func =  AmiglWindowPos3sMESA;
     else if(strcmp(proc, "glWindowPos3fMESA") == 0) func =  AmiglWindowPos3fMESA;
     else if(strcmp(proc, "glWindowPos3dMESA") == 0) func =  AmiglWindowPos3dMESA;
     else if(strcmp(proc, "glWindowPos3ivMESA") == 0) func =  AmiglWindowPos3ivMESA;
     else if(strcmp(proc, "glWindowPos3svMESA") == 0) func =  AmiglWindowPos3svMESA;
     else if(strcmp(proc, "glWindowPos3fvMESA") == 0) func =  AmiglWindowPos3fvMESA;
     else if(strcmp(proc, "glWindowPos3dvMESA") == 0) func =  AmiglWindowPos3dvMESA;
     else if(strcmp(proc, "glWindowPos4iMESA") == 0) func =  AmiglWindowPos4iMESA;
     else if(strcmp(proc, "glWindowPos4sMESA") == 0) func =  AmiglWindowPos4sMESA;
     else if(strcmp(proc, "glWindowPos4fMESA") == 0) func =  AmiglWindowPos4fMESA;
     else if(strcmp(proc, "glWindowPos4dMESA") == 0) func =  AmiglWindowPos4dMESA;
     else if(strcmp(proc, "glWindowPos4ivMESA") == 0) func =  AmiglWindowPos4ivMESA;
     else if(strcmp(proc, "glWindowPos4svMESA") == 0) func =  AmiglWindowPos4svMESA;
     else if(strcmp(proc, "glWindowPos4fvMESA") == 0) func =  AmiglWindowPos4fvMESA;
     else if(strcmp(proc, "glWindowPos4dvMESA") == 0) func =  AmiglWindowPos4dvMESA;
     else if(strcmp(proc, "glResizeBuffersMESA") == 0) func =  AmiglResizeBuffersMESA;
     else if(strcmp(proc, "glDrawRangeElements") == 0) func =  AmiglDrawRangeElements;
     else if(strcmp(proc, "glTexImage3D") == 0) func =  AmiglTexImage3D;
     else if(strcmp(proc, "glTexSubImage3D") == 0) func =  AmiglTexSubImage3D;
     else if(strcmp(proc, "glCopyTexSubImage3D") == 0) func =  AmiglCopyTexSubImage3D;
     else if(strcmp(proc, "gluLookAt") == 0) func = AmigluLookAt;
     else if(strcmp(proc, "gluOrtho2D") == 0) func = AmigluOrtho2D;
     else if(strcmp(proc, "gluPerspective") == 0) func = AmigluPerspective;
     else if(strcmp(proc, "gluPickMatrix") == 0) func = AmigluPickMatrix;
     else if(strcmp(proc, "gluProject") == 0) func = AmigluProject;
     else if(strcmp(proc, "gluUnProject") == 0) func = AmigluUnProject;
     else if(strcmp(proc, "gluErrorString") == 0) func = AmigluErrorString;
     else if(strcmp(proc, "gluScaleImage") == 0) func = AmigluScaleImage;
     else if(strcmp(proc, "gluBuild1DMipmaps") == 0) func = AmigluBuild1DMipmaps;
     else if(strcmp(proc, "gluBuild2DMipmaps") == 0) func = AmigluBuild2DMipmaps;
     else if(strcmp(proc, "gluNewQuadric") == 0) func = AmigluNewQuadric;
     else if(strcmp(proc, "gluDeleteQuadric") == 0) func = AmigluDeleteQuadric;
     else if(strcmp(proc, "gluQuadricDrawStyle") == 0) func = AmigluQuadricDrawStyle;
     else if(strcmp(proc, "gluQuadricOrientation") == 0) func = AmigluQuadricOrientation;
     else if(strcmp(proc, "gluQuadricNormals") == 0) func = AmigluQuadricNormals;
     else if(strcmp(proc, "gluQuadricTexture") == 0) func = AmigluQuadricTexture;
     else if(strcmp(proc, "gluQuadricCallback") == 0) func = AmigluQuadricCallback;
     else if(strcmp(proc, "gluCylinder") == 0) func = AmigluCylinder;
     else if(strcmp(proc, "gluSphere") == 0) func = AmigluSphere;
     else if(strcmp(proc, "gluDisk") == 0) func = AmigluDisk;
     else if(strcmp(proc, "gluPartialDisk") == 0) func = AmigluPartialDisk;
     else if(strcmp(proc, "gluNewNurbsRenderer") == 0) func = AmigluNewNurbsRenderer;
     else if(strcmp(proc, "gluDeleteNurbsRenderer") == 0) func = AmigluDeleteNurbsRenderer;
     else if(strcmp(proc, "gluLoadSamplingMatrices") == 0) func = AmigluLoadSamplingMatrices;
     else if(strcmp(proc, "gluNurbsProperty") == 0) func = AmigluNurbsProperty;
     else if(strcmp(proc, "gluGetNurbsProperty") == 0) func = AmigluGetNurbsProperty;
     else if(strcmp(proc, "gluBeginCurve") == 0) func = AmigluBeginCurve;
     else if(strcmp(proc, "gluEndCurve") == 0) func = AmigluEndCurve;
     else if(strcmp(proc, "gluNurbsCurve") == 0) func = AmigluNurbsCurve;
     else if(strcmp(proc, "gluBeginSurface") == 0) func = AmigluBeginSurface;
     else if(strcmp(proc, "gluEndSurface") == 0) func = AmigluEndSurface;
     else if(strcmp(proc, "gluNurbsSurface") == 0) func = AmigluNurbsSurface;
     else if(strcmp(proc, "gluBeginTrim") == 0) func = AmigluBeginTrim;
     else if(strcmp(proc, "gluEndTrim") == 0) func = AmigluEndTrim;
     else if(strcmp(proc, "gluPwlCurve") == 0) func = AmigluPwlCurve;
     else if(strcmp(proc, "gluNurbsCallback") == 0) func = AmigluNurbsCallback;
     else if(strcmp(proc, "gluNewTess") == 0) func = AmigluNewTess;
     else if(strcmp(proc, "gluTessCallback") == 0) func = AmigluTessCallback;
     else if(strcmp(proc, "gluDeleteTess") == 0) func = AmigluDeleteTess;
     else if(strcmp(proc, "gluBeginPolygon") == 0) func = AmigluBeginPolygon;
     else if(strcmp(proc, "gluEndPolygon") == 0) func = AmigluEndPolygon;
     else if(strcmp(proc, "gluNextContour") == 0) func = AmigluNextContour;
     else if(strcmp(proc, "gluTessVertex") == 0) func = AmigluTessVertex;
     else if(strcmp(proc, "gluGetString") == 0) func = AmigluGetString;

     return func;
}

#endif /*HAVE_OPENGL*/
