/* KDPpoly v0.8 wreck/knp ydhz/knp

  slow :(

*/


#ifndef KDPPOLY_H
#define KDPPOLY_H

#include "kdp.h"
#include "kdpgfx.h"

typedef struct
        {
        float x,y;
        float z;
        float u,v;
        UBYTE c;
        } KDPvertex;

typedef struct
        {
        int a,b,c;
        int type;
        UBYTE *texture;
        UBYTE color;
        } KDPface;

/* types:  (for KDPdrawpoly)

  -1: dont draw

   0: draw points
   1: wireframe
   2: add wireframe
   3: flat
   4: gouraud
   5: gouraud with colortable
   6: textured
   7: flat shaded textured
   8: gouraud shaded textured

*/

void KDPffpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE col,UBYTE *vmem);
void KDPfgpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE *vmem);
void KDPfgcpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE col,UBYTE *vmem);
void KDPftpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE *tex,UBYTE *vmem);
void KDPfftpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE col,UBYTE *tex,UBYTE *vmem);
void KDPfgtpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE *tex,UBYTE *vmem);



void KDPdrawPoly(KDPface *fac,KDPvertex *vert,UBYTE *vmem);

#endif
