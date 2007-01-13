/***************************************************************************
                          misc.h  -  description
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

#ifndef __MISC_H
#define __MISC_H

/*
====================================================================
Vector
====================================================================
*/
typedef struct {
    float   x, y;
} Vector;
typedef Vector Coord;
/*
====================================================================
Integer Coordinates
====================================================================
*/
typedef struct {
    int x,y;
} ICoord;
/*
====================================================================
Line
====================================================================
*/
typedef struct {
    float x; /* position if vertical line */
    int vertical; /* if this is set monotony is not computed */
    float m; /* monotony */
    float n; /* y_offset */
} Line;
/*
====================================================================
Integer Line
====================================================================
*/
typedef struct {
    int x; /* position if vertical line */
    int vertical; /* if this is set monotony is not computed */
    int m_4096; /* monotony * 4096 */
    int n; /* y_offset */
} ILine;

/*
====================================================================
Target of a shot or a ball.
====================================================================
*/
enum { SIDE_UNDEFINED = -1, SIDE_TOP = 0, SIDE_RIGHT, SIDE_BOTTOM, SIDE_LEFT, 
							CORNER_UPPER_LEFT, CORNER_UPPER_RIGHT, 
							CORNER_LOWER_LEFT, CORNER_LOWER_RIGHT };
typedef struct {
    int     exists; /* is there a target */
    int     mx, my; /* position in map */
    float   x, y; /* reset position of ball */
    int     time; /* time till contact */
    int     cur_tm; /* current time */
    Vector  perp_vector; /* reflection vector */
    int     side; /* side/corner of brick hit */
} Target;

/*
====================================================================
Load background according to id and draw background to offscreen.
Return Value: loaded background surface
====================================================================
*/
SDL_Surface* bkgnd_draw( int id );
/*
====================================================================
Confirm request. Darkens/gray screen a bit and display text.
Return Value: True if successful
====================================================================
*/
enum{ CONFIRM_YES_NO, CONFIRM_ANY_KEY, CONFIRM_PAUSE };
int confirm( StkFont *font, char *str, int type );
/*
====================================================================
Display a info message (gray screen a bit and display text), 
send a MSG_READY when player has clicked and wait for a remote answer
(timeout 10 secs). Waiting may be cancelled by pressing ESCAPE which
results in sending a MSG_GAME_EXITED.
Return Value: True if both peers clicked to continue, False if the
connection was cancelled for some reason.
====================================================================
*/
int display_info( StkFont *font, char *str, Net_Socket *peer );
/*
====================================================================
Create shadow surface for specified region in surface.
Return Value: Shadow surface
====================================================================
*/
SDL_Surface* create_shadow( SDL_Surface *surf, int x, int y, int w, int h );

/*
====================================================================
Return vector struct with the specified coordinates.
====================================================================
*/
Vector vector_get( float x, float y );
/*
====================================================================
Give vector the normed length of 1.
====================================================================
*/
void vector_norm( Vector *v );
/*
====================================================================
Return monotony of vector. If vertical return 0
====================================================================
*/
float vector_monotony( Vector v );
/*
====================================================================
Set length of a vector.
====================================================================
*/
void vector_set_length( Vector *v, float length );

/*
====================================================================
Initiate a line struct.
====================================================================
*/
void line_set( Line *line, float x, float y, float m );
void line_set_vert( Line *line, float x );
void line_set_hori( Line *line, float y );
/*
====================================================================
Intersect lines and set 'pos' to intersecting point.
Return Value: True if lines intersect.
====================================================================
*/
int line_intersect( Line *line, Line *target, Coord *pos );

/*
====================================================================
Initiate a line struct.
====================================================================
*/
void iline_set( ILine *line, int x, int y, int m_4096 );
void iline_set_vert( ILine *line, int x );
void iline_set_hori( ILine *line, int y );
/*
====================================================================
Intersect lines and set 'pos' to intersecting point.
Return Value: True if lines intersect.
====================================================================
*/
int iline_intersect( ILine *line, ILine *target, ICoord *pos );

/*
====================================================================
Intersect line pos+t*v with circle (x+m)²=r²
Important length of v MUST be 1.
Return Value: True if intersecting, Intersecting points
====================================================================
*/
int circle_intersect( Vector m, int r, Vector pos, Vector v, Vector *t1, Vector *t2 );

/*
====================================================================
Enter a string and return True if ENTER received and False
if ESCAPE received.
====================================================================
*/
int enter_string( StkFont *font, char *caption, char *edit, int limit );

/*
====================================================================
Display text blinking.
====================================================================
*/
void write_text_with_cursor( StkFont *sfnt, SDL_Surface *dest,
    int x, int y, char *str, int alpha);

#endif
