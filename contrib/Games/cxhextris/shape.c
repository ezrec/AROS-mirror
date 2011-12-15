/*
 * hextris Copyright 1990 David Markley, dm3e@+andrew.cmu.edu, dam@cs.cmu.edu
 *
 * Permission to use, copy, modify, and distribute, this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders be used in
 * advertising or publicity pertaining to distribution of the software with
 * specific, written prior permission, and that no fee is charged for further
 * distribution of this software, or any modifications thereof.  The copyright
 * holder make no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDER DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA, PROFITS, QPA OR GPA, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "header.h"

/* This is the mess that defines each piece. There are six rotations to
 * each piece, so there are six rows of numbers to each piece. For example,
 * the second rotation of the third piece is on the row labled 21. Each row
 * has 16 numbers, that can be broken down into 2 sets of 4 pairs. Each pair
 * is a set of relative coordinates inf row and column from the current
 * position of the piece. There are two sets of 4 pairs, because the offsets
 * differ depending on whether the current position of the piece is in a
 * hex that is up to its neighbors in its row, or down to them. One row in the
 * game moves up and down, from left to right.
 */
int shape[NUMBEROFPIECES*6][16]
  = {{0,0,-1,0,1,-1,1,1,-1,0,0,-1,0,1,0,0},       /* 00 */
       {0,0,0,-1,0,1,1,0,-1,-1,-1,1,1,0,0,0},     /* 01 */
       {0,0,-1,0,1,-1,1,1,-1,0,0,-1,0,1,0,0},     /* 02 */
       {0,0,0,-1,0,1,1,0,-1,-1,-1,1,1,0,0,0},     /* 03 */
       {0,0,-1,0,1,-1,1,1,-1,0,0,-1,0,1,0,0},     /* 04 */
       {0,0,0,-1,0,1,1,0,-1,-1,-1,1,1,0,0,0},     /* 05 */
       {0,0,-1,0,1,0,2,0,-1,0,1,0,2,0,0,0},       /* 10 */
       {0,0,0,-1,1,1,1,2,-1,-1,0,1,1,2,0,0},      /* 11 */
       {0,0,1,-1,0,1,-1,2,0,-1,-1,1,-1,2,0,0},    /* 12 */
       {0,0,-2,0,-1,0,1,0,-2,0,-1,0,1,0,0,0},     /* 13 */
       {0,0,-1,-2,0,-1,1,1,-1,-2,-1,-1,0,1,0,0},  /* 14 */
       {0,0,1,-2,1,-1,0,1,1,-2,0,-1,-1,1,0,0},    /* 15 */
       {0,0,-1,0,1,0,1,1,-1,0,1,0,0,1,0,0},       /* 20 */
       {0,0,0,-1,1,1,0,1,-1,-1,0,1,-1,1,0,0},     /* 21 */
       {0,0,1,-1,0,1,-1,0,0,-1,-1,0,-1,1,0,0},    /* 22 */
       {0,0,0,-1,-1,0,1,0,-1,-1,-1,0,1,0,0,0},    /* 23 */
       {0,0,0,-1,1,-1,1,1,0,-1,-1,-1,0,1,0,0},    /* 24 */
       {0,0,1,-1,1,0,0,1,0,-1,1,0,-1,1,0,0},      /* 25 */
       {0,0,-1,0,1,-1,1,0,-1,0,0,-1,1,0,0,0},     /* 30 */
       {0,0,0,-1,1,0,1,1,-1,-1,1,0,0,1,0,0},      /* 31 */
       {0,0,1,-1,0,1,1,1,0,-1,-1,1,0,1,0,0},      /* 32 */
       {0,0,-1,0,1,0,0,1,-1,0,1,0,-1,1,0,0},      /* 33 */
       {0,0,0,-1,-1,0,1,1,-1,-1,-1,0,0,1,0,0},    /* 34 */
       {0,0,0,-1,1,-1,0,1,-1,-1,0,-1,-1,1,0,0},   /* 35 */
       {-1,0,0,-1,1,-1,1,0,-1,0,-1,-1,0,-1,1,0},  /* 40 */
       {0,-1,1,-1,1,0,1,1,-1,-1,0,-1,1,0,0,1},    /* 41 */
       {1,-1,1,0,1,1,0,1,0,-1,1,0,0,1,-1,1},      /* 42 */
       {1,0,1,1,0,1,-1,0,1,0,0,1,-1,1,-1,0},      /* 43 */
       {1,1,0,1,-1,0,0,-1,0,1,-1,1,-1,0,-1,-1},   /* 44 */
       {0,1,-1,0,0,-1,1,-1,-1,1,-1,0,-1,-1,0,-1}, /* 45 */
       {-1,0,1,1,2,1,0,0,-1,0,0,0,0,1,1,1},       /* 50 */
       {0,-1,0,0,0,1,0,2,-1,-1,0,0,-1,1,0,2},     /* 51 */
       {1,-1,0,0,-1,0,-1,1,0,-1,0,0,-1,0,-2,1},   /* 52 */
       {1,0,0,0,0,-1,-1,-1,1,0,0,0,-1,-1,-2,-1},  /* 53 */
       {0,-2,1,-1,0,0,1,1,0,-2,0,-1,0,0,0,1},     /* 54 */
       {2,-1,1,0,0,0,0,1,1,-1,1,0,0,0,-1,1},      /* 55 */
       {0,0,1,-1,1,0,1,1,0,0,0,-1,1,0,0,1},	  /* 60 */
       {0,0,1,0,1,1,0,1,0,0,1,0,0,1,-1,1},	  /* 61 */
       {0,0,1,1,0,1,-1,0,0,0,0,1,-1,1,-1,0},      /* 62 */
       {0,0,0,1,-1,0,0,-1,0,0,-1,1,-1,0,-1,-1},   /* 63 */
       {0,0,-1,0,0,-1,1,-1,0,0,-1,0,-1,-1,0,-1},  /* 64 */
       {0,0,0,-1,1,-1,1,0,0,0,-1,-1,0,-1,1,0},    /* 65 */
       {-1,0,0,0,1,0,2,1,-1,0,0,0,1,0,1,1},	  /* 70 */
       {0,-1,0,0,1,1,0,2,-1,-1,0,0,0,1,0,2},      /* 71 */
       {1,-1,0,0,0,1,-1,1,0,-1,0,0,-1,1,-2,1},    /* 72 */
       {1,0,0,0,-1,0,-1,-1,1,0,0,0,-1,0,-2,-1},   /* 73 */
       {0,-2,0,-1,0,0,1,1,0,-2,-1,-1,0,0,0,1},    /* 74 */
       {2,-1,1,-1,0,0,0,1,1,-1,0,-1,0,0,-1,1},    /* 75 */
       {-1,0,0,0,1,0,2,-1,-1,0,0,0,1,0,1,-1},     /* 80 */
       {0,-1,0,0,1,1,2,1,-1,-1,0,0,0,1,1,1},      /* 81 */
       {1,-1,0,0,0,1,0,2,0,-1,0,0,-1,1,0,2},      /* 82 */
       {1,0,0,0,-1,0,-1,1,1,0,0,0,-1,0,-2,1},     /* 83 */
       {-1,-1,0,-1,0,0,1,1,-2,-1,-1,-1,0,0,0,1},  /* 84 */
       {0,-2,1,-1,0,0,0,1,0,-2,0,-1,0,0,-1,1},    /* 85 */
       {-1,0,0,0,1,-1,2,-1,-1,0,0,0,0,-1,1,-1},   /* 90 */
       {0,-1,0,0,1,0,2,1,-1,-1,0,0,1,0,1,1},      /* 91 */
       {1,-1,0,0,1,1,0,2,0,-1,0,0,0,1,0,2},       /* 92 */
       {1,0,0,0,0,1,-1,1,1,0,0,0,-1,1,-2,1},      /* 93 */
       {1,1,0,0,-1,0,-1,-1,0,1,0,0,-1,0,-2,-1},   /* 94 */
       {0,-2,0,-1,0,0,0,1,0,-2,-1,-1,0,0,-1,1}};  /* 95 */


