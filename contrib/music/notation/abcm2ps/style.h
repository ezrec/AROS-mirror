/*
 *  This file is part of abc2ps, Copyright (C) 1996,1997 Michael Methfessel
 *  See file abc2ps.c for details.
 */

/*   Global style parameters for the note spacing and Knuthian glue. */

/*   Parameters here are used to set spaces around notes.
     Names ending in p: prefered natural spacings
     Units: everything is based on a staff which is 24 points high
     (ie. 6 pt between two staff lines). */

/* name for this set of parameters */
#define STYLE "std"

/* ----- Parameters for note spacing ----- */
/* -- bnn determines how strongly the first note enters into the spacing.
      For bnn=1, the spacing is calculated using the first note.
      For bnn=0, the spacing is the average for the two notes.
   -- fnn multiplies the spacing under a beam, to compress the notes a bit
   -- gnn multiplies the spacing a second time within an n-tuplet */

#define bnnp 0.9
#define fnnp 0.9
#define gnnp 0.8

