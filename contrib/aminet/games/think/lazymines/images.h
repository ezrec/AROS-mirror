/*
 * images.h
 * ========
 * The game images.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#ifndef IMAGES_H
#define IMAGES_H

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>


/* Attributes for digits */
#define DIGITWIDTH    13
#define DIGITHEIGHT   21
#define EMPTYDIGIT    10

/* The image objects */
extern struct Image  *flag_image;
extern struct Image  *mine_image;
extern struct Image  *digit_images[];
extern struct Image  *bigmine_image;
extern struct Image  *face_image;


/* Initialization of game images */
BOOL                                  /* success? */
init_images (
   struct Screen  *scr,               /* screen to use images on */
   BOOL            create_digits,     /* create digit images? */
   BOOL            display_colors);   /* colorfull images? */

/* Change image colors */
void
update_images (
   BOOL   display_colors);   /* colorfull images? */

/* Finalization of game images */
void
free_images (
   struct Screen  *scr);   /* screen that images were used on */

#endif /* IMAGES_H */
